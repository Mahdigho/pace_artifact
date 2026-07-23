#include "include/inf.h"
#include "include/app.h"
#include "include/reporting.h"
#include "include/uptime.h"
#include <libmspprintf/mspprintf.h>

__nv bool simple_inf_happening = false;

__nv uint32_t inference_up;
__nv uint32_t inference_rt;
__nv uint32_t inference_ckpt;

__nv uint32_t inference_start_ticks = 0;

void base_engine(op_t *layers, uint16_t length, result_t *inf) {
  inf->last_layer = 0;

  uint32_t total_exec_time = 0;
  uint16_t exit_counter = 0;
  uint16_t last_done_exit = 0;
  uint32_t start_ticks, last_exit_ticks, end_ticks;
  // Initialized to the end. If no exit is calculated, latency should fail
  // automatically.
  last_exit_ticks = inf->deadline + 10000;

  for (uint8_t i = 0; i < length; i++) {
    bool ran_exit = false;
    op_t op = layers[i];

    if (REPORT_STATS) {
      start_ticks = get_ticks();
    }

    // Run operation
    if (op.ee != NULL) {
      // reached exit, increment exit counter
      exit_counter += 1;
      if (!(inf->anytime) && exit_counter != inf->exit_goal) {
        // Execution of the exit should be skipped if
        // 1. This is not Anytime mode (all exits are done then)
        // 2. This is not the desired exit.
      } else {
        // Else, run the exit
        op.fn(op.s, op.d, inf->width);
        op.ee(op.s, op.d, inf->width);
        ran_exit = true;
      }
    } else {
      // if not exit layer, run as normal
      op.fn(op.s, op.d, inf->width);
    }

    inf->last_layer += 1;
    end_ticks = get_ticks();

    if (REPORT_STATS) {
      total_exec_time += end_ticks - start_ticks;
      // msp_printf("%u -- %n\n\r", i, end_ticks - start_ticks);
    }

    if ((inf->deadline < end_ticks)) {
      // If latency has expired, leave the loop
      break;
    } else if (ran_exit) {
      // Update exit number only if an exit was ran and latency was not violated
      last_done_exit = exit_counter;
      last_exit_ticks = end_ticks;
    }

    if (last_done_exit == inf->exit_goal) {
      // If exit goal has been achieved, leave the loop
      break;
    }
  }

  // update exit based on what really happened
  inf->exit_achieved = last_done_exit;
  inf->exit_t = last_exit_ticks;
  inf->end_t = end_ticks;

  // Ensure Program was not reset while application was running
  // i.e. we didn't travel in time
  if ((inf->start_t) > last_exit_ticks) {
    inf->exit_achieved = 0;
  }
}

typedef struct {
  uint32_t uptime_till_prev_exit;
  uint32_t uptime_till_next_exit;
  uint32_t max_runtime;
  uint32_t deadline;
  uint32_t prev_uptime;
  nn_fn prev_exit;
} slumber_early_adapt_t;

static inline void slumber_update_times(slumber_early_adapt_t *slumber_info,
                                        uint32_t start_ticks,
                                        uint32_t end_ticks, nn_fn ee_fn,
                                        uint16_t next_exit_latency) {
  __disable_interrupt();

  // Calculate how long the current inference has been running
#if REPORT_STATS
  uint32_t up_this_charge = end_ticks - start_ticks;
  uint32_t current_uptime = slumber_info->prev_uptime + up_this_charge;
#else
  uint32_t up_this_charge = (end_ticks - henergy_ticks);
  uint32_t current_uptime = inference_up + up_this_charge;
#endif
  // uptime since last uptime
  uint32_t uptime_elapsed = current_uptime - slumber_info->prev_uptime;
  slumber_info->uptime_till_next_exit -= uptime_elapsed;
  slumber_info->prev_uptime = current_uptime;

  // if early exit, progress the uptime counters to the next exit
  if (ee_fn != NULL) {
    slumber_info->uptime_till_prev_exit = slumber_info->uptime_till_next_exit;
    slumber_info->uptime_till_next_exit = next_exit_latency - current_uptime;
  }

  // If uptime until next exit is less than uptime needed for prev exit,
  if (slumber_info->prev_exit != NULL &&
      (slumber_info->uptime_till_next_exit <=
       slumber_info->uptime_till_prev_exit)) {
    // set prev_exit to NULL (no point in exiting early basically)
    slumber_info->prev_exit = NULL;
  }

  __enable_interrupt();
}

