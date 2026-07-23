#ifndef __SLIMRE_REPORT_H
#define __SLIMRE_REPORT_H
#include "inf.h"
#include <libmsp/mspbase.h>
#include <stddef.h>

// ----- Specific Information Regarding An Inference Attempt -------
#define EXIT_COUNT 100
extern __hinv result_t result_data[EXIT_COUNT];
extern __nv size_t exit_idx;

// -------------- Wake Up & Sleep Responsibilties ------------------- //
#define TIME_MAX 15
extern __hinv uint32_t recharge_times[TIME_MAX + 1];
extern __hinv uint32_t up_times[TIME_MAX + 1];
extern __hinv size_t rep_idx;

static inline void update_report_times(uint32_t recharge_time,
                                       uint32_t up_time) {
  up_times[rep_idx] = up_time;
  recharge_times[rep_idx++] = recharge_time;
  if (rep_idx >= TIME_MAX) {
    rep_idx = 0;
  }
}

// ----- General Information Regarding Inference Attempts -------
typedef struct {
  uint16_t succeses;
  uint16_t fails;
} width_res_t;

typedef struct {
  uint16_t exit1;
  uint16_t exit2;
  uint16_t exit3;
  uint16_t fails;
} slumber_res_t;

extern __nv slumber_res_t taken_slumber[WIDTH_SIZE + 1];
extern __nv slumber_res_t taken_lookup_slex[WIDTH_SIZE + 1];
extern __nv width_res_t taken_timed_switch[WIDTH_SIZE + 1];
extern __nv width_res_t taken_timed_exit[MAX_EXIT + 1];
extern __nv width_res_t taken_zygarde[MAX_EXIT + 1];
extern __nv width_res_t taken_eperceptive[MAX_EXIT + 1];
extern __nv uint32_t success_count;
extern __nv uint32_t attempt_count;
extern __nv uint32_t attempt_adjusted;
extern __nv uint32_t success_adjusted;

static inline void reset_reporting() {
  exit_idx = 0;
  success_count = 0;
  attempt_count = 0;
  attempt_adjusted = 0;
}

static inline void reset_stats() {
  for (size_t i = 0; i < WIDTH_SIZE + 1; i++) {
    taken_slumber[i] = (slumber_res_t){0, 0, 0, 0};
    taken_lookup_slex[i] = (slumber_res_t){0, 0, 0, 0};
    taken_timed_switch[i] = (width_res_t){0, 0};
  }
  for (size_t i = 0; i < MAX_EXIT + 1; i++) {
    taken_timed_exit[i] = (width_res_t){0, 0};
    taken_zygarde[i] = (width_res_t){0, 0};
    taken_eperceptive[i] = (width_res_t){0, 0};
  }
}

#endif