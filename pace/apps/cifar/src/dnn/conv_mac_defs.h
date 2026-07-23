#ifndef __LIBDNN_CONV_MAC_H
#define __LIBDNN_CONV_MAC_H
#include "include/dnn.h"
#include "lea.h"
#include "lea_kernels.h"

__nv _q15 *__lea_conv_flt = lea_ram;
__nv _q15 *__lea_conv_tmp;
__nv _q15 *__lea_conv_dst;
__nv _q15 *__lea_conv_chunk;
__nv uint16_t mat_mul_rows;
__nv uint16_t mat_mul_cols;
__nv uint16_t rows_to_load = 1;
__nv uint16_t chns_to_load = 1;
__nv bool odd_chn_rows = false;

__nv uint16_t _conv_mac_deinter_cmdID;

static inline void _set_flt_mpy() {
  __set_const_mpy_params(__lea_conv_flt, mat_mul_rows);
}

/* Takes a src and adds it to _lea_conv_dst in place.*/
static inline void _set_add_row() {
  __set_add_params(__lea_conv_dst, __lea_conv_dst, mat_mul_cols);
}

/* in place striding of the chunk. ensure things are 0ed out properly */
static inline void _stride_cols() {
  __deinter(__lea_conv_chunk, _conv_mac_deinter_cmdID);
}

static inline void _set_stride_params(stride_t strides) {
  __deinter_params(mat_mul_cols * mat_mul_rows, strides.cols, __lea_conv_chunk);
  _conv_mac_deinter_cmdID = LEACMD__DEINTERLEAVEEVENEVEN;
  if (strides.cols & 1) {
    _conv_mac_deinter_cmdID = LEACMD__DEINTERLEAVEEVENODD;
  }
}

/* helper function to calculate the dimensions of matrix multiply
 */
static inline void _calc_dimensions(mat_t *src, mat_t *dst, mat_t *flt,
                                    stride_t strides) {
  uint16_t dst_row_size = dst->strides[2];
  uint16_t src_row_size = src->strides[2];
  uint16_t src_chn_count = src->dims[1];
  uint16_t flt_size = flt->strides[1];

  rows_to_load = 1;
  chns_to_load = 0;
  // TODO: adjust these based on the architecture
  switch (src_row_size) {
  case 34:
    chns_to_load = 3;
    break;
  case 32:
    chns_to_load = 5;
    break;
  case 16:
    chns_to_load = 10;
    break;
  case 8:
    chns_to_load = 20;
    break;
  default:
    chns_to_load = 1;
  }

  // the space needed to put the input, including the strides
  uint16_t cols = MAKE_ALIGN_2(rows_to_load * dst_row_size * strides.cols);
  uint16_t rows = MAKE_ALIGN_2(chns_to_load * flt_size);
  // space needed to do a conv
  uint16_t per_iconv_row = rows * cols;
  // space for output accumulation + space needed for saving the filter
  uint16_t aux_per_iconv_row = cols * 2 + rows;

  uint16_t total_space = per_iconv_row + aux_per_iconv_row;

  // double check the space constraints
  if (total_space > LEA_RAM_SIZE) {
    chns_to_load = 1;
    rows_to_load = 1;
  }

  mat_mul_cols = MAKE_ALIGN_2(rows_to_load * dst_row_size * strides.cols);
  mat_mul_rows = chns_to_load * flt_size;
  odd_chn_rows = false;
  if ((mat_mul_rows & 0x1) == 0x1) {
    mat_mul_rows += 1;
    odd_chn_rows = true;
  }

  __lea_conv_tmp = __lea_conv_flt + mat_mul_rows;
  __lea_conv_dst = __lea_conv_tmp + mat_mul_cols;
  __lea_conv_chunk = __lea_conv_dst + mat_mul_cols;

  // calculate the real rows
  mat_mul_cols = MAKE_ALIGN_2(rows_to_load * dst_row_size);
}

// TODO: enable support for multiple row upload
static inline void _prep_chunk_for_conv(fixed *src, uint16_t src_chn_size,
                                        uint16_t src_line, uint16_t dst_line,
                                        stride_t strides, uint16_t kernel_rows,
                                        uint16_t kernel_cols) {
  uint16_t src_next_line = src_line * strides.rows;
  uint16_t dst_increment = dst_line * strides.cols;
  uint16_t src_next_chn = src_chn_size;
  fixed *dst = (fixed *)__lea_conv_chunk;
  uint16_t move_size = dst_line * strides.cols;

  // for each chn to be loaded
  for (uint16_t chns_left = chns_to_load; chns_left != 0; chns_left--) {
    fixed *src_ptr = src;
    for (uint8_t i = 0; i < kernel_rows; i++) {
      for (uint8_t j = 0; j < kernel_cols; j++) {
        FRAM_MOVE((src_ptr + j), dst, move_size);
        // go to the next row in the same column
        dst += dst_increment;
      }
      src_ptr += src_line;
    }
    src += src_chn_size;
  }

  if (strides.cols != 1) {
    _stride_cols();
  }
}

static inline void _load_filter(fixed *flt_ptr, fixed scale) {
  FRAM_MOVE(flt_ptr, __lea_conv_flt, mat_mul_rows);
  if (odd_chn_rows) {
    __lea_conv_flt[mat_mul_rows - 1] = 0;
  }
  if (scale != 0x7fff) {
    __const_mpy_q15(__lea_conv_flt, scale);
  }
}

static inline void _gmm_chn(fixed *dst_chn, bool first_chns) {
  __mpy_matrix_row_q15(__lea_conv_flt);
  if (!first_chns) {
    FRAM_MOVE(dst_chn, __lea_conv_dst, mat_mul_cols);
    __add_q15(__lea_conv_tmp);
    FRAM_MOVE(__lea_conv_dst, dst_chn, mat_mul_cols);
  } else {
    FRAM_MOVE(__lea_conv_tmp, dst_chn, mat_mul_cols);
  }
}

static inline void _readjust_dims(uint16_t new_chns_to_load, uint16_t flt_size,
                                  stride_t strides) {
  chns_to_load = new_chns_to_load;
  mat_mul_rows = chns_to_load * flt_size;
  odd_chn_rows = false;
  if ((mat_mul_rows & 0x1) == 0x1) {
    mat_mul_rows += 1;
    odd_chn_rows = true;
  }
  _set_flt_mpy();
  _set_stride_params(strides);
  __set_mpy_matrix_row_params(__lea_conv_tmp, __lea_conv_chunk, mat_mul_rows,
                              mat_mul_cols);
}

#endif // _LIBDNN_CONV_MAC_H