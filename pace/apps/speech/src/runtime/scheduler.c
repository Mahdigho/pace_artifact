#include "_checkpoint.h"
#include "_scheduler.h"
#include "libmsp/timer.h"

__nv uint16_t sched_state = 0;
__nv uint16_t sched_ckpt_time = 0;

void schedule_ckpt(uint16_t ms_count) {
  sched_ckpt_time = ms_count;
  reschedule_ckpt();
}

void reschedule_ckpt() {
  CKPT_PRINTF("Planning CKPT\n\r");
  uint16_t int_state = __get_interrupt_state();
  __disable_interrupt();
  timer_setup_up(2, ACLK, 4, 8, sched_ckpt_time);
  timer_IFG_enable(2);
  timer_start_up(2);
  __set_interrupt_state(int_state);
}

void __attribute__((interrupt(TIMER2_A0_VECTOR))) fake_comp_isr(void) {
  __disable_interrupt();
  timer_halt(2);
  timer_IFG_disable(2);
  CKPT_PRINTF("Causing checkpoint\n\r");
  ckpt_make();
}
