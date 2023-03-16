#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/KEY.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/audio.h>
#include <intelfpgaup/HEX.h>

#include "params.h"
#include "display.h"
#include "camera.h"

Image read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: %s not found\n", filename);
        exit(1);
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file);
    int width, height, bit_depth;
    width = *(int*)&header[18];
    height = *(int*)&header[22];
    bit_depth = *(int*)&header[28];
    
    if (height != SCREEN_HEIGHT || width != SCREEN_WIDTH) {
        printf("Warning: img size != screen size, display middle w/ padding\n");
    }

    unsigned char ***tensor;
    int row_size = ((bit_depth * width + 31) / 32) * 4;
    int data_size = row_size * height;

    unsigned char* data = (unsigned char*)malloc(data_size * sizeof(unsigned char));
    fread(data, sizeof(unsigned char), data_size, file);

    tensor = (unsigned char***)malloc(height * sizeof(unsigned char**));
    for (int i = 0; i < height; i++) {
        tensor[i] = (unsigned char**)malloc(width * sizeof(unsigned char*));
        for (int j = 0; j < width; j++) {
            tensor[i][j] = (unsigned char*)malloc(3 * sizeof(unsigned char));
            tensor[i][j][0] = data[i * row_size + j * 3 + 2];
            tensor[i][j][1] = data[i * row_size + j * 3 + 1];
            tensor[i][j][2] = data[i * row_size + j * 3 + 0];
        }
    }
    fclose(file);
    free(data);

    Image image; 
    image.width = width;
    image.height = height;
    image.data = tensor;
    return image;
}

uint16_t conv24to16bit(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)((int)(b / 256.0 * 32) + ((int)(g / 256.0 * 64) << 5) + ((int)(r / 256.0 * 32) << 11)); 
}

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int withinHueRange(unsigned char *rgb, int *args, int argCount) {
    if (argCount != 2 || args[0] > args[1]) { 
        printf("Warning: Hue low is greater than hue high, no filter applied\n");
        return 1;
    }
    double r = rgb[0] / 255.0, 
        g = rgb[1] / 255.0, 
        b = rgb[2] / 255.0,
        cmax = fmax(r, fmax(g, b)), 
        cmin = fmin(r, fmin(g, b)), 
        delta = cmax - cmin, 
        h = 0;
    if (delta == 0) { h = 0; }
    else if (cmax == r) { h = 60 * fmod(((g - b) / delta), 6); }
    else if (cmax == g) { h = 60 * (((b - r) / delta) + 2); }
    else if (cmax == b) { h = 60 * (((r - g) / delta) + 4); }
    if (h < 0) { h += 360; }
    return (args[0] <= (int)h && (int)h <= args[1]); 
}

int noFilter(unsigned char *rgb, int *args, int argCount) {
    return 1;
}

void drawImageFiltered(Image image, int (*localFilter)(unsigned char *, int *, int), int *args, int argCount) {
    int scnHeight = SCREEN_HEIGHT;
    int scnWidth = SCREEN_WIDTH;

    // display image centered on screen
    int top = max(0, (image.height - scnHeight) / 2);
    int left = max(0, (image.width - scnWidth) / 2);
    int bottom = min(image.height, top + scnHeight);
    int right = min(image.width, left + scnWidth);
    
    for (int i = top; i < bottom; i++) {
        for (int j = left; j < right; j++) {
            unsigned char *rgb = image.data[image.height - i - 1][j]; 
            if (localFilter(rgb, args, argCount)) {
                video_pixel(j - left, i - top, conv24to16bit(rgb[0], rgb[1], rgb[2]));
            } else {
                video_pixel(j - left, i - top, 0);
            }
        }
    }
    switchScreen();
}

int binaryToInt(int binary) {
    int decimal = 0;
    int base = 1;
    while (binary > 0) {
        if (binary & 1) {
            decimal += base;
        }
        binary >>= 1;
        base *= 2;
    }
    return decimal;
}
