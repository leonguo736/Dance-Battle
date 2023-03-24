#include "altera_up_avalon_video_dma_controller.h"
#include "altera_up_avalon_video_rgb_resampler.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

#define RGB_24BIT_BLACK			0x000000
#define RGB_24BIT_WHITE			0xFFFFFF
#define RGB_24BIT_BLUE			0x0000FF
#define RGB_24BIT_INTEL_BLUE		0x0071C5

/*
 * This stucture holds a pointer to each of the open devices
*/
struct alt_up_dev {
	alt_up_video_dma_dev *pixel_dma_dev;
};


