#include "lea.h"

DSPLIB_DATA(lea_ram, 4) _q15 lea_ram[LEA_RAM_SIZE];
DSPLIB_DATA(lea_res, 4) _iq31 mac_res;
DSPLIB_DATA(lea_fir_params, 4) MSP_LEA_FIR_PARAMS lea_fir_params;
DSPLIB_DATA(lea_mac_params, 4) MSP_LEA_MAC_PARAMS lea_mac_params;
DSPLIB_DATA(lea_dinter_params, 4)
MSP_LEA_DEINTERLEAVE_PARAMS lea_deinter_params;

DSPLIB_DATA(lea_fill_params, 4) MSP_LEA_ADDMATRIX_PARAMS lea_fill_params;
DSPLIB_DATA(lea_fill_vector, 4) _q15 lea_fill_vector[2];

DSPLIB_DATA(lea_mpy_params, 4) MSP_LEA_MPYMATRIX_PARAMS lea_mpy_params;
DSPLIB_DATA(lea_const_mpy_params, 4)
MSP_LEA_MPYMATRIX_PARAMS lea_const_mpy_params;
DSPLIB_DATA(lea_const_mpy_vector, 4) _q15 lea_const_mpy_vector[2];

DSPLIB_DATA(lea_const_rshift_params, 4)
MSP_LEA_MPYMATRIX_PARAMS lea_const_rshift_params;
DSPLIB_DATA(lea_const_rshift_vector, 4) _q15 lea_const_rshift_vector[2];

DSPLIB_DATA(lea_add_params, 4) MSP_LEA_ADDMATRIX_PARAMS lea_add_params;
DSPLIB_DATA(lea_const_add_params, 4)
MSP_LEA_ADDMATRIX_PARAMS lea_const_add_params;
DSPLIB_DATA(lea_const_add_vector, 4) _q15 lea_const_add_vector[2];
DSPLIB_DATA(lea_add_self_params, 4)
MSP_LEA_ADDMATRIX_PARAMS lea_add_self_params;

DSPLIB_DATA(lea_mpymatrixrow_params, 4)
MSP_LEA_MPYMATRIXROW_PARAMS lea_mpymatrixrow_params;

void print_range(fixed *src, uint16_t start, uint16_t end, uint16_t cut) {
  uint16_t count = 0;
  while (start != end) {
    msp_printf("%i\t", src[start]);
    start++;
    count++;
    if (count == cut) {
      msp_printf("\n\r");
      count = 0;
    }
  }
  msp_printf("\n\r");
}