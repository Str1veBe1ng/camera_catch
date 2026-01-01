#include "mf_camera.h"
#include "jpg_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char* prog) {
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -q <0-100>  JPEG quality (default: 85)\n");
    fprintf(stderr, "  -o <path>   Output file path (default: auto-generated)\n");
    fprintf(stderr, "  -h          Show this help\n");
}

int main(int argc, char* argv[]) {
    int quality = 85;
    char* output_path = NULL;
    char auto_path[260] = {0};
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            quality = atoi(argv[++i]);
            if (quality < 0) quality = 0;
            if (quality > 100) quality = 100;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    // Generate filename if not specified
    if (!output_path) {
        if (jpg_generate_filename(auto_path, sizeof(auto_path)) != 0) {
            fprintf(stderr, "Error: Failed to generate filename\n");
            return 2;
        }
        output_path = auto_path;
    }
    
    // Initialize Media Foundation
    MFCameraError mfErr = mf_init();
    if (mfErr != MF_CAM_SUCCESS) {
        fprintf(stderr, "Error: %s\n", mf_error_string(mfErr));
        return 1;
    }
    
    // Capture image
    MFImageData image = {0};
    mfErr = mf_capture(&image);
    if (mfErr != MF_CAM_SUCCESS) {
        fprintf(stderr, "Error: %s\n", mf_error_string(mfErr));
        mf_cleanup();
        return 1;
    }
    
    // Save as JPEG
    JPGError jpgErr = jpg_save(output_path, image.data, 
                               image.width, image.height, image.stride,
                               quality);
    if (jpgErr != JPG_SUCCESS) {
        fprintf(stderr, "Error: %s\n", jpg_error_string(jpgErr));
        mf_free_image(&image);
        mf_cleanup();
        return 2;
    }
    
    // Output result
    printf("%s\n", output_path);
    
    // Cleanup
    mf_free_image(&image);
    mf_cleanup();
    
    return 0;
}
