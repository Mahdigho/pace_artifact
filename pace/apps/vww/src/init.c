#include "dnn/include/dnn.h"
#include "include/app.h"
#include "include/reporting.h"
#include "include/uptime.h"
#include "libmspio/uartio.h"
#include <libmsp/mspbase.h>

// -------------- INITIALIZATION ------------------- //
#if REPORT_STATS
__nv bool check_first_boot = false;
__nv bool init_wait_high = false;
__nv bool cont_power = true;
#else
__nv bool check_first_boot = true;
__nv bool init_wait_high = true;
__nv bool cont_power = false;
#endif

void pin_init() {
  gpio_clear(1, 1);
  gpio_clear(1, 0);
  gpio_dir_out(1, 1);
  gpio_dir_out(1, 0);
}

void other_init() {
  dnn_init();
#if REPORT_STATS
  uartio_open(0);
  __delay_cycles(10000);
#endif
}

// -------------- Wake Up & Sleep Responsibilties ------------------- //

__nv uint32_t recharge_time;
__nv uint32_t up_time;
__nv uint32_t prev_start_ticks;
__nv uint32_t prev_henergy_ticks;

void low_energy_sequence() {
  // Turn these down so it doesn't hard crash
  gpio_clear(1, 0);
  gpio_clear(1, 1);
}

/* Timeline of the ticks in a soft wake up.
 * 1. prev_henergy_ticks
 * 2. prev_start_ticks, which may be the previous henergy_ticks or the starting
 *                      ticks of inference
 * 2. lenergy_ticks
 * 3. pre_ckpt_ticks
 * 4. post_ckpt_ticks
 * 5. henergy_ticks
 *
 * UpTime is between prev_henergy_ticks and pre_ckpt
 * RecTime is between post_ckpt and henergy
 * Ckpt time is between pre and post ckpt
 */
__nv uint16_t overhead_count = 0;
void wakeup_sequence(uint8_t s) {
  if (s == WAKE_SOFT) {
    uint32_t pre_ckpt_corrected = pre_ckpt_ticks;
    if (overhead_count == 7) {
      overhead_count = 0;
      pre_ckpt_corrected -= 3;
    } else {
      overhead_count += 1;
      pre_ckpt_corrected -= 2;
    }
    // Ticks are in 8192 per second
    recharge_time = henergy_ticks - post_ckpt_ticks;
    up_time = pre_ckpt_corrected - prev_henergy_ticks;
    update_series(recharge_time, up_time);

    if (inf_happening) {
      update_lookup(recharge_time, up_time);
      update_report_times(recharge_time, up_time);
      inference_rt += recharge_time;
      inference_up += pre_ckpt_corrected - prev_start_ticks;
      inference_ckpt += post_ckpt_ticks - pre_ckpt_corrected;
    }

    prev_start_ticks = henergy_ticks;
    prev_henergy_ticks = henergy_ticks;

    if (app_state == STATE_COMMU) {
      // Do not count wake ups in this state in the light restore count
      light_restore_count -= 1;
    }
  } else {
    // reset the series collection on a hard reset
    recharge_time = -1;
    up_time = -1;
    reset_series();

    if (app_state == STATE_COMMU) {
      // If hardreset, resume from beginning
      app_state = STATE_START;
    }
  }
}