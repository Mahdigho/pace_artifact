#include "include/dnn.h"
#include "lea.h"
#include "lea_kernels.h"
#include "libdsp/DSPLib_support.h"
#include <libmsp/dma.h>

#define _LEA_PW_MAC_MAX_ROW 128
#define _LEA_PW_MAC_MAX_FLT 128
#define _LEA_PW_MAC_MAX_IN (128 * 10)
#define _lea_pw_mac_res lea_ram
#define _lea_pw_mac_sum (_lea_pw_mac_res + _LEA_PW_MAC_MAX_ROW)
#define _lea_pw_mac_flt (_lea_pw_mac_sum + _LEA_PW_MAC_MAX_ROW)
#define _lea_pw_mac_mat (_lea_pw_mac_flt + _LEA_PW_MAC_MAX_FLT)

__nv uint16_t __pw_deinter_cmdID;
__nv uint8_t __pw_deinter_cols;

/* stride columns in place */
static inline void _set_stride_params(stride_t strides) {
  if (strides.cols != 1) {
    __deinter_params(0, strides.cols, _lea_pw_mac_mat);
    __pw_deinter_cols = true;
    __pw_deinter_cmdID = LEACMD__DEINTERLEAVEEVENEVEN;
    if (strides.cols & 1) {
      __pw_deinter_cmdID = LEACMD__DEINTERLEAVEEVENODD;
    }
  }
}

static inline void _stride_cols(uint16_t length) {
  lea_deinter_params.vectorSize = length;
  __deinter(_lea_pw_mac_mat, __pw_deinter_cmdID);
}

/* scale flt in place */
static inline void _set_scale_flt() {
  __set_const_mpy_params(_lea_pw_mac_flt, 0);
}

static inline void _scale_flt(fixed scale, uint16_t length) {
  if (scale != 0x7fff) {
    lea_const_mpy_params.vectorSize = length;
    __const_mpy_q15(_lea_pw_mac_flt, scale);
  }
}

/* mat mul and save in output */
static inline void _set_mat_mpy(fixed *output) {
  __set_mpy_matrix_row_params(output, _lea_pw_mac_mat, 0, 0);
}

static inline void _mat_mpy(uint16_t rows, uint16_t cols) {
  lea_mpymatrixrow_params.rowSize = MAKE_ALIGN_2(rows);
  lea_mpymatrixrow_params.colSize = MAKE_ALIGN_2(cols);
  __mpy_matrix_row_q15(_lea_pw_mac_flt);
}

/* Accumulate values in lea_pw_mac_sum .*/
static inline void _set_acc_params() {
  __set_add_params(_lea_pw_mac_sum, _lea_pw_mac_sum, 0);
}
static inline void _accumulate(uint16_t length) {
  lea_add_params.vectorSize = length;
  __add_q15(_lea_pw_mac_res);
}

/* for inplace biasing and shifting */
const uint16_t rshift_vector[16] = {
    0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100,
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001};

static inline void _set_shift_bias() {
  __set_const_add_params(_lea_pw_mac_sum, 0);
  __set_add_self_params(_lea_pw_mac_sum, 0);
  __set_const_rshift_params(_lea_pw_mac_sum, 0);
}

static inline void _shift(int16_t shift_count, uint16_t length) {
  if (shift_count > 0) {
    lea_add_self_params.vectorSize = MAKE_ALIGN_2(length);
    for (uint8_t i = 0; i < shift_count; i++) {
      __add_self_q15(_lea_pw_mac_sum);
    }
  } else if (shift_count < 0) {
    lea_const_mpy_params.vectorSize = MAKE_ALIGN_2(length);
    __const_rshift_q15(_lea_pw_mac_sum, rshift_vector[-shift_count]);
  }
}
static inline void _bias(fixed bias, uint16_t length) {
  lea_const_add_params.vectorSize = MAKE_ALIGN_2(length);
  __const_add_q15(_lea_pw_mac_sum, bias);
}

/* The src_row size and associate values.
 *
 *    src_chn_size = src_row_size * src_row_segments + src_row_rem
 *    if src_chn_size <= max_src_row_size, then
 *          src_row_size = src_chn_size,
 *          src_row_segments = 0,
 *          src_row_rem = src_row_size
 *    else use the count up
 *
 *  The channel count and associate values.
 *     MAC_MAX_IN = sch_to_load * src_row_size + mac_max_unused
 *     sch_to_load should be the biggest multiple of src_row_size
 *     that can fit in the mac_max_in at the same time.
 *     TODO: the max sch_to_load is 128, so adjust properly?
 *     TODO: There is an inherent assumption that if there are strides,
 *     then the input and output will be even.
 */
