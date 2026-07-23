#include "include/dnn.h"
#include "lea.h"
#include <libmsp/dma.h>

// Allocate max possible line
#define MAC_LINE_SIZE (LEA_RAM_SIZE >> 1)
#define _linear_src lea_ram
#define _linear_weights (lea_ram + MAC_LINE_SIZE)

static inline void set_mac_q15() {
  lea_mac_params.input2 = MSP_LEA_CONVERT_ADDRESS(_linear_weights);
  lea_mac_params.output = MSP_LEA_CONVERT_ADDRESS(&mac_res);
  // lea_mac_params.reserved = -32768; // Only used for SCALEDLEA
  LEAPMS0 = MSP_LEA_CONVERT_ADDRESS(_linear_src);
  LEAPMS1 = MSP_LEA_CONVERT_ADDRESS(&lea_mac_params);
}

/* Scaled mac is used as it is less likely to lead to overflow issues. */
static inline void lea_mac(fixed *src_ptr, fixed *weights_ptr,
                           size_t weights_increment, uint16_t line_size,
                           mat_t *dst, int16_t shift) {
  // dst->strides[0] : lenght of ouput

  // Dirty shortcut for getting the upper 16 bits of the result
  fixed *mac_res_ptr = (fixed *)&mac_res;

  // Transfer the input
  FRAM_MOVE(src_ptr, _linear_src, line_size);

  ADDR_FOR(dst_ptr, dst->data, dst->strides[0], 1) {
    // DMA only if using LEA
    FRAM_MOVE(weights_ptr, _linear_weights, line_size);

    // LEA MAC normal
    msp_lea_invokeCommand(LEACMD__MAC);
    if (shift > 0) {
      mac_res <<= shift;
    } else {
      mac_res >>= (-shift);
    }
    // mac_res *= (int32_t)shift;
    *dst_ptr = __saturated_add_q15((mac_res_ptr[1]), *dst_ptr);
    weights_ptr += weights_increment;
  }
}

void linear(mat_t *src, mat_t *dst, mat_t *weights, mat_t *bias) {
  // Mac multiplies two 16 bit QM.N and QX.Y and produces 32 bit value
  // However, the true result should have been a Q31, so it's multiplied by 2
  // To get to a final QO.P, we need to right shift S = N + Y - P + 1
  // right shift can be made cheap if we multiply by 16 - S instead
  int16_t shift = 16 - (src->p2quant + weights->p2quant + 1 - dst->p2quant);
  // shift = 1 << shift;

  // Parameter values for lea that do not change.
  set_mac_q15();

  // Add the bias by copying into destination
  // bias->dims[0]: bias length
  FRAM_MOVE(bias->data, dst->data, dst->strides[0]);

  // Calculate line size
  // lea_line_size will always be divisble by 2
  // lea_remain_size may not be, there are protections in place for this case
  uint16_t lea_remain_size = src->strides[0] % MAC_LINE_SIZE;

  fixed *weights_ptr = weights->data;
  lea_mac_params.vectorSize = MAC_LINE_SIZE;
  // repeat for line sized regions until the very last
  ADDR_FOR(src_ptr, src->data, src->strides[0] - lea_remain_size,
           MAC_LINE_SIZE) {
    lea_mac(src_ptr, weights_ptr, weights->strides[0], MAC_LINE_SIZE, dst,
            shift);
    weights_ptr += MAC_LINE_SIZE;
  }
  if (lea_remain_size) {
    // if odd, take precautions
    // Also if odd, lea_remain_size < LEA_SRC_SIZE
    if (lea_remain_size & 0x1) {
      _linear_src[lea_remain_size] = 0;
      _linear_weights[lea_remain_size] = 0;
      lea_remain_size += 1;
    }
    lea_mac_params.vectorSize = lea_remain_size;
    lea_mac(src_ptr, weights_ptr, weights->strides[0], lea_remain_size, dst,
            shift);
  }
}