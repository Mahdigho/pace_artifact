#include "include/dnn.h"
#include "lea.h"
#include "lea_kernels.h"
#include <libdsp/DSPLib.h>
#include <librng/rng.h>
#include <stdbool.h>

#define MSP_MIN 0x8000

#define SHIFT_LINE_SIZE (LEA_RAM_SIZE)
#define _lea_shift_sr0 lea_ram
#define _lea_shift_dst lea_ram

void dnn_init() {
  /* init LEA */
  if (!(LEAPMCTL & LEACMDEN)) {
    msp_lea_init();
  }
}

/* A max pooling based on iteration */
void max_pool(mat_t *src, mat_t *dst, stride_t strides) {
  uint16_t row_incr = src->strides[2] * strides.rows;
  uint16_t col_incr = strides.cols;
  // src->strides[0] : length of input
  // src->strides[1] : length of channel of input
  // src->strides[2] : length of row of input

  int16_t max;

  // For each input/output channel, until the last possible address
  // iloc : location of input data for this channel
  // oloc : location of output data.
  // The algorithm follows a channel, row, column paradigm, so
  // we can just increment oloc once a value is calculated.
  fixed *oloc = dst->data;
  ADDR_FOR(iloc, src->data, src->strides[0], src->strides[1]) {
    // For each output row
    ADDR_FOR(irow_min, iloc, src->strides[1], row_incr) {
      // Per each output column
      ADDR_FOR(icol_min, irow_min, src->strides[2], col_incr) {
        max = MSP_MIN;
        // For each input row
        ADDR_FOR(irow, icol_min, row_incr, src->strides[2]) {
          // For each input column
          ADDR_FOR(ival, irow, col_incr, 1) {
            max = (max > *ival ? max : *ival);
          } // ival
          ; // Need this so the foramtter doesn't mess up
        } // irow
        *(oloc) = max;
        oloc += 1;
      } // icol_min
      ;
    } // irow_min
    ;
  } // iloc
  ;
}

/*
 * An average pooling based on iteration
 * Use the variable mult to handle the division.
 * For instance, for an 8x8 stride, we want to use 512 as the mult
 * Essentially, strides*mult == 2**15, hence, get as close to it as possible.
 * NOTE: The 2x2 version is 2.6x faster, but it can also handle variable mult.
 * Since it really doesn't normally take that long anyways, we use the variable
 * mult.
 */
void avg_pool(mat_t *src, mat_t *dst, stride_t strides, int16_t mult) {
  uint16_t row_incr = src->strides[2] * strides.rows;
  uint16_t col_incr = strides.cols;
  // src->strides[0] : length of input
  // src->strides[1] : length of channel of input
  // src->strides[2] : length of row of input

  int32_t sum;
  bool shortcut = strides.rows == 2 && strides.cols == 2;

  // For each input/output channel, until the last possible address
  // iloc : location of input data for this channel
  // oloc : location of output data.
  // The algorithm follows a channel, row, column paradigm, so
  // we can just increment oloc once a value is calculated.
  fixed *oloc = dst->data;
  ADDR_FOR(iloc, src->data, src->strides[0], src->strides[1]) {
    // For each output row
    ADDR_FOR(irow_min, iloc, src->strides[1], row_incr) {
      // Per each output column
      ADDR_FOR(icol_min, irow_min, src->strides[2], col_incr) {
        sum = 0;
        // For each input row
        ADDR_FOR(irow, icol_min, row_incr, src->strides[2]) {
          // For each input column
          ADDR_FOR(ival, irow, col_incr, 1) {
            sum += *ival;
          } // ival
          ; // Need this so the foramtter doesn't mess up
        } // irow
        if (shortcut) {
          *(oloc) = (fixed)(sum >> 2);
        } else {
          int32_t value = sum * (int32_t)mult;
          // Assuming both mult values are Q15
          // Result will be a Q30, shown as Q31
          value <<= 1;
          int16_t top_part = ((int16_t *)&value)[1];
          *(oloc) = top_part;
        }
        oloc += 1;
      } // icol_min
      ;
    } // irow_min
    ;
  } // iloc
  ;
}

/* relu */
void relu(mat_t *input, mat_t *output, fixed max) {
  // input->strides[0] : length of input
  fixed *out = output->data;
  ADDR_FOR(val_ptr, input->data, input->strides[0], 1) {
    *out = (*val_ptr) < 0 ? 0 : ((max < (*val_ptr)) ? (max) : (*val_ptr));
    out += 1;
  }
}

/* relu */
void inplace_relu(mat_t *input, fixed max) {
  // input->strides[0] : length of input
  ADDR_FOR(val_ptr, input->data, input->strides[0], 1) {
    *val_ptr = (*val_ptr) < 0 ? 0 : ((max < (*val_ptr)) ? (max) : (*val_ptr));
  }
}

