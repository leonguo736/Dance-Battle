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

#define AUDIO_RATE 8000
#define VOLUME 0x7FFFFFF

#define D5SHARP 622.25
#define F5 698.46

double C_SCALE[8] = {261.63,293.66,329.63,349.23,392.00,440.00,493.88,523.25};

#define PLUM 0x3947 // 7,10,7
#define PLUM_DARK 0x18a3 // 3,5,3
#define PLUM_LIGHT 0x59eb // 11,15,11

#define WIDTH 320
#define HEIGHT 240
#define BORDER_PADDING 10

#define SONGLIST_TOP 20
#define SONGLIST_LEFT 20
#define SONGLIST_WIDTH 50
#define SONGLIST_HEIGHT 25
#define SONGLIST_ELEMENTS 4

#define SONGLIST_COLOR1 0xc638
#define SONGLIST_COLOR2 0xffff
#define SONGLIST_FONTCOLOR 0x18e2

int screenBuffNum = 0;

double angle = 0;
double lastAngles[2] = {0, 0};
double angleVelocity = 0;

char * songNames[8] = {"Song A", "Song B", "Song C", "Song D", "Song E", "Song F", "Never Gonna Give You Up"};
short songColors[8] = {video_RED, video_ORANGE, video_YELLOW, video_GREEN, video_BLUE, video_MAGENTA, video_PINK, video_GREY};
int songSelection = 0;
int songTop = 0;

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

void drawBackground(void) {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            video_pixel(x, y, PLUM);
        }
    }
    switchScreen();
}

void drawHand() {
    video_line(200, 120, 200 + 80 * cos(lastAngles[screenBuffNum]), 120 + 80 * sin(lastAngles[screenBuffNum]), PLUM);
    sketchCircle(200 + 80 * cos(lastAngles[screenBuffNum]), 120 + 80 * sin(lastAngles[screenBuffNum]), 5, PLUM);
    video_line(200, 120, 200 + 80 * cos(angle), 120 + 80 * sin(angle), video_BLUE);
    sketchCircle(200 + 80 * cos(angle), 120 + 80 * sin(angle), 5, video_BLUE);
    lastAngles[screenBuffNum] = angle;
    switchScreen();
}

void drawSongList() {
    for (int i = 0; i < SONGLIST_ELEMENTS; i++) {
        short color = i + songTop == songSelection ? SONGLIST_COLOR2 : SONGLIST_COLOR1;
        video_box(SONGLIST_LEFT, SONGLIST_TOP + SONGLIST_HEIGHT * i, SONGLIST_LEFT + SONGLIST_WIDTH, SONGLIST_TOP + SONGLIST_HEIGHT * (i + 1), color);
        video_box(SONGLIST_LEFT + 5, SONGLIST_TOP + SONGLIST_HEIGHT * i + 5, SONGLIST_LEFT + SONGLIST_WIDTH - 5, SONGLIST_TOP + SONGLIST_HEIGHT * (i + 1) - 5, songColors[i + songTop]);
        //video_text(SONGLIST_LEFT, SONGLIST_TOP + SONGLIST_HEIGHT * i, songNames[i + songTop]);
    }
    switchScreen();
}

void writeAudio(int sample) {
    audio_wait_write();
    audio_write_left(sample);
    audio_write_right(sample);
}

void playTone(double frequency, double duration) {
    int sampleCount = (int)(duration * AUDIO_RATE);
    int sampleArray[sampleCount];
    for (int s = 0; s < sampleCount; s++) {
        sampleArray[s] = (int)(VOLUME * sin(s * M_PI * frequency / AUDIO_RATE));
    }
    for (int s = 0; s < sampleCount; s++) {
        writeAudio(sampleArray[s]);
    }
}

void * vgaThread(void *vargp) {
    while (1) {
        angle += angleVelocity;
        drawHand();
        //drawSongList();
        sleep(1 / 60.0);
    }
}

void * audioThread(void *vargp) {
    for (int tone = 0; tone < 8; tone++) {
        playTone(C_SCALE[tone], 0.5);
    }
    while (1);
}

