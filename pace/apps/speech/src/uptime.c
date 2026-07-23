#include "include/uptime.h"
#include "include/app.h"
#include "include/reporting.h"
#include "libmspio/uartio.h"
#include <libmspprintf/mspprintf.h>

// -------------- Globals -------------------------
__nv uint32_t recharge_sidx = 0;
__nv uint32_t rt_series[RT_SERIES_MAX] = {};
__nv uint32_t up_series[RT_SERIES_MAX] = {};
__nv uint32_t tick_series[RT_SERIES_MAX] = {};
__nv bool recharge_series_full;
__nv uint32_t ewma_rt_f = 0;
__nv uint32_t last_var = 0;
extern __nv uint32_t recharge_time;
// -------------- SERIES FUNCTIONS ----------------

#define CKPT_COST 57

void reset_series() {
  recharge_sidx = 0;
  recharge_series_full = false;
  ewma_rt_f = 0;
}

void update_series(uint32_t rt, uint32_t ut) {
  uint32_t time = get_ticks();
  // update the series of information
  tick_series[recharge_sidx] = time;
  up_series[recharge_sidx] = ut;
  rt_series[recharge_sidx++] = rt;

  // update the rt ewma information
  time_fixed_t rt_fixed = rt * MOMENTUM_FIXED;
  if (ewma_rt_f == 0) {
    ewma_rt_f = rt_fixed;
  } else {
    ewma_rt_f =
        ((MOMENTUM_DIV - 1) * ewma_rt_f + rt_fixed) >> MOMENTUM_DIV_SHIFT;
  }

  if (recharge_sidx >= RT_SERIES_MAX) {
    recharge_sidx = 0;
    recharge_series_full = true;
  }
}

time_fixed_t get_avg_series(bool tmax, uint32_t base, uint32_t *series) {
  uint32_t sum = 0;
  size_t max_idx = RT_SERIES_MAX;

  if (!recharge_series_full) {
    // recharge series is not full, add partial
    max_idx = recharge_sidx;
  }

  if (!recharge_series_full && max_idx == 0) {
    // If no values exist in the series, provid the base
    sum = base;
    max_idx = 1;
  } else if (!tmax) {
    // do not enforce max time count
    for (size_t i = 0; i < max_idx; i++) {
      sum += series[i];
    }
  } else {

    // enforce max time count
    size_t count = 0;

    // tmax count includes at least one value.
    size_t max_index = 0;
    uint32_t max_ticks = tick_series[max_index];

    uint32_t tmax_cutoff = get_ticks();
    // min 0
    if (RT_SERIES_TMAX < tmax_cutoff) {
      tmax_cutoff -= RT_SERIES_TMAX;
    } else {
      tmax_cutoff = 0;
    }

    for (size_t i = 0; i < max_idx; i++) {
      // check if it later than max_index
      if (max_ticks < tick_series[i]) {
        max_index = i;
        max_ticks = tick_series[i];
      }

      // check if current series index is after  the cutoff
      if (tmax_cutoff < tick_series[i]) {
        count += 1;
        sum += series[i];
      }
    }

    // if no values within the last TMAX were found,
    // provide the value at max time,
    if (count == 0) {
      sum = series[max_index];
      max_idx = 1;
    } else {
      max_idx = count;
    }
  }

  sum *= MOMENTUM_FIXED;
  sum /= max_idx;

  return sum;
}

// -------------- LOOKUP FUNCTIONS ----------------

__nv time_fixed_t up_time_lookup[UP_LOOKUP_MAX];

static inline uint16_t get_index(time_fixed_t rt) {
  if (rt >= (((uint32_t)UP_LOOKUP_MAX) << UP_LOOKUP_SHIFT)) {
    return 127;
  } else {
    return (rt >> UP_LOOKUP_SHIFT);
  }
}

static inline time_fixed_t get_lookup(time_fixed_t rt) {
  // ensure index is maxed out properly
  uint16_t up_time = up_time_lookup[get_index(rt)];

  // Just don't return 0
  if (up_time == 0) {
    return 1;
  } else {
    return (up_time);
  }
}

