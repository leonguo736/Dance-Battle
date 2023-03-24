#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"

#define INTEL_BLUE 0x0071C5
extern int screen_x;
extern int screen_y;
/* these globals are written by interrupt service routines; we have to declare
 * these as volatile to avoid the compiler caching their values in registers */
extern volatile int timeout; // used to synchronize with the timer

/* function prototypes */
void video_text(int, int, char *);
void video_box(int, int, int, int, short);
int  resample_rgb(int, int);
int  get_data_bits(int);

/*******************************************************************************
 * This program performs the following:
 * 1. Draws a blue box on the video display, and places a text string inside
 *    the box. Also, moves the word INTEL around the display, "bouncing"
 *    off the blue box and screen edges
 * 2. the speed of refreshing the video screen is controlled by interrupts from
 *    the interval timer
 ******************************************************************************/
int main(void) {

    /* Declare volatile pointers to I/O registers (volatile means that IO load
       and store instructions will be used to access these pointer locations,
       instead of regular memory loads and stores) */
    volatile int * interval_timer_ptr =
        (int *)TIMER_BASE; // interal timer base address
    volatile int * video_resolution = (int *)(LCD_PIXEL_BUF_CTRL_BASE + 0x8);
    volatile int * char_resolution = (int *)(LCD_CHAR_BUF_CTRL_BASE + 0x8);
    volatile int * rgb_status       = (int *)(LCD_RGB_RESAMPLER_BASE);

    /* initialize some variables */
    timeout = 0; // synchronize with the timer

    int   INTEL_x1;
    int   INTEL_x2;
    int   INTEL_y;
    int   INTEL_inc_x;
    int   INTEL_inc_y;
    int   blue_x1;
    int   blue_y1;
    int   blue_x2;
    int   blue_y2;
    int   char_buffer_x;
    int   char_buffer_y;
    short color;

    /* set the interval timer period for scrolling the HEX displays */
    int counter = 10000000; // 1/(100 MHz) x (10000000) = 100 msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1

    NIOS2_WRITE_IENABLE(
        0x1); /* set interrupt mask bits for level 0 (interval timer) */

    NIOS2_WRITE_STATUS(1); // enable Nios II interrupts

    /* create a messages to be displayed on the video and LCD displays */
    char text_top[20]    = "Intel FPGA\0";
    char text_bottom[20] = "Computer Systems\0";
    char text_INTEL[10]  = "INTEL\0";
    char text_erase[10]  = "     \0";

    /* the following variables give the size of the pixel buffer and number of color bits*/
    screen_x = *(video_resolution)&0xFFFF;
    screen_y = (*(video_resolution) >> 16) & 0xFFFF;
    int db   = get_data_bits(*rgb_status & 0x3F);

    color      = 0;
    video_box(0, 0, screen_x, screen_y,
              color); // erase everything on the screen
    // draw a medium-blue box around the above text, based on the character
    // buffer coordinates
    blue_x1 = 40;
    blue_x2 = 58;
    blue_y1 = 28;
    blue_y2 = 32;
    // character coords * 4 since characters are 4 x 4 pixel buffer coords (8 x
    // 8 video coords)
    color = resample_rgb(db, INTEL_BLUE);
    video_box(blue_x1 * 4, blue_y1 * 4, blue_x2 * 4 - 1, blue_y2 * 4 - 1,
              color);
    /* output text message in the middle of the video monitor */
    /* First clear the character buffer */
    int * p;
    for (p = (int *)FPGA_LCD_CHAR_BASE; p < (int *)FPGA_LCD_CHAR_END; ++p)
        *p = 0;
    video_text(blue_x1 + 4, blue_y1 + 1, text_top);
    video_text(blue_x1 + 1, blue_y1 + 2, text_bottom);

    char_buffer_x = *(char_resolution)&0xFFFF;
    char_buffer_y = (*(char_resolution) >> 16) & 0xFFFF;
    INTEL_x1      = 0;
    INTEL_x2      = 4 /* INTEL = 5 chars */;
    INTEL_y       = 0;
    INTEL_inc_x   = 1;
    INTEL_inc_y   = 1;
    video_text(INTEL_x1, INTEL_y, text_INTEL);

    while (1) {
        while (!timeout)
            ; // wait to synchronize with timer

        /* move the INTEL text around on the video screen */
        video_text(INTEL_x1, INTEL_y, text_erase); // erase
        INTEL_x1 += INTEL_inc_x;
        INTEL_x2 += INTEL_inc_x;
        INTEL_y += INTEL_inc_y;

        if ((INTEL_y == char_buffer_y) || (INTEL_y == 0))
            INTEL_inc_y = -(INTEL_inc_y);
        if ((INTEL_x2 == char_buffer_x) || (INTEL_x1 == 0))
            INTEL_inc_x = -(INTEL_inc_x);

        if ((INTEL_y >= blue_y1 - 1) && (INTEL_y <= blue_y2 + 1)) {
            if (((INTEL_x1 >= blue_x1 - 1) && (INTEL_x1 <= blue_x2 + 1)) ||
                ((INTEL_x2 >= blue_x1 - 1) && (INTEL_x2 <= blue_x2 + 1))) {
                if ((INTEL_y == (blue_y1 - 1)) || (INTEL_y == (blue_y2 + 1)))
                    INTEL_inc_y = -(INTEL_inc_y);
                else
                    INTEL_inc_x = -(INTEL_inc_x);
            }
        }
        video_text(INTEL_x1, INTEL_y, text_INTEL);

        timeout = 0;
    }
}

/********************************************************************************
 * Subroutine to send a string of text to the video monitor
 ******************************************************************************/
void video_text(int x, int y, char * text_ptr) {
    int             offset;
    volatile char * character_buffer =
        (char *)FPGA_LCD_CHAR_BASE; // video character buffer

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
    int pixel_buf_ptr = *(int *)LCD_PIXEL_BUF_CTRL_BASE;
    int pixel_ptr, row, col;

    /* assume that the box coordinates are valid */
    for (row = y1; row <= y2; row++)
        for (col = x1; col <= x2; ++col) {
            pixel_ptr = pixel_buf_ptr + (row << 10) + (col << 1);
            *(short *)pixel_ptr = pixel_color; // set pixel color
        }
}

