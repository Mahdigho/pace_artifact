#include "include/dnn.h"
#include "lea.h"
#include "lea_kernels.h"
#include <libmsp/dma.h>
#include <stdint.h>

__nv _q15 *__lea_dsc_flt = lea_ram;
__nv _q15 *__lea_dsc_dst;
__nv _q15 *__lea_dsc_chunk;
__nv uint16_t dsc_mul_rows;
__nv uint16_t dsc_load_cols;
__nv uint16_t dsc_mul_cols;
__nv uint16_t src_rows_to_load;
__nv bool dsc_odd_rows = false;

__nv uint16_t _dsc_mac_deinter_cmdID;

static inline void _calc_dims(mat_t *src, mat_t *dst, mat_t *flt,
                              stride_t strides) {
  uint16_t dst_row_size = dst->strides[2];
  uint16_t src_chn_count = src->dims[1];
  uint16_t flt_size = flt->strides[1];
  // Total space needed, which should be less than LEA_RAM_SIZE
  // = mat_mul_rows * mat_mul_cols + mat_mul_rows + mat_mul_cols
  // = (mat_mul_rows + 1) * mat_mul_cols + mat_mul_rows
  // where mat_mul_cols is a multiple of src_row_size
  dsc_mul_rows = flt_size;
  // TODO: adjust based on architecture
  switch (dst_row_size * strides.cols) {
  case 6:
    src_rows_to_load = 6;
    break;
  case 12:
    if (strides.cols == 1) {
      src_rows_to_load = 12;
    } else {
      src_rows_to_load = 6;
    }
    break;
  case 24:
    src_rows_to_load = 6;
    break;
  case 48:
    src_rows_to_load = 3;
    break;
  case 96:
  default:
    src_rows_to_load = 1;
  }
  dsc_load_cols = MAKE_ALIGN_2(src_rows_to_load * dst_row_size * strides.cols);
  dsc_mul_cols = MAKE_ALIGN_2(src_rows_to_load * dst_row_size);

  dsc_odd_rows = false;

  if ((dsc_mul_rows & 0x1) == 0x1) {
    dsc_mul_rows += 1;
    dsc_odd_rows = true;
  }

  __lea_dsc_dst = __lea_dsc_flt + dsc_mul_rows;
  __lea_dsc_chunk = __lea_dsc_dst + dsc_load_cols;
}

/* For scaling the filters by a constatnt factor */
static inline void _set_flt_mpy() {
  __set_const_mpy_params(__lea_dsc_flt, dsc_mul_rows);
}

/* in place striding of the chunk. ensure things are 0ed out properly */
static inline void _stride_cols() {
  __deinter(__lea_dsc_chunk, _dsc_mac_deinter_cmdID);
}

static inline void _set_stride_params(stride_t strides, uint16_t col_count) {
  __deinter_params(col_count * dsc_mul_rows, strides.cols, __lea_dsc_chunk);
  _dsc_mac_deinter_cmdID = LEACMD__DEINTERLEAVEEVENEVEN;
  if (strides.cols & 1) {
    _dsc_mac_deinter_cmdID = LEACMD__DEINTERLEAVEEVENODD;
  }
}

static inline void _load_filter(fixed *flt_ptr, fixed scale) {
  FRAM_MOVE(flt_ptr, __lea_dsc_flt, dsc_mul_rows);
  if (dsc_odd_rows) {
    __lea_dsc_flt[dsc_mul_rows - 1] = 0;
  }
  if (scale != 0x7fff) {
    __const_mpy_q15(__lea_dsc_flt, scale);
  }
}

static inline void _adjust_dims(stride_t strides, uint16_t col_count) {
  _set_stride_params(strides, col_count);
  __set_mpy_matrix_row_params(__lea_dsc_dst, __lea_dsc_chunk, dsc_mul_rows,
                              col_count);
}

static inline void _place_part_chunk(fixed *src, uint16_t sline_size,
                                     fixed *dst, uint16_t dst_next_line,
                                     uint16_t kernel_rows, uint16_t kernel_cols,
                                     uint16_t move_size) {
  for (uint8_t i = 0; i < kernel_rows; i++) {
    for (uint8_t j = 0; j < kernel_cols; j++) {
      FRAM_MOVE((src + j), dst, move_size);
      // go to the next row in the same column
      dst += dst_next_line;
    }
    src += sline_size;
  }
}

// TODO: add chn to this
static inline void _load_mat(fixed *src, uint16_t sline_size,
                             uint16_t snext_row, fixed *dst,
                             uint16_t dline_size, uint16_t dnext_row,
                             uint16_t row_load_count, uint16_t kernel_rows,
                             uint16_t kernel_cols, uint16_t col_stride) {

  fixed *dst_row_ptr = dst;
  fixed *src_row_ptr = src;

  for (uint16_t row = row_load_count; row != 0; row--) {
    _place_part_chunk(src_row_ptr, sline_size, dst_row_ptr, dnext_row,
                      kernel_rows, kernel_cols, dline_size);
    src_row_ptr += snext_row;
    dst_row_ptr += dline_size;
  }

  if (col_stride != 1) {
    _stride_cols();
  }
}