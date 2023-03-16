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

    // init
    setup_camera(); 

    int swState = 0;
    int keyState = 0; 
    
    while (!(keyState >> 3) & 1) { // while KEY3 is not pressed
        SW_read(&swState); 
        KEY_read(&keyState); 
        
        // camera
        if ((swState >> STATE_CAMERA_SW) & 1) {
            updateCameraStates(swState, keyState);
            drawFilteredImage(); 
        }
    }

    video_close();
    SW_close();
    KEY_close();
    HEX_close();

    return 0;
}
