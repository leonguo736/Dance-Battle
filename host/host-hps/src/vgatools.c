#include <stdio.h>
#include <string.h>

#ifdef GCC
#include "video.h"
#else
#include <intelfpgaup/video.h>
#endif

#include "vgatools.h"

int game_pbar_rect[] = {80, 210, 160, 15};

void drawBackground(void) {
    video_box(0, 0, WIDTH - 1, BORDER, COLOR_BORDER);
    video_box(0, HEIGHT - 1 - BORDER, WIDTH - 1, HEIGHT - 1, COLOR_BORDER);
    video_box(0, BORDER, BORDER, HEIGHT - 1 - BORDER, COLOR_BORDER);
    video_box(WIDTH - 1 - BORDER, BORDER, WIDTH - 1, HEIGHT - 1 - BORDER, COLOR_BORDER);
    video_box(BORDER + 1, BORDER + 1, WIDTH - 1 - BORDER, HEIGHT - 1 - BORDER, COLOR_BG);
}

void drawPixel(int x, int y, short color) {
    if (x <= BORDER || x >= WIDTH - BORDER || y <= BORDER || y >= HEIGHT - BORDER) {
        return;
    } else {
        video_pixel(x, y, color);
    }
}

void drawChar(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], int c, int tlx, int tly, short color, int scale) {
    for (int x = 0; x < FONT_WIDTH; x++) {
        for (int y = 0; y < FONT_HEIGHT; y++) {
            if (font[c][x][y]) {
                for (int x2 = 0; x2 < scale; x2++) {
                    for (int y2 = 0; y2 < scale; y2++) {
                        drawPixel(tlx + x * scale + x2, tly + y * scale + y2, color);
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

void drawStringCenter(int font[NUM_CHARS][FONT_WIDTH][FONT_HEIGHT], char s[], int x, int y, short color, int scale, int kerning) {
    int w = strlen(s) * FONT_WIDTH * scale + (strlen(s) - 1) * kerning;
    int h = FONT_HEIGHT * scale;
    drawString(font, s, x - w / 2, y - h / 2, color, scale, kerning);
}

void drawVLine(int x, int y1, int y2, short color) {
    for (int y = y1; y <= y2; y++) {
        drawPixel(x, y, color);
    }
}

void drawHLine(int y, int x1, int x2, short color) {
    for (int x = x1; x <= x2; x++) {
        drawPixel(x, y, color);
    }
}

void drawPBarOutline(void) {
    int r[] = {game_pbar_rect[0], game_pbar_rect[1], game_pbar_rect[2], game_pbar_rect[3]};
    video_line(r[0], r[1], r[0] + r[2], r[1], COLOR_GAME_PBAR_BORDER);
    video_line(r[0], r[1] + r[3], r[0] + r[2], r[1] + r[3], COLOR_GAME_PBAR_BORDER);
    video_line(r[0], r[1], r[0], r[1] + r[3], COLOR_GAME_PBAR_BORDER);
    video_line(r[0] + r[2], r[1], r[0] + r[2], r[1] + r[3], COLOR_GAME_PBAR_BORDER);
}

void drawPBarFill(int prevWidth, int currWidth) {
    int r[] = {game_pbar_rect[0], game_pbar_rect[1], game_pbar_rect[2], game_pbar_rect[3]};
    video_box(r[0] + prevWidth + 1, r[1] + 1, r[0] + currWidth, r[1] + r[3] - 1, COLOR_GAME_PBAR_FILL);
}

void drawGameVLines(void) {
    video_line(GAME_VLINE_MARGIN, BORDER + 1, GAME_VLINE_MARGIN, HEIGHT - BORDER - 1, COLOR_GAME_VLINE);
    video_line(WIDTH - GAME_VLINE_MARGIN, BORDER + 1, WIDTH - GAME_VLINE_MARGIN, HEIGHT - BORDER - 1, COLOR_GAME_VLINE);
}

void drawPose(struct Pose p, int x, int erase) {
    if (p.isDefender) {
        // Vertical lines
        video_line(x, GAME_HLINE_MARGIN + 1, x, GAME_POSE_Y + POSE_LINE_ATTACK_Y1,
                   erase ? COLOR_BG : COLOR_GAME_HLINE);
        video_line(x, HEIGHT - GAME_HLINE_MARGIN - 1, x, GAME_POSE_Y + POSE_LINE_ATTACK_Y2,
                   erase ? COLOR_BG : COLOR_GAME_HLINE);

        // Body
        video_line(x - POSE_SHOULDER_LENGTH, GAME_POSE_Y + POSE_CHEST_Y, x + POSE_SHOULDER_LENGTH, GAME_POSE_Y + POSE_CHEST_Y, erase ? COLOR_BG : COLOR_POSE_BODY);
        video_line(x, GAME_POSE_Y + POSE_CHEST_Y, x, GAME_POSE_Y + POSE_PELVIS_Y, erase ? COLOR_BG : COLOR_POSE_BODY);

        // Head
        drawStringCenter(basicFont, ".", x, GAME_POSE_Y + POSE_HEAD_Y, erase ? COLOR_BG : COLOR_POSE_BODY, 1, 0);

        // Arms
        video_line(x - POSE_SHOULDER_LENGTH, GAME_POSE_Y + POSE_CHEST_Y, x + p.larmx,
                   GAME_POSE_Y + POSE_CHEST_Y + p.larmy,
                   erase ? COLOR_BG : COLOR_POSE_LARM);
        video_line(x + POSE_SHOULDER_LENGTH, GAME_POSE_Y + POSE_CHEST_Y, x + p.rarmx,
                   GAME_POSE_Y + POSE_CHEST_Y + p.rarmy,
                   erase ? COLOR_BG : COLOR_POSE_RARM);

        // Legs
        video_line(x, GAME_POSE_Y + POSE_PELVIS_Y, x + p.llegx,
                   GAME_POSE_Y + POSE_PELVIS_Y + p.llegy,
                   erase ? COLOR_BG : COLOR_POSE_LLEG);
        video_line(x, GAME_POSE_Y + POSE_PELVIS_Y, x + p.rlegx,
                   GAME_POSE_Y + POSE_PELVIS_Y + p.rlegy,
                   erase ? COLOR_BG : COLOR_POSE_RLEG);

        // Shield
        drawStringCenter(basicFont, "(", x + p.rarmx, GAME_POSE_Y + POSE_CHEST_Y + p.rarmy, erase ? COLOR_BG : COLOR_DEFEND_ICON, 1, 0);
    } else {
        // Vertical lines
        video_line(x, GAME_HLINE_MARGIN + 1, x, GAME_POSE_Y + POSE_LINE_ATTACK_Y1,
                   erase ? COLOR_BG : COLOR_GAME_PBAR_FILL);
        video_line(x, HEIGHT - GAME_HLINE_MARGIN - 1, x, GAME_POSE_Y + POSE_LINE_ATTACK_Y2,
                   erase ? COLOR_BG : COLOR_GAME_PBAR_FILL);

        // Icon
        drawStringCenter(basicFont, "*", x, GAME_POSE_Y - 10, erase ? COLOR_BG : COLOR_ATTACK_ICON, 2, 0);
    }
}
