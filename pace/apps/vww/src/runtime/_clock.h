#ifndef __CHKPT_INTERM_CLOCK
#define __CHKPT_INTERM_CLOCK

#include "include/defs.h"
#include <libmsp/mspbase.h>

/*
 * Setups the clocks, timers, and other stuff needed for the ticker counter.
 * Each second will be 1024 ticks, but we won't be using seconds, just ticks
 *   as an asbstraction. (lfxt: 2^15. Divs are 8 x 4 = 32 --> Timer f: 2^10)
 * We do not utilize a remanance timekeeper, so there is no point in having
 *   a non_volatile tick counter. Our goal is to NEVER die.
 * Some concerns:
 *   1. TODO: LFXT takes time to wake up and just spins when doing so. Make this
 *            more efficient.
 *
 *   2. Clock 0 is used to remain in group B. Use other group B peripherals.
 *     2.1 TA0 & 3
 *     2.2 Comp
 *     2.3 ADC
 *     2.4 REF
 *   3. The module assumes that interrupts are enabled all the time.
 *
 * In general, if you're running something with a deadline, you should
 *   always have this code running. Stop/reset the ticks only when starting
 *   fresh. DONOT use this clock to sleep or wake up.
 *
 * TODO: Setup clock TA3 as a wakeup source?
 */

extern volatile uint32_t tick_counter;
#define tick_counter_init(WAIT_FOR_LFXT)                                       \
  {                                                                            \
    __disable_interrupt();                                                     \
    /* Setup FRAM & LFXTCLK pins */                                            \
    FRCTL0 = FRCTLPW | STICH(FRAMST__, DCOFREQ);                               \
    PJSEL0 |= BIT4 | BIT5;                                                     \
    CSCTL0_H = CSKEY_H;                                                        \
    /* First turn on the LFXTCLK */                                            \
    CSCTL4 = HFXTOFF;                                                          \
    CSCTL1 = DCOFSEL_0;                                                        \
    CSCTL3 = DIVA__4 | DIVS__4 | DIVM__32;                                     \
    CSCTL2 =                                                                   \
        SELA__LFXTCLK | STICH(SELM__, MCLK_SRC) | STICH(SELS__, SMCLK_SRC);    \
    do {                                                                       \
      CSCTL5 &= ~LFXTOFFG;                                                     \
      SFRIFG1 &= ~OFIFG;                                                       \
    } while (WAIT_FOR_LFXT && (SFRIFG1 & OFIFG));                              \
    /* Fix for CS12 per errata */                                              \
    CSCTL3 = DIVA__4 | DIVS__4 | DIVM__4;                                      \
    CSCTL1 = STICH(DCOFREQ__, DCOFREQ);                                        \
    /* Per errata have to wait 10us, max speed of 16MHz requires 60 cycles*/   \
    __delay_cycles(ERRATA_WAIT);                                               \
    CSCTL3 = STICH(DIVA__, ACLK_DIV) | STICH(DIVM__, MCLK_DIV) |               \
             STICH(DIVS__, SMCLK_DIV);                                         \
    CSCTL0_H = 0;                                                              \
    timer_setup_cont(TICK_TIMER, ACLK, 1, 4);                                  \
    timer_IFG_disable(TICK_TIMER);                                             \
  }

#define tick_counter_start()                                                   \
  {                                                                            \
    timer_halt(TICK_TIMER);                                                    \
    timer_IFG_disable(TICK_TIMER);                                             \
    tick_counter = 0;                                                          \
    timer_reset(TICK_TIMER);                                                   \
    timer_IFG_enable(TICK_TIMER);                                              \
    timer_start_cont(TICK_TIMER);                                              \
  }

#define tick_counter_halt()                                                    \
  {                                                                            \
    timer_halt(TICK_TIMER);                                                    \
    timer_IFG_disable(TICK_TIMER);                                             \
  }

#define tick_counter_resume()                                                  \
  {                                                                            \
    timer_IFG_enable(TICK_TIMER);                                              \
    timer_resume_up(TICK_TIMER);                                               \
  }

#define tick_counter_reset() tick_counter_start()
uint32_t get_ticks();

#endif // __CHKPT_INTERM_CLOCK