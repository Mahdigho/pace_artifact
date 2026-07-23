#ifndef __LIBDNN_LEA_KERNELS_H
#define __LIBDNN_LEA_KERNELS_H
#include "include/dnn.h"
#include "lea.h"
#include "msp430fr5994.h"
#include <libmsp/dma.h>

/* Quick inlines for the commonly used lea command and such.
 * NOTE: None of the functions check or align the length to be power of 2.
 * That's up to the user.
 *
 */

static inline void __invoke_lea(void *pms0, void *pms1, uint16_t command) {
  /* Load source arguments to LEA. */
  LEAPMS0 = MSP_LEA_CONVERT_ADDRESS(pms0);
  LEAPMS1 = MSP_LEA_CONVERT_ADDRESS(pms1);

  /* Invoke the LEACMD__ADDMATRIX command. */
  msp_lea_invokeCommand(command);
}

static inline void __set_mpy_params(_q15 *input2, _q15 *output,
                                    uint16_t length) {
  lea_mpy_params.vectorSize = length;
  lea_mpy_params.input1Offset = 1;
  lea_mpy_params.input2Offset = 1;
  lea_mpy_params.outputOffset = 1;
  lea_mpy_params.input2 = MSP_LEA_CONVERT_ADDRESS(input2);
  lea_mpy_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __mpy_q15(_q15 *input) {
  __invoke_lea(input, &lea_mpy_params, LEACMD__MPYMATRIX);
}

static inline void __set_const_mpy_params(_q15 *output, uint16_t length) {
  lea_const_mpy_params.vectorSize = length;
  lea_const_mpy_params.input1Offset = 1;
  lea_const_mpy_params.input2Offset = 0;
  lea_const_mpy_params.outputOffset = 1;
  lea_const_mpy_params.input2 =
      MSP_LEA_CONVERT_ADDRESS((_q15 *)&lea_const_mpy_vector);
  lea_const_mpy_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __const_mpy_q15(_q15 *input, _q15 value) {
  lea_const_mpy_vector[0] = value;
  lea_const_mpy_vector[1] = value;
  __invoke_lea(input, &lea_const_mpy_params, LEACMD__MPYMATRIX);
}

static inline void __set_const_rshift_params(_q15 *output, uint16_t length) {
  lea_const_rshift_params.vectorSize = length;
  lea_const_rshift_params.input1Offset = 1;
  lea_const_rshift_params.input2Offset = 0;
  lea_const_rshift_params.outputOffset = 1;
  lea_const_rshift_params.input2 =
      MSP_LEA_CONVERT_ADDRESS((_q15 *)&lea_const_rshift_vector);
  lea_const_rshift_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __const_rshift_q15(_q15 *input, _q15 value) {
  lea_const_rshift_vector[0] = value;
  lea_const_rshift_vector[1] = value;
  __invoke_lea(input, &lea_const_rshift_params, LEACMD__MPYMATRIX);
}

static inline void __set_add_params(_q15 *input2, _q15 *output,
                                    uint16_t length) {
  lea_add_params.vectorSize = length;
  lea_add_params.input1Offset = 1;
  lea_add_params.input2Offset = 1;
  lea_add_params.outputOffset = 1;
  lea_add_params.input2 = MSP_LEA_CONVERT_ADDRESS(input2);
  lea_add_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}

static inline void __add_q15(_q15 *input) {
  __invoke_lea(input, &lea_add_params, LEACMD__ADDMATRIX);
}

static inline void __set_add_self_params(_q15 *input2, uint16_t length) {
  lea_add_self_params.vectorSize = length;
  lea_add_self_params.input1Offset = 1;
  lea_add_self_params.input2Offset = 1;
  lea_add_self_params.outputOffset = 1;
  lea_add_self_params.input2 = MSP_LEA_CONVERT_ADDRESS(input2);
  lea_add_self_params.output = MSP_LEA_CONVERT_ADDRESS(input2);
}

static inline void __add_self_q15(_q15 *input) {
  __invoke_lea(input, &lea_add_self_params, LEACMD__ADDMATRIX);
}

static inline void __set_const_add_params(_q15 *output, uint16_t length) {
  lea_const_add_params.vectorSize = length;
  lea_const_add_params.input1Offset = 1;
  lea_const_add_params.input2Offset = 0;
  lea_const_add_params.outputOffset = 1;
  lea_const_add_params.input2 =
      MSP_LEA_CONVERT_ADDRESS((_q15 *)&lea_const_add_vector);
  lea_const_add_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __const_add_q15(_q15 *input, _q15 value) {
  lea_const_add_vector[1] = value;
  lea_const_add_vector[0] = value;
  __invoke_lea(input, &lea_const_add_params, LEACMD__ADDMATRIX);
}

static inline void __set_fir_params(uint16_t length, uint16_t tapLength,
                                    _q15 *output) {
  lea_fir_params.vectorSize = length;
  lea_fir_params.tapLength = tapLength;
  lea_fir_params.bufferMask = 0xffff;
  lea_fir_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __fir_q15(_q15 *coeffs, _q15 *src) {
  /* Set MSP_LEA_FIR_PARAMS structure. */
  lea_fir_params.coeffs = MSP_LEA_CONVERT_ADDRESS(coeffs);
  __invoke_lea(src, &lea_fir_params, LEACMD__FIR);
}

static inline void __deinter_params(uint16_t length, uint16_t interDepth,
                                    _q15 *output) {
  lea_deinter_params.vectorSize = length;
  lea_deinter_params.interleaveDepth = interDepth;
  lea_deinter_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __deinter(_q15 *input, uint16_t deinter_CMDID) {
  __invoke_lea(input, &lea_deinter_params, deinter_CMDID);
}

static inline void __set_fill_params(_q15 *output, uint16_t length) {
  lea_fill_params.input1Offset = 0;
  lea_fill_params.input2Offset = 0;
  lea_fill_params.outputOffset = 1;
  lea_fill_params.vectorSize = length;
  lea_fill_params.input2 = MSP_LEA_CONST_ZERO;
  lea_fill_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}

static inline void __fill_q15(_q15 value) {
  lea_fill_vector[0] = value;
  lea_fill_vector[1] = value;
  __invoke_lea(&lea_fill_vector, &lea_fill_params, LEACMD__ADDMATRIX);
}

static inline void __set_mpy_matrix_row_params(_q15 *output, _q15 *matrix,
                                               uint16_t matrix_row,
                                               uint16_t matrix_col) {
  lea_mpymatrixrow_params.rowSize = matrix_row;
  lea_mpymatrixrow_params.colSize = matrix_col;
  lea_mpymatrixrow_params.colVector = MSP_LEA_CONVERT_ADDRESS(matrix);
  lea_mpymatrixrow_params.output = MSP_LEA_CONVERT_ADDRESS(output);
}
static inline void __mpy_matrix_row_q15(_q15 *vector) {
  __invoke_lea(vector, &lea_mpymatrixrow_params, LEACMD__MPYMATRIXROW);
}

#endif // !__LIBDNN_LEA_KERNELS_H