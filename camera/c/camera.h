#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

typedef struct Image {
    int width;
    int height;
    unsigned char*** data; // height x width x RGB
} Image;

Image read_bmp(const char* filename);
uint16_t conv24to16bit(uint8_t r, uint8_t g, uint8_t b);
int withinHueRange(unsigned char *rgb, int *args, int argCount);
int noFilter(unsigned char *rgb, int *args, int argCount);
void drawImageFiltered(Image image, int (*localFilter)(unsigned char *, int *, int), int *args, int argCount);
int binaryToInt(int binary);

#endif
