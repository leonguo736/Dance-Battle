#ifndef VGATOOLS_H
#define VGATOOLS_H
#include <stdio.h>

#include "pose.h"
#include "fonts.h"

// Dimensions

#define WIDTH 320
#define HEIGHT 240

#define BORDER 5

#define INIT_DANCE_X 50
#define INIT_DANCE_Y 30
#define INIT_DANCE_SCALE 4
#define INIT_DANCE_KERNING 2
#define INIT_BATTLE_X 175.0
#define INIT_BATTLE_Y 70.0
#define INIT_BATTLE_SCALE 2
#define INIT_BATTLE_KERNING 2
#define INIT_STATUS_X 45
#define INIT_STATUS_Y 205
#define INIT_STATUS_SCALE 1
#define INIT_STATUS_KERNING 1
#define INIT_TITLE_LERP 0.25

#define INIT_NAME_Y 140
#define INIT_NAME_AMPLITUDE 25
#define INIT_NAME_FREQUENCY 0.04

#define GAME_VLINE_MARGIN 50
#define GAME_HLINE_MARGIN 50
#define GAME_POSE_Y 120

// Colors

#define COLOR_BG 0x2804
#define COLOR_BORDER 0x8033

// #define COLOR_INIT_DANCE 0xb254
#define COLOR_INIT_DANCE 0xfa3b
// #define COLOR_INIT_BATTLE 0xc14b
#define COLOR_INIT_BATTLE 0xf04c
// #define COLOR_INIT_TUBE 0x1823
#define COLOR_INIT_TUBE 0x3066
// #define COLOR_INIT_HLINE 0xdeab
#define COLOR_INIT_STATUS 0xfab4
#define COLOR_INIT_HLINE 0xaf72

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

void drawPixel(int x, int y, short color);

void drawChar(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], int c, int tlx, int tly, short color, int scale);
void drawString(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], char s[], int tlx, int tly, short color, int scale, int kerning);

void drawVLine(int x, int y1, int y2, short color);
void drawHLine(int y, int x1, int x2, short color);

// Init screen methods

void drawLoadWheel(double angle);

// Lobby screen methods



// Game screen methods

void drawPBarOutline(void);
void drawPBarFill(int prevWidth, int currWidth);
void drawPose(struct ScreenPose sp, int erase);

#endif
