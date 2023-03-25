#include <sys/alt_irq.h>
#include <altera_avalon_timer_regs.h>
#include <system.h>
#include <stdio.h>
#include "camera.h"

static void cameraTimer(void *context, alt_u32 id);
static CameraInterface *cameraInterface;

void initCameraTimer(int devId)
{
    alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, (void *)cameraTimer, NULL, 0x0);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
    if (cameraInterface == NULL) {
        cameraInterface = CameraInterface_new(devId);
    } else {
        printf("Warning `initCameraTimer`: Camera interface already initialized\n");
    }
}

static void cameraTimer(void *context, alt_u32 id)
{
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
    CameraInterface_updateCoords(cameraInterface);
}

CameraInterface * getTimerCameraInterface(void)
{
    return cameraInterface;
}