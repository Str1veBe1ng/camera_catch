#include "bmp_storage.h"
#include <stdio.h>
#include <time.h>

#pragma pack(push, 1)
typedef struct {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BMPFILEHEADER;

typedef struct {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BMPINFOHEADER;
#pragma pack(pop)

int bmp_generate_filename(char* buffer, size_t size) {
    if (!buffer || size < 30) return -1;
    
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    if (!t) return -1;
    
    int n = snprintf(buffer, size, "capture_%04d%02d%02d_%02d%02d%02d.bmp",
                     t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                     t->tm_hour, t->tm_min, t->tm_sec);
    return (n > 0 && (size_t)n < size) ? 0 : -1;
}

BMPError bmp_save(const char* filepath, 
                  const unsigned char* data,
                  int width, int height, int stride) {
    if (!filepath || !data || width <= 0 || height <= 0) {
        return BMP_ERROR_INVALID;
    }
    
    // BMP row must be 4-byte aligned
    int bmpStride = ((width * 3 + 3) / 4) * 4;
    int imageSize = bmpStride * height;
    int fileSize = 54 + imageSize;
    
    BMPFILEHEADER fh = {0};
    fh.bfType = 0x4D42; // "BM"
    fh.bfSize = fileSize;
    fh.bfOffBits = 54;
    
    BMPINFOHEADER ih = {0};
    ih.biSize = 40;
    ih.biWidth = width;
    ih.biHeight = height; // positive = bottom-up
    ih.biPlanes = 1;
    ih.biBitCount = 24;
    ih.biCompression = 0; // BI_RGB
    ih.biSizeImage = imageSize;
    
    FILE* fp = fopen(filepath, "wb");
    if (!fp) return BMP_ERROR_FILE;
    
    fwrite(&fh, sizeof(fh), 1, fp);
    fwrite(&ih, sizeof(ih), 1, fp);
    
    // Write pixel data (BMP is bottom-up, input may be top-down)
    unsigned char* row = (unsigned char*)malloc(bmpStride);
    if (!row) {
        fclose(fp);
        return BMP_ERROR_FILE;
    }
    
    for (int y = height - 1; y >= 0; y--) {
        const unsigned char* src = data + y * stride;
        memset(row, 0, bmpStride);
        memcpy(row, src, width * 3);
        fwrite(row, bmpStride, 1, fp);
    }
    
    free(row);
    fclose(fp);
    return BMP_SUCCESS;
}

const char* bmp_error_string(BMPError error) {
    switch (error) {
        case BMP_SUCCESS: return "Success";
        case BMP_ERROR_INVALID: return "Invalid image data";
        case BMP_ERROR_FILE: return "File write error";
        default: return "Unknown error";
    }
}