typedef struct {
  uint16_t rows;
  uint16_t src_cols;
  uint16_t dst_cols;
} mat_dim;

static inline mat_dim _calc_dims(mat_t *src, mat_t *dst, stride_t strides) {
  uint16_t dst_row_size = dst->strides[2];
  uint16_t src_row_size = src->strides[2];
  uint16_t src_chn_size = src->strides[1];
  uint16_t dst_chn_size = dst->strides[1];
  mat_dim dimensions;

  // TODO: the current setup ignores the fact that a piecewise with
  // col stride 2 and odd input rows will not work here.
  if (src_chn_size <= _LEA_PW_MAC_MAX_ROW) {
    // If chn can be loaded in one go, no need for extra math.
    dimensions.src_cols = src_chn_size;
    // dst_cols may be differnt from src_cols (in case of strides)
    dimensions.dst_cols = dst_chn_size;
  } else if (strides.rows == 1) {
    // If rows should not be strided and input dimensions are not odd
    // we can treat each input channel as a 1D stream
    dimensions.src_cols = _LEA_PW_MAC_MAX_ROW;
    dimensions.dst_cols = _LEA_PW_MAC_MAX_ROW;
  } else {
    // rows are strided, and entire channel cannot be loaded in one go.
    // cols will need to be multiples of src_rows
    uint16_t mult, rem;
    _count(_LEA_PW_MAC_MAX_ROW, src_row_size, &mult, &rem);
    dimensions.src_cols = src_row_size * mult;
    dimensions.dst_cols = dst_row_size * mult;
  }

  // Calculate how many channels can be loaded at the same time
  uint16_t sch_to_load, mac_max_unused;
  _count(_LEA_PW_MAC_MAX_IN, MAKE_ALIGN_2(dimensions.src_cols),
         &(dimensions.rows), &mac_max_unused);

  // ensure max dimensions is always even
  dimensions.rows -= (dimensions.rows & 0x1);

  return dimensions;
}

static inline void _load_inputs(fixed *src_ptr, uint16_t src_chn_size,
                                uint16_t src_row_size, stride_t strides,
                                mat_dim dimensions) {
  _q15 *lea_addr = _lea_pw_mac_mat;
  if (strides.rows == 1) {
    // none strided case, can be odd
    bool elem_size_is_odd = (dimensions.src_cols & 0x1) == 1;
    uint16_t even_elem_size = MAKE_ALIGN_2(dimensions.src_cols);
    // for each input channel in this chunk
    for (uint16_t i = dimensions.rows; i != 0; i--) {
      FRAM_MOVE(src_ptr, lea_addr, dimensions.src_cols);
      if (elem_size_is_odd) {
        // odd elem_size, 0 end.
        lea_addr[dimensions.src_cols] = 0;
      }
      src_ptr += src_chn_size;
      lea_addr += even_elem_size;
    }
  } else {
    // strided case, CANNOT BE ODD
    // for each input channel in this chunk
    for (uint16_t i = dimensions.rows; i != 0; i--) {
      uint16_t row_count = 0;
      uint16_t left_cols = dimensions.src_cols;
      // for each loadable number of columns
      fixed *cur_row = src_ptr;
      while (left_cols != 0) {
        if (row_count == 0) {
          FRAM_MOVE(cur_row, lea_addr, src_row_size);
          lea_addr += src_row_size;
          left_cols -= src_row_size;
          row_count = strides.rows;
        }
        row_count -= 1;
        cur_row += src_row_size;
      }
      src_ptr += src_chn_size;
    }
    if (__pw_deinter_cols) {
      _stride_cols(dimensions.dst_cols * dimensions.rows);
    }
  }
}

