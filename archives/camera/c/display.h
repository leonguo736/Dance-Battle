#ifndef DISPLAY_H
#define DISPLAY_H

#include <intelfpgaup/video.h>

extern const int SCREEN_WIDTH; 
extern const int SCREEN_HEIGHT; 
extern int screenBuffNum; // 0 or 1

void switchScreen(void); 

#endif
