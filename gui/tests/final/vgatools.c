#include <stdio.h>
#include <string.h>
#include <math.h>
#include <intelfpgaup/video.h>

#include "vgatools.h"

struct ScreenPose convertPose(struct Pose pose, double spb, int so) {
    struct ScreenPose sp = { (int)(pose.beat * spb + so), -1, POSE_HOUR_LENGTH * cos(pose.hourAngle), POSE_HOUR_LENGTH * sin(pose.hourAngle), POSE_MINUTE_LENGTH * cos(pose.minuteAngle), POSE_MINUTE_LENGTH * sin(pose.minuteAngle) };

    return sp;
}

void drawBackground(void) {
    video_box(0, 0, WIDTH - 1, BORDER, COLOR_BORDER);
    video_box(0, HEIGHT - 1 - BORDER, WIDTH - 1, HEIGHT - 1, COLOR_BORDER);
    video_box(0, BORDER, BORDER, HEIGHT - 1 - BORDER, COLOR_BORDER);
    video_box(WIDTH - 1 - BORDER, BORDER, WIDTH - 1, HEIGHT - 1 - BORDER, COLOR_BORDER);
    video_box(BORDER, BORDER, WIDTH - 1 - BORDER, HEIGHT - 1 - BORDER, COLOR_BG);
}

void drawChar(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], int c, int tlx, int tly, short color, int scale) {
    for (int x = 0; x < FONT_WIDTH; x++) {
        for (int y = 0; y < FONT_HEIGHT; y++) {
            if (c == 100) {
                printf("Value of d at [%d][%d]: %d\n", x, y, font[c][x][y]);
            }
            if (font[c][x][y]) {
                for (int x2 = 0; x2 < scale; x2++) {
                    for (int y2 = 0; y2 < scale; y2++) {
                        video_pixel(tlx + x * scale + x2, tly + y * scale + y2, color);
                    }
                }
            }
        }
    }
}

void drawString(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], char s[], int tlx, int tly, short color, int scale, int kerning) {
    int offset = 0;
    for (int c = 0; c < strlen(s); c++) {
        drawChar(font, s[c], tlx + offset, tly, color, scale);
        offset += FONT_WIDTH * scale + kerning;
    }
}

void drawVLine(int x, short color) {
    video_line(x, 0, x, HEIGHT, color);
}

void drawHLine(int y, short color) {
    video_line(0, y, WIDTH, y, color);
}

void drawPBarOutline(void) {
    int * r = GAME_PBAR_RECT;
    video_line(r[0], r[1], r[0] + r[2], r[1], COLOR_GAME_PBAR_BORDER);
    video_line(r[0], r[1] + r[3], r[0] + r[2], r[1] + r[3], COLOR_GAME_PBAR_BORDER);
    video_line(r[0], r[1], r[0], r[1] + r[3], COLOR_GAME_PBAR_BORDER);
    video_line(r[0] + r[2], r[1], r[0] + r[2], r[1] + r[3], COLOR_GAME_PBAR_BORDER);
}

void drawPBarFill(int prevWidth, int currWidth) {
    int * r = GAME_PBAR_RECT;
    video_box(r[0] + prevWidth + 1, r[1] + 1, r[0] + currWidth, r[1] + r[3] - 1, COLOR_GAME_PBAR_FILL);
}

void drawPose(struct ScreenPose sp, int erase) {
    video_line(sp.x, GAME_POSE_Y, sp.x + sp.hx, GAME_POSE_Y + sp.hy, erase ? COLOR_BG : COLOR_POSE_HHAND);
    video_line(sp.x, GAME_POSE_Y, sp.x + sp.mx, GAME_POSE_Y + sp.my, erase ? COLOR_BG : COLOR_POSE_MHAND);
    video_pixel(sp.x, GAME_POSE_Y, COLOR_POSE_DOT);
    
}