#include "include/inf_approach.h"
#include "include/app.h"
#include "include/model.h"
#include "include/reporting.h"
#include "include/uptime.h"

void timed_slumber(bool adapt) {
  // Update appropriate globals
  attempt_count += 1;

  result_t *result = &(result_data[exit_idx]);

  *result = (result_t){
      .anytime = false,
  };

  // The deadline is set now
  result->deadline = get_ticks() + LATENCY;
  uint16_t int_state;
  NO_INTERRUPT(int_state);

  int8_t config_idx = 0;
  while (config_idx < SLUM_CONFIG_SIZE) {
    result->est_late = get_latency_of_uptime(slumber_configs[config_idx].uptime,
                                             false, result);
    if ((get_ticks() + result->est_late) < result->deadline) {
      // if current config would finish before the deadline
      break;
    }
    config_idx += 1;
  }
  if (config_idx == SLUM_CONFIG_SIZE) {
    config_idx -= 1;
  }

  int8_t width_index = slumber_configs[config_idx].width - MIN_WIDTH;
  result->width = slumber_configs[config_idx].width;
  result->exit_goal = slumber_configs[config_idx].exit;

  // The actual computation for diagnostics purpuses starts here
  uint16_t recharges_before = light_restore_count;
  result->start_t = get_ticks();
  start_inf(result->start_t);
  RESUME_INTERRUPT(int_state)

  // Call inference
  run_slumber(result, early_slim_uptimes[width_index], adapt);
  end_inf();
  result->took_rt = inference_rt;
  result->took_up = inference_up;
  result->took_ckpt = inference_ckpt;
  result->took_R = light_restore_count - recharges_before;

  // Determine if deadline was met
  // 1. Deadline is met if an exit was calculated &&
  // 2. Said exit was done in time.
  result->made_it =
      (result->exit_achieved != 0) && (result->exit_t <= result->deadline);

  if (result->made_it) {
    success_count += 1;
    switch (result->exit_achieved) {
    case 1:
      taken_slumber[width_index].exit1 += 1;
      break;
    case 2:
      taken_slumber[width_index].exit2 += 1;
      break;
    case 3:
      taken_slumber[width_index].exit3 += 1;
      break;
    default:
      // shouldn't happent but alas
      taken_slumber[width_index].fails += 1;
    }
  } else {
    taken_slumber[width_index].fails += 1;
  }
}

void timed_switch() {
  // Update appropriate globals
  attempt_count += 1;

  result_t *result = &(result_data[exit_idx]);

  *result = (result_t){
      .width = MIN_WIDTH,
      .exit_goal = MAX_EXIT,
      .anytime = false,
  };

  // The deadline is set now
  result->deadline = get_ticks() + LATENCY;
  uint16_t int_state;
  NO_INTERRUPT(int_state);

  int8_t width_index = WIDTH_SIZE - 1;
  while (-1 < width_index) {
    result->est_late =
        get_latency_of_uptime(slim_switch_uptime[width_index], false, result);
    if ((get_ticks() + result->est_late) < result->deadline) {
      // if current config would finish before the deadline
      break;
    }
    width_index -= 1;
  }

  if (width_index == -1) {
    width_index = 0;
  }

  result->width += width_index;

  // The actual computation for diagnostics purpuses starts here
  uint16_t recharges_before = light_restore_count;
  result->start_t = get_ticks();
  start_inf(result->start_t);
  RESUME_INTERRUPT(int_state)

  // Call inference
  run_model(result);
  end_inf();
  result->took_rt = inference_rt;
  result->took_up = inference_up;
  result->took_ckpt = inference_ckpt;
  result->took_R = light_restore_count - recharges_before;

  // Determine if deadline was met
  // 1. Deadline is met if an exit was calculated &&
  // 2. Said exit was done in time.
  result->made_it =
      (result->exit_achieved != 0) && (result->exit_t <= result->deadline);

  if (result->made_it) {
    taken_timed_switch[width_index].succeses += 1;
    success_count += 1;
  } else {
    taken_timed_switch[width_index].fails += 1;
  }
}

void timed_exit(bool adapt) {
  // Update appropriate globals
  attempt_count += 1;

  result_t *result = &(result_data[exit_idx]);

  *result = (result_t){
      .width = MIN_WIDTH + WIDTH_SIZE - 1,
      .exit_goal = 1,
      .anytime = false,
  };

  // The deadline is set now
  result->deadline = get_ticks() + LATENCY;
  uint16_t int_state;
  NO_INTERRUPT(int_state);

  int8_t exit_index = MAX_EXIT - 1;
  while (-1 < exit_index) {
    result->est_late =
        get_latency_of_uptime(ee_uptime[exit_index], false, result);
    if ((get_ticks() + result->est_late) < result->deadline) {
      // if current config would finish before the deadline
      break;
    }
    exit_index -= 1;
  }

  if (exit_index == -1) {
    exit_index = 0;
  }

  result->exit_goal += exit_index;

  // The actual computation for diagnostics purpuses starts here
  uint16_t recharges_before = light_restore_count;
  result->start_t = get_ticks();
  start_inf(result->start_t);
  RESUME_INTERRUPT(int_state)

  // Call inference
  run_slumber(result, early_slim_uptimes[WIDTH_SIZE - 1], adapt);
  end_inf();
  result->took_rt = inference_rt;
  result->took_up = inference_up;
  result->took_ckpt = inference_ckpt;
  result->took_R = light_restore_count - recharges_before;

  // Determine if deadline was met
  // 1. Deadline is met if an exit was calculated &&
  // 2. Said exit was done in time.
  exit_index = result->exit_achieved;
  result->made_it = (exit_index != 0) && (result->exit_t <= result->deadline);

  if (exit_index == 0) {
    exit_index = MAX_EXIT;
  } else {
    exit_index -= 1;
  }

  if (result->made_it) {
    taken_timed_exit[exit_index].succeses += 1;
    success_count += 1;
  } else {
    taken_timed_exit[exit_index].fails += 1;
  }
}