void update_lookup(uint32_t recharge_time, uint32_t up_time) {
  time_fixed_t up_fixed = up_time * MOMENTUM_FIXED;
  time_fixed_t rt_fixed = recharge_time * MOMENTUM_FIXED;

  // ensure index is maxed out properly
  uint16_t index = get_index(rt_fixed);

  if (up_time_lookup[index] == 0) {
    up_time_lookup[index] = up_fixed;
  } else {
    up_time_lookup[index] =
        ((MOMENTUM_DIV - 1) * up_time_lookup[index] + up_fixed) >>
        MOMENTUM_DIV_SHIFT;
  }
}

void print_lookup() {
  uint16_t count = 0;
  while (count != UP_LOOKUP_MAX) {
    msp_printf(" %u,", up_time_lookup[count]);
    count++;
    if ((count & 15) == 0) {
      msp_printf("\n\r");
    }
  }
  msp_printf("\n\r");
}

// -------------- LATENCY FUNCTIONS ----------------

time_fixed_t get_time_left_this_cycle(time_fixed_t avg_uptime, uint16_t *cap) {
  uint32_t current_volts = read_cap();
  *cap = (uint16_t)current_volts;
  uint32_t energy = (current_volts * current_volts) >> DELTA_E_SHIFT;
  if (energy <= MIN_ESQ) {
    return 0;
  } else {
    energy -= MIN_ESQ;
    return ((energy * avg_uptime) / DELTA_ENERGY);
  }
}

uint32_t get_latency_of_uptime(uint32_t uptime, bool tmax, result_t *inf) {
  uint16_t int_state;
  NO_INTERRUPT(int_state)

  uint16_t cap = 0;
  time_fixed_t avg_rt_f = get_ewma_rt();
  time_fixed_t avg_up_f = get_lookup(avg_rt_f);
  time_fixed_t time_left_in_this_charge_f =
      get_time_left_this_cycle(avg_up_f, &cap);
  time_fixed_t uptime_f = uptime * MOMENTUM_FIXED;

  // calculate minimum of how many full cycles we'll need to accomdate for
  // uptime
  uint32_t up_cycles_fixed =
      ((uptime_f - time_left_in_this_charge_f) * MOMENTUM_FIXED) / avg_up_f;
  // uint32_t up_cycles = (up_cycles_fixed >> MOMENTUM_FIXED_SHIFT) + 1;
  uint32_t up_cycles = (up_cycles_fixed >> MOMENTUM_FIXED_SHIFT);

  time_fixed_t full_cycle_period_f = avg_rt_f + avg_up_f + CKPT_COST;

  // how much uptime we still need even after the recharges
  time_fixed_t neg_quotient_f =
      time_left_in_this_charge_f + avg_up_f * up_cycles;
  time_fixed_t ret_f =
      time_left_in_this_charge_f + full_cycle_period_f * up_cycles;
  // TODO: does it need another recharge phase?
  if (neg_quotient_f < uptime_f) {
    ret_f += (uptime_f - neg_quotient_f) + avg_rt_f + CKPT_COST;
    up_cycles_fixed += MOMENTUM_FIXED;
  }

  uint32_t ret = (ret_f >> MOMENTUM_FIXED_SHIFT) + UPTIME_BUFFER;

  inf->needed_up = uptime;
  inf->est_rt = avg_rt_f >> 3;
  inf->est_up = avg_up_f >> 3;
  inf->i_time = time_left_in_this_charge_f;
  inf->i_energy = cap;
  inf->est_R = up_cycles_fixed;

  RESUME_INTERRUPT(int_state);
  return ret;
}

