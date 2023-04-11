#include <sys/alt_irq.h>
#include <altera_avalon_timer_regs.h>
#include <system.h>
#include <stdio.h>
#include "camera.h"

static void cameraTimerIsr(void *context, alt_u32 id);
CameraInterface *cameraInterface;

void initCameraTimer(CameraInterface *_cameraInterface)
{
    alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, (void *)cameraTimerIsr, NULL, 0x0);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
    if (cameraInterface == NULL)
    {
        cameraInterface = _cameraInterface;
    }
    else
    {
        printf("Warning `initCameraTimer`: Camera interface already initialized\n");
    }
}

static void cameraTimerIsr(void *context, alt_u32 id)
{
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
    CameraInterface_updateCoords(cameraInterface);
}

CameraInterface *getTimerCameraInterface(void)
{
    if (cameraInterface == NULL)
    {
        printf("Warning `getTimerCameraInterface`: Camera interface not initialized\n");
    }
    return cameraInterface;
}