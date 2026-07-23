#include "conv_mac_defs.h"
#include "include/dnn.h"
#include "lea.h"
#include <libmsp/dma.h>
#include <stdint.h>

void convNN(mat_t *src, mat_t *dst, mat_t *flt, mat_t *bias, mat_t *scale,
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

  if (padding.bottom != 0 || padding.top != 0 || padding.left != 0 ||
      padding.right != 0) {
    mat_move(src, dst);
    fixed *src_ptr = src->data;
    src->data = dst->data;
    pad(src, src_ptr, padding);
    src->data = src_ptr;
  }

  // chunk
  _calc_dimensions(src, dst, flt, strides);
  _set_flt_mpy();
  _set_stride_params(strides);
  _set_add_row();
  __set_mpy_matrix_row_params(__lea_conv_tmp, __lea_conv_chunk, mat_mul_rows,
                              mat_mul_cols);
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
  // 1. for each batch of input channels
  //    2. for each batch of output rows
  //       3. for each output channel

  uint16_t src_next_row = padded_row_size * strides.rows * rows_to_load;
  uint16_t src_next_chunk = padded_chn_size * chns_to_load;
  uint16_t dst_next_row = dst_row_size * rows_to_load;
  uint16_t flt_next_ichns = flt_chn_size * chns_to_load;
  bool first_chns = true;

  uint16_t src_chns_left = src_chn_num;
  ADDR_FOR(src_ptr, src->data, padded_all_size, src_next_chunk) {
    if (src_chns_left < chns_to_load) {
      _readjust_dims(src_chns_left, flt_chn_size, strides);
    }
    fixed *cur_pad_row_ptr = src_ptr;
    ADDR_FOR(dst_row_ptr, dst->data, dst_chn_size, dst_next_row) {
      // load chunk
      _prep_chunk_for_conv(cur_pad_row_ptr, padded_chn_size, padded_row_size,
                           dst_row_size, strides, flt_row_num, flt_col_num);
      // for each output channel, operate on the chunk
      fixed *oflt_ptr = flt_ptr;
      fixed *local_scale_ptr = scale_ptr;
      ADDR_FOR(dst_ptr, dst_row_ptr, dst->strides[0], dst_chn_size) {
        _load_filter(oflt_ptr, *local_scale_ptr);

        _gmm_chn(dst_ptr, first_chns, dst_next_row);
        oflt_ptr += flt_ochn_all_size;
        local_scale_ptr += scale_increment;
      }

      cur_pad_row_ptr += src_next_row;
    }

    first_chns = false;
    flt_ptr += flt_next_ichns;
    src_chns_left -= chns_to_load;
  }
  // Shift
  int16_t shift = dst->p2quant - (src->p2quant + flt->p2quant - 15);
  if (shift >= 0) {
    _left_shift_and_bias(dst, bias, shift);
    // Do left shift
  } else if (shift < 0) {
    msp_printf("You need to right shift\n\r");
    // Do right shift
  }

  return;
}