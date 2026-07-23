#include "_clock.h"
#include <msp430.h>

volatile uint32_t tick_counter;
uint32_t get_ticks() {
  // For reasons that I think have to do with types and such,
  // cannot write this piece as a macro. Hence, it has to be a function.
  // NOTE: TI recommends stopping timer and reading this value, but
  // for our purpuses, it's ok if tick is +- 2-3, so it doesn't really matter.
  uint32_t time = tick_counter;
  time = time << 16;
  time |= STIC3(TA, TICK_TIMER, R);
  return time;
}

__attribute__((interrupt(STIC3(TIMER, TICK_TIMER, _A1_VECTOR)))) void
STIC3(TIMER, TICK_TIMER, _A1_ISR)(void) {
  switch (__even_in_range(STIC3(TA, TICK_TIMER, IV), TAIV__TAIFG)) {
    {
    case TAIV__NONE:
      break; // No interrupt
    case TAIV__TACCR1:
      break; // CCR1 not used
    case TAIV__TACCR2:
      break; // CCR2 not used
    case TAIV__TACCR3:
      break; // reserve d
    case TAIV__TACCR4:
      break; // reserved
    case TAIV__TACCR5:
      break; // reserved
    case TAIV__TACCR6:
      break;          // reserved
    case TAIV__TAIFG: // overflow
      tick_counter += 1;
      break;
    default:
      break;
    }
  }
}