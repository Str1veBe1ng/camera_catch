#include "jpg_storage.h"
#include <wincodec.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

int jpg_generate_filename(char* buffer, size_t size) {
    if (!buffer || size < 30) return -1;
    
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    if (!t) return -1;
    
    int n = snprintf(buffer, size, "capture_%04d%02d%02d_%02d%02d%02d.jpg",
                     t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                     t->tm_hour, t->tm_min, t->tm_sec);
    return (n > 0 && (size_t)n < size) ? 0 : -1;
}

JPGError jpg_save(const char* filepath, 
                  const unsigned char* data,
                  int width, int height, int stride,
                  int quality) {
    if (!filepath || !data || width <= 0 || height <= 0) {
        return JPG_ERROR_INVALID;
    }
    
    if (quality < 0) quality = 0;
    if (quality > 100) quality = 100;
    
    HRESULT hr;
    IWICImagingFactory* pFactory = NULL;
    IWICBitmapEncoder* pEncoder = NULL;
    IWICBitmapFrameEncode* pFrame = NULL;
    IWICStream* pStream = NULL;
    IPropertyBag2* pPropertyBag = NULL;
    JPGError result = JPG_ERROR_ENCODE;
    
    // Convert filepath to wide string
    int wlen = MultiByteToWideChar(CP_UTF8, 0, filepath, -1, NULL, 0);
    WCHAR* wpath = (WCHAR*)malloc(wlen * sizeof(WCHAR));
    if (!wpath) return JPG_ERROR_INIT;
    MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wpath, wlen);
    
    // Initialize COM (may already be initialized)
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    bool comInitialized = SUCCEEDED(hr);
    
    // Create WIC factory
    hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) {
        free(wpath);
        if (comInitialized) CoUninitialize();
        return JPG_ERROR_INIT;
    }
    
    // Create stream
    hr = pFactory->CreateStream(&pStream);
    if (FAILED(hr)) goto cleanup;
    
    hr = pStream->InitializeFromFilename(wpath, GENERIC_WRITE);
    if (FAILED(hr)) {
        result = JPG_ERROR_FILE;
        goto cleanup;
    }
    
    // Create JPEG encoder
    hr = pFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &pEncoder);
    if (FAILED(hr)) goto cleanup;
    
    hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
    if (FAILED(hr)) goto cleanup;
    
    // Create frame
    hr = pEncoder->CreateNewFrame(&pFrame, &pPropertyBag);
    if (FAILED(hr)) goto cleanup;
    
    // Set JPEG quality
    if (pPropertyBag) {
        PROPBAG2 option = {0};
        option.pstrName = L"ImageQuality";
        VARIANT varValue;
        VariantInit(&varValue);
        varValue.vt = VT_R4;
        varValue.fltVal = (float)quality / 100.0f;
        pPropertyBag->Write(1, &option, &varValue);
    }
    
    hr = pFrame->Initialize(pPropertyBag);
    if (FAILED(hr)) goto cleanup;
    
    // Set size
    hr = pFrame->SetSize(width, height);
    if (FAILED(hr)) goto cleanup;
    
    // Set pixel format (BGR -> RGB conversion needed)
    WICPixelFormatGUID pixelFormat = GUID_WICPixelFormat24bppBGR;
    hr = pFrame->SetPixelFormat(&pixelFormat);
    if (FAILED(hr)) goto cleanup;
    
    // Write pixels (image is bottom-up in our data, WIC expects top-down)
    // Also need to flip vertically
    unsigned char* flipped = (unsigned char*)malloc(stride * height);
    if (!flipped) goto cleanup;
    
    for (int y = 0; y < height; y++) {
        memcpy(flipped + y * stride, data + (height - 1 - y) * stride, stride);
    }
    
    hr = pFrame->WritePixels(height, stride, stride * height, flipped);
    free(flipped);
    if (FAILED(hr)) goto cleanup;
    
    // Commit
    hr = pFrame->Commit();
    if (FAILED(hr)) goto cleanup;
    
    hr = pEncoder->Commit();
    if (SUCCEEDED(hr)) {
        result = JPG_SUCCESS;
    }

cleanup:
    if (pPropertyBag) pPropertyBag->Release();
    if (pFrame) pFrame->Release();
    if (pEncoder) pEncoder->Release();
    if (pStream) pStream->Release();
    if (pFactory) pFactory->Release();
    free(wpath);
    
    return result;
}

const char* jpg_error_string(JPGError error) {
    switch (error) {
        case JPG_SUCCESS: return "Success";
        case JPG_ERROR_INVALID: return "Invalid image data";
        case JPG_ERROR_INIT: return "Failed to initialize WIC";
        case JPG_ERROR_ENCODE: return "JPEG encoding failed";
        case JPG_ERROR_FILE: return "File write error";
        default: return "Unknown error";
    }
}
