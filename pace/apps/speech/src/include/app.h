#ifndef __SLIMRE_APP_H
#define __SLIMRE_APP_H

#include "../dnn/include/dnn.h"
#include "../runtime/_scheduler.h"
#include "../runtime/include/runtime.h"
#include "inf.h"
#include <libmsp/mspbase.h>

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

// ----- Application Information -------
extern __nv volatile uint16_t app_state;
#define LATENCY1 (900 * 8)
#define LATENCY2 (1350 * 8)
#define LATENCY3 ((uint32_t)1800 * 8)
#define LATENCY4 (2250 * 8)
#define LATENCY5 (2700 * 8)
#define LATENCY LATENCY3

#define GREEDY_LOOKUP false
#define TUNED_LATE false

#endif