void pad(mat_t *src, fixed *dst, pad_t padding) {
  uint16_t src_row_length = src->strides[2];
  uint16_t src_row_num = src->dims[2];
  uint16_t src_channels = src->dims[1];

  uint16_t ZERO = 0;
  uint16_t output_row_length = src_row_length + padding.left + padding.right;
  uint16_t total_size = output_row_length *
                        (src_row_num + padding.top + padding.bottom) *
                        src_channels;

  // set everything to zero
  FRAM_SET(&ZERO, dst, total_size);

  uint16_t top_increment = output_row_length * padding.top;
  uint16_t bottom_increment = output_row_length * padding.bottom;
  dst += padding.left;
  ADDR_FOR(src_ptr, src->data, src->strides[0], src->strides[1]) {
    fixed *local_src_ptr = src_ptr;
    dst += top_increment;
    for (uint16_t i = src_row_num; i != 0; i--) {
      FRAM_MOVE(local_src_ptr, dst, src_row_length);
      dst += output_row_length;
      local_src_ptr += src_row_length;
    }
    dst += bottom_increment;
  }
}

void _left_shift_and_bias(mat_t *dst, mat_t *bias, int16_t shift_count) {
  uint16_t dst_chn_length = dst->strides[1];
  fixed *bias_ptr = bias->data;
  ADDR_FOR(dst_ptr, dst->data, dst->strides[0], dst_chn_length) {
    uint16_t length = dst_chn_length;
    fixed *cur_dst_ptr = dst_ptr;
    __set_const_add_params(_lea_shift_dst, SHIFT_LINE_SIZE);
    __set_add_self_params(_lea_shift_dst, SHIFT_LINE_SIZE);

    while (length >= SHIFT_LINE_SIZE) {
      // Move data into proper location
      FRAM_MOVE(cur_dst_ptr, _lea_shift_sr0, SHIFT_LINE_SIZE);

      // Add up
      for (uint8_t i = 0; i < shift_count; i++) {
        __add_self_q15(_lea_shift_sr0);
      }
      __const_add_q15(_lea_shift_sr0, *bias_ptr);

      // Move result out
      FRAM_MOVE(_lea_shift_dst, cur_dst_ptr, SHIFT_LINE_SIZE);

      // Update values
      cur_dst_ptr += SHIFT_LINE_SIZE;
      length -= SHIFT_LINE_SIZE;
    }

    // Repeat for leftovers
    if (length) {
      lea_const_add_params.vectorSize = MAKE_ALIGN_2(length);
      lea_add_self_params.vectorSize = MAKE_ALIGN_2(length);
      // Move data into proper location
      FRAM_MOVE(cur_dst_ptr, _lea_shift_sr0, length);

      // Add up
      for (uint8_t i = 0; i < shift_count; i++) {
        __add_self_q15(_lea_shift_sr0);
      }
      __const_add_q15(_lea_shift_sr0, *bias_ptr);

      // Move result out
      FRAM_MOVE(_lea_shift_dst, cur_dst_ptr, length);
    }
    bias_ptr += 1;
  }
}

/* Shift factor lookup table. */
const uint16_t shift_vector[16] = {
    0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100,
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001};

void _right_shift_and_bias(mat_t *dst, mat_t *bias, int16_t shift_count) {
  uint16_t dst_chn_length = dst->strides[1];
  fixed *bias_ptr = bias->data;
  fixed shift_value = shift_vector[shift_count];
  ADDR_FOR(dst_ptr, dst->data, dst->strides[0], dst_chn_length) {
    uint16_t length = dst_chn_length;
    fixed *cur_dst_ptr = dst_ptr;
    __set_const_add_params(_lea_shift_dst, SHIFT_LINE_SIZE);
    __set_const_rshift_params(_lea_shift_dst, SHIFT_LINE_SIZE);
    while (length >= SHIFT_LINE_SIZE) {
      // Move data into proper location
      FRAM_MOVE(cur_dst_ptr, _lea_shift_sr0, SHIFT_LINE_SIZE);

      // right shift
      __const_rshift_q15(_lea_shift_sr0, shift_value);
      __const_add_q15(_lea_shift_sr0, *bias_ptr);

      // Move result out
      FRAM_MOVE(_lea_shift_dst, cur_dst_ptr, SHIFT_LINE_SIZE);

      // Update values
      cur_dst_ptr += SHIFT_LINE_SIZE;
      length -= SHIFT_LINE_SIZE;
    }
    // Repeat for leftovers
    if (length) {
      lea_const_add_params.vectorSize = MAKE_ALIGN_2(length);
      lea_const_rshift_params.vectorSize = MAKE_ALIGN_2(length);

      // Move data into proper location
      FRAM_MOVE(cur_dst_ptr, _lea_shift_sr0, length);

      // Add up
      __const_rshift_q15(_lea_shift_sr0, shift_value);
      __const_add_q15(_lea_shift_sr0, *bias_ptr);

      // Move result out
      FRAM_MOVE(_lea_shift_dst, cur_dst_ptr, length);
    }
    bias_ptr += 1;
  }
}

/* figures out x and y, where sum = mult * count + y. */
void _count(uint16_t sum, uint16_t mult, uint16_t *count, uint16_t *rem) {
  uint16_t _count = 0;
  while (mult <= sum) {
    _count++;
    sum -= mult;
  }
  *count = _count;
  *rem = sum;
}

void mat_move(mat_t *src, mat_t *dst) {
  FRAM_MOVE(src->data, dst->data, src->strides[0]);
}