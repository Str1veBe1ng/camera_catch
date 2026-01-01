#ifndef MF_CAMERA_H
#define MF_CAMERA_H

#include <windows.h>

typedef enum {
    MF_CAM_SUCCESS = 0,
    MF_CAM_ERROR_INIT = -1,
    MF_CAM_ERROR_NO_DEVICE = -2,
    MF_CAM_ERROR_CAPTURE = -3,
    MF_CAM_ERROR_FORMAT = -4
} MFCameraError;

typedef struct {
    unsigned char* data;
    int width;
    int height;
    int stride;
} MFImageData;

MFCameraError mf_init();
MFCameraError mf_capture(MFImageData* image);
void mf_free_image(MFImageData* image);
void mf_cleanup();
const char* mf_error_string(MFCameraError error);

#endif
