#include "address_map_nios2.h"

/* function prototypes */
void video_text(int, int, char *);
void video_box(int, int, int, int, short);
int  resample_rgb(int, int);
int  get_data_bits(int);

#define INTEL_BLUE 0x0071C5
/* global variables */
int screen_x;
int screen_y;

/*******************************************************************************
 * This program demonstrates use of the video in the computer system.
 * Draws a blue box on the video display, and places a text string inside the
 *box
 ******************************************************************************/
int main(void) {
    volatile int * video_resolution = (int *)(LCD_PIXEL_BUF_CTRL_BASE + 0x8);
    screen_x                        = *video_resolution & 0xFFFF;
    screen_y                        = (*video_resolution >> 16) & 0xFFFF;
    
    volatile int * rgb_status = (int *)(RGB_RESAMPLER_BASE);
    int            db         = get_data_bits(*rgb_status & 0x3F);

    /* create a message to be displayed on the video and LCD displays */
    char text_top_row[40]    = "Intel FPGA\0";
    char text_bottom_row[40] = "Computer Systems\0";
    
    /* update color */
    short background_color = resample_rgb(db, INTEL_BLUE);

    video_text(44, 29, text_top_row);
    video_text(41, 30, text_bottom_row);
    video_box(0, 0, screen_x, screen_y, 0); // clear the screen
    video_box(40 * 4, 28 * 4, 58 * 4 - 1, 32 * 4 - 1, background_color);
}

/*******************************************************************************
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

/********************************************************************************
 * Resamples 24-bit color to 16-bit or 8-bit color
 *******************************************************************************/
int resample_rgb(int num_bits, int color) {
    if (num_bits == 8) {
        color = (((color >> 16) & 0x000000E0) | ((color >> 11) & 0x0000001C) |
                 ((color >> 6) & 0x00000003));
        color = (color << 8) | color;
    } else if (num_bits == 16) {
        color = (((color >> 8) & 0x0000F800) | ((color >> 5) & 0x000007E0) |
                 ((color >> 3) & 0x0000001F));
    }
    return color;
}

/********************************************************************************
 * Finds the number of data bits from the mode
 *******************************************************************************/
int get_data_bits(int mode) {
    switch (mode) {
    case 0x0:
        return 1;
    case 0x7:
        return 8;
    case 0x11:
        return 8;
    case 0x12:
        return 9;
    case 0x14:
        return 16;
    case 0x17:
        return 24;
    case 0x19:
        return 30;
    case 0x31:
        return 8;
    case 0x32:
        return 12;
    case 0x33:
        return 16;
    case 0x37:
        return 32;
    case 0x39:
        return 40;
    }
}
