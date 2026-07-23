#include "include/reporting.h"
#include "include/app.h"
#include "include/uptime.h"
#include <libmspio/uartio.h>
#include <libmspprintf/mspprintf.h>

// -------------- Reporting Duties ------------------- //
__hinv uint32_t recharge_times[TIME_MAX + 1] = {};
__hinv uint32_t up_times[TIME_MAX + 1] = {};
// __nv uint16_t hcap_volts[TIME_MAX + 1] = {};
__nv uint16_t pre_ckpt_volts[TIME_MAX + 1] = {};
__nv size_t rep_idx = 0;

// Specific Inference Info
__hinv result_t result_data[EXIT_COUNT];
__nv size_t exit_idx;

// General Inference Info
__hinv slumber_res_t taken_slumber[WIDTH_SIZE + 1];
__hinv slumber_res_t taken_lookup_slex[WIDTH_SIZE + 1];
__hinv width_res_t taken_timed_switch[WIDTH_SIZE + 1];
__hinv width_res_t taken_timed_exit[MAX_EXIT + 1];
__hinv width_res_t taken_zygarde[MAX_EXIT + 1];
__hinv width_res_t taken_eperceptive[MAX_EXIT + 1];
__hinv uint32_t success_count;
__hinv uint32_t attempt_count;
__hinv uint32_t attempt_adjusted;
__hinv uint32_t success_adjusted;

void print_result(uint16_t idx, result_t *r) {
  uint32_t diff = 0;
  uint32_t runtime = r->exit_t - r->start_t;
  int16_t sign = 0;
  if (runtime < r->est_late) {
    diff = r->est_late - runtime;
    sign = 1;
  } else {
    diff = runtime - r->est_late;
    sign = -1;
  }

  if (r->made_it) {
    msp_printf("%02x :||||||", idx);
  } else {
    msp_printf("%02x :------", idx);
  }

  msp_printf(" Any %u."
             "\tPlan: W -E %u-%u"
             "\tReal: ll-E %u-%u"
             "\tInit: e -T %u-%u"
             "\tEst : rt-up-l-8R %n-%n-%n-%u"
             "\tReal: late- R - diff   %n - %u, %i * %n"
             "\tHist: Rt-Up-Ck-Tot %n-%n-%n-%n"
             "\tNeed: %n"
             "\tDEADLINE: %n"
             "\n\r",
             (uint16_t)r->anytime, (uint16_t)r->width, (uint16_t)r->exit_goal,
             (uint16_t)r->last_layer, (uint16_t)r->exit_achieved, (r->i_energy),
             (r->i_time), r->est_rt, r->est_up, (r->est_late), r->est_R,
             runtime, (uint16_t)r->took_R, sign, diff, (r->took_rt >> 0),
             (r->took_up >> 0), (r->took_ckpt >> 0),
             (r->took_ckpt + r->took_rt + r->took_up) >> 0, r->needed_up,
             (r->deadline - r->start_t) >> 3);
}

void print_u16range(uint16_t *src, uint16_t start, uint16_t end, uint16_t cut) {
  uint16_t count = 0;
  while (start != end) {
    msp_printf(" %u,", src[start]);
    start++;
    count++;
    if (count == cut) {
      msp_printf("\n\r");
      count = 0;
    }
  }
  msp_printf("\n\r");
}

void print_u32range(uint32_t *src, uint16_t start, uint16_t end, uint16_t cut) {
  uint16_t count = 0;
  while (start != end) {
    msp_printf(" %n,", src[start]);
    start++;
    count++;
    if (count == cut) {
      msp_printf("\n\r");
      count = 0;
    }
  }
  msp_printf("\n\r");
}

void print_s32range(int32_t *src, uint16_t start, uint16_t end, uint16_t cut) {
  uint16_t count = 0;
  while (start != end) {
    msp_printf(" %l,", src[start]);
    start++;
    count++;
    if (count == cut) {
      msp_printf("\n\r");
      count = 0;
    }
  }
  msp_printf("\n\r");
}