extern __nv uint32_t recharge_time;

void zygarde() {
  // Update appropriate globals
  attempt_count += 1;

  result_t *result = &(result_data[exit_idx]);
  *result = (result_t){
      .width = MIN_WIDTH + WIDTH_SIZE - 1,
      .exit_goal = MAX_EXIT,
      .anytime = true,
      .est_rt = get_avg_rt(false),
      .est_up = 0,
      .est_late = 0,
      .i_time = 0,
      .i_energy = read_cap(),
  };

  // Call inference
  result->deadline = get_ticks() + LATENCY;
  result->start_t = get_ticks();
  uint16_t recharges_before = light_restore_count;
  start_inf(result->start_t);
  run_model(result);
  end_inf();
  result->took_rt = inference_rt;
  result->took_up = inference_up;
  result->took_ckpt = inference_ckpt;
  result->took_R = light_restore_count - recharges_before;

  // Determine if deadline was met
  // 1. Deadline is met if an exit was calculated &&
  // 2. Said exit was done in time.
  result->made_it =
      (result->exit_achieved != 0) && (result->exit_t <= result->deadline);

  uint16_t exit_index = result->exit_achieved;
  if (exit_index == 0) {
    exit_index = MAX_EXIT;
  } else {
    exit_index -= 1;
  }

  if (result->made_it) {
    taken_zygarde[exit_index].succeses += 1;
    success_count += 1;
  } else {
    taken_zygarde[exit_index].fails += 1;
  }
}

void lookup_slex() {
  // Update appropriate globals
  attempt_count += 1;

  result_t *result = &(result_data[exit_idx]);
  uint32_t rt = recharge_time;

  *result = (result_t){
      .width = MIN_WIDTH,
      .exit_goal = MAX_EXIT,
      .anytime = false,
      .est_rt = rt,
      .est_up = 0,
      .est_late = 0,
      .i_time = 0,
      .i_energy = read_cap(),
  };

  result->deadline = get_ticks() + LATENCY;

  int8_t config_index = 0;
  while (config_index < (SLUM_CONFIG_SIZE - 1) &&
         rt < slexnet_lookup_rt[config_index + 1]) {
    config_index++;
  }

  config_index = SLUM_CONFIG_SIZE - config_index - 1;
  int8_t width_index = slumber_configs[config_index].width - MIN_WIDTH;
  result->width = slumber_configs[config_index].width;
  result->exit_goal = slumber_configs[config_index].exit;

  uint16_t recharges_before = light_restore_count;
  result->start_t = get_ticks();
  start_inf(result->start_t);

  // Call inference
  run_model(result);
  end_inf();
  result->took_rt = inference_rt;
  result->took_up = inference_up;
  result->took_ckpt = inference_ckpt;
  result->took_R = light_restore_count - recharges_before;

  // Determine if deadline was met
  // 1. Deadline is met if an exit was calculated &&
  // 2. Said exit was done in time.
  result->made_it =
      (result->exit_achieved != 0) && (result->exit_t <= result->deadline);

  if (result->made_it) {
    success_count += 1;
    switch (result->exit_achieved) {
    case 1:
      taken_lookup_slex[width_index].exit1 += 1;
      break;
    case 2:
      taken_lookup_slex[width_index].exit2 += 1;
      break;
    case 3:
      taken_lookup_slex[width_index].exit3 += 1;
      break;
    default:
      // shouldn't happent but alas
      taken_lookup_slex[width_index].fails += 1;
    }
  } else {
    taken_lookup_slex[width_index].fails += 1;
  }
}

void ePerceptive() {
  // Update appropriate globals
  attempt_count += 1;

  result_t *result = &(result_data[exit_idx]);
  uint32_t rt = recharge_time;

  *result = (result_t){
      .width = MIN_WIDTH + WIDTH_SIZE - 1,
      .exit_goal = 1,
      .anytime = false,
      .est_rt = rt,
      .est_up = 0,
      .est_late = 0,
      .i_time = 0,
      .i_energy = read_cap(),
  };
  result->deadline = get_ticks() + LATENCY;

  int8_t exit_index = 0;
  while (exit_index < (MAX_EXIT - 1) && rt < ee_lookup_rt[exit_index + 1]) {
    exit_index++;
  }

  result->exit_goal += exit_index;

  uint16_t recharges_before = light_restore_count;
  result->start_t = get_ticks();
  start_inf(result->start_t);

  // Call inference
  run_model(result);
  end_inf();
  result->took_rt = inference_rt;
  result->took_up = inference_up;
  result->took_ckpt = inference_ckpt;
  result->took_R = light_restore_count - recharges_before;

  // Determine if deadline was met
  // 1. Deadline is met if an exit was calculated &&
  // 2. Said exit was done in time.
  result->made_it =
      (result->exit_achieved != 0) && (result->exit_t <= result->deadline);

  if (result->made_it) {
    taken_eperceptive[exit_index].succeses += 1;
    success_count += 1;
  } else {
    taken_eperceptive[exit_index].fails += 1;
  }
}