static inline void _conv_per_segment(mat_t *src, mat_t *flt, mat_t *bias,
                                     mat_t *scale, mat_t *dst,
                                     uint16_t cur_chn_index, mat_dim dimensions,
                                     bool last, bool first, stride_t strides) {
  fixed *src_seg_ptr = src->data + (src->strides[1] * cur_chn_index);
  fixed *cur_dst_elem_ptr = dst->data;
  int16_t shift = dst->p2quant - (src->p2quant + flt->p2quant - 15);
  uint16_t dst_chn_size = dst->strides[2];
  if ((dimensions.rows & 0x1) == 1) {
    // in case of oddness
    _lea_pw_mac_flt[dimensions.rows] = 0;
  }

  // Count the number of segments of dst left
  uint16_t dst_elems_left = dst->strides[1];
  while (dst_elems_left != 0) {
    if (dst_elems_left <= dimensions.dst_cols) {
      dimensions.dst_cols = dst_elems_left;
      dimensions.src_cols = dst_elems_left * strides.cols;
    }
    _load_inputs(src_seg_ptr, src->strides[1], src->strides[2], strides,
                 dimensions);

    fixed *flt_start = flt->data + cur_chn_index;
    fixed *scale_ptr = scale->data;
    fixed *bias_ptr = bias->data;
    // for each output channel
    ADDR_FOR(cur_dst_chn_ptr, cur_dst_elem_ptr, dst->strides[0],
             dst->strides[1]) {
      // load the filter
      FRAM_MOVE(flt_start, _lea_pw_mac_flt, dimensions.rows)
      _scale_flt(*scale_ptr, MAKE_ALIGN_2(dimensions.rows));

      _mat_mpy(dimensions.rows, dimensions.dst_cols);
      if (!first) {
        FRAM_MOVE(cur_dst_chn_ptr, _lea_pw_mac_sum, dimensions.dst_cols);
        _accumulate(dimensions.dst_cols);
      }
      if (last) {
        // left shift and bias
        _shift(shift, dimensions.dst_cols);
        _bias(*bias_ptr, dimensions.dst_cols);
      }

      FRAM_MOVE(_lea_pw_mac_sum, cur_dst_chn_ptr, dimensions.dst_cols);

      flt_start += flt->strides[0];
      bias_ptr += 1;
      scale_ptr += 1;
    }

    cur_dst_elem_ptr += dimensions.dst_cols;
    dst_elems_left -= dimensions.dst_cols;
    src_seg_ptr += dimensions.src_cols * strides.rows;
  }
}

/*
 * Due to the construction of pw conv, this function operates by treating each
 * of the input channels not as 2d elements, but rather as 1d elements. Hence,
 * it treats them as rows that need to be MACed into an output row (i.e. the
 * output channel).
 *
 * It operates by using Matrix Vector multiply, where the matrix is a
 * compromised by a row of channels (also called segment) and the vector is the
 * respective conv weights for the input/output channels.
 *
 * Structure of for loops
 * 1. Input channels: for each sch_to_load group of input channels that can be
 * loaded together
 *    2. for each segment that can be built from the group of input channel
 *       LOAD INPUT
 *       3. for each output channel
 *          LOAD FLT
 *          MAC
 */
void pwNN(mat_t *src, mat_t *dst, mat_t *flt, mat_t *bias, mat_t *scale,
          stride_t strides) {
  uint16_t src_chn_count = src->dims[1];

  // calculate dimensions
  mat_dim biggest_dims = _calc_dims(src, dst, strides);
  // setup stride parameters and
  _set_stride_params(strides);
  _set_scale_flt();
  _set_acc_params();
  _set_shift_bias();

  // setup loop variables

  // keep track of the first and last batch of channels
  bool last_chns = false;
  bool first_chns = true;

  uint16_t sch_left = src_chn_count;
  uint16_t sch_done = 0;
  // count the number of input channels left
  uint16_t flt_increment = biggest_dims.rows;
  while (sch_left != 0) {
    if (sch_left <= biggest_dims.rows) {
      // adjust if less chns are left than the max
      biggest_dims.rows = sch_left;
      last_chns = true;
    }
    // No matter what the case is, the output will be ready in _lea_pw_mac_sum
    if (first_chns) {
      _set_mat_mpy(_lea_pw_mac_sum);
    } else {
      _set_mat_mpy(_lea_pw_mac_res);
    }
    _conv_per_segment(src, flt, bias, scale, dst, sch_done, biggest_dims,
                      last_chns, first_chns, strides);

    // Proceed to next channels
    sch_left -= biggest_dims.rows;
    sch_done += biggest_dims.rows;
    first_chns = false;
  }
  return;
}