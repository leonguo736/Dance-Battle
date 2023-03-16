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

#include "camera.h"
#include "params.h"

int main(void) {
    if (!video_open()) return -1;
    if (!SW_open()) return -1;
    if (!KEY_open()) return -1;
    if (!HEX_open()) return -1;
    
    video_clear();
    video_erase();

    int swState = 0;
    int keyState = 0;

    int imgCount = 2; 
    Image images[imgCount];
    images[0] = read_bmp("./images/barack_obama.bmp");
    images[1] = read_bmp("./images/shrug.bmp");
    if (images[imgCount - 1].data == NULL) {
        printf("Incorrect imgCount\n");
        return -1;
    }

    int hue_low = 0; 
    int hue_high = 359; 

    int imgIndex = 0; 
    while (!(keyState >> 3) & 1) {
        SW_read(&swState); 
        KEY_read(&keyState); 
        
        if ((swState >> STATE_CAMERA_SW) & 1) {
            int hueValue = binaryToInt((swState & 0b11111) << 4);
            HEX_set(hueValue); 
            if ((keyState >> 2) & 1 ) {
                imgIndex = 1 % imgCount; 
            } else if ((keyState >> 1) & 1) {
                hue_low = hueValue > 359 ? 359 : hueValue;
            } else if ((keyState >> 0) & 1) {
                hue_high = hueValue > 359 ? 359 : hueValue;
            }
            printf("hue_low: %d, hue_high: %d\n", hue_low, hue_high);
            
            drawImageFiltered(images[imgIndex], withinHueRange, (int[]){hue_low, hue_high}, 2);
        }
    }

    video_close();
    SW_close();
    KEY_close();
    HEX_close();

    return 0;
}
