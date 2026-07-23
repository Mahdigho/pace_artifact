#ifndef __CKPT_INTERM_RUNTIME
#define __CKPT_INTERM_RUNTIME

#include "../_clock.h"
#include "defs.h"
#include <libmsp/mspbase.h>

#define NO_INTERRUPT(old)                                                      \
  {                                                                            \
    old = __get_interrupt_state();                                             \
    __disable_interrupt();                                                     \
  }

#define RESUME_INTERRUPT(old)                                                  \
  {                                                                            \
    __set_interrupt_state(old);                                                \
  }

/* Scheduler managed variables */
extern __nv uint16_t heavy_restore_count;
extern __nv uint16_t on_from_burn_out;
extern __nv uint16_t failed_to_light;
extern __nv uint16_t light_restore_count;

/* User defined variables.
 * 1. Determines whether upon boot if the runtime should check for first
 *  boot. If true, runtime will attempt to resume operation. If false, runtime
 *  will restart application.
 * 2. Determines if upon boot if runtime should wait until high energy is
 * available.
 * 3. Is a super set of the above two (essentially. Enable when running on
 * constant power).
 *
 * */
extern __nv bool check_first_boot;
extern __nv bool init_wait_high;
extern __nv bool cont_power;
/*
 * Pin init is for quick initlization of pins (to avoid unncessary drainage and
 * such). Do not do heavy duty stuff in pin init.
 * Other init is for more time consuming initilization of things.
 */
void pin_init();
void other_init();

/* main application to be run in a loop */
void application();

/* reporting program ran when report pin is high */
void report();

/* QUICK program to run when waking up. leave emptuy if not needed.
 * DO NOT USE INTERRUPTS IN YOUR CODE. If you do, disable them afterwards.
 * The input s specifies whether,
 *   1. WAKE_SOFT: soft wake up from sleep
 *   2. WAKE_HARD: hard wake up from checkpoint restore
 *
 */
#define WAKE_SOFT 0
#define WAKE_HARD 1
void wakeup_sequence(uint8_t s);
void low_energy_sequence();
uint16_t read_cap();

extern __nv uint32_t henergy_ticks;
extern __nv uint32_t lenergy_ticks;
extern __nv uint32_t pre_ckpt_ticks;
extern __nv uint32_t post_ckpt_ticks;
extern __nv uint16_t ldet_count;
extern __nv uint16_t hdet_count;
extern __nv uint32_t lcap_read;
extern __nv uint32_t hcap_read;
extern __nv uint16_t pre_ckpt_cap_read;
extern __nv uint16_t post_ckpt_cap_read;

#endif // __CKPT_INTERM_RUNTIME
       //
