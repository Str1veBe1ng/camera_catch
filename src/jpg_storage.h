#ifndef JPG_STORAGE_H
#define JPG_STORAGE_H

#include <windows.h>

typedef enum {
    JPG_SUCCESS = 0,
    JPG_ERROR_INVALID = -1,
    JPG_ERROR_INIT = -2,
    JPG_ERROR_ENCODE = -3,
    JPG_ERROR_FILE = -4
} JPGError;

// Generate timestamp filename
int jpg_generate_filename(char* buffer, size_t size);

// Save as JPEG using WIC
// quality: 0-100 (default 85)
JPGError jpg_save(const char* filepath, 
                  const unsigned char* data,
                  int width, int height, int stride,
                  int quality);

const char* jpg_error_string(JPGError error);

#endif
