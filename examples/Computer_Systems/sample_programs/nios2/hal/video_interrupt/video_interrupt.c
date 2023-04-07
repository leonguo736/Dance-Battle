#include "globals.h"

/* these globals are written by interrupt service routines; we have to declare
 * these as volatile to avoid the compiler caching their values in registers */
extern volatile int      timeout; // used to synchronize with the timer
extern struct alt_up_dev up_dev;  /* pointer to struct that holds pointers to
                                      open devices */

/* function prototypes */
void interval_timer_ISR(void *, unsigned int);

/*******************************************************************************
 * This program performs the following:
 *	1. draws a blue box on the VGA display, and places a text string inside
 *	   the box. Also, moves the word INTEL around the display, "bouncing" off
 *	   the blue box and screen edges.
 *	2. the speed of refreshing the VGA screen is controlled by interrupts from
 *	   the interval timer.
 ******************************************************************************/
int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/

    /* declare device driver pointers for devices */
    alt_up_video_dma_dev * char_dma_dev;
    alt_up_video_dma_dev * pixel_dma_dev;
    /* declare volatile pointer for interval timer, which does not have HAL
     * functions */
    volatile int * interval_timer_ptr =
        (int *)INTERVAL_TIMER_BASE; // interal timer base address

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
    int   color_text_background = RGB_24BIT_INTEL_BLUE;

    /* set the interval timer period */
    int counter = 20000000; // 1/(100 MHz) x (20000000) = 200 msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1

    /* use the HAL facility for registering interrupt service routines. */
    /* Note: we are passsing a pointer to up_dev to each ISR (using the context
     * argument) as
     * a way of giving the ISR a pointer to every open device. This is useful
     * because some of the
     * ISRs need to access more than just one device (e.g. the pushbutton ISR
     * accesses both
     * the pushbutton device and the audio device) */
    alt_irq_register(0, (void *)&up_dev, (void *)interval_timer_ISR);

    /* create a messages to be displayed on the VGA and LCD displays */
    char text_top_VGA[20]    = "Intel FPGA\0";
    char text_bottom_VGA[20] = "Computer Systems\0";
    char text_INTEL[10]      = "INTEL\0";
    char text_erase[10]      = "     \0";

    /* output text message in the middle of the VGA monitor */
    char_dma_dev = alt_up_video_dma_open_dev(
        "/dev/VGA_Subsystem_Char_Buf_Subsystem_Char_Buf_DMA");
    if (char_dma_dev == NULL) {
        alt_printf(
            "Error: could not open character buffer's DMA controller device\n");
        return -1;
    } else
        alt_printf("Opened character buffer's DMA controller device\n");
    /* Make sure the front and back buffers point to base address of the onchip
     * memory */
    alt_up_video_dma_ctrl_set_bb_addr(
        char_dma_dev, VGA_SUBSYSTEM_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);
    alt_up_video_dma_ctrl_swap_buffers(char_dma_dev);
    while (alt_up_video_dma_ctrl_check_swap_status(char_dma_dev))
        ;
    alt_up_video_dma_ctrl_set_bb_addr(
        char_dma_dev, VGA_SUBSYSTEM_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);

    alt_up_video_dma_screen_clear(char_dma_dev, 0);

    int char_mid_x = char_dma_dev->x_resolution / 2;
    int char_mid_y = char_dma_dev->y_resolution / 2;
    alt_up_video_dma_draw_string(char_dma_dev, text_top_VGA, char_mid_x - 5,
                                 char_mid_y - 1, 0);
    alt_up_video_dma_draw_string(char_dma_dev, text_bottom_VGA, char_mid_x - 8,
                                 char_mid_y, 0);

    /* open the pixel buffer */
    pixel_dma_dev =
        alt_up_video_dma_open_dev("/dev/VGA_Subsystem_VGA_Pixel_DMA");
    if (pixel_dma_dev == NULL) {
        alt_printf("Error: could not open VGA's DMA controller device\n");
        return -1;
    } else {
        alt_printf("Opened VGA's DMA controller device\n"); 
        up_dev.pixel_dma_dev = pixel_dma_dev; // store for use by ISRs
    }

    alt_up_video_dma_screen_clear(pixel_dma_dev, 0);
    /* draw a medium-blue box in the middle of the screen, using character
     * buffer coordinates.
     * Use character coord * 4 since characters are 4 x 4 pixel buffer coords (8
     * x 8 VGA coords) */
    blue_x1 = (char_mid_x - 9);
    blue_x2 = (char_mid_x + 9);
    blue_y1 = (char_mid_y - 2);
    blue_y2 = (char_mid_y + 2);

    int x_diff_factor =
        pixel_dma_dev->x_resolution / char_dma_dev->x_resolution;
    int y_diff_factor =
        pixel_dma_dev->y_resolution / char_dma_dev->y_resolution;
    // character coords must multiply by the factors above
    
    /* update colors */
    if (pixel_dma_dev->data_width == 1) 
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_text_background);
    else if (pixel_dma_dev->data_width == 2) 
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_text_background);

    alt_up_video_dma_draw_box(
        pixel_dma_dev, color_text_background, blue_x1 * x_diff_factor, blue_y1 * y_diff_factor,
        blue_x2 * x_diff_factor - 1, blue_y2 * y_diff_factor - 1, 0, 1);

    char_buffer_x = 79;
    char_buffer_y = 59;
    INTEL_x1      = 0;
    INTEL_x2      = 4 /* INTEL = 5 chars */;
    INTEL_y       = 0;
    INTEL_inc_x   = 1;
    INTEL_inc_y   = 1;
    alt_up_video_dma_draw_string(char_dma_dev, text_INTEL, INTEL_x1, INTEL_y,
                                 0);

    /* this loops "bounces" the word INTEL around on the VGA screen */
    while (1) {
        while (!timeout)
            ; // wait to synchronize with timeout, which is set by the interval
              // timer ISR

        /* move the INTEL text around on the VGA screen */
        alt_up_video_dma_draw_string(char_dma_dev, text_erase, INTEL_x1,
                                     INTEL_y, 0); // erase
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
        alt_up_video_dma_draw_string(char_dma_dev, text_INTEL, INTEL_x1,
                                     INTEL_y, 0);

        timeout = 0;
    }
}
