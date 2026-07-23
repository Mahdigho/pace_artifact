#ifndef __CKPT_INTERM_SCHEDULE
#define __CKPT_INTERM_SCHEDULE

#include "_checkpoint.h"
#include "_energy.h"
#include "include/runtime.h"
#include <libmsp/mspbase.h>
#include <stdint.h>

#define SCHED_NONE 0
#define SCHED_WAIT 1
#define SCHED_CKPT 2
extern __nv uint16_t sched_state;

#define wait_high_energy(voltage)                                              \
  {                                                                            \
    sched_state = SCHED_WAIT;                                                  \
    energy_det_high(voltage);                                                  \
  }

#define wait_low_energy(voltage)                                               \
  {                                                                            \
    sched_state = SCHED_WAIT;                                                  \
    energy_det_low(voltage);                                                   \
  }

#define ckpt_high_energy(voltage)                                              \
  {                                                                            \
    sched_state = SCHED_CKPT;                                                  \
    energy_det_high(voltage);                                                  \
  }

#define ckpt_low_energy(voltage)                                               \
  {                                                                            \
    sched_state = SCHED_CKPT;                                                  \
    energy_det_low(voltage);                                                   \
  }

#define stop_energy_det(OLD_VAL)                                               \
  {                                                                            \
    OLD_VAL = (CEINT & (CEIIE | CEIE));                                        \
    CEINT &= ~(CEIIE | CEIE);                                                  \
    sched_state = SCHED_NONE;                                                  \
  }

// #define resume_energy_det(OLD_VAL)                                             \
//   { CEINT |= OLD_VAL; }

#define high_energy_detected()                                                 \
  {                                                                            \
    CKPT_PRINTF("COMP detected high\n\r");                                     \
    switch (sched_state) {                                                     \
    case SCHED_WAIT:                                                           \
      _bic_SR_register_on_exit(LPM3_bits);                                     \
      comp_off();                                                              \
      break;                                                                   \
    case SCHED_CKPT:                                                           \
      ckpt_stop();                                                             \
      comp_off();                                                              \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
    sched_state = SCHED_NONE;                                                  \
  }

#define low_energy_detected()                                                  \
  {                                                                            \
    CKPT_PRINTF("COMP detected low\n\r");                                      \
    switch (sched_state) {                                                     \
    case SCHED_WAIT:                                                           \
      _bic_SR_register_on_exit(LPM3_bits);                                     \
      comp_off();                                                              \
      break;                                                                   \
    case SCHED_CKPT:                                                           \
      ckpt_make();                                                             \
      comp_off();                                                              \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
    sched_state = SCHED_NONE;                                                  \
    low_energy_sequence();                                                     \
  }

void schedule_ckpt(uint16_t ms_count);
void reschedule_ckpt();
void stop_sched_ckpt();

#endif // __CKPT_INTERM_SCHEDULE