uint32_t get_uptime_within_latency(uint32_t deadline, bool tmax) {
  uint16_t int_state;
  NO_INTERRUPT(int_state)

  uint16_t cap = 0;
  time_fixed_t avg_rt_f = get_avg_rt(tmax);
  time_fixed_t avg_up_f = get_lookup(avg_rt_f);
  time_fixed_t time_left_in_this_charge_f =
      get_time_left_this_cycle(avg_up_f, &cap);
  time_fixed_t latency_f = (deadline - get_ticks()) * MOMENTUM_FIXED;
  time_fixed_t full_cycle_period_f = avg_rt_f + avg_up_f + CKPT_COST;

  // calculate how many full cycles we can expect to have
  uint32_t full_cycle_count_f =
      ((latency_f - time_left_in_this_charge_f) * MOMENTUM_FIXED) /
      full_cycle_period_f;
  uint32_t full_cycle_count = (full_cycle_count_f >> MOMENTUM_FIXED_SHIFT);

  time_fixed_t ret_f = time_left_in_this_charge_f + full_cycle_count * avg_up_f;

  // calculate how much will be left over after the cycle count
  time_fixed_t neg_quotient_f =
      time_left_in_this_charge_f + full_cycle_count_f * full_cycle_period_f;
  time_fixed_t quotient = 0;
  if (neg_quotient_f < latency_f) {
    quotient = latency_f - neg_quotient_f;
  }
  // If the quotient has enough time
  if ((avg_rt_f + CKPT_COST) < quotient) {
    ret_f += quotient - avg_rt_f - CKPT_COST;
  }

  uint32_t ret = (ret_f >> MOMENTUM_FIXED_SHIFT);
  RESUME_INTERRUPT(int_state);
  return ret;
}

// These values are stored as fixed point
#if CAP_CONFIG == CAP_CONFIG_330uF
__nv uint32_t up_time_lookup[UP_LOOKUP_MAX] = {
    0,    0,    0,    0,    0,   0,   0,   0,   0,   0,   3280, 1928, 1578,
    1354, 1214, 1122, 1055, 985, 983, 963, 871, 815, 804, 793,  824,  760,
    745,  732,  704,  690,  722, 663, 661, 667, 665, 672, 633,  642,  656,
    589,  617,  636,  626,  612, 611, 620, 595, 594, 626, 616,  591,  608,
    584,  573,  577,  599,  622, 571, 590, 580, 584, 564, 583,  572,  568,
    555,  578,  565,  548,  569, 558, 558, 541, 561, 551, 542,  560,  549,
    558,  560,  553,  553,  549, 551, 560, 555, 560, 538, 560,  555,  552,
    552,  280,  352,  568,  576, 368, 432, 568, 574, 0,   240,  496,  568,
    624,  248,  0,    536,  576, 576, 0,   496, 560, 584, 560,  280,  256,
    0,    272,  392,  256,  0,   0,   0,   0,   0,   0,   0,
};
#elif CAP_CONFIG == CAP_CONFIG_1mF
__nv time_fixed_t up_time_lookup[UP_LOOKUP_MAX] = {
    1,    1,    1,    1,    1,    3465, 3905, 2478, 2381, 64026, 2130, 1832,
    1938, 1784, 1795, 1710, 1716, 1595, 1585, 1541, 1521, 1571,  1499, 1552,
    1532, 1538, 1382, 1322, 1415, 1489, 1542, 1511, 1307, 1508,  1340, 1206,
    1244, 1409, 1317, 1395, 1441, 1418, 1454, 1256, 1436, 1263,  1358, 1411,
    1139, 1184, 1210, 1306, 1277, 623,  1395, 1424, 1370, 1402,  1248, 1338,
    1256, 203,  87,   126,  283,  1,    1,    1,    1,    1,     1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,     1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,     1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,     1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,     1,    1,
    1,    1,    1,    1,    1,    1,    1,    145,

};
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
__nv time_fixed_t up_time_lookup[UP_LOOKUP_MAX] = {
    3,    3,    3,    3,     3,    3,    3648, 13485, 11390, 18011, 7920, 9758,
    8888, 7847, 7488, 15283, 6789, 6275, 6062, 5772,  5645,  5390,  5501, 5309,
    5195, 5109, 4716, 5078,  4807, 3534, 3593, 4685,  3557,  4593,  4423, 3615,
    4439, 3626, 3865, 3678,  3636, 4537, 4316, 3619,  3745,  3661,  3534, 3790,
    3534, 3534, 3759, 3554,  3551, 2401, 4078, 4221,  4293,  3534,  3534, 3689,
    130,  3,    917,  372,   3,    372,  808,  130,   3,     806,   368,  130,
    982,  479,  3,    351,   4128, 3,    3,    785,   471,   3,     3,    883,
    127,  3,    3,    877,   126,  3,    3,    874,   3,     126,   3,    3,
    3,    3,    3,    3,     3,    3,    3,    3,     3,     3,     3,    3,
    3,    3,    3,    3,     3,    3,    3,    3,     3,     3,     3,    3,
    3,    3,    3,    3,     3,    3,    3,    3,
};
#endif
