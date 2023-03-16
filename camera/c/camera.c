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

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

int screenBuffNum = 0;

void switchScreen(void) {
    video_show();
    screenBuffNum = (screenBuffNum + 1) % 2;
}

void sketchCircle(double centerX, double centerY, int radius, short colour) {
    int radiusSquared = radius * radius;
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            if (x * x + y * y < radiusSquared) {
                video_pixel(round(centerX) + x, round(centerY) + y, colour);
            }
        }
    }
}

void * vgaThread(void *vargp) {
    // while (1) {
    //     angle += angleVelocity;
    //     drawHand();
    //     //drawSongList();
    //     sleep(1 / 60.0);
    // }
    return NULL;
}

typedef struct Image {
    int width;
    int height;
    unsigned char*** data; // height x width x RGB
} Image;

Image read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");

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
    uint16_t color = 0;
    int r2 = (int)(r / 256.0 * 32);
    int g2 = (int)(g / 256.0 * 64);
    int b2 = (int)(b / 256.0 * 32);
    color = b2 + (g2 << 5) + (r2 << 11);
    return color;
}

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int withinHueRange(unsigned char *rgb, int *args, int argCount) {
    if (argCount != 2) {
        printf("Error: filterbyHue takes 2 arguments\n");
        return 0;
    } 
    int hsv_low = args[0];
    int hsv_high = args[1];

    int r = rgb[0];
    int g = rgb[1];
    int b = rgb[2];

    double r_ = r / 255.0;
    double g_ = g / 255.0;
    double b_ = b / 255.0;
    double cmax = max(r_, max(g_, b_));
    double cmin = min(r_, min(g_, b_));
    double delta = cmax - cmin;
    
    double h = 0;
    if (delta == 0) {
        h = 0;
    } else if (cmax == r_) {
        h = 60 * fmod(((g_ - b_) / delta), 6);
    } else if (cmax == g_) {
        h = 60 * (((b_ - r_) / delta) + 2);
    } else if (cmax == b_) {
        h = 60 * (((r_ - g_) / delta) + 4);
    }

    return hsv_low <= h && h <= hsv_high;
}

void filterbyHue(Image image, int hsv_low, int hsv_high) {
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            if (!withinHueRange(image.data[i][j], (int[]){hsv_low, hsv_high}, 2)) {
                image.data[i][j][0] = 0;
                image.data[i][j][1] = 0;
                image.data[i][j][2] = 0;
            } else {
                image.data[i][j][0] = image.data[i][j][0]; 
                image.data[i][j][1] = image.data[i][j][1];
                image.data[i][j][2] = image.data[i][j][2];
            }
        }
    }
}

void drawImage(Image image) {
    int scnHeight = SCREEN_HEIGHT;
    int scnWidth = SCREEN_WIDTH;

    int top = max(0, (image.height - scnHeight) / 2);
    int left = max(0, (image.width - scnWidth) / 2);
    int bottom = min(image.height, top + scnHeight);
    int right = min(image.width, left + scnWidth);
    
    for (int i = top; i < bottom; i++) {
        for (int j = left; j < right; j++) {
            if (i < top || i >= bottom || j < left || j >= right) {
                video_pixel(j - left, i - top, 0); // TODO: test to see if it works
            } else {
                unsigned char *rgb = image.data[image.height - i - 1][j]; 
                uint16_t color = conv24to16bit(rgb[0], rgb[1], rgb[2]);
                video_pixel(j - left, i - top, color);
            }
        }
    }
    switchScreen();
}

void drawImageFiltered(Image image, int (*localFilter)(unsigned char *, int *, int), int *args, int argCount) {
    int scnHeight = SCREEN_HEIGHT;
    int scnWidth = SCREEN_WIDTH;

    int top = max(0, (image.height - scnHeight) / 2);
    int left = max(0, (image.width - scnWidth) / 2);
    int bottom = min(image.height, top + scnHeight);
    int right = min(image.width, left + scnWidth);

    for (int i = top; i < bottom; i++) {
        for (int j = left; j < right; j++) {
            if (i < top || i >= bottom || j < left || j >= right) {
                video_pixel(j - left, i - top, 0); // TODO: test to see if it works
            } else {
                unsigned char rgb[3] = {image.data[image.height - i - 1][j][0], image.data[image.height - i - 1][j][1], image.data[image.height - i - 1][j][2]};
                if (localFilter(rgb, args, argCount)) {
                    video_pixel(j - left, i - top, conv24to16bit(rgb[0], rgb[1], rgb[2]));
                } else {
                    video_pixel(j - left, i - top, 0);
                }
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

int main(void) {
    if (!video_open()) return -1;
    if (!SW_open()) return -1;
    if (!KEY_open()) return -1;
    if (!HEX_open()) return -1;
    
    video_clear();
    video_erase();

    // drawBackground();
    // drawBackground();

    // pthread_t vga_thread_id;
    // pthread_create(&vga_thread_id, NULL, vgaThread, NULL);

    int swState = 0;
    int keyState = 0;

    Image barackObama = read_bmp("barack_obama.bmp");
    Image barackObamaFiltered = read_bmp("barack_obama.bmp");
    Image shrug = read_bmp("shrug.bmp");
    Image shrugFiltered = read_bmp("shrug.bmp");

    filterbyHue(barackObamaFiltered, 0, 30);
    filterbyHue(shrugFiltered, 0, 30);

    while (swState != 0b1111) {
        // Get UI inputs
        SW_read(&swState); 
        KEY_read(&keyState); 

        int swValue = binaryToInt(swState);
        printf("swValue: %d\n", swValue);
        HEX_set(swValue); 
        // print switches and keys as binary
        printf("SW: %d%d%d%d%d%d%d%d\n", (swState >> 7) & 1, (swState >> 6) & 1, (swState >> 5) & 1, (swState >> 4) & 1, (swState >> 3) & 1, (swState >> 2) & 1, (swState >> 1) & 1, swState & 1);
        
        clock_t start_time, end_time;
        start_time = clock();
        drawImage(barackObama);
        end_time = clock();
        printf("Time taken: %fms\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);

        sleep(1);

        start_time = clock();
        // drawImage(barackObamaFiltered);
        drawImageFiltered(barackObama, withinHueRange, (int[]){0, 30}, 2);
        end_time = clock();
        printf("Time taken: %fms\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);

        sleep(1);
    }

    video_close();
    SW_close();
    KEY_close();
    HEX_close();

    return 0;
}
