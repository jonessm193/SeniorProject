//Coded mostly by SJones

#include <system.h>

/* This will keep track of how many ticks that the system
*  has been running for */
int timer_ticks = 0;
int stupid = 0;

//Handles the timer
void timer_handler(struct regs *r)
{
    //Increment our 'tick count'
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen 
    *    This does not work at all like it's supposed to
    *    Need to find the correct timing to get any sort
    *    of timing functionality*/
    //if (timer_ticks % 180 == 0)
    //{
        //puts((unsigned char*)"This is supposed to be one second...\n");
    //}
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}