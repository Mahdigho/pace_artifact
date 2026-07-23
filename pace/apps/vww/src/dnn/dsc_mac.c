
#include "dsc_mac_defs.h"
#include "include/dnn.h"
#include "lea.h"
#include "lea_kernels.h"
#include <libmsp/dma.h>
#include <stdint.h>

// TODO: figure out strides for this.
void dscNN(mat_t *src, mat_t *dst, mat_t *flt, mat_t *bias, mat_t *scale,
           pad_t padding, stride_t strides) {
  // base information
  uint16_t flt_col_num = flt->dims[3];
  uint16_t flt_row_num = flt->dims[2];
  uint16_t flt_ochn_all_size = flt->strides[0];
  uint16_t flt_chn_size = flt->strides[1];
  uint16_t flt_row_size = flt->strides[2];
  uint16_t src_row_num = src->dims[2];
  uint16_t src_chn_num = src->dims[1];
  uint16_t src_chn_size = src->strides[1];
  uint16_t src_row_size = src->strides[2];
  uint16_t dst_row_num = dst->dims[2];
  uint16_t dst_chn_size = dst->strides[1];
  uint16_t dst_row_size = dst->strides[2];

  // padded information
  uint16_t padded_row_size = src_row_size + padding.left + padding.right;
  uint16_t padded_row_num = src_row_num + padding.top + padding.bottom;
  uint16_t padded_chn_size = padded_row_num * padded_row_size;
  uint16_t padded_all_size = padded_chn_size * src->dims[1];
  uint16_t padded_next_load_row = padded_row_size * strides.rows;
  uint16_t dst_load_line = dst_row_size * strides.cols;

  if (padding.bottom != 0 || padding.top != 0 || padding.left != 0 ||
      padding.right != 0) {
    mat_move(src, dst);
    fixed *src_ptr = src->data;
    src->data = dst->data;
    pad(src, src_ptr, padding);
    src->data = src_ptr;
  }

  _calc_dims(src, dst, flt, strides);
  _set_flt_mpy();

  // setup for scale and flt
  fixed *flt_ptr = (flt->data);

  fixed ONE = 0x7fff;
  fixed *scale_ptr = &ONE;
  uint16_t scale_increment = 0;
  if (scale != NULL) {
    scale_increment += 1;
    scale_ptr = scale->data;
  }

  // the loop structure
  // 1. for each input/output channel
  //    load filter
  //   2. load chunk of rows
  //     3. matmul
  fixed *dst_ptr = dst->data;
  ADDR_FOR(src_ptr, src->data, padded_all_size, padded_chn_size) {
    _load_filter(flt_ptr, *scale_ptr);
    _adjust_dims(strides, dsc_mul_cols);

    uint16_t row_left = dst_row_num;
    uint16_t rows_to_load = src_rows_to_load;
    uint16_t dst_next_row = dsc_load_cols;
    uint16_t dst_move_size = dsc_mul_cols;
    fixed *src_row_ptr = src_ptr;
    fixed *dst_row_ptr = dst_ptr;
    while (row_left != 0) {
      // Readjust if needed
      if (row_left < src_rows_to_load) {
        rows_to_load = row_left;
        dst_next_row = MAKE_ALIGN_2(rows_to_load * dst_row_size * strides.cols);
        dst_move_size = MAKE_ALIGN_2(rows_to_load * dst_row_size);
        _adjust_dims(strides, dst_move_size);
      }

      // load data chunk
      _load_mat(src_row_ptr, padded_row_size, padded_next_load_row,
                __lea_dsc_chunk, dst_load_line, dst_next_row, rows_to_load,
                flt_row_num, flt_col_num, strides.cols);

      __mpy_matrix_row_q15(__lea_dsc_flt);
      FRAM_MOVE(__lea_dsc_dst, dst_row_ptr, dst_move_size)

      src_row_ptr += padded_next_load_row * rows_to_load;
      dst_row_ptr += dst_row_size * rows_to_load;
      row_left -= rows_to_load;
    }

    scale_ptr += scale_increment;
    flt_ptr += flt_chn_size;
    dst_ptr += dst_chn_size;
  }

  // Shift
  int16_t shift = dst->p2quant - (src->p2quant + flt->p2quant - 15);
  if (shift >= 0) {
    _left_shift_and_bias(dst, bias, shift);
    // Do left shift
  } else if (shift < 0) {
    _right_shift_and_bias(dst, bias, -shift);
    // Do right shift
  }
}