void read_bmp(const char* filename, unsigned char**** image, int* height, int* width) {
    FILE* file = fopen(filename, "rb");
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file);

    *width = *(int*)&header[18];
    *height = *(int*)&header[22];
    int bit_depth = *(int*)&header[28];

    printf("Width: %d, Height: %d, Bit Depth: %d\n", *width, *height, bit_depth);

    int row_size = ((bit_depth * *width + 31) / 32) * 4;
    int data_size = row_size * *height;

    unsigned char* data = (unsigned char*)malloc(data_size * sizeof(unsigned char));
    fread(data, sizeof(unsigned char), data_size, file);

    *image = (unsigned char***)malloc(*height * sizeof(unsigned char**));
    for (int i = 0; i < *height; i++) {
        (*image)[i] = (unsigned char**)malloc(*width * sizeof(unsigned char*));
        for (int j = 0; j < *width; j++) {
            (*image)[i][j] = (unsigned char*)malloc(3 * sizeof(unsigned char));
            (*image)[i][j][0] = data[i * row_size + j * 3 + 2];
            (*image)[i][j][1] = data[i * row_size + j * 3 + 1];
            (*image)[i][j][2] = data[i * row_size + j * 3 + 0];
        }
    }

    fclose(file);
    free(data);
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

void filterByHue(unsigned char *rgb, int hsv_low, int hsv_high) {
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

    if (hsv_low <= h && h <= hsv_high) {
        rgb[0] = r;
        rgb[1] = g;
        rgb[2] = b;
    } else {
        rgb[0] = 0;
        rgb[1] = 0;
        rgb[2] = 0;
    }
}

void drawImage(char* filename) {
    int scnHeight = (int)HEIGHT;
    int scnWidth = (int)WIDTH;

    unsigned char*** image;
    int imgHeight, width;
    read_bmp(filename, &image, &imgHeight, &width);
    if (imgHeight != (int)HEIGHT || width != (int)WIDTH) {
        printf("Image size does not match screen size, printing middle of image and padding with black\n");
    }

    int top = max(0, (imgHeight - scnHeight) / 2);
    int left = max(0, (width - scnWidth) / 2);
    int bottom = min(imgHeight, top + scnHeight);
    int right = min(width, left + scnWidth);
    
    for (int i = top; i < bottom; i++) {
        for (int j = left; j < right; j++) {
            if (i < top || i >= bottom || j < left || j >= right) {
                video_pixel(j - left, i - top, 0); // TODO: test to see if it works
            } else {
                unsigned char *rgb = image[imgHeight - i - 1][j];
                filterByHue(rgb, 0, 30); 
                uint16_t color = conv24to16bit(rgb[0], rgb[1], rgb[2]);
                video_pixel(j - left, i - top, color);
            }
        }
    }
    switchScreen();
}

unsigned char*** loadImage(char* filename) {
    unsigned char*** image;
    int imgHeight, width;
    read_bmp(filename, &image, &imgHeight, &width);
    return image;
}

int main(void) {
    if (!video_open()) return -1;
    if (!SW_open()) return -1;
    if (!KEY_open()) return -1;
    
    video_clear();
    video_erase();
    
    // read ./barack_obama.bmp

    // drawBackground();
    // drawBackground();

    // pthread_t vga_thread_id;
    // pthread_create(&vga_thread_id, NULL, vgaThread, NULL);

    int swState = 0;
    int keyState = 0;

    while (swState != 0b1111) {
        // Get UI inputs
        SW_read(&swState);
        KEY_read(&keyState);
        
        clock_t start_time, end_time;
        start_time = clock();
        drawImage("shrug.bmp");
        end_time = clock();
        printf("Time taken: %fms\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);

        sleep(5);

        start_time = clock();
        drawImage("barack_obama.bmp");
        end_time = clock();
        printf("Time taken: %fms\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);

        sleep(5);
    }

    video_close();
    SW_close();
    KEY_close();

    return 0;
}
