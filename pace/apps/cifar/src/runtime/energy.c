#include "_clock.h"
#include "_energy.h"
#include "_scheduler.h"

__nv uint16_t hdet = 0;
__nv uint16_t ldet = 0;
__nv uint16_t totaldet = 0;
__nv uint32_t henergy_ticks = 0;
__nv uint32_t lenergy_ticks = 0;
__nv uint16_t ldet_count = 0;
__nv uint16_t hdet_count = 0;
__nv uint32_t lcap_read = 0;
__nv uint32_t hcap_read = 0;

// voltage (mV) = level_to_vol[idx] * 100
__ro_hinv const uint16_t level_to_volt[NUM_LEVEL] = {
    1006, 1020, 1055, 1067, 1093, 1102, 1128,
    1166, 1182, 1192, 1255, 1317, 1336, 1700};
/* The comp_e has two resistive laders.
 * REF0 is used when CEOUT = 0
 * REF1 is used when CEOUT = 1
 * This way a hysterisis can be set.
 * Since we have that Vcap: V- and Vref: V+ -> CEOUT : V- < V+
 * Then to have a hysterisis, we want something like
 *  STATUS of CEOUT    --> 1   1   1   1   0   0  0
 *  Increasing voltage -->               VREF1
 *
 *  STATUS of CEOUT    --> 1   0   0   0   0   0  0
 *  Decreasing voltage -->   VREF0
 *
 *  Hence, REF0 will always be smaller than REF1.
 *  The following list entails the value of VREF0 -- VREF1.
 *  VREF0 is the voltage where 0 becomes 1.
 *  VREF1 is the voltage where 1 becomes 0.
 */
__ro_hinv const uint16_t level_to_reg[NUM_LEVEL] = {
    CEREFL__1P2V | CEREF0_14 | CEREF1_15, /*  520 --  555 */
    CEREFL__1P2V | CEREF0_15 | CEREF1_16, /*  555 --  595 */
    CEREFL__1P2V | CEREF0_16 | CEREF1_17, /*  595 --  635 */
    CEREFL__1P2V | CEREF0_17 | CEREF1_18, /*  635 --  680 */
    CEREFL__1P2V | CEREF0_18 | CEREF1_19, /*  675 --  710 */
    CEREFL__1P2V | CEREF0_19 | CEREF1_20, /*  710 --  750 */
    CEREFL__1P2V | CEREF0_20 | CEREF1_21, /*  750 --  790 */
    CEREFL__1P2V | CEREF0_21 | CEREF1_22, /*  790 --  825 */
    CEREFL__1P2V | CEREF0_22 | CEREF1_23, /*  825 --  860 */
    CEREFL__1P2V | CEREF0_23 | CEREF1_24, /*  860 --  900 */
    CEREFL__1P2V | CEREF0_24 | CEREF1_25, /*  900 --  940 */
    CEREFL__1P2V | CEREF0_25 | CEREF1_26, /*  940 --  970 */
    CEREFL__1P2V | CEREF0_26 | CEREF1_27, /*  970 -- 1010 */
    CEREFL__1P2V | CEREF0_27 | CEREF1_28, /* 1010 -- 1050 */
    CEREFL__1P2V | CEREF0_28 | CEREF1_29, /* 1050 -- 1090 */
    CEREFL__1P2V | CEREF0_29 | CEREF1_30, /* 1090 -- 1145 */
    CEREFL__2P0V | CEREF0_18 | CEREF1_19, /* 1145 -- 1205 */
    CEREFL__2P0V | CEREF0_19 | CEREF1_20, /* 1205 -- 1270 */
    CEREFL__2P0V | CEREF0_20 | CEREF1_21, /* 1270 -- 1340 */
    CEREFL__2P0V | CEREF0_21 | CEREF1_22, /* 1340 -- 1400 */
    CEREFL__2P0V | CEREF0_22 | CEREF1_23, /* 1400 -- 1460 */
    CEREFL__2P0V | CEREF0_23 | CEREF1_24, /* 1460 -- 1520 */
    CEREFL__2P0V | CEREF0_24 | CEREF1_25, /* 1520 -- 1585 */
    CEREFL__2P0V | CEREF0_25 | CEREF1_26, /* 1585 -- 1645 */
    CEREFL__2P0V | CEREF0_26 | CEREF1_27, /* 1645 -- 1710 */
    CEREFL__2P0V | CEREF0_27 | CEREF1_28, /* 1710 -- 1780 */
    CEREFL__2P0V | CEREF0_28 | CEREF1_29, /* 1780 -- 1845 */
    CEREFL__2P0V | CEREF0_29 | CEREF1_30, /* 1845 -- 1910 */
    CEREFL__2P0V | CEREF0_30 | CEREF1_31, /* 1910 -- 1965 */
    CEREFL__2P5V | CEREF0_25 | CEREF1_26, /* 1965 -- 2040 */
    CEREFL__2P5V | CEREF0_26 | CEREF1_27, /* 2040 -- 2095 */
    CEREFL__2P5V | CEREF0_27 | CEREF1_28, /* 2095 -- 2180 */
    CEREFL__2P5V | CEREF0_28 | CEREF1_29, /* 2180 -- 2280 */
    CEREFL__2P5V | CEREF0_29 | CEREF1_30, /* 2280 -- 2350 */
    CEREFL__2P5V | CEREF0_30 | CEREF1_31, /* 2350 -- 2410 */
};

/* returns the voltage in mili volts */
uint16_t read_cap() {
  uint16_t int_state;
  NO_INTERRUPT(int_state);
  ADC12CTL0 &= ~ADC12ENC;
  ref_busy();
  ref_start(20);
  // The following are defaults. Uncomment if something needs to be changed
  // ADC12MCTL2 = ADC12RES__12BIT
  // ADC12CTL2 |= ADC12CSTARTADD__ADC12MEM0;
  //  TODO: Use SMCLK instead of MODCLK?
  ADC12CTL1 = ADC12SHP; // smaple and hold
  ADC12MCTL0 = ADC12VRSEL_1 |
               STICH(ADC12INCH_, ADC_ANALOG_NUM); // VR+ = VREF, VR- = AVSS
  ADC12CTL0 |= ADC12SHT0_2 | ADC12ON; // sample and hold 256 clk, start

  ref_ready(); // check the ref
  ADC12CTL0 |= ADC12ENC | ADC12SC;
  adc_busy();

  uint16_t output = ADC12MEM0;

  ADC12CTL0 &= ~(ADC12ON | ADC12ENC);
  ref_stop();

  // Vcap = output * 2000 / 4096
  // * 2000 due to reference voltage
  // / 4096 due to 12 bit accuracy
  // Experimentally, the adc is tuned for the 900-1200 mV region, +- 10mV
  // output = output * 0.488;
  output = output >> 1;
  RESUME_INTERRUPT(int_state);
  return output;
}

__attribute__((interrupt(COMP_E_VECTOR))) void COMP_VBANK_ISR(void) {
  uint16_t int_val = CEINT;
  CEINT = 0;
  totaldet += 1;
  __disable_interrupt();
  if ((int_val & CEIIFG)) {
    // TODO: The CEIIFG has a chance to misfire as it settles?!!, so check the
    // adc?
    hdet += 1;
    henergy_ticks = get_ticks();
    hcap_read = read_cap();
    // round it off
    high_energy_detected();
  } else if ((int_val & CEIFG)) {
    ldet += 1;
    lenergy_ticks = get_ticks();
    lcap_read = read_cap();
    low_energy_detected();
  }
  _bis_SR_register_on_exit(GIE);
}
