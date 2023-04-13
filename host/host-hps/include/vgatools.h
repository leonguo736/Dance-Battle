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

#define LOBBY_TITLE_X 103
#define LOBBY_TITLE_Y 14
#define LOBBY_TITLE_SCALE 3
#define LOBBY_TITLE_KERNING 2

#define LOBBY_PLAYER_X1 57
#define LOBBY_PLAYER_X2 164
#define LOBBY_PLAYER_Y 57
#define LOBBY_PLAYER_W 100
#define LOBBY_PLAYER_H 36
#define LOBBY_PLAYER_BORDER 4
#define LOBBY_PLAYER_SCALE 1
#define LOBBY_PLAYER_KERNING 1

#define LOBBY_MODE_X 56
#define LOBBY_MODE_Y 107
#define LOBBY_MODE_W 208
#define LOBBY_MODE_H 26
#define LOBBY_MODE_BORDER 4
#define LOBBY_MODE_SCALE 1
#define LOBBY_MODE_KERNING 1
#define LOBBY_MODE_SW_X 280

#define LOBBY_SONG_X 77
#define LOBBY_SONG_Y 149
#define LOBBY_SONG_W 166
#define LOBBY_SONG_H 22
#define LOBBY_SONG_KEY_X 280

#define GAME_VLINE_MARGIN 50
#define GAME_HLINE_MARGIN 50
#define GAME_POSE_Y 120
#define GAME_TOP_X 160
#define GAME_TOP_Y 20
#define GAME_FLASH_DURATION 4
#define GAME_MODEB_TOP_X 105

#define RESULTS_BAR_WIDTH 10
#define RESULTS_GAMEOVER_AMPLITUDE 100
#define RESULTS_GAMEOVER_ENDY 80
#define RESULTS_GAMEOVER_WAVELENGTH 30000
#define RESULTS_CONTINUE_ENDY 210
#define RESULTS_WAVEY 145
#define RESULTS_WAVE_AMPLITUDE 10
#define RESULTS_WAVE_WAVELENGTH_X 50
#define RESULTS_WAVE_WAVELENGTH_TIME 5000

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

#define COLOR_LOBBY_TITLE 0xfa3b
#define COLOR_LOBBY_BG2 0x3066
#define COLOR_LOBBY_PLAYER_BORDER 0x8033
#define COLOR_LOBBY_PLAYER_FILL 0x2804
#define COLOR_LOBBY_PLAYER_MISSING 0xe9e7
#define COLOR_LOBBY_PLAYER_READY 0x5f67
#define COLOR_LOBBY_MODE_BORDER 0x8033
#define COLOR_LOBBY_MODE_FILL1 0x3066
#define COLOR_LOBBY_MODE_FILL2 0x80ee
#define COLOR_LOBBY_MODE_TEXT1 0x7ba9
#define COLOR_LOBBY_MODE_TEXT2 0xde92
#define COLOR_LOBBY_SONG_FILL 0x2044
#define COLOR_LOBBY_SONG_TEXT 0xde92
#define COLOR_LOBBY_SONG_ARROW 0xf72c
#define COLOR_LOBBY_GO_OFF 0x0000
#define COLOR_LOBBY_GO_ON 0x5f67

#define COLOR_GAME_HLINE 0xffd8
#define COLOR_GAME_VLINE 0x50a7
#define COLOR_GAME_PBAR_BORDER 0xb194
#define COLOR_GAME_PBAR_FILL 0xfab4
#define COLOR_GAME_NICE 0x37e9
#define COLOR_GAME_OUCH 0xfa46

#define COLOR_POSE_DOT 0xefa9
#define COLOR_POSE_HHAND 0xee3
#define COLOR_POSE_MHAND 0xb3b

#define COLOR_RESULTS_BG1 0x2804
#define COLOR_RESULTS_BG2 0x3066
#define COLOR_RESULTS_GAMEOVER 0xfa3b
#define COLOR_RESULTS_DOT 0xefa9
#define COLOR_RESULTS_WAVE_WINNER 0x67e6
#define COLOR_RESULTS_CONTINUE 0xfab4
#define COLOR_RESULTS_WAVE_DOT 0xefa9

// General Methods

void drawBorder(short color);
void drawBackground(void);

void drawPixel(int x, int y, short color);
void drawResultsPixel(int x, int y, int offset);

void drawChar(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], int c, int tlx, int tly, short color, int scale);
void drawString(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], char s[], int tlx, int tly, short color, int scale, int kerning);
void drawStringCenter(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], char s[], int x, int y, short color, int scale, int kerning);

void drawVLine(int x, int y1, int y2, short color);
void drawHLine(int y, int x1, int x2, short color);

// Init screen methods

void drawLoadWheel(double angle);

// Lobby screen methods



// Game screen methods

void drawPBarOutline(void);
void drawPBarFill(int prevWidth, int currWidth);

void drawGameVLines(void);
void drawPose(struct Pose p, int x, int erase);

#endif
