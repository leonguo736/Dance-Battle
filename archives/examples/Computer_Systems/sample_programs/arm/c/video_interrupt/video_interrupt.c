#include "address_map_arm.h"

#define STANDARD_X 320
#define STANDARD_Y 240
#define INTEL_BLUE 0x0071C5

extern int screen_x;
extern int screen_y;
extern int res_offset;
extern int col_offset;
/* these globals are written by interrupt service routines; we have to declare
 * these as volatile to avoid the compiler caching their values in registers */
extern volatile int timeout; // used to synchronize with the timer

/* function prototypes */
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_MPcore_private_timer(void);
void enable_A9_interrupts(void);
void video_text(int, int, char *);
void video_box(int, int, int, int, short);
int  resample_rgb(int, int);
int  get_data_bits(int);

/********************************************************************************
 * This program performs the following:
 * 	   Draws a blue box on the video display, and places a text string inside
 * 	   the box. Also, moves the word INTEL around the display, "bouncing" off
 * 	   the blue box and screen edges
********************************************************************************/
int main(void) {
    volatile int * video_resolution = (int *)(PIXEL_BUF_CTRL_BASE + 0x8);
    volatile int * rgb_status       = (int *)(RGB_RESAMPLER_BASE);
    /* initialize some variables */
    timeout = 0; // synchronize with the timer

    int   INT_x1;
    int   INT_x2;
    int   INT_y;
    int   INT_inc_x;
    int   INT_inc_y;
    int   blue_x1;
    int   blue_y1;
    int   blue_x2;
    int   blue_y2;
    int   char_buffer_x;
    int   char_buffer_y;
    short color;

    set_A9_IRQ_stack();            // initialize the stack pointer for IRQ mode
    config_GIC();                  // configure the general interrupt controller
    config_MPcore_private_timer(); // configure ARM A9 private timer

    enable_A9_interrupts(); // enable interrupts

    /* create a message to be displayed on the video display */
    char text_top_video[20]    = "Intel FPGA\0";
    char text_bottom_video[20] = "Computer Systems\0";
    char text_INTEL[10]        = "INTEL\0";
    char text_erase[10]        = "      \0";

    /* the following variables give the size of the pixel buffer */
    screen_x = *(video_resolution)&0xFFFF;
    screen_y = (*(video_resolution) >> 16) & 0xFFFF;
    int db   = get_data_bits(*rgb_status & 0x3F);

    /* check if resolution is smaller than the standard 320 x 240 */
    res_offset = (screen_x == 160) ? 1 : 0;

    /* check if number of color bits is less than the standard 16-bits */
    col_offset = (db == 8) ? 1 : 0;

    color = 0;
    video_box(0, 0, screen_x, screen_y, color); // fill the screen with grey
    // draw an Intel box around the above text, based on the character
    // buffer coordinates
    blue_x1 = 31;
    blue_x2 = 49;
    blue_y1 = 28;
    blue_y2 = 32;
    // character coords * 4 since characters are 4 x 4 pixel buffer coords (8 x
    // 8 video coords)
    color = resample_rgb(db, INTEL_BLUE);
    video_box(blue_x1 * 4, blue_y1 * 4, blue_x2 * 4 - 1, blue_y2 * 4 - 1,
              color);
    /* output text message in the middle of the video monitor */
    video_text(blue_x1 + 4, blue_y1 + 1, text_top_video);
    video_text(blue_x1 + 1, blue_y1 + 2, text_bottom_video);

    char_buffer_x = 79;
    char_buffer_y = 59;
    INT_x1        = 0;
    INT_x2        = 5 /* INTEL = 6 chars */;
    INT_y         = 0;
    INT_inc_x     = 1;
    INT_inc_y     = 1;
    video_text(INT_x1, INT_y, text_INTEL);

    while (1) {
        while (!timeout)
            ; // wait to synchronize with timer

        /* move the INTEL text around on the video screen */
        video_text(INT_x1, INT_y, text_erase); // erase
        INT_x1 += INT_inc_x;
        INT_x2 += INT_inc_x;
        INT_y += INT_inc_y;

        if ((INT_y == char_buffer_y) || (INT_y == 0))
            INT_inc_y = -(INT_inc_y);
        if ((INT_x2 == char_buffer_x) || (INT_x1 == 0))
            INT_inc_x = -(INT_inc_x);

        if ((INT_y >= blue_y1 - 1) && (INT_y <= blue_y2 + 1)) {
            if (((INT_x1 >= blue_x1 - 1) && (INT_x1 <= blue_x2 + 1)) ||
                ((INT_x2 >= blue_x1 - 1) && (INT_x2 <= blue_x2 + 1))) {
                if ((INT_y == (blue_y1 - 1)) || (INT_y == (blue_y2 + 1)))
                    INT_inc_y = -(INT_inc_y);
                else
                    INT_inc_x = -(INT_inc_x);
            }
        }
        video_text(INT_x1, INT_y, text_INTEL);

        timeout = 0;
    }
}

/* setup private timer in the ARM A9 */
void config_MPcore_private_timer() {
    volatile int * MPcore_private_timer_ptr =
        (int *)MPCORE_PRIV_TIMER; // timer base address

    /* set the timer period */
    int counter = 20000000; // period = 1/(200 MHz) x 40x10^6 = 0.2 sec
    *(MPcore_private_timer_ptr) = counter; // write to timer load register

    /* write to control register to start timer, with interrupts */
    *(MPcore_private_timer_ptr + 2) = 0x7; // int mask = 1, mode = 1, enable = 1
}

/****************************************************************************************
 * Subroutine to send a string of text to the video monitor
****************************************************************************************/
void video_text(int x, int y, char * text_ptr) {
    int             offset;
    volatile char * character_buffer =
        (char *)FPGA_CHAR_BASE; // video character buffer

    /* assume that the text string fits on one line */
    offset = (y << 7) + x;
    while (*(text_ptr)) {
        *(character_buffer + offset) =
            *(text_ptr); // write to the character buffer
        ++text_ptr;
        ++offset;
    }
}

/*******************************************************************************
 * Draw a filled rectangle on the video monitor
 ******************************************************************************/
void video_box(int x1, int y1, int x2, int y2, short pixel_color) {
    int pixel_buf_ptr = *(int *)PIXEL_BUF_CTRL_BASE;
    int pixel_ptr, row, col;
    int x_factor = 0x1 << (res_offset + col_offset);
    int y_factor = 0x1 << (res_offset);
    x1           = x1 / x_factor;
    x2           = x2 / x_factor;
    y1           = y1 / y_factor;
    y2           = y2 / y_factor;

    /* assume that the box coordinates are valid */
    for (row = y1; row <= y2; row++)
        for (col = x1; col <= x2; ++col) {
            pixel_ptr = pixel_buf_ptr +
                        (row << (10 - res_offset - col_offset)) + (col << 1);
            *(short *)pixel_ptr = pixel_color; // set pixel color
        }
}
