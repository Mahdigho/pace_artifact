#include "include/app.h"
#include "include/inf.h"
#include "include/inf_approach.h"
#include "include/reporting.h"
#include <libmatAbstract/mat.h>
#include <libmspio/uartio.h>
#include <libmspprintf/mspprintf.h>
#include <stdbool.h>

__nv volatile uint16_t app_state;

void print_result(uint16_t idx, result_t *res);

void application() {
  if (REPORT_STATS) {
    inf_ret_t ret;
    for (uint16_t width = MIN_WIDTH; width < (MIN_WIDTH + WIDTH_SIZE);
         width += 1) {
      result_t result = {
          .width = width,
          .exit_goal = MAX_EXIT,
          .anytime = false,
      };
      msp_printf("--------Width %u -------\n\r", width);
      msp_printf("Drop w/ latency\n\r");

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline =
          result.start_t + (early_slim_uptimes[width - MIN_WIDTH][0]);
      run_slumber(&result, early_slim_uptimes[width - MIN_WIDTH], true);
      end_inf();
      print_result(0, &result);

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline =
          result.start_t + (early_slim_uptimes[width - MIN_WIDTH][1]);
      run_slumber(&result, early_slim_uptimes[width - MIN_WIDTH], true);
      end_inf();
      print_result(0, &result);

      msp_printf("run_slumber w/ exit\n\r");

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline = result.start_t + LATENCY;
      result.exit_goal = 1;
      run_slumber(&result, early_slim_uptimes[width - MIN_WIDTH], false);
      end_inf();
      print_result(0, &result);

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline = result.start_t + LATENCY;
      result.exit_goal = 2;
      run_slumber(&result, early_slim_uptimes[width - MIN_WIDTH], false);
      end_inf();
      print_result(0, &result);

      msp_printf("Slim w/ exit\n\r");

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline = result.start_t + LATENCY;
      result.exit_goal = 1;
      run_model(&result);
      end_inf();
      print_result(0, &result);

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline = result.start_t + LATENCY;
      result.exit_goal = 2;
      run_model(&result);
      end_inf();
      print_result(0, &result);
    }
    while (true)
      ;
    return;
  } else {
    reset_reporting();

    result_t result = {
        .width = WIDTH_SIZE,
        .exit_goal = MAX_EXIT,
        .anytime = false,
    };

    while (true) {
      switch (app_state) {
      case STATE_START:
        gpio_set(1, 0);
        _bis_SR_register(LPM3_bits + GIE);
        app_state = STATE_INFER;
        break;
      case STATE_INFER:
        reset_stats();

        // Reset idx stats
        for (size_t i = 0; i <= TIME_MAX; i++) {
          up_times[i] = 0;
          recharge_times[i] = 0;
        }
        rep_idx = 0;
        while (rep_idx <= (TIME_MAX >> 1)) {
          result.start_t = get_ticks();
          result.deadline = result.start_t + LATENCY;
          warmup_run(&result);
        }
        result.start_t = get_ticks();
        result.deadline = result.start_t + LATENCY;
        warmup_run(&result);

#if LOOKUP_SLEX
        for (size_t count = 0; count < EXP_RUN; count++) {
          lookup_slex();
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

#if TIMED_SLIM
        for (size_t count = 0; count < EXP_RUN; count++) {
          timed_switch();
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

#if SLUMBER
        for (size_t count = 0; count < EXP_RUN; count++) {
          timed_slumber(true);
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

#if LOOKUP_EE
        for (size_t count = 0; count < EXP_RUN; count++) {
          lookup_exit();
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

#if EPERCEPTIVE
        for (size_t count = 0; count < EXP_RUN; count++) {
          ePerceptive();
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

#if TIMED_EE
        for (size_t count = 0; count < EXP_RUN; count++) {
          timed_exit(true);
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

#if ZYGARDE
        for (size_t count = 0; count < EXP_RUN; count++) {
          zygarde();
          exit_idx += 1;
          if (exit_idx >= EXIT_COUNT) {
            exit_idx = 0;
          }
        }
#endif

        app_state = STATE_COMMU;
        break;

      case STATE_COMMU:
        gpio_set(1, 1);
        _bis_SR_register(LPM3_bits + GIE);
        break;
      }
    }
  }
}
