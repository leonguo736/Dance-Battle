#include "display.h"

const int SCREEN_WIDTH = 320; 
const int SCREEN_HEIGHT = 240;

int screenBuffNum = 0; 

void switchScreen(void) {
    video_show();
    screenBuffNum = (screenBuffNum + 1) % 2;
}
