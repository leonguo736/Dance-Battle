#include "altera_up_avalon_video_dma_controller.h"
#include "sys/alt_stdio.h"

void draw_big_A(alt_up_video_dma_dev *);

/*******************************************************************************
 * This program demonstrates use of the character and pixel buffer HAL code.
 *
 * It performs the following:
 *	1. places a blue box on the LCD, and places a text string inside the box.
 *	2. draws a big A on the screen
 *	3. "bounces" a collapsible box around the screen
*******************************************************************************/
int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/

    alt_up_video_dma_dev * pixel_buffer_dev;
    alt_up_video_dma_dev * char_dma_dev;

    /* used for drawing coordinates */
    int x1, y1, x2, y2, deltax_1, deltax_2, deltay_1, deltay_2, delay = 0;

    /* create a message to be displayed on the LCD */
    char text_top_row[40]    = "Intel FPGA\0";
    char text_bottom_row[40] = "Computer Systems\0";

    /* initialize the pixel buffer HAL */
    pixel_buffer_dev = alt_up_video_dma_open_dev("/dev/LCD_Subsystem_LCD_Pixel_DMA");
    if (pixel_buffer_dev == NULL)
        alt_printf("Error: could not open LCD pixel buffer device\n");
    else
        alt_printf("Opened character LCD pixel buffer device\n");

    /* clear the graphics screen */
    alt_up_video_dma_screen_clear(pixel_buffer_dev, 0);

    /* output text message in the middle of the LCD */
    char_dma_dev = alt_up_video_dma_open_dev(
        "/dev/LCD_Subsystem_LCD_Char_Buf_Subsystem_Char_Buf_DMA");
    if (char_dma_dev == NULL) {
        alt_printf(
            "Error: could not open LCD character buffer's DMA controller device\n");
        return -1;
    } else
        alt_printf("Opened LCD character buffer's DMA controller device\n");
    /* Make sure the front and back buffers point to base address of the onchip
     * memory */
    alt_up_video_dma_ctrl_set_bb_addr(
        char_dma_dev, LCD_SUBSYSTEM_LCD_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);
    alt_up_video_dma_ctrl_swap_buffers(char_dma_dev);
    while (alt_up_video_dma_ctrl_check_swap_status(char_dma_dev))
        ;
    alt_up_video_dma_ctrl_set_bb_addr(
        char_dma_dev, LCD_SUBSYSTEM_LCD_CHAR_BUF_SUBSYSTEM_ONCHIP_SRAM_BASE);

    alt_up_video_dma_screen_clear(char_dma_dev, 0);

    int char_mid_x = char_dma_dev->x_resolution / 2;
    int char_mid_y = char_dma_dev->y_resolution / 2;
    alt_up_video_dma_draw_string(char_dma_dev, text_top_row, char_mid_x - 5,
                                 char_mid_y - 1, 0);
    alt_up_video_dma_draw_string(char_dma_dev, text_bottom_row, char_mid_x - 8,
                                 char_mid_y, 0);

    /* now draw a background box for the text */
    alt_up_video_dma_draw_box(
        pixel_buffer_dev, 0x187F, (char_mid_x - 9) * 4, (char_mid_y - 2) * 4,
        (char_mid_x + 9) * 4 - 1, (char_mid_y + 2) * 4 - 1, 0, 1);

    /* now draw a big A */
    draw_big_A(pixel_buffer_dev);

    /* now draw a red rectangle with diagonal green lines */
    x1 = 20;
    y1 = 20;
    x2 = 50;
    y2 = 50;
    alt_up_video_dma_draw_box(pixel_buffer_dev, 0xF800, x1, y1, x2, y2, 0, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0x07e0, x1, y1, x2, y2, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0x07e0, x1, y2, x2, y1, 0);
    alt_up_video_dma_ctrl_swap_buffers(pixel_buffer_dev);

    /* set the direction in which the box will move */
    deltax_1 = deltax_2 = deltay_1 = deltay_2 = 1;

    while (1) {
        if (alt_up_video_dma_ctrl_check_swap_status(pixel_buffer_dev) == 0) {
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
                alt_up_video_dma_draw_box(pixel_buffer_dev, 0, x1, y1, x2, y2, 0, 0);
                alt_up_video_dma_draw_line(pixel_buffer_dev, 0, x1, y1, x2, y2, 0);
                alt_up_video_dma_draw_line(pixel_buffer_dev, 0, x1, y2, x2, y1, 0);

                // move the rectangle
                x1 = x1 + deltax_1;
                x2 = x2 + deltax_2;
                y1 = y1 + deltay_1;
                y2 = y2 + deltay_2;
                if ((deltax_1 > 0) &&
                    (x1 >= pixel_buffer_dev->x_resolution - 1)) {
                    x1 = pixel_buffer_dev->x_resolution - 1;
                    deltax_1 = -deltax_1;
                } else if ((deltax_1 < 0) && (x1 <= 0)) {
                    x1       = 0;
                    deltax_1 = -deltax_1;
                }
                if ((deltax_2 > 0) &&
                    (x2 >= pixel_buffer_dev->x_resolution - 1)) {
                    x2 = pixel_buffer_dev->x_resolution - 1;
                    deltax_2 = -deltax_2;
                } else if ((deltax_2 < 0) && (x2 <= 0)) {
                    x2       = 0;
                    deltax_2 = -deltax_2;
                }
                if ((deltay_1 > 0) &&
                    (y1 >= pixel_buffer_dev->y_resolution - 1)) {
                    y1 = pixel_buffer_dev->y_resolution - 1;
                    deltay_1 = -deltay_1;
                } else if ((deltay_1 < 0) && (y1 <= 0)) {
                    y1       = 0;
                    deltay_1 = -deltay_1;
                }
                if ((deltay_2 > 0) &&
                    (y2 >= pixel_buffer_dev->y_resolution - 1)) {
                    y2 = pixel_buffer_dev->y_resolution - 1;
                    deltay_2 = -deltay_2;
                } else if ((deltay_2 < 0) && (y2 <= 0)) {
                    y2       = 0;
                    deltay_2 = -deltay_2;
                }

                // redraw Rectangle with diagonal lines
                alt_up_video_dma_draw_box(pixel_buffer_dev, 0xF800, x1, y1, x2, y2, 0, 0);
                alt_up_video_dma_draw_line(pixel_buffer_dev, 0x07e0, x1, y1, x2, y2, 0);
                alt_up_video_dma_draw_line(pixel_buffer_dev, 0x07e0, x1, y2, x2, y1, 0);

                // redraw the box in the foreground
                alt_up_video_dma_draw_box(
                    pixel_buffer_dev, 0x187F, (char_mid_x - 9) * 4,
                    (char_mid_y - 2) * 4, (char_mid_x + 9) * 4 - 1,
                    (char_mid_y + 2) * 4 - 1, 0, 1);

                draw_big_A(pixel_buffer_dev);
            }

            /* Execute a swap buffer command. This will allow us to check if the
             * screen has
             * been redrawn before generating a new animation frame. */
            alt_up_video_dma_ctrl_swap_buffers(pixel_buffer_dev);
        }
    }
}

/* draws a big letter A on the screen */
void draw_big_A(alt_up_video_dma_dev * pixel_buffer_dev) {
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 10, 88, 44, 10, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 44, 10, 72, 10, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 72, 10, 106, 88, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 106, 88, 81, 88, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 81, 88, 75, 77, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 75, 77, 41, 77, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 41, 77, 35, 88, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 35, 88, 10, 88, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 47, 60, 58, 32, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 58, 32, 69, 60, 0);
    alt_up_video_dma_draw_line(pixel_buffer_dev, 0xffff, 69, 60, 47, 60, 0);
}
