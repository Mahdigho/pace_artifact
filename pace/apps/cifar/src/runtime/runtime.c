#include "include/runtime.h"
#include "_checkpoint.h"
#include "_energy.h"
#include "_scheduler.h"
#include "include/defs.h"
#include "librng/rng.h"
#include <libmspio/uartio.h>

#define MAGIC_BOOT 0x3234
extern __nv uint16_t chkpt_complete;
__nv uint16_t first_boot = 0;
__nv uint16_t on_from_burn_out = 0;
extern __nv uint16_t ldet_count;
extern __nv uint16_t hdet_count;

static inline void first_boot_action() {
  if (first_boot != MAGIC_BOOT) {
    on_from_burn_out -= 1;
  }
  first_boot = MAGIC_BOOT;
  _enable_interrupts();
  // NOTE: Ideally the application should run forever.
  // But if it ends for some reason(??) we do not want a broken checkpoint.
  while (true) {
    application();
  }
}
static inline void resume_action() {
  ckpt_restore_cpu();
  // The above shouldn't if all went well, but if ckpt was broken
  // spin + emergency blink
  gpio_dir_out(1, 0);
  gpio_clear(1, 0);
  while (1) {
    gpio_toggle(1, 0);
    __delay_cycles(500000);
    gpio_toggle(1, 0);
    __delay_cycles(100000);
  };
}

static inline void report_action() {
  // Check if user wants a report instead of running the application.
  gpio_dir_in(POWER_REP_PORT, POWER_REP_PIN);
  if (gpio_read(POWER_REP_PORT, POWER_REP_PIN)) {
    // setup the clock as it was not setup previously
    clock_init();
    uartio_open(0);
    __delay_cycles(10000);
    msp_printf("STATUS %x, CHKP_MADE? %u\n\r", ckpt_status, chkpt_complete);
    msp_printf("DETECT T:L:H %u, %u, %u\n\r", totaldet, ldet, hdet);
    msp_printf("ASKED L:H %u, %u\n\r", ldet_count, hdet_count);
    report();
    while (true) {
      LPM4;
    }
  }
  gpio_dir_out(POWER_REP_PORT, POWER_REP_PIN);
}

int main() {
  // Basic things to do all the time
  watchdog_disable();
  gpio_init_all_ports();

  // initilize pins first
  energy_setup();
  pin_init();

  gpio_unlock();
  gpio_clear_interrupts();

  // uncomment consistent, reproducible seed
  // pcg_state = BASIC_SEED;
  
  // Check whether it should report.
  report_action();

  // Sleep until energy is as high as it needs to be.
  _disable_interrupts();
  if (init_wait_high || !cont_power) {
    wait_high_energy(MAX_THRESH);
    _bis_SR_register(LPM3_bits + GIE);
  }
  on_from_burn_out += 1;

  // initlize more time consuming aspects
  tick_counter_init(true);
  tick_counter_start();
  henergy_ticks = 0;
  lenergy_ticks = 0;
  other_init();

  // uncomment for debuging
  // uartio_open(0);
  // __delay_cycles(10000);

  // Disable interrupts so nothing breaks the process
  // each option should enable interrupts on their own
  _disable_interrupts();
  if (first_boot == MAGIC_BOOT && check_first_boot && !cont_power) {
    resume_action();
  } else {
    // starting program, start checkpoint process
    if (!cont_power) {
      ckpt_init();
    }
    first_boot_action();
  }
}
