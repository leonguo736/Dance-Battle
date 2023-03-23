#ifndef VGATOOLS_H
#define VGATOOLS_H
#include <stdio.h>

#include "pose.h"
#include "fonts.h"

// Dimensions

#define WIDTH 320
#define HEIGHT 240

#define BORDER 5

#define INIT_NUM_DOTS 7

#define GAME_VLINE_MARGIN 50
#define GAME_HLINE_MARGIN 50
#define GAME_PBAR_RECT {80, 210, 60, 15}
#define GAME_POSE_Y 120

// Colors

#define COLOR_BG 0x2804
#define COLOR_BORDER 0x1802

#define COLOR_INIT_DANCE 0xb254
#define COLOR_INIT_BATTLE 0xc14b
#define COLOR_INIT_HLINE 0xdeab
#define COLOR_INIT_LOADWHEEL 0x4779
#define COLOR_INIT_STATUS 0xfab4

#define COLOR_GAME_HLINE 0xffd8
#define COLOR_GAME_VLINE 0x50a7
#define COLOR_GAME_PBAR_BORDER 0xb194
#define COLOR_GAME_PBAR_FILL 0xfab4

#define COLOR_POSE_DOT 0xefa9
#define COLOR_POSE_HHAND 0xee3
#define COLOR_POSE_MHAND 0xb3b

// Screen Pose

struct ScreenPose {
    int sample;
    int x;
    double hx;
    double hy;
    double mx;
    double my;
};

struct ScreenPose convertPose(struct Pose pose, double spb, int so);

// General Methods

void drawBackground(void);

void drawChar(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], int c, int tlx, int tly, short color, int scale);
void drawString(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], char s[], int tlx, int tly, short color, int scale, int kerning);

void drawVLine(int x, short color);
void drawHLine(int y, short color);

// Init screen methods

void drawLoadWheel(double angle);

// Lobby screen methods



// Game screen methods

void drawPBarOutline(void);
void drawPBarFill(int prevWidth, int currWidth);
void drawPose(struct ScreenPose sp, int erase);

#endif