void general_report() {
  msp_printf("----------------------\n\r");
  msp_printf("Reporting\n\r");
  msp_printf("Woke up from burnout %u\n\r", on_from_burn_out);
  msp_printf("Heavy Restore count %u\n\r", heavy_restore_count);
  msp_printf("Light Restore count %u\n\r", light_restore_count);
  msp_printf("Failed to light count %u\n\r", failed_to_light);

  msp_printf("----------------------\n\r");
  msp_printf("Attempt: %u\n\r", attempt_count);
  msp_printf("Success: %u\n\r", success_count);
  msp_printf("Attempt Adjusted: %u\n\r", attempt_adjusted);
  msp_printf("Success Adjusted: %u\n\r", success_adjusted);
}

void result_report(size_t count) {
  msp_printf("----------------------\n\r");
  if (count > EXIT_COUNT || count == 0) {
    count = EXIT_COUNT;
  }
  for (size_t e; e < count; e++) {
    print_result(e, result_data + e);
  }
}

void exp_report() {
  msp_printf("----------------------\n\r");
  uint16_t zygarde_fails = 0;
  uint16_t epercetive_fails = 0;
  uint16_t timed_ee_fails = 0;
  uint16_t slumber_fails = 0;
  // uint16_t timed_slim_fails = 0;
  uint16_t lookup_slex_fails = 0;

#if EPERCEPTIVE
  msp_printf("----------------------\n\r");
  msp_printf("Eperceptive\n\rExit,Fail,Success\n\r[0, %u, %u]\n\r",
             taken_eperceptive[MAX_EXIT].fails,
             taken_eperceptive[MAX_EXIT].succeses);
  epercetive_fails += taken_eperceptive[MAX_EXIT].fails;
  for (uint8_t i = 0; i < MAX_EXIT; i++) {
    msp_printf("[%u, %u, %u]\n\r", i + 1, taken_eperceptive[i].fails,
               taken_eperceptive[i].succeses);
    epercetive_fails += taken_eperceptive[i].fails;
  }
#endif

#if TIMED_EE
  msp_printf("----------------------\n\r");
  msp_printf("timed exits\n\rExit,Fail,Success\n\r[0, %u, %u]\n\r",
             taken_timed_exit[MAX_EXIT].fails,
             taken_timed_exit[MAX_EXIT].succeses);
  timed_ee_fails += taken_timed_exit[MAX_EXIT].fails;
  for (uint8_t i = 0; i < MAX_EXIT; i++) {
    msp_printf("[%u, %u, %u]\n\r", i + 1, taken_timed_exit[i].fails,
               taken_timed_exit[i].succeses);
    timed_ee_fails += taken_timed_exit[i].fails;
  }
#endif

#if ZYGARDE
  msp_printf("----------------------\n\r");
  msp_printf("Zygarde\n\rExit,Fail,Success\n\r[0, %u, %u]\n\r",
             taken_zygarde[MAX_EXIT].fails, taken_zygarde[MAX_EXIT].succeses);
  zygarde_fails += taken_zygarde[MAX_EXIT].fails;
  for (uint8_t i = 0; i < MAX_EXIT; i++) {
    msp_printf("[%u, %u, %u]\n\r", i + 1, taken_zygarde[i].fails,
               taken_zygarde[i].succeses);
    zygarde_fails += taken_zygarde[i].fails;
  }
#endif

#if LOOKUP_SLEX
  msp_printf("----------------------\n\r");
  msp_printf("Lookup Slex\n\rFail,Exit1,Exit2,Exit3\n\r");
  msp_printf("[0, %u, %u, %u, %u]\n\r", taken_lookup_slex[WIDTH_SIZE].fails,
             taken_lookup_slex[WIDTH_SIZE].exit1,
             taken_lookup_slex[WIDTH_SIZE].exit2,
             taken_lookup_slex[WIDTH_SIZE].exit3);

  lookup_slex_fails += taken_lookup_slex[WIDTH_SIZE].fails;
  for (uint8_t i = 0; i < WIDTH_SIZE; i++) {
    msp_printf("[%u, %u, %u, %u, %u]\n\r", i + MIN_WIDTH,
               taken_lookup_slex[i].fails, taken_lookup_slex[i].exit1,
               taken_lookup_slex[i].exit2, taken_lookup_slex[i].exit3);
    lookup_slex_fails += taken_lookup_slex[i].fails;
  }
#endif

#if SLUMBER
  msp_printf("----------------------\n\r");
  msp_printf("Slumber\n\rFail,Exit1,Exit2,Exit3\n\r");
  msp_printf("[0, %u, %u, %u, %u]\n\r", taken_slumber[WIDTH_SIZE].fails,
             taken_slumber[WIDTH_SIZE].exit1, taken_slumber[WIDTH_SIZE].exit2,
             taken_slumber[WIDTH_SIZE].exit3);

  slumber_fails += taken_slumber[WIDTH_SIZE].fails;
  for (uint8_t i = 0; i < WIDTH_SIZE; i++) {
    msp_printf("[%u, %u, %u, %u, %u]\n\r", i + MIN_WIDTH,
               taken_slumber[i].fails, taken_slumber[i].exit1,
               taken_slumber[i].exit2, taken_slumber[i].exit3);
    slumber_fails += taken_slumber[i].fails;
  }
#endif

  msp_printf("----------------------\n\r");
  msp_printf(
      "EnergyTrace,EMode,Latency,CapMode,Dataset,InfMode,Failed,S2E1,S2E2,"
      "S2E3,S3E1,S3E2,S3E3,S4E1,S4E2,S4E3,S5E1,S5E2,S5E3,S6E1,S6E2,S6E3,"
      "S7E1,S7E2,S7E3,S8E1,S8E2,S8E3,S9E1,S9E2,S9E3,S10E1,S10E2,S10E3\n\r");

#if EPERCEPTIVE
  msp_printf(",,,,SPEECH,eperceptive,%u,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"
             "0,0,0,0,",
             epercetive_fails);
  for (uint8_t i = 0; i < MAX_EXIT; i++) {
    msp_printf("%u,", taken_eperceptive[i].succeses);
  }
  msp_printf("\n\r");
#endif

#if TIMED_EE
  msp_printf(
      ",,,,SPEECH,sched_ee,%u,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,",
      timed_ee_fails);
  for (uint8_t i = 0; i < MAX_EXIT; i++) {
    msp_printf("%u,", taken_timed_exit[i].succeses);
  }
  msp_printf("\n\r");
#endif

#if ZYGARDE
  msp_printf(",,,,SPEECH,zygarde,%u,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"
             "0,0,0,0,0,",
             zygarde_fails);
  for (uint8_t i = 0; i < MAX_EXIT; i++) {
    msp_printf("%u,", taken_zygarde[i].succeses);
  }
  msp_printf("\n\r");
#endif

#if LOOKUP_SLEX
  msp_printf(",,,,SPEECH,lookup_slex,%u,", lookup_slex_fails);
  for (uint8_t i = 0; i < WIDTH_SIZE; i++) {
    msp_printf("%u,%u,%u,", taken_lookup_slex[i].exit1,
               taken_lookup_slex[i].exit2, taken_lookup_slex[i].exit3);
  }
  msp_printf("\n\r");
#endif

#if SLUMBER
  msp_printf(",,,,SPEECH,SLUMBER,%u,", slumber_fails);
  for (uint8_t i = 0; i < WIDTH_SIZE; i++) {
    msp_printf("%u,%u,%u,", taken_slumber[i].exit1, taken_slumber[i].exit2,
               taken_slumber[i].exit3);
  }
  msp_printf("\n\r");
#endif
}

void runtime_report() {
  msp_printf("----------------------\n\r");
  msp_printf("LATEST SERIES IDX %u\n\r", rep_idx);
  print_u32range(recharge_times, 0, TIME_MAX + 1, TIME_MAX + 1);
  print_u32range(up_times, 0, TIME_MAX + 1, TIME_MAX + 1);
  print_u32range(tick_series, 0, 16, 16);
  print_lookup();
}

void mat_report() {
  msp_printf("----------------------\n\r");
  MAT_DUMP(&mat_out, 0);
}

void report() {

  // Connected to continous power, report
  gpio_dir_out(1, 1);
  gpio_set(1, 1);
  uartio_open(0);
  __delay_cycles(10000);

  general_report();
  result_report(0);
  exp_report();
  runtime_report();
  mat_report();

  gpio_dir_out(1, 0);
  gpio_set(1, 0);
  uartio_close(0);
}