static inline bool slumber_exit_early(slumber_early_adapt_t slumber_info) {
  if (slumber_info.prev_exit == NULL) {
    return false;
  } else {
    // Determine if exiting early is the right choice
#if REPORT_STATS
    uint32_t time_left = slumber_info.max_runtime - slumber_info.prev_uptime;
    uint32_t next_exit_end_prediction =
        get_ticks() + slumber_info.uptime_till_next_exit - 20;
#else
    result_t res;
    uint32_t next_exit_end_prediction =
        get_latency_of_uptime(slumber_info.uptime_till_next_exit, true, &res) +
        get_ticks() - 150;
#endif
    // recommend early exit if deadline is less than latency of next exit
    return (slumber_info.deadline < next_exit_end_prediction);
  }
}

static inline void slumber_update_after_ee(slumber_early_adapt_t *slumber_info,
                                           uint32_t end_ticks,
                                           uint32_t start_ticks) {
  __disable_interrupt();
  // Update slumber info
#if REPORT_STATS
  uint32_t up_this_charge = end_ticks - start_ticks;
  uint32_t current_uptime = slumber_info->prev_uptime + up_this_charge;
#else
  uint32_t up_this_charge = (end_ticks - henergy_ticks);
  uint32_t current_uptime = inference_up + up_this_charge;
#endif
  // uptime since last uptime
  uint32_t uptime_elapsed = current_uptime - slumber_info->prev_uptime;
  slumber_info->prev_uptime = current_uptime;
  slumber_info->uptime_till_prev_exit -= uptime_elapsed;
  slumber_info->prev_exit = NULL;
  __enable_interrupt();
}

void slumber_engine(op_t *layers, uint16_t length, result_t *inf,
                    uint16_t *latencies, bool adapt) {
  // initializations
  inf->last_layer = 0;
  uint16_t exit_counter = 0;
  uint16_t last_done_exit = 0;
  uint32_t last_exit_ticks, end_ticks;
  // Initialized to the end. If no exit is calculated, succes test should fail
  // automatically.
  last_exit_ticks = inf->deadline + 10000;

  slumber_early_adapt_t slumber_info = {
      .uptime_till_prev_exit = -1,
      .uptime_till_next_exit = latencies[exit_counter],
      .max_runtime = inf->deadline - inf->start_t,
      .deadline = inf->deadline,
      .prev_exit = NULL,
      .prev_uptime = 0,
  };

  for (uint8_t layer_idx = 0; layer_idx < length; layer_idx++) {
    uint32_t start_ticks = get_ticks();
    bool ran_exit = false;
    op_t op = layers[layer_idx];

    // Run operation
    if (op.ee != NULL) {
      // reached exit, increment exit counter
      exit_counter += 1;
      if (!(inf->anytime) && exit_counter != inf->exit_goal) {
        // Execution of the exit should be skipped if
        // 1. This is not Anytime mode (all exits are done then)
        // 2. This is not the desired exit.

        // If exit was not run, it is a potential option that can be ran
        op.fn(op.s, op.d, inf->width);
        slumber_info.prev_exit = op.ee;
      } else {
        // Else, run the exit
        op.fn(op.s, op.d, inf->width);
        op.ee(op.s, op.d, inf->width);
        ran_exit = true;
      }
    } else {
      // if not exit layer, run as normal
      op.fn(op.s, op.d, inf->width);
    }

    inf->last_layer += 1;
    end_ticks = get_ticks();

    if ((inf->deadline < end_ticks)) {
      // If latency has expired, leave the loop
      break;
    } else if (ran_exit) {
      // Update exit number only if an exit was ran and latency was not violated
      last_done_exit = exit_counter;
      last_exit_ticks = end_ticks;
    }

    if (last_done_exit == inf->exit_goal) {
      // If exit goal has been achieved, leave the loop
      break;
    }

    // Slumber's smart early exit logic
    // In theory, this is running before every layer except the first,
    // so we place it here for ease of comparison + seperation of tasks

    // update uptime expectations
    slumber_update_times(&slumber_info, start_ticks, end_ticks, op.ee,
                         latencies[exit_counter]);
    // We know that uptime_till_prev_exit < uptime_till_next_exit

    // if slumber thinks there is not enough time to get to the next exit
    // execute early exit smartly
    if (adapt && slumber_exit_early(slumber_info)) {
      attempt_adjusted += 1;
      start_ticks = end_ticks;
      slumber_info.prev_exit(NULL, op.s, inf->width);
      end_ticks = get_ticks();
      // end only if we're out of bounds
      if ((inf->deadline < end_ticks)) {
        break;
      } else {
        last_done_exit = exit_counter;
        last_exit_ticks = end_ticks;
        success_adjusted += 1;
        // update the slumber information
        slumber_update_after_ee(&slumber_info, last_exit_ticks, end_ticks);
      }
    }
  }

  // update exit based on what really happened
  inf->exit_achieved = last_done_exit;
  inf->exit_t = last_exit_ticks;
  inf->end_t = end_ticks;

  // Ensure Program was not reset while application was running
  // i.e. we didn't travel in time
  if ((inf->start_t) > last_exit_ticks) {
    inf->exit_achieved = 0;
  }
}