#include "include/dnn.h"
#include "lea.h"
#include "lea_kernels.h"
#include "libdsp/DSPLib_support.h"
#include <libmsp/dma.h>
#include <stdint.h>

#define ADD_LINE_SIZE (LEA_RAM_SIZE >> 1)
// #define add_LINE_SIZE 32
#define _lea_add_sr0 lea_ram
#define _lea_add_sr1 (lea_ram + 1 * ADD_LINE_SIZE)
#define _lea_add_dst (lea_ram + 1 * ADD_LINE_SIZE)

static inline void _add_vectors() { __add_q15(_lea_add_sr0); }
static inline void _set_add_vectors() {
  __set_add_params(_lea_add_sr1, _lea_add_dst, ADD_LINE_SIZE);
}

/* Shift factor lookup table. */
const uint16_t shift_right_factors[16] = {
    0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100,
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001};

/*
 * matrix/vector point wise addition.
 * the length of addition is picked from sr0.
 */
void mat_add(mat_t *sr0, mat_t *sr1, mat_t *dst) {
  uint16_t length = sr0->strides[0];
  _set_add_vectors();
  fixed *sr0_ptr = sr0->data;
  fixed *sr1_ptr = sr1->data;
  fixed *dst_ptr = dst->data;

  // Right shift one of the inputs so they have the same quant
  // Check which one needs to be right shifted (to match p2quant)
  uint16_t sr0_right_shift = 0;
  uint16_t sr1_right_shift = 0;
  uint16_t min_p2quant = sr0->p2quant;
  if (sr0->p2quant < sr1->p2quant) {
    uint16_t diff = sr1->p2quant - sr0->p2quant;
    sr1_right_shift = shift_right_factors[diff];
  } else if (sr0->p2quant > sr1->p2quant) {
    min_p2quant = sr1->p2quant;
    uint16_t diff = sr0->p2quant - sr1->p2quant;
    sr0_right_shift = shift_right_factors[diff];
  }

  int16_t dst_shift = dst->p2quant - min_p2quant;

  while (length >= ADD_LINE_SIZE) {
    // Move data into proper location
    FRAM_MOVE(sr0_ptr, _lea_add_sr0, ADD_LINE_SIZE);
    FRAM_MOVE(sr1_ptr, _lea_add_sr1, ADD_LINE_SIZE);

    // right shift as needed
    if (sr1_right_shift != 0) {
      __set_const_mpy_params(_lea_add_sr1, ADD_LINE_SIZE);
      __const_mpy_q15(_lea_add_sr1, sr1_right_shift);
    } else if (sr0_right_shift != 0) {
      __set_const_mpy_params(_lea_add_sr0, ADD_LINE_SIZE);
      __const_mpy_q15(_lea_add_sr0, sr0_right_shift);
    }

    // Add up
    _add_vectors();

    // Shift the output
    if (dst_shift > 0) {
      // need to left shift
      __set_add_self_params(_lea_add_dst, ADD_LINE_SIZE);
      for (uint8_t i = 0; i < dst_shift; i++) {
        __add_self_q15(_lea_add_dst);
      }
    } else if (dst_shift < 0) {
      // need to right shift
      __set_const_mpy_params(_lea_add_dst, ADD_LINE_SIZE);
      __const_mpy_q15(_lea_add_dst, shift_right_factors[-dst_shift]);
    }

    // Move result out
    FRAM_MOVE(_lea_add_dst, dst_ptr, ADD_LINE_SIZE);

    // Update values
    sr0_ptr += ADD_LINE_SIZE;
    sr1_ptr += ADD_LINE_SIZE;
    dst_ptr += ADD_LINE_SIZE;
    length -= ADD_LINE_SIZE;
  }

  // Repeat for leftovers
  if (length) {
    lea_add_params.vectorSize = MAKE_ALIGN_2(length);
    // Move data into proper location
    FRAM_MOVE(sr0_ptr, _lea_add_sr0, length);
    FRAM_MOVE(sr1_ptr, _lea_add_sr1, length);

    // right shift as needed
    if (sr1_right_shift != 0) {
      __set_const_mpy_params(_lea_add_sr1, length);
      __const_mpy_q15(_lea_add_sr1, sr1_right_shift);
    } else if (sr0_right_shift != 0) {
      __set_const_mpy_params(_lea_add_sr0, length);
      __const_mpy_q15(_lea_add_sr0, sr0_right_shift);
    }

    // Add up
    _add_vectors();

    // Shift the output
    if (dst_shift > 0) {
      // need to left shift
      __set_add_self_params(_lea_add_dst, length);
      for (uint8_t i = 0; i < dst_shift; i++) {
        __add_self_q15(_lea_add_dst);
      }
    } else if (dst_shift < 0) {
      // need to right shift
      __set_const_mpy_params(_lea_add_dst, length);
      __const_mpy_q15(_lea_add_dst, shift_right_factors[-dst_shift]);
    }

    // Move result out
    FRAM_MOVE(_lea_add_dst, dst_ptr, length);
  }
}