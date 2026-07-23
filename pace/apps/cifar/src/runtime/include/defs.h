#ifndef __CKPT_INTERM_DEFS
#define __CKPT_INTERM_DEFS
#include <msp430.h>
/*
 * These variables are board/configuration specific and are used by both the
 * runtime and applications. Adjust as needed based on your use case and
 * configuration.
 */

// Which MSP clock should be used as the internal timer/oscillator
#define TICK_TIMER 0

// Which MSP pin should be used by the comparator (needed for checkpointing)
#define COMP_PIN 5
#define COMP_PORT 1
#define COMP_ANALOG_NUM 5

// Which MSP pin should be used by the runtime adc (needed for checkpointing and
// tracking).
#define ADC_PIN 1
#define ADC_PORT 3
#define ADC_ANALOG_NUM 13

// Which MSP pin is used to determine whether board is in report MODE (runs the
// report function) or application mode (runs the application function)
#define POWER_REP_PORT 1
#define POWER_REP_PIN 2

// Flags used by the checkpointing strategy
#define CKPT_STAT_RCPU BIT8
#define CKPT_STAT_RLEA BIT9
#define CKPT_STAT_RDMA BITA

// What the checkpointing runtime should checkpoint (any combination of cpu,
// dma, lea) for normal applications, these should remain as is; only alter for
// debugging
#define CKPT_STAT_RDO (CKPT_STAT_RDMA | CKPT_STAT_RCPU | CKPT_STAT_RLEA)

// Capacitor configuration; we had multiple configurations that we adjusted on
// as needed. There are several primary values used by the runtime/applications.
//
// First are the thresholds for high and low capacitor voltages. The high
// voltage (MAX_THRESH) determines when to wake up and the low voltage
// (MIN_THRESH) determines when to sleep. Threshold enums are indices that
// correspond to register configurations for the comparator. Note that the
// comparator works with a hysterisis, so these thresholds are approximate and
// not allways 100% accurate. Additionally, we set MIN_ and MAX_CAP_VALUE to be
// the average minimum and maximum capcitor voltage (approx. the middle range of
// the hysterisis).
//
// Next set of values are the minimum and maximum estimated capacitor energy
// (MIN_,MAS_ESQ). These values are relative to the capacitor value and can be
// used by the application to know how much energy is left (relatively). For
// simplicity, we use the square of the MIN_ and MAX_CAP_VALUE.
// NOTE: It is important for these values to remain relative to capacitor value,
// otherwise PACE cannot operate under capacitor degradation.
//
enum voltage {
  VL_520 = 0,
  VL_555 = 1,
  VL_595 = 2,
  VL_635 = 3,
  VL_680 = 4,
  VL_710 = 5,
  VL_750 = 6,
  VL_790 = 7,
  VL_825 = 8,
  VL_860 = 9,
  VL_900 = 10,
  VL_940 = 11,
  VL_970 = 12,
  VL_1010 = 13,
  VL_1050 = 14,
  VL_1090 = 15,
  VL_1145 = 16,
  VL_1205 = 17,
  VL_1270 = 18,
  VL_1340 = 19,
  VL_1400 = 20,
  VL_1460 = 21,
  VL_1520 = 22,
  VL_1585 = 23,
  VL_1645 = 24,
  VL_1710 = 25,
  VL_1780 = 26,
  VL_1845 = 27,
  VL_1910 = 28,
  VL_1965 = 29,
  VL_2040 = 30,
  VL_2095 = 31,
  VL_2180 = 32,
  VL_2280 = 33,
  VL_2350 = 34,
  VH_555 = 0,
  VH_595 = 1,
  VH_635 = 2,
  VH_680 = 3,
  VH_710 = 4,
  VH_750 = 5,
  VH_790 = 6,
  VH_825 = 7,
  VH_860 = 8,
  VH_900 = 9,
  VH_940 = 10,
  VH_970 = 11,
  VH_1010 = 12,
  VH_1050 = 13,
  VH_1090 = 14,
  VH_1145 = 15,
  VH_1205 = 16,
  VH_1270 = 17,
  VH_1340 = 18,
  VH_1400 = 19,
  VH_1460 = 20,
  VH_1520 = 21,
  VH_1585 = 22,
  VH_1645 = 23,
  VH_1710 = 24,
  VH_1780 = 25,
  VH_1845 = 26,
  VH_1910 = 27,
  VH_1965 = 28,
  VH_2040 = 29,
  VH_2095 = 30,
  VH_2180 = 31,
  VH_2280 = 32,
  VH_2350 = 33,
  VH_2410 = 34,
};

#define CAP_CONFIG_330uF 1
#define CAP_CONFIG_1mF 2
#define CAP_CONFIG_1mFBig 3

// choose a capacitor config
#define CAP_CONFIG CAP_CONFIG_1mF
// #define CAP_CONFIG CAP_CONFIG_1mFBig
// #define CAP_CONFIG CAP_CONFIG_330uF

#if CAP_CONFIG == CAP_CONFIG_330uF
#define MAX_THRESH VH_1340
#define MAX_CAP_VALUE 1417
#define MIN_THRESH VL_1205
#define MIN_CAP_VALUE 1292
#elif CAP_CONFIG == CAP_CONFIG_1mF
#define MAX_THRESH VH_1340
#define MAX_CAP_VALUE 1417
#define MIN_THRESH VL_1205
#define MIN_CAP_VALUE 1292
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
#define MAX_THRESH VH_1520
#define MAX_CAP_VALUE 1417
#define MIN_THRESH VL_1205
#define MIN_CAP_VALUE 1292
#endif

#if CAP_CONFIG == CAP_CONFIG_330uF
#define MIN_ESQ 1630 // 1292 * 1292 / 1024
#define MAX_ESQ 1960 // 1417 * 1417 / 1024
#elif CAP_CONFIG == CAP_CONFIG_1mF
#define MIN_ESQ 1630 // 1292 * 1292 / 1024
#define MAX_ESQ 1960 // 1417 * 1417 / 1024
#elif CAP_CONFIG == CAP_CONFIG_1mFBig
#define MIN_ESQ 1630 // 1290 * 1292 / 1024
#define MAX_ESQ 2730 // 1672 * 1672 / 1024
#endif

#define DELTA_ENERGY (MAX_ESQ - MIN_ESQ)
#define DELTA_E_SHIFT 10

#endif // !__CKPT_INTERM_DEFS
