#ifndef REGS_H
#define REGS_H

#include <stdio.h>

#include "const.h"
#include "hps_0.h"
#include "hwlib.h"
#include "socal/alt_gpio.h"
#include "socal/hps.h"
#include "socal/socal.h"

#define HW_REGS_BASE (ALT_STM_OFST)
#define HW_REGS_SPAN (0x04000000)
#define HW_REGS_MASK (HW_REGS_SPAN - 1)

bool regs_init(void** virtual_base);
bool regs_close(void* virtual_base);

#endif