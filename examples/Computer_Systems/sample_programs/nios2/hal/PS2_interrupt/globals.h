#include "altera_up_avalon_ps2.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

#ifndef HEX5_HEX4_BASE
#define HEX5_HEX4_BASE HEX7_HEX4_BASE
#endif

/*
 * This stucture holds a pointer to each of the open devices
*/
struct alt_up_dev {
	alt_up_ps2_dev *PS2_dev;
};
