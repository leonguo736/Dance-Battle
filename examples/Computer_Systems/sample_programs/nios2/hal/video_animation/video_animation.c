#include "altera_up_avalon_video_dma_controller.h"
#include "altera_up_avalon_video_rgb_resampler.h"
#include "sys/alt_stdio.h"

#define RGB_24BIT_BLACK			0x000000
#define RGB_24BIT_WHITE			0xFFFFFF
#define RGB_24BIT_RED			0xFF0000
#define RGB_24BIT_GREEN			0x00FF00
#define RGB_24BIT_BLUE			0x0000FF
#define RGB_24BIT_INTEL_BLUE	0x0071C5

void draw_big_A(alt_up_video_dma_dev *, int color);

/*******************************************************************************
 * This program demonstrates animation with the video out port HAL code.
 *
 * It performs the following:
 *	1. places a blue box on the VGA display, and places a text string inside
 *	   the box.
 *	2. draws a big A on the screen
 *	3. "bounces" a collapsible box around the screen
*******************************************************************************/
int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/

    alt_up_video_dma_dev * pixel_dma_dev;
    alt_up_video_dma_dev * char_dma_dev;

    /* used for drawing coordinates */
    int x1, y1, x2, y2, deltax_1, deltax_2, deltay_1, deltay_2, delay = 0;

    /* create a message to be displayed on the VGA display */
    char text_top_row[40]    = "Intel FPGA\0";
    char text_bottom_row[40] = "Computer Systems\0";

    /* colors for objects */
    int color_large_A			= RGB_24BIT_WHITE;
    int color_text_background	= RGB_24BIT_INTEL_BLUE;
    int color_red_box			= RGB_24BIT_RED;
    int color_green_x			= RGB_24BIT_GREEN;

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
    if (pixel_dma_dev->data_width == 1) {
        color_large_A			= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_large_A);
        alt_printf("%d\n", color_large_A);
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_text_background);
        color_red_box			= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_red_box);
        color_green_x			= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_8BIT(color_green_x);
    } else if (pixel_dma_dev->data_width == 2) {
        color_large_A			= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_large_A);
        color_text_background	= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_text_background);
        color_red_box			= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_red_box);
        color_green_x			= ALT_UP_VIDEO_RESAMPLE_RGB_24BIT_TO_16BIT(color_green_x);
    }    
    
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

    /* now draw a big A */
    draw_big_A(pixel_dma_dev, color_large_A);

	// Calculate multiplication factor
	int mx = pixel_dma_dev->x_resolution / 160;
	int my = pixel_dma_dev->y_resolution / 120;

    /* now draw a red rectangle with diagonal green lines */
    x1 = 10 * mx;
    y1 = 10 * my;
    x2 = 25 * mx;
    y2 = 25 * my;
    alt_up_video_dma_draw_box(pixel_dma_dev, color_red_box, x1, y1, x2, y2, 0, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color_green_x, x1, y1, x2, y2, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color_green_x, x1, y2, x2, y1, 0);
    alt_up_video_dma_ctrl_swap_buffers(pixel_dma_dev);

    /* set the direction in which the box will move */
    deltax_1 = deltax_2 = deltay_1 = deltay_2 = 1;

    while (1) {
        if (alt_up_video_dma_ctrl_check_swap_status(pixel_dma_dev) == 0) {
            /* If the screen has been drawn completely then we can draw a new
             * image. This
             * section of the code will only be entered once every 60th of a
             * second, because
             * this is how long it take the VGA controller to copy the image
             * from memory to
             * the screen. */
            delay = 1 - delay;

            if (delay == 0) {
                /* The delay is inserted to slow down the animation from 60
                 * frames per second
                 * to 30. Every other refresh cycle the code below will execute.
                 * We first erase
                 * the box with Erase Rectangle */
                alt_up_video_dma_draw_box(pixel_dma_dev, 0, x1, y1, x2, y2, 0, 0);
                alt_up_video_dma_draw_line(pixel_dma_dev, 0, x1, y1, x2, y2, 0);
                alt_up_video_dma_draw_line(pixel_dma_dev, 0, x1, y2, x2, y1, 0);

                // move the rectangle
                x1 = x1 + deltax_1;
                x2 = x2 + deltax_2;
                y1 = y1 + deltay_1;
                y2 = y2 + deltay_2;
                if ((deltax_1 > 0) &&
                    (x1 >= pixel_dma_dev->x_resolution - 1)) {
                    x1 = pixel_dma_dev->x_resolution - 1;
                    deltax_1 = -deltax_1;
                } else if ((deltax_1 < 0) && (x1 <= 0)) {
                    x1       = 0;
                    deltax_1 = -deltax_1;
                }
                if ((deltax_2 > 0) &&
                    (x2 >= pixel_dma_dev->x_resolution - 1)) {
                    x2 = pixel_dma_dev->x_resolution - 1;
                    deltax_2 = -deltax_2;
                } else if ((deltax_2 < 0) && (x2 <= 0)) {
                    x2       = 0;
                    deltax_2 = -deltax_2;
                }
                if ((deltay_1 > 0) &&
                    (y1 >= pixel_dma_dev->y_resolution - 1)) {
                    y1 = pixel_dma_dev->y_resolution - 1;
                    deltay_1 = -deltay_1;
                } else if ((deltay_1 < 0) && (y1 <= 0)) {
                    y1       = 0;
                    deltay_1 = -deltay_1;
                }
                if ((deltay_2 > 0) &&
                    (y2 >= pixel_dma_dev->y_resolution - 1)) {
                    y2 = pixel_dma_dev->y_resolution - 1;
                    deltay_2 = -deltay_2;
                } else if ((deltay_2 < 0) && (y2 <= 0)) {
                    y2       = 0;
                    deltay_2 = -deltay_2;
                }

                // redraw Rectangle with diagonal lines
                alt_up_video_dma_draw_box(pixel_dma_dev, color_red_box, x1, y1, x2, y2, 0, 0);
                alt_up_video_dma_draw_line(pixel_dma_dev, color_green_x, x1, y1, x2, y2, 0);
                alt_up_video_dma_draw_line(pixel_dma_dev, color_green_x, x1, y2, x2, y1, 0);

                // redraw the box in the foreground
                alt_up_video_dma_draw_box(
                    pixel_dma_dev, color_text_background,
					(char_mid_x - 9) * x_diff_factor, (char_mid_y - 2) * y_diff_factor,
					(char_mid_x + 9) * x_diff_factor - 1, (char_mid_y + 2) * y_diff_factor - 1,
					0, 1);

                draw_big_A(pixel_dma_dev, color_large_A);
            }

            /* Execute a swap buffer command. This will allow us to check if the
             * screen has
             * been redrawn before generating a new animation frame. */
            alt_up_video_dma_ctrl_swap_buffers(pixel_dma_dev);
        }
    }
}

/* draws a big letter A on the screen */
void draw_big_A(alt_up_video_dma_dev * pixel_dma_dev, int color) {
	// Calculate multiplication factor
	int mx = pixel_dma_dev->x_resolution / 160;
	int my = pixel_dma_dev->y_resolution / 120;
   
   	alt_up_video_dma_draw_line(pixel_dma_dev, color,  5 * mx, 44 * my, 22 * mx,  5 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 22 * mx,  5 * my, 36 * mx,  5 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 36 * mx,  5 * my, 53 * mx, 44 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 53 * mx, 44 * my, 40 * mx, 44 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 40 * mx, 44 * my, 37 * mx, 38 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 37 * mx, 38 * my, 20 * mx, 38 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 20 * mx, 38 * my, 17 * mx, 44 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 17 * mx, 44 * my,  5 * mx, 44 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 23 * mx, 30 * my, 29 * mx, 16 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 29 * mx, 16 * my, 34 * mx, 30 * my, 0);
    alt_up_video_dma_draw_line(pixel_dma_dev, color, 34 * mx, 30 * my, 23 * mx, 30 * my, 0);
}
