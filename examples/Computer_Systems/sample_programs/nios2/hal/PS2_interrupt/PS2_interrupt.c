#include "globals.h"

/* these globals are written by interrupt service routines; we have to declare
 * these as volatile to avoid the compiler caching their values in registers */
extern volatile unsigned char byte1, byte2,
    byte3;                        // modified by PS/2 interrupt service routine
extern volatile int      timeout; // used to synchronize with the timer
extern struct alt_up_dev up_dev;  /* pointer to struct that holds pointers to
                                      open devices */

/* function prototypes */
void HEX_PS2(unsigned char, unsigned char, unsigned char);
void interval_timer_ISR(void *, unsigned int);
void PS2_ISR(void *, unsigned int);

/*******************************************************************************
 * This program performs the following:
 *	Displays the last three bytes of data received from the PS/2 port
 *	on the HEX displays on the board. The PS/2 port is handled using
 *	interrupts.
 ******************************************************************************/
int main(void) {
    /* The base addresses of devices are listed in the "BSP/system.h" file*/

    /* declare device driver pointers for devices */
    alt_up_ps2_dev * PS2_dev;
    /* declare volatile pointer for interval timer, which does not have HAL
     * functions */
    volatile int * interval_timer_ptr =
        (int *)INTERVAL_TIMER_BASE; // interal timer base address

    /* initialize some variables */
    byte1   = 0;
    byte2   = 0;
    byte3   = 0; // used to hold PS/2 data
    timeout = 0; // synchronize with the timer

    /* set the interval timer period for scrolling the HEX displays */
    int counter = 20000000; // 1/(100 MHz) x (20000000) = 200 msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1

    // open the PS2 port
    PS2_dev = alt_up_ps2_open_dev("/dev/PS2_Port");
    if (PS2_dev == NULL) {
        alt_printf("Error: could not open PS2 device\n");
        return -1;
    } else {
        alt_printf("Opened PS2 device\n");
        up_dev.PS2_dev = PS2_dev; // store for use by ISRs
    }
    (void)alt_up_ps2_write_data_byte(PS2_dev, 0xFF); // reset
    alt_up_ps2_enable_read_interrupt(
        PS2_dev); // enable interrupts from PS/2 port

    /* use the HAL facility for registering interrupt service routines. */
    /* Note: we are passsing a pointer to up_dev to each ISR (using the context
     * argument) as a way of giving the ISR a pointer to every open device. This
     * is useful because some of the ISRs need to access more than just one
     * device (e.g. the pushbutton ISR accesses both the pushbutton device and
     * the audio device) */
    alt_irq_register(0, (void *)&up_dev, (void *)interval_timer_ISR);
    alt_irq_register(7, (void *)&up_dev, (void *)PS2_ISR);

    while (1) {
        while (!timeout)
            ; // wait to synchronize with timeout, which is set by the interval
              // timer ISR

        /* also, display any PS/2 data (from its interrupt service routine) on
         * HEX displays */
        HEX_PS2(byte1, byte2, byte3);
        timeout = 0;
    }
}

/*******************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
 * Note that we are using pointer accesses for the HEX displays parallel port.
 * We could also use the HAL functions for these ports instead
 ******************************************************************************/
void HEX_PS2(unsigned char b1, unsigned char b2, unsigned char b3) {
    volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display, for the hex digits 0 - F */
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
    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}
