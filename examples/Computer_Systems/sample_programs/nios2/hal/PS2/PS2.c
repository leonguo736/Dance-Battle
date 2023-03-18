#include "altera_avalon_pio_regs.h"
#include "altera_up_avalon_ps2.h"
#include "sys/alt_stdio.h"
#include "system.h"

#ifndef HEX5_HEX4_BASE
#define HEX5_HEX4_BASE HEX7_HEX4_BASE
#endif

/* function prototypes */
void HEX_PS2(unsigned char, unsigned char, unsigned char);

/*******************************************************************************
 * This program performs the following:
 *    Displays the last three bytes of data received from the PS/2 port on the
 *    HEX displays.
 ******************************************************************************/

int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/
    alt_up_ps2_dev * PS2_dev;

    /* used for PS/2 port data */
    unsigned char PS2_data;
    unsigned char byte1 = 0, byte2 = 0, byte3 = 0;

    /* open the PS2 port */
    PS2_dev = alt_up_ps2_open_dev("/dev/PS2_Port");
    if (PS2_dev == NULL) {
        alt_printf("Error: could not open PS2 device\n");
        return -1;
    } else
        alt_printf("Opened PS2 device\n");

    (void)alt_up_ps2_write_data_byte(PS2_dev, 0xFF); // reset
    while (1) {
        /* check for PS/2 data--display on HEX displays */
        if (alt_up_ps2_read_data_byte(PS2_dev, &PS2_data) == 0) {
            /* shift the next data byte into the display */
            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data;
            HEX_PS2(byte1, byte2, byte3);

            if ((byte2 == (unsigned char)0xAA) &&
                (byte3 == (unsigned char)0x00))
                // mouse inserted; initialize sending of data
                (void)alt_up_ps2_write_data_byte(PS2_dev, 0xF4);
        }
    }
}

/*******************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
 * Note that we are using pointer accesses for the HEX displays parallel port.
 * We could also use the HAL functions for these ports instead.
 ******************************************************************************/
void HEX_PS2(unsigned char b1, unsigned char b2, unsigned char b3) {

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display in the computer system, for the hex digits 0 - F
     */
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int  shift_buffer, nibble;
    unsigned char code;
    int           i;

    shift_buffer = (b1 << 16) | (b2 << 8) | b3;
    for (i = 0; i < 6; ++i) {
        nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
        code   = seven_seg_decode_table[nibble];
        hex_segs[i]  = code;
        shift_buffer = shift_buffer >> 4;
    }
    /* drive the hex displays */
    IOWR_ALTERA_AVALON_PIO_DATA(HEX3_HEX0_BASE, *(int *)(hex_segs));
    IOWR_ALTERA_AVALON_PIO_DATA(HEX5_HEX4_BASE, *(int *)(hex_segs + 4));
}
