#ifndef __SLIMRE_APP_H
#define __SLIMRE_APP_H

#include "../dnn/include/dnn.h"
#include "../runtime/_scheduler.h"
#include "../runtime/include/runtime.h"
#include "inf.h"
#include <libmsp/mspbase.h>
#include <librng/rng.h>

// Program state
#define STATE_START 0
#define STATE_INFER 1
#define STATE_COMMU 2

// Which application/configuration to run
#define REPORT_STATS false
#define EXP_RUN 10
#define TIMED_EE false   // PACE with EE
#define TIMED_SLIM false // PACE with Slimmables
#define SLUMBER false    // PACE with SLEXNET
#define LOOKUP_SLEX true // SLEXNet with ePerceptive style lookup table
#define EPERCEPTIVE false
#define ZYGARDE false

// ----
// 1.3 on 18 has rt 280
// 1.2 on 18 has rt 260
// 1.1 on 18 has rt 240
// 1.0 on 18 has rt 200

// ----- Application Information -------
// latencies: 1000, 1500, 2000, 2500, 3000, 4000, 5000, 6000, rng

#define RNG_MAX 128
extern __nv uint32_t rngs[RNG_MAX];
extern __nv uint16_t rng_idx;

static inline uint32_t rand_late() {
  uint32_t ret = rngs[rng_idx];
  rng_idx += 1;
  if (rng_idx == RNG_MAX) {
    rng_idx = 0;
  }

  return ret;
}

extern __nv volatile uint16_t app_state;
#define LATENCY1 (1000 * 8)
#define LATENCY2 (1500 * 8)
#define LATENCY3 ((uint32_t)1000 * 8)
#define LATENCY4 (2500 * 8)
#define LATENCY5 (3000 * 8)
#define LATENCY_RNG (rand_late())
#define LATENCY LATENCY_RNG

#define GREEDY_LOOKUP false
#define TUNED_LATE false

#endif
