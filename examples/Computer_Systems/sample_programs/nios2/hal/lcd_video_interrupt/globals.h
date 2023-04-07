#include "altera_up_avalon_video_dma_controller.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

/*
 * This stucture holds a pointer to each of the open devices
*/
struct alt_up_dev {
	alt_up_video_dma_dev *pixel_buffer_dev;
};
