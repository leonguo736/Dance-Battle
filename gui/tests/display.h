#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <intelfpgaup/video.h>

#include "song.h"

// Dimensions

#define WIDTH 320
#define HEIGHT 240

#define BORDER 5

// Colors

#define COLOR_BG 0x2804
#define COLOR_BORDER 0x1802

#define COLOR_HLINE 0xffd8
#define COLOR_VLINE 0x50a7
#define COLOR_DOT 0xefa9
#define COLOR_HHAND 0xee3
#define COLOR_MHAND 0xb3b

#define COLOR_PROGRESSBORDER 0xb194
#define COLOR_PROGRESSFILL 0xfab4

// Draws the vertical line
void drawVLine(void) {
    video_line(50, BORDER, 50, HEIGHT - BORDER, COLOR_VLINE);
}

// Draws the horizontal lines
void drawHLines(void) {
    video_line(BORDER, 50, WIDTH - BORDER, 50, COLOR_HLINE);
    video_line(BORDER, HEIGHT - 50, WIDTH - BORDER, HEIGHT - 50, COLOR_HLINE);
}

// Draws the outline of the progress bar
void drawProgressOutline(void) {
    video_line(80, 210, 240, 210, COLOR_PROGRESSBORDER);
    video_line(80, 225, 240, 225, COLOR_PROGRESSBORDER);
    video_line(80, 210, 80, 225, COLOR_PROGRESSBORDER);
    video_line(240, 210, 240, 225, COLOR_PROGRESSBORDER);
}

// Places the background on both buffers
void drawBackground(void) {
    for (int s = 0; s < 2; s++) {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                if (x < BORDER || y < BORDER || x > WIDTH - BORDER || y > HEIGHT - BORDER) {
                    video_pixel(x, y, COLOR_BORDER);
                } else {
                    video_pixel(x, y, COLOR_BG);
                }
            }
        }

        drawVLine();
        drawHLines();
        drawProgressOutline();

        video_show();
    }
}