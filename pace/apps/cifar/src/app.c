#include "include/app.h"
#include "include/inf.h"
#include "include/inf_approach.h"
#include "include/reporting.h"
#include "include/uptime.h"
#include "librng/rng.h"
#include <libmatAbstract/mat.h>
#include <libmspio/uartio.h>
#include <libmspprintf/mspprintf.h>
#include <stdbool.h>

__nv volatile uint16_t app_state;

__nv uint32_t rngs[RNG_MAX] = {
    8424,  47104, 13024, 22472, 33128, 22288, 37088, 8968,  15136, 19304, 34384,
    39480, 13824, 40040, 19304, 10896, 38568, 36352, 36216, 10080, 8744,  30128,
    39496, 33920, 33720, 33240, 13520, 29280, 12880, 47568, 12656, 13032, 35592,
    24976, 41288, 32064, 21232, 27344, 13784, 43248, 39176, 26552, 34520, 33448,
    28808, 24472, 37880, 19552, 39488, 12976, 28232, 8424,  30600, 13936, 39944,
    36944, 31512, 39240, 17400, 11992, 21608, 10752, 14976, 30440, 10256, 40344,
    37048, 19152, 36840, 29904, 37984, 15024, 10720, 31296, 42160, 44912, 31288,
    46408, 23952, 45704, 27904, 27096, 27744, 24264, 39952, 35248, 47896, 17472,
    16808, 42736, 28792, 12208, 17840, 11432, 24800, 25336, 12424, 13216, 40312,
    23744, 39944, 29968, 29680, 44296, 9760,  33544, 17296, 33880, 21264, 25840,
    17528, 9352,  43824, 38720, 34752, 17600, 36808, 14056, 23344, 15504, 29600,
    43728, 47680, 29920, 41568, 9616,  42128, 37744};
__nv uint16_t rng_idx = 0;

void print_result(uint16_t idx, result_t *res);

extern __nv uint32_t rt_series[RT_SERIES_MAX];
extern __nv bool recharge_series_full;
void application() {
  if (REPORT_STATS) {
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

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline =
          result.start_t + (early_slim_uptimes[width - MIN_WIDTH][2]);
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

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline = result.start_t + LATENCY;
      result.exit_goal = 3;
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

      result.start_t = get_ticks();
      start_inf(result.start_t);
      result.deadline = result.start_t + LATENCY;
      result.exit_goal = 3;
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
    pcg_state = BASIC_SEED;

    while (true) {
      switch (app_state) {
      case STATE_START:
        gpio_set(1, 0);
        _bis_SR_register(LPM3_bits + GIE);
        app_state = STATE_INFER;
        break;
      case STATE_INFER:
        // reset_stats();

        // Reset idx stats
        for (size_t i = 0; i <= TIME_MAX; i++) {
          up_times[i] = 0;
          recharge_times[i] = 0;
        }
        rep_idx = 0;

        uint16_t old_rng_idx = rng_idx;

        while (rep_idx <= (TIME_MAX >> 1)) {
          result.start_t = get_ticks();
          result.deadline = result.start_t + LATENCY;
          warmup_run(&result);
        }
        result.start_t = get_ticks();
        result.deadline = result.start_t + LATENCY;
        warmup_run(&result);
        rng_idx = old_rng_idx;

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
