#ifndef BMP_STORAGE_H
#define BMP_STORAGE_H

#include <windows.h>

typedef enum {
    BMP_SUCCESS = 0,
    BMP_ERROR_INVALID = -1,
    BMP_ERROR_FILE = -2
} BMPError;

int bmp_generate_filename(char* buffer, size_t size);

BMPError bmp_save(const char* filepath, 
                  const unsigned char* data,
                  int width, int height, int stride);

const char* bmp_error_string(BMPError error);

#endif
