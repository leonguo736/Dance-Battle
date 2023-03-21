#include "altera_up_avalon_video_dma_controller.h"
#include "altera_up_avalon_video_rgb_resampler.h"
#include "sys/alt_stdio.h"

#define RGB_24BIT_BLACK			0x000000
#define RGB_24BIT_WHITE			0xFFFFFF
#define RGB_24BIT_BLUE			0x0000FF
#define RGB_24BIT_INTEL_BLUE		0x0071C5

/*******************************************************************************
 * This program demonstrates use of the video in the computer system.
 * Draws a blue box on the video display, and places a text string inside the
 * box
 ******************************************************************************/
int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/

    alt_up_video_dma_dev * pixel_dma_dev;
    alt_up_video_dma_dev * char_dma_dev;

    /* create a message to be displayed on the VGA display */
    char text_top_row[40]    = "Intel FPGA\0";
    char text_bottom_row[40] = "Computer Systems\0";

    int color_text_background	= RGB_24BIT_INTEL_BLUE;

    /* initialize the pixel buffer HAL */
    pixel_dma_dev = alt_up_video_dma_open_dev("/dev/VGA_Subsystem_VGA_Pixel_DMA");
    if (pixel_dma_dev == NULL)
        alt_printf("Error: could not open VGA's DMA controller device\n");
    else
        alt_printf("Opened VGA's DMA controller device\n"); 

    /* clear the graphics screen */
    alt_up_video_dma_screen_clear(pixel_dma_dev, 0);

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

    /* update colors */
    if (pixel_dma_dev->data_width == 1) 
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_text_background);
    else if (pixel_dma_dev->data_width == 2) 
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_text_background);

    alt_up_video_dma_screen_clear(char_dma_dev, 0);

    int char_mid_x = char_dma_dev->x_resolution / 2;
    int char_mid_y = char_dma_dev->y_resolution / 2;
    alt_up_video_dma_draw_string(char_dma_dev, text_top_row, char_mid_x - 5,
                                 char_mid_y - 1, 0);
    alt_up_video_dma_draw_string(char_dma_dev, text_bottom_row, char_mid_x - 8,
                                 char_mid_y, 0);

    /* now draw a background box for the text */
	int x_diff_factor = pixel_dma_dev->x_resolution / char_dma_dev->x_resolution;
	int y_diff_factor = pixel_dma_dev->y_resolution / char_dma_dev->y_resolution;
    alt_up_video_dma_draw_box(
        pixel_dma_dev, color_text_background, 
		(char_mid_x - 9) * x_diff_factor, (char_mid_y - 2) * y_diff_factor,
        (char_mid_x + 9) * x_diff_factor - 1, (char_mid_y + 2) * y_diff_factor - 1,
		0, 1);

	return;
}

