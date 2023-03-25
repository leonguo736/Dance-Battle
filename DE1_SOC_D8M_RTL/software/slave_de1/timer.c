#include <sys/alt_irq.h>
#include <altera_avalon_timer_regs.h>
#include <system.h>

static void timer100msIsr(void *context, alt_u32 id);
static unsigned long long timer100ms = 0;

void initTimer100ms(void)
{
    alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, (void *)timer100msIsr, NULL, 0x0);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
}

static void timer100msIsr(void *context, alt_u32 id)
{
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
    timer100ms++; 
}

unsigned long long getTimer100ms(void)
{
    return timer100ms;
}