#include "mf_camera.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")

static bool g_initialized = false;

MFCameraError mf_init() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return MF_CAM_ERROR_INIT;
    }
    
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        CoUninitialize();
        return MF_CAM_ERROR_INIT;
    }
    
    g_initialized = true;
    return MF_CAM_SUCCESS;
}

void mf_cleanup() {
    if (g_initialized) {
        MFShutdown();
        CoUninitialize();
        g_initialized = false;
    }
}

MFCameraError mf_capture(MFImageData* image) {
    if (!image) return MF_CAM_ERROR_CAPTURE;
    
    HRESULT hr = S_OK;
    IMFAttributes* pConfig = NULL;
    IMFActivate** ppDevices = NULL;
    UINT32 count = 0;
    IMFMediaSource* pSource = NULL;
    IMFSourceReader* pReader = NULL;
    MFCameraError result = MF_CAM_ERROR_CAPTURE;
    
    memset(image, 0, sizeof(MFImageData));
    
    // Create attribute store for device enumeration
    hr = MFCreateAttributes(&pConfig, 1);
    if (FAILED(hr)) {
        return MF_CAM_ERROR_INIT;
    }
    
    hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                          MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (FAILED(hr)) {
        pConfig->Release();
        return MF_CAM_ERROR_INIT;
    }
    
    // Enumerate video capture devices
    hr = MFEnumDeviceSources(pConfig, &ppDevices, &count);
    pConfig->Release();
    pConfig = NULL;
    
    if (FAILED(hr) || count == 0) {
        return MF_CAM_ERROR_NO_DEVICE;
    }
    
    // Activate first device
    hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(&pSource));
    
    // Release device list
    for (UINT32 i = 0; i < count; i++) {
        ppDevices[i]->Release();
    }
    CoTaskMemFree(ppDevices);
    ppDevices = NULL;
    
    if (FAILED(hr)) {
        return MF_CAM_ERROR_CAPTURE;
    }
    
    // Create source reader with attributes for video processing
    IMFAttributes* pReaderConfig = NULL;
    hr = MFCreateAttributes(&pReaderConfig, 1);
    if (SUCCEEDED(hr)) {
        pReaderConfig->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
        hr = MFCreateSourceReaderFromMediaSource(pSource, pReaderConfig, &pReader);
        pReaderConfig->Release();
    }
    
    pSource->Release();
    pSource = NULL;
    
    if (FAILED(hr) || !pReader) {
        return MF_CAM_ERROR_CAPTURE;
    }
    
    // Configure output format - try RGB24 first, then RGB32
    IMFMediaType* pType = NULL;
    hr = MFCreateMediaType(&pType);
    if (SUCCEEDED(hr)) {
        pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24);
        hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);
        
        if (FAILED(hr)) {
            // Try RGB32
            pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
            hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);
        }
        pType->Release();
    }
    
    // If still failed, let MF choose native format
    if (FAILED(hr)) {
        hr = S_OK; // Continue with native format
    }
    
    // Get actual media type to determine dimensions
    IMFMediaType* pActualType = NULL;
    hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pActualType);
    if (FAILED(hr)) {
        pReader->Release();
        return MF_CAM_ERROR_FORMAT;
    }
    
    UINT32 width = 0, height = 0;
    hr = MFGetAttributeSize(pActualType, MF_MT_FRAME_SIZE, &width, &height);
    
    GUID subtype = {0};
    pActualType->GetGUID(MF_MT_SUBTYPE, &subtype);
    pActualType->Release();
    
    if (FAILED(hr) || width == 0 || height == 0) {
        pReader->Release();
        return MF_CAM_ERROR_FORMAT;
    }
    
    // Determine bytes per pixel based on format
    int bytesPerPixel = 3;
    if (IsEqualGUID(subtype, MFVideoFormat_RGB32) || IsEqualGUID(subtype, MFVideoFormat_ARGB32)) {
        bytesPerPixel = 4;
    }
    
    // Read samples (skip first few for camera warmup)
    IMFSample* pSample = NULL;
    DWORD streamIndex = 0, flags = 0;
    LONGLONG timestamp = 0;
    
    for (int attempt = 0; attempt < 10; attempt++) {
        hr = pReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            &streamIndex,
            &flags,
            &timestamp,
            &pSample
        );
        
        if (SUCCEEDED(hr) && pSample) {
            if (attempt >= 3) break; // Use frame after warmup
            pSample->Release();
            pSample = NULL;
        }
        Sleep(50);
    }
    
    if (!pSample) {
        pReader->Release();
        return MF_CAM_ERROR_CAPTURE;
    }
    
    // Get buffer from sample
    IMFMediaBuffer* pBuffer = NULL;
    hr = pSample->ConvertToContiguousBuffer(&pBuffer);
    if (FAILED(hr)) {
        pSample->Release();
        pReader->Release();
        return MF_CAM_ERROR_CAPTURE;
    }
    
    BYTE* pData = NULL;
    DWORD maxLen = 0, curLen = 0;
    hr = pBuffer->Lock(&pData, &maxLen, &curLen);
    if (FAILED(hr)) {
        pBuffer->Release();
        pSample->Release();
        pReader->Release();
        return MF_CAM_ERROR_CAPTURE;
    }
    
    // Allocate and copy image data (convert to BGR24 if needed)
    image->width = (int)width;
    image->height = (int)height;
    image->stride = (int)width * 3;
    
    size_t dataSize = (size_t)image->stride * image->height;
    image->data = (unsigned char*)malloc(dataSize);
    
    if (image->data) {
        if (bytesPerPixel == 4) {
            // Convert RGB32/ARGB32 to BGR24
            for (UINT32 y = 0; y < height; y++) {
                unsigned char* dst = image->data + y * image->stride;
                BYTE* src = pData + y * width * 4;
                for (UINT32 x = 0; x < width; x++) {
                    dst[x * 3 + 0] = src[x * 4 + 0]; // B
                    dst[x * 3 + 1] = src[x * 4 + 1]; // G
                    dst[x * 3 + 2] = src[x * 4 + 2]; // R
                }
            }
        } else {
            memcpy(image->data, pData, dataSize);
        }
        result = MF_CAM_SUCCESS;
    }
    
    pBuffer->Unlock();
    pBuffer->Release();
    pSample->Release();
    pReader->Release();
    
    return result;
}

void mf_free_image(MFImageData* image) {
    if (image && image->data) {
        free(image->data);
        image->data = NULL;
        image->width = 0;
        image->height = 0;
        image->stride = 0;
    }
}

const char* mf_error_string(MFCameraError error) {
    switch (error) {
        case MF_CAM_SUCCESS: return "Success";
        case MF_CAM_ERROR_INIT: return "Failed to initialize Media Foundation";
        case MF_CAM_ERROR_NO_DEVICE: return "No camera device found";
        case MF_CAM_ERROR_CAPTURE: return "Failed to capture image";
        case MF_CAM_ERROR_FORMAT: return "Unsupported video format";
        default: return "Unknown error";
    }
}
