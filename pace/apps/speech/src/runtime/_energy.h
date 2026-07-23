#ifndef __CHKPT_INTERM_ECOMP
#define __CHKPT_INTERM_ECOMP

#include "include/defs.h"
#include <libmsp/mspbase.h>
#include <msp430.h>

extern __nv uint16_t hdet;
extern __nv uint16_t ldet;
extern __nv uint16_t totaldet;
extern __nv uint32_t henergy_ticks;
extern __nv uint32_t lenergy_ticks;
extern __nv uint16_t ldet_count;
extern __nv uint16_t hdet_count;
extern __nv uint32_t lcap_read;
extern __nv uint32_t hcap_read;

#define CAP_ROUND(VOLTS) ((VOLTS + 0b10) & (~(0b11)))

/* Usage Notes:
 * 1. Enable interrupts after using these, they do not enable interrupts on
 *    their own.
 * 2. In general, we want to immidietly fall asleep after setting a HIGH
 *    threshold (as we're recharing). Do it ASAP.
 * 3. When setting comp LOW, we want to enable interrupts ASAP to prevent too
 *    much energy usage.
 * 4. The VCap pin can be used for both ADC and COMP. Feel free to use read_cap
 *    to see the actual value being read.
 */

#define NUM_LEVEL 35

extern __ro_hinv const uint16_t level_to_volt[NUM_LEVEL];
extern __ro_hinv const uint16_t level_to_reg[NUM_LEVEL];

/*
 * Set Interrupt for lower or upper threshold.
 * In both modes, comparator is setup such that
 * V- < V+ --> CEOUT = 1
 * V- > V+ --> CEOUT = 0
 * CEIFG  raised when CEOUT: 0 --> 1
 * CEIIFG raised when CEOUT: 1 --> 0
 *
 * Detect low energy, i.e. when CapPower < Ref.
 *    Raise interrupt on CEOUT risising edge
 *    V-: Vcap
 *    V+: Vref
 *    ----> Starts with Vcap > Vref --> CEOUT = 0
 *    ----> Ends with   Vcap < Vref --> CEOUT = 1
 *    ----> Interrupt raised.
 *
 * Detect High Energy, i.e. CapPower > Ref.
 *    Raise interrupt on CEOUT falling edge (via inversion int)
 *    V-: Vcap
 *    V+: Vref
 *    ----> Starts with Vcap < Vref --> CEOUT = 1
 *    ----> Ends with   Vcap > Vref --> CEOUT = 0
 *    ----> Interrupt raised.
 *
 * Operation:
 * 1. Port is set to V+ or V-
 * 2. CEREFLx should be set to zero first then moved to value (based on TI).
 * 3. Ref is set to opposite of port for V+ or V- with reference ladder.
 * 4. Turn on
 * 5. Interrupt vector is cleaned, Power Mode low (CEPWRMD_2)
 * 6. Wait for settling
 * 6. Clean interrupts (to clean settling).
 * 7. if current state is equal to end state (i.e. no switching will happen),
 *       software set the interrupt. Else, just set the interrupt.
 * 8. Sleep and enable GIE. --> To be done by user.
 */
static inline __attribute__((always_inline)) void _COMP_BASE(uint16_t level) {
  CECTL1 &= ~CEON;
  CEINT = 0;
  CECTL0 = CEIMEN | STICH(CEIMSEL_, COMP_ANALOG_NUM);
  CECTL2 = CEREFL_0;
  CECTL2 = CERS_2 | level_to_reg[level];
  CECTL1 = CEPWRMD_2 | CEON;
  CEINT = 0;
  while (!(CEINT & CERDYIFG))
    ;
}

static inline __attribute__((always_inline)) void
energy_det_low(uint16_t level) {
  ldet_count++;
  _COMP_BASE(level);
  if ((CECTL1 & CEOUT)) {
    CEINT = CEIE | CEIFG;
  } else {
    CEINT = CEIE;
  }
}

static inline __attribute__((always_inline)) void
energy_det_high(uint16_t level) {
  hdet_count++;
  _COMP_BASE(level);
  if ((CECTL1 & CEOUT)) {
    CEINT = CEIIE;
  } else {
    CEINT = CEIIE | CEIIFG;
  }
}

/* Setup comparator/adc Cap pin --> Pin 1.5 w/ disabled buffer */
static inline __attribute__((always_inline)) void energy_setup() {
  comp_gpio(COMP_PORT, COMP_PIN);
  adc_gpio(ADC_PORT, ADC_PIN);
  CECTL3 |= STICH(CEPD, COMP_ANALOG_NUM);
}

#endif // __CHKPT_INTERM_ECOMP
