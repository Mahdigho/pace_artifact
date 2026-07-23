#ifndef __SLIMRE_UPTIME_H
#define __SLIMRE_UPTIME_H
#include "../runtime/include/runtime.h"
#include "inf.h"
#include <libmsp/mspbase.h>
#include <stddef.h>
#include <stdint.h>

// ----- Up Time Stats ------

// EWMA momentum value
#define MOMENTUM_DIV 32
#define MOMENTUM_DIV_SHIFT 5

#define UPTIME_BUFFER 150 // TBIAS, approx 20ms

// Fixed point value lookup
#define MOMENTUM_FIXED 8
#define MOMENTUM_FIXED_SHIFT 3

#define UP_LOOKUP_MAX 128
#if CAP_CONFIG == CAP_CONFIG_330uF
#define UP_LOOKUP_SHIFT (3 + MOMENTUM_FIXED_SHIFT)
#elif CAP_CONFIG == CAP_CONFIG_1mF
#define UP_LOOKUP_SHIFT (5 + MOMENTUM_FIXED_SHIFT)
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
#define UP_LOOKUP_SHIFT (6 + MOMENTUM_FIXED_SHIFT)
#endif

typedef uint32_t time_fixed_t;

// ----- Series Stats ------
// Series are used for book keeping and debugging, not in the pace logic anymore
#define RT_SERIES_MAX 16
#define RT_SHIFT 4
#define RT_SERIES_TMAX (8 * 200)
extern __nv uint32_t rt_series[RT_SERIES_MAX];
extern __nv uint32_t up_series[RT_SERIES_MAX];
extern __nv uint32_t tick_series[RT_SERIES_MAX];

// ---- Functional stats
extern __nv time_fixed_t up_time_lookup[UP_LOOKUP_MAX];
extern __nv uint32_t ewma_rt_f;

/* Updates the look up using exponentially moving average
 * This function is expected to be used every recharge, so
 * it is as efficient as we can make it.
 */
void update_lookup(uint32_t recharge_time, uint32_t up_time);
void print_lookup();

/* Updates the series entries used for getting the current avg rt_time. */
time_fixed_t get_avg_series(bool tmax, uint32_t base, uint32_t *series);
void update_series(uint32_t recharge_time, uint32_t up_time);
void reset_series();

static inline time_fixed_t get_avg_up(bool tmax) {
  return get_avg_series(tmax, 1, up_series);
}
static inline time_fixed_t get_avg_rt(bool tmax) {
  return get_avg_series(tmax, 1000, rt_series);
}

static inline time_fixed_t get_ewma_rt() { return ewma_rt_f; }

/* Utilize these functions to handle scheduling.
 * These are not expensive, but they are not cheap either.
 * Use freely but infrequently.
 */
uint16_t get_expected_uptime(uint32_t rt);
uint32_t get_time_left_this_cycle(time_fixed_t fixed_uptime, uint16_t *cap);

/* Get latency of a function with a certain uptime; automatically adds TBIAS.
 * result_t value is used to store book keeping value; returned values is the
 * latency.
 * */
uint32_t get_latency_of_uptime(uint32_t uptime, bool tmax, result_t *inf);

#endif
