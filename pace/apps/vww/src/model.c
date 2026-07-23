#include "dnn/include/dnn.h"
#include "include/inf.h"
#include "weights/include/b0.h"
#include "weights/include/b10_dsc.h"
#include "weights/include/b10_pw.h"
#include "weights/include/b11_dsc.h"
#include "weights/include/b11_pw.h"
#include "weights/include/b12_dsc.h"
#include "weights/include/b12_pw.h"
#include "weights/include/b13_dsc.h"
#include "weights/include/b13_pw.h"
#include "weights/include/b14_dsc.h"
#include "weights/include/b14_pw.h"
#include "weights/include/b2_dsc.h"
#include "weights/include/b2_pw.h"
#include "weights/include/b3_dsc.h"
#include "weights/include/b3_pw.h"
#include "weights/include/b4_dsc.h"
#include "weights/include/b4_pw.h"
#include "weights/include/b5_dsc.h"
#include "weights/include/b5_pw.h"
#include "weights/include/b6_dsc.h"
#include "weights/include/b6_pw.h"
#include "weights/include/b7_dsc.h"
#include "weights/include/b7_pw.h"
#include "weights/include/b8_dsc.h"
#include "weights/include/b8_pw.h"
#include "weights/include/b9_dsc.h"
#include "weights/include/b9_pw.h"
#include "weights/include/inputs.h"
#include "weights/include/linear0.h"
#include "weights/include/linear1.h"
#include "weights/include/linear2.h"
#include "weights/include/out0_dsc.h"
#include "weights/include/out0_pw.h"
#include "weights/include/out1_dsc.h"
#include "weights/include/out1_pw.h"

/* Memory Overhead
 *
Base neurons: 22000
Early Exit +11000
EarlySlim:

| Layer   | All  | Slim Weights |
| ------- | ---- | ------------ |
| b0      | 108  | 8            |
| b2      | 68   | 90           |
| b3      | 200  | 210          |
| b4      | 400  | 280          |
| b5      | 400  | 280          |
| b6      | 400  | 280          |
| b7      | 656  | 420          |
| b8      | 1312 | 560          |
| b9      | 1312 | 560          |
| b10     | 1312 | 560          |
| b11     | 1312 | 560          |
| b12     | 1312 | 560          |
| b13     | 2336 | 840          |
| b13     | 4672 | 1120         |
| linaer2 | 128  | 0            |
| linaer0 | 128  | 0            |
| out0    | 200  | 210          |
| linaer1 | 128  | 0            |
| out1    | 400  | 280          |

Base:  22000 + 15928 = 37928
Early Exit: 37928 + 11000 + 856 = 49784
Slimmable: 37928 + 6328 = 44256
EarlySlim: 49784 + 6818 = 49784 + 6818

*/

#define MAX_SIZE 11000
__ro_hinv fixed _padb0[MAX_SIZE];
__ro_hinv fixed _padb1[MAX_SIZE];
__ro_hinv fixed _padb2[MAX_SIZE];
__ro_hinv fixed _out_buf0[11];

__ro_hinv mat_t bf0 = {.dims = {1, MAX_SIZE},
                       .len_dims = 2,
                       .strides = {MAX_SIZE, 1},
                       .data = (fixed *)_padb0,
                       .p2quant = 1};

__ro_hinv mat_t bf1 = {.dims = {1, MAX_SIZE},
                       .len_dims = 2,
                       .strides = {MAX_SIZE, 1},
                       .data = (fixed *)_padb1,
                       .p2quant = 1};

__ro_hinv mat_t bfe = {.dims = {1, MAX_SIZE},
                       .len_dims = 2,
                       .strides = {MAX_SIZE, 1},
                       .data = (fixed *)_padb2,
                       .p2quant = 1};

__ro_hinv mat_t mat_out = {.dims = {1, 2},
                           .len_dims = 2,
                           .strides = {2, 1},
                           .data = (fixed *)_out_buf0,
                           .p2quant = 1};

static inline void __dsc(mat_t *input, mat_t *output, uint16_t width,
                         uint16_t width_seperation, mat_t *outshape,
                         mat_t *weights, mat_t *bias, mat_t *scalar,
                         pad_t padding, stride_t strides, uint16_t reduct) {
  MAT_SAMESHAPE(output, outshape);
  UPDATE_WIDTH(output, width_seperation, (width), reduct);
  dscNN(input, output, weights, bias, scalar, padding, strides);
}

static inline void __pw(mat_t *input, mat_t *output, uint16_t width,
                        uint16_t width_seperation, mat_t *outshape,
                        mat_t *weights, mat_t *bias, mat_t *scalar,
                        stride_t strides, uint16_t reduct) {
  MAT_SAMESHAPE(output, outshape);
  UPDATE_WIDTH(output, width_seperation, width, reduct);
  pwNN(input, output, weights, bias, scalar, strides);
}

void b1c(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 1;
  width = width >> 1;
  MAT_SAMESHAPE(output, &mat_b0out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b0w, &mat_b0b_4, &mat_b0s_4, (pad_t){0, 0, 0, 0},
         (stride_t){.cols = 2, .rows = 2});
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b2_dscscales[] = {
    b2_dscs_1,
    b2_dscs_2,
    b2_dscs_3,
    b2_dscs_4,
};

__ro_hinv fixed *b2_dscbiases[] = {
    b2_dscb_1,
    b2_dscb_2,
    b2_dscb_3,
    b2_dscb_4,
};

__ro_hinv fixed *b2_pwscales[] = {b2_pws_2, b2_pws_3, b2_pws_4, b2_pws_5,
                                  b2_pws_6, b2_pws_7, b2_pws_8};

__ro_hinv fixed *b2_pwbiases[] = {b2_pwb_2, b2_pwb_3, b2_pwb_4, b2_pwb_5,
                                  b2_pwb_6, b2_pwb_7, b2_pwb_8};

void b2d(mat_t *input, mat_t *output, uint16_t width) {
  width = width >> 1;
  mat_b2_dscb_4.data = b2_dscbiases[width - 1];
  mat_b2_dscs_4.data = b2_dscscales[width - 1];
  __dsc(input, output, width, 1, &mat_b2_dscout, &mat_b2_dscw, &mat_b2_dscb_4,
        &mat_b2_dscs_4, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 2, .rows = 2},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b2p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b2_pwb_8.data = b2_pwbiases[width - 2];
  mat_b2_pws_8.data = b2_pwscales[width - 2];
  __pw(input, output, width, 1, &mat_b2_pwout, &mat_b2_pww, &mat_b2_pwb_8,
       &mat_b2_pws_8, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b3_dscscales[] = {b3_dscs_2, b3_dscs_3, b3_dscs_4, b3_dscs_5,
                                   b3_dscs_6, b3_dscs_7, b3_dscs_8};

__ro_hinv fixed *b3_dscbiases[] = {b3_dscb_2, b3_dscb_3, b3_dscb_4, b3_dscb_5,
                                   b3_dscb_6, b3_dscb_7, b3_dscb_8};

__ro_hinv fixed *b3_pwscales[] = {b3_pws_4,  b3_pws_6,  b3_pws_8, b3_pws_10,
                                  b3_pws_12, b3_pws_14, b3_pws_16};

__ro_hinv fixed *b3_pwbiases[] = {b3_pwb_4,  b3_pwb_6,  b3_pwb_8, b3_pwb_10,
                                  b3_pwb_12, b3_pwb_14, b3_pwb_16};
void b3d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b3_dscb_8.data = b3_dscbiases[width - 2];
  mat_b3_dscs_8.data = b3_dscscales[width - 2];
  __dsc(input, output, width, 1, &mat_b3_dscout, &mat_b3_dscw, &mat_b3_dscb_8,
        &mat_b3_dscs_8, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b3p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b3_pwb_16.data = b3_pwbiases[width - 2];
  mat_b3_pws_16.data = b3_pwscales[width - 2];
  __pw(input, output, width, 2, &mat_b3_pwout, &mat_b3_pww, &mat_b3_pwb_16,
       &mat_b3_pws_16, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b4_dscscales[] = {b4_dscs_4,  b4_dscs_6,  b4_dscs_8,
                                   b4_dscs_10, b4_dscs_12, b4_dscs_14,
                                   b4_dscs_16};

__ro_hinv fixed *b4_dscbiases[] = {b4_dscb_4,  b4_dscb_6,  b4_dscb_8,
                                   b4_dscb_10, b4_dscb_12, b4_dscb_14,
                                   b4_dscb_16};

__ro_hinv fixed *b4_pwscales[] = {b4_pws_4,  b4_pws_6,  b4_pws_8, b4_pws_10,
                                  b4_pws_12, b4_pws_14, b4_pws_16};

__ro_hinv fixed *b4_pwbiases[] = {b4_pwb_4,  b4_pwb_6,  b4_pwb_8, b4_pwb_10,
                                  b4_pwb_12, b4_pwb_14, b4_pwb_16};

void b4d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b4_dscb_16.data = b4_dscbiases[width - 2];
  mat_b4_dscs_16.data = b4_dscscales[width - 2];
  __dsc(input, output, width, 2, &mat_b4_dscout, &mat_b4_dscw, &mat_b4_dscb_16,
        &mat_b4_dscs_16, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b4p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b4_pwb_16.data = b4_pwbiases[width - 2];
  mat_b4_pws_16.data = b4_pwscales[width - 2];
  __pw(input, output, width, 2, &mat_b4_pwout, &mat_b4_pww, &mat_b4_pwb_16,
       &mat_b4_pws_16, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b5_dscscales[] = {b5_dscs_4,  b5_dscs_6,  b5_dscs_8,
                                   b5_dscs_10, b5_dscs_12, b5_dscs_14,
                                   b5_dscs_16};

__ro_hinv fixed *b5_dscbiases[] = {b5_dscb_4,  b5_dscb_6,  b5_dscb_8,
                                   b5_dscb_10, b5_dscb_12, b5_dscb_14,
                                   b5_dscb_16};

__ro_hinv fixed *b5_pwscales[] = {b5_pws_4,  b5_pws_6,  b5_pws_8, b5_pws_10,
                                  b5_pws_12, b5_pws_14, b5_pws_16};
__ro_hinv fixed *b5_pwbiases[] = {b5_pwb_4,  b5_pwb_6,  b5_pwb_8, b5_pwb_10,
                                  b5_pwb_12, b5_pwb_14, b5_pwb_16};

void b5d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b5_dscb_16.data = b5_dscbiases[width - 2];
  mat_b5_dscs_16.data = b5_dscscales[width - 2];
  __dsc(input, output, width, 2, &mat_b5_dscout, &mat_b5_dscw, &mat_b5_dscb_16,
        &mat_b5_dscs_16, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b5p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b5_pwb_16.data = b5_pwbiases[width - 2];
  mat_b5_pws_16.data = b5_pwscales[width - 2];
  __pw(input, output, width, 2, &mat_b5_pwout, &mat_b5_pww, &mat_b5_pwb_16,
       &mat_b5_pws_16, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b6_dscscales[] = {b6_dscs_4,  b6_dscs_6,  b6_dscs_8,
                                   b6_dscs_10, b6_dscs_12, b6_dscs_14,
                                   b6_dscs_16};

__ro_hinv fixed *b6_dscbiases[] = {b6_dscb_4,  b6_dscb_6,  b6_dscb_8,
                                   b6_dscb_10, b6_dscb_12, b6_dscb_14,
                                   b6_dscb_16};

__ro_hinv fixed *b6_pwscales[] = {b6_pws_4,  b6_pws_6,  b6_pws_8, b6_pws_10,
                                  b6_pws_12, b6_pws_14, b6_pws_16};
__ro_hinv fixed *b6_pwbiases[] = {b6_pwb_4,  b6_pwb_6,  b6_pwb_8, b6_pwb_10,
                                  b6_pwb_12, b6_pwb_14, b6_pwb_16};

void b6d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b6_dscb_16.data = b6_dscbiases[width - 2];
  mat_b6_dscs_16.data = b6_dscscales[width - 2];
  __dsc(input, output, width, 2, &mat_b6_dscout, &mat_b6_dscw, &mat_b6_dscb_16,
        &mat_b6_dscs_16, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b6p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b6_pwb_16.data = b6_pwbiases[width - 2];
  mat_b6_pws_16.data = b6_pwscales[width - 2];
  __pw(input, output, width, 2, &mat_b6_pwout, &mat_b6_pww, &mat_b6_pwb_16,
       &mat_b6_pws_16, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b7_dscscales[] = {b7_dscs_4,  b7_dscs_6,  b7_dscs_8,
                                   b7_dscs_10, b7_dscs_12, b7_dscs_14,
                                   b7_dscs_16};

__ro_hinv fixed *b7_dscbiases[] = {b7_dscb_4,  b7_dscb_6,  b7_dscb_8,
                                   b7_dscb_10, b7_dscb_12, b7_dscb_14,
                                   b7_dscb_16};

__ro_hinv fixed *b7_pwscales[] = {b7_pws_8,  b7_pws_12, b7_pws_16, b7_pws_20,
                                  b7_pws_24, b7_pws_28, b7_pws_32};

__ro_hinv fixed *b7_pwbiases[] = {b7_pwb_8,  b7_pwb_12, b7_pwb_16, b7_pwb_20,
                                  b7_pwb_24, b7_pwb_28, b7_pwb_32};

void b7d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b7_dscb_16.data = b7_dscbiases[width - 2];
  mat_b7_dscs_16.data = b7_dscscales[width - 2];
  __dsc(input, output, width, 2, &mat_b7_dscout, &mat_b7_dscw, &mat_b7_dscb_16,
        &mat_b7_dscs_16, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 2, .rows = 2},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b7p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b7_pwb_32.data = b7_pwbiases[width - 2];
  mat_b7_pws_32.data = b7_pwscales[width - 2];
  __pw(input, output, width, 4, &mat_b7_pwout, &mat_b7_pww, &mat_b7_pwb_32,
       &mat_b7_pws_32, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b8_dscscales[] = {b8_dscs_8,  b8_dscs_12, b8_dscs_16,
                                   b8_dscs_20, b8_dscs_24, b8_dscs_28,
                                   b8_dscs_32};

__ro_hinv fixed *b8_dscbiases[] = {b8_dscb_8,  b8_dscb_12, b8_dscb_16,
                                   b8_dscb_20, b8_dscb_24, b8_dscb_28,
                                   b8_dscb_32};
__ro_hinv fixed *b8_pwscales[] = {b8_pws_8,  b8_pws_12, b8_pws_16, b8_pws_20,
                                  b8_pws_24, b8_pws_28, b8_pws_32};

__ro_hinv fixed *b8_pwbiases[] = {b8_pwb_8,  b8_pwb_12, b8_pwb_16, b8_pwb_20,
                                  b8_pwb_24, b8_pwb_28, b8_pwb_32};

void b8d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b8_dscb_32.data = b8_dscbiases[width - 2];
  mat_b8_dscs_32.data = b8_dscscales[width - 2];
  __dsc(input, output, width, 4, &mat_b8_dscout, &mat_b8_dscw, &mat_b8_dscb_32,
        &mat_b8_dscs_32, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b8p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b8_pwb_32.data = b8_pwbiases[width - 2];
  mat_b8_pws_32.data = b8_pwscales[width - 2];
  __pw(input, output, width, 4, &mat_b8_pwout, &mat_b8_pww, &mat_b8_pwb_32,
       &mat_b8_pws_32, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b9_dscscales[] = {b9_dscs_8,  b9_dscs_12, b9_dscs_16,
                                   b9_dscs_20, b9_dscs_24, b9_dscs_28,
                                   b9_dscs_32};

__ro_hinv fixed *b9_dscbiases[] = {b9_dscb_8,  b9_dscb_12, b9_dscb_16,
                                   b9_dscb_20, b9_dscb_24, b9_dscb_28,
                                   b9_dscb_32};
__ro_hinv fixed *b9_pwscales[] = {b9_pws_8,  b9_pws_12, b9_pws_16, b9_pws_20,
                                  b9_pws_24, b9_pws_28, b9_pws_32};

__ro_hinv fixed *b9_pwbiases[] = {b9_pwb_8,  b9_pwb_12, b9_pwb_16, b9_pwb_20,
                                  b9_pwb_24, b9_pwb_28, b9_pwb_32};

void b9d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b9_dscb_32.data = b9_dscbiases[width - 2];
  mat_b9_dscs_32.data = b9_dscscales[width - 2];
  __dsc(input, output, width, 4, &mat_b9_dscout, &mat_b9_dscw, &mat_b9_dscb_32,
        &mat_b9_dscs_32, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b9p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b9_pwb_32.data = b9_pwbiases[width - 2];
  mat_b9_pws_32.data = b9_pwscales[width - 2];
  __pw(input, output, width, 4, &mat_b9_pwout, &mat_b9_pww, &mat_b9_pwb_32,
       &mat_b9_pws_32, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b10_dscscales[] = {b10_dscs_8,  b10_dscs_12, b10_dscs_16,
                                    b10_dscs_20, b10_dscs_24, b10_dscs_28,
                                    b10_dscs_32};

__ro_hinv fixed *b10_dscbiases[] = {b10_dscb_8,  b10_dscb_12, b10_dscb_16,
                                    b10_dscb_20, b10_dscb_24, b10_dscb_28,
                                    b10_dscb_32};
__ro_hinv fixed *b10_pwscales[] = {b10_pws_8,  b10_pws_12, b10_pws_16,
                                   b10_pws_20, b10_pws_24, b10_pws_28,
                                   b10_pws_32};

__ro_hinv fixed *b10_pwbiases[] = {b10_pwb_8,  b10_pwb_12, b10_pwb_16,
                                   b10_pwb_20, b10_pwb_24, b10_pwb_28,
                                   b10_pwb_32};

void b10d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b10_dscb_32.data = b10_dscbiases[width - 2];
  mat_b10_dscs_32.data = b10_dscscales[width - 2];
  __dsc(input, output, width, 4, &mat_b10_dscout, &mat_b10_dscw,
        &mat_b10_dscb_32, &mat_b10_dscs_32, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b10p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b10_pwb_32.data = b10_pwbiases[width - 2];
  mat_b10_pws_32.data = b10_pwscales[width - 2];
  __pw(input, output, width, 4, &mat_b10_pwout, &mat_b10_pww, &mat_b10_pwb_32,
       &mat_b10_pws_32, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b11_dscscales[] = {b11_dscs_8,  b11_dscs_12, b11_dscs_16,
                                    b11_dscs_20, b11_dscs_24, b11_dscs_28,
                                    b11_dscs_32};

__ro_hinv fixed *b11_dscbiases[] = {b11_dscb_8,  b11_dscb_12, b11_dscb_16,
                                    b11_dscb_20, b11_dscb_24, b11_dscb_28,
                                    b11_dscb_32};
__ro_hinv fixed *b11_pwscales[] = {b11_pws_8,  b11_pws_12, b11_pws_16,
                                   b11_pws_20, b11_pws_24, b11_pws_28,
                                   b11_pws_32};

__ro_hinv fixed *b11_pwbiases[] = {b11_pwb_8,  b11_pwb_12, b11_pwb_16,
                                   b11_pwb_20, b11_pwb_24, b11_pwb_28,
                                   b11_pwb_32};

void b11d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b11_dscb_32.data = b11_dscbiases[width - 2];
  mat_b11_dscs_32.data = b11_dscscales[width - 2];
  __dsc(input, output, width, 4, &mat_b11_dscout, &mat_b11_dscw,
        &mat_b11_dscb_32, &mat_b11_dscs_32, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b11p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b11_pwb_32.data = b11_pwbiases[width - 2];
  mat_b11_pws_32.data = b11_pwscales[width - 2];
  __pw(input, output, width, 4, &mat_b11_pwout, &mat_b11_pww, &mat_b11_pwb_32,
       &mat_b11_pws_32, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b12_dscscales[] = {b12_dscs_8,  b12_dscs_12, b12_dscs_16,
                                    b12_dscs_20, b12_dscs_24, b12_dscs_28,
                                    b12_dscs_32};

__ro_hinv fixed *b12_dscbiases[] = {b12_dscb_8,  b12_dscb_12, b12_dscb_16,
                                    b12_dscb_20, b12_dscb_24, b12_dscb_28,
                                    b12_dscb_32};

__ro_hinv fixed *b12_pwscales[] = {b12_pws_8,  b12_pws_12, b12_pws_16,
                                   b12_pws_20, b12_pws_24, b12_pws_28,
                                   b12_pws_32};

__ro_hinv fixed *b12_pwbiases[] = {b12_pwb_8,  b12_pwb_12, b12_pwb_16,
                                   b12_pwb_20, b12_pwb_24, b12_pwb_28,
                                   b12_pwb_32};

void b12d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b12_dscb_32.data = b12_dscbiases[width - 2];
  mat_b12_dscs_32.data = b12_dscscales[width - 2];
  __dsc(input, output, width, 4, &mat_b12_dscout, &mat_b12_dscw,
        &mat_b12_dscb_32, &mat_b12_dscs_32, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b12p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b12_pwb_32.data = b12_pwbiases[width - 2];
  mat_b12_pws_32.data = b12_pwscales[width - 2];
  __pw(input, output, width, 4, &mat_b12_pwout, &mat_b12_pww, &mat_b12_pwb_32,
       &mat_b12_pws_32, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b13_dscscales[] = {b13_dscs_8,  b13_dscs_12, b13_dscs_16,
                                    b13_dscs_20, b13_dscs_24, b13_dscs_28,
                                    b13_dscs_32};

__ro_hinv fixed *b13_dscbiases[] = {b13_dscb_8,  b13_dscb_12, b13_dscb_16,
                                    b13_dscb_20, b13_dscb_24, b13_dscb_28,
                                    b13_dscb_32};
__ro_hinv fixed *b13_pwscales[] = {b13_pws_16, b13_pws_24, b13_pws_32,
                                   b13_pws_40, b13_pws_48, b13_pws_56,
                                   b13_pws_64};

__ro_hinv fixed *b13_pwbiases[] = {b13_pwb_16, b13_pwb_24, b13_pwb_32,
                                   b13_pwb_40, b13_pwb_48, b13_pwb_56,
                                   b13_pwb_64};

void b13d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b13_dscb_32.data = b13_dscbiases[width - 2];
  mat_b13_dscs_32.data = b13_dscscales[width - 2];
  __dsc(input, output, width, 4, &mat_b13_dscout, &mat_b13_dscw,
        &mat_b13_dscb_32, &mat_b13_dscs_32, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 2, .rows = 2}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b13p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b13_pwb_64.data = b13_pwbiases[width - 2];
  mat_b13_pws_64.data = b13_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_b13_pwout, &mat_b13_pww, &mat_b13_pwb_64,
       &mat_b13_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b14_dscscales[] = {
    b14_dscs_16, b14_dscs_24, b14_dscs_32, b14_dscs_40,
    b14_dscs_48, b14_dscs_56, b14_dscs_64,
};

__ro_hinv fixed *b14_dscbiases[] = {
    b14_dscb_16, b14_dscb_24, b14_dscb_32, b14_dscb_40,
    b14_dscb_48, b14_dscb_56, b14_dscb_64,
};

__ro_hinv fixed *b14_pwscales[] = {b14_pws_16, b14_pws_24, b14_pws_32,
                                   b14_pws_40, b14_pws_48, b14_pws_56,
                                   b14_pws_64};

__ro_hinv fixed *b14_pwbiases[] = {b14_pwb_16, b14_pwb_24, b14_pwb_32,
                                   b14_pwb_40, b14_pwb_48, b14_pwb_56,
                                   b14_pwb_64};

void b14d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b14_dscb_64.data = b14_dscbiases[width - 2];
  mat_b14_dscs_64.data = b14_dscscales[width - 2];
  __dsc(input, output, width, 8, &mat_b14_dscout, &mat_b14_dscw,
        &mat_b14_dscb_64, &mat_b14_dscs_64, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b14p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b14_pwb_64.data = b14_pwbiases[width - 2];
  mat_b14_pws_64.data = b14_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_b14_pwout, &mat_b14_pww, &mat_b14_pwb_64,
       &mat_b14_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void e20(mat_t *input, mat_t *output, uint16_t width) { return; }
void ee20(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 8;
  MAT_SAMESHAPE(output, &mat_linear2in);
  MAT_SAMESHAPE(&mat_out, &mat_linear2out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(input, output, (stride_t){.cols = 6, .rows = 6}, 910);
  linear(output, &mat_out, &mat_linear2w, &mat_linear2b);
}

__ro_hinv fixed *out0_dscscales[] = {out0_dscs_2, out0_dscs_3, out0_dscs_4,
                                     out0_dscs_5, out0_dscs_6, out0_dscs_7,
                                     out0_dscs_8};

__ro_hinv fixed *out0_dscbiases[] = {out0_dscb_2, out0_dscb_3, out0_dscb_4,
                                     out0_dscb_5, out0_dscb_6, out0_dscb_7,
                                     out0_dscb_8};

__ro_hinv fixed *out0_pwscales[] = {out0_pws_4,  out0_pws_6,  out0_pws_8,
                                    out0_pws_10, out0_pws_12, out0_pws_14,
                                    out0_pws_16};

__ro_hinv fixed *out0_pwbiases[] = {out0_pwb_4,  out0_pwb_6,  out0_pwb_8,
                                    out0_pwb_10, out0_pwb_12, out0_pwb_14,
                                    out0_pwb_16};

void e00(mat_t *input, mat_t *output, uint16_t width) {
  // move input buf_exit for later storage
  MAT_SAMESHAPE(&bfe, input);
  mat_move(input, &bfe);
}
void ee00(mat_t *input, mat_t *output, uint16_t width) {
  input = &bfe;

  mat_out0_dscb_8.data = out0_dscbiases[width - 2];
  mat_out0_dscs_8.data = out0_dscscales[width - 2];
  __dsc(input, output, width, 1, &mat_out0_dscout, &mat_out0_dscw,
        &mat_out0_dscb_8, &mat_out0_dscs_8, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  MAT_SAMESHAPE(input, output);
  relu(output, input, 6 << input->p2quant);

  mat_out0_pwb_16.data = out0_pwbiases[width - 2];
  mat_out0_pws_16.data = out0_pwscales[width - 2];
  __pw(input, output, width, 2, &mat_out0_pwout, &mat_out0_pww,
       &mat_out0_pwb_16, &mat_out0_pws_16, (stride_t){.cols = 1, .rows = 1}, 0);
  MAT_SAMESHAPE(input, output);
  relu(output, input, 6 << input->p2quant);

  int width_seperation = 2;
  MAT_SAMESHAPE(output, &mat_linear0in);
  MAT_SAMESHAPE(&mat_out, &mat_linear0out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(input, output, (stride_t){.cols = 12, .rows = 12}, 228);
  linear(output, &mat_out, &mat_linear0w, &mat_linear0b);
}

__ro_hinv fixed *out1_dscscales[] = {out1_dscs_4,  out1_dscs_6,  out1_dscs_8,
                                     out1_dscs_10, out1_dscs_12, out1_dscs_14,
                                     out1_dscs_16};

__ro_hinv fixed *out1_dscbiases[] = {out1_dscb_4,  out1_dscb_6,  out1_dscb_8,
                                     out1_dscb_10, out1_dscb_12, out1_dscb_14,
                                     out1_dscb_16};

__ro_hinv fixed *out1_pwscales[] = {out1_pws_4,  out1_pws_6,  out1_pws_8,
                                    out1_pws_10, out1_pws_12, out1_pws_14,
                                    out1_pws_16};
__ro_hinv fixed *out1_pwbiases[] = {out1_pwb_4,  out1_pwb_6,  out1_pwb_8,
                                    out1_pwb_10, out1_pwb_12, out1_pwb_14,
                                    out1_pwb_16};

void e10(mat_t *input, mat_t *output, uint16_t width) {
  // move input buf_exit for later storage
  MAT_SAMESHAPE(&bfe, input);
  mat_move(input, &bfe);
}
void ee10(mat_t *input, mat_t *output, uint16_t width) {
  input = &bfe;

  mat_out1_dscb_16.data = out1_dscbiases[width - 2];
  mat_out1_dscs_16.data = out1_dscscales[width - 2];
  __dsc(input, output, width, 2, &mat_out1_dscout, &mat_out1_dscw,
        &mat_out1_dscb_16, &mat_out1_dscs_16, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  MAT_SAMESHAPE(input, output);
  relu(output, input, 6 << input->p2quant);

  mat_out1_pwb_16.data = out1_pwbiases[width - 2];
  mat_out1_pws_16.data = out1_pwscales[width - 2];
  __pw(input, output, width, 2, &mat_out1_pwout, &mat_out1_pww,
       &mat_out1_pwb_16, &mat_out1_pws_16, (stride_t){.cols = 1, .rows = 1}, 0);
  MAT_SAMESHAPE(input, output);
  relu(output, input, 6 << input->p2quant);

  int width_seperation = 2;
  MAT_SAMESHAPE(output, &mat_linear1in);
  MAT_SAMESHAPE(&mat_out, &mat_linear1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(input, output, (stride_t){.cols = 12, .rows = 12}, 228);
  linear(output, &mat_out, &mat_linear1w, &mat_linear1b);
}

__ro_hinv op_t dnn_layers[LAYER_LENGHT] = {
    (op_t){.fn = b1c, .s = &mat_in0, .d = &bf0},
    (op_t){.fn = b2d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b2p, .s = &bf1, .d = &bf0},
    (op_t){.fn = e00, .s = &bf0, .d = &bf1, .ee = ee00, .v = 6},
    (op_t){.fn = b3d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b3p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b4d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b4p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b5d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b5p, .s = &bf1, .d = &bf0},
    (op_t){.fn = e10, .s = &bf0, .d = &bf1, .ee = ee10, .v = 13},
    (op_t){.fn = b6d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b6p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b7d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b7p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b8d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b8p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b9d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b9p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b10d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b10p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b11d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b11p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b12d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b12p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b13d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b13p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b14d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b14p, .s = &bf1, .d = &bf0},
    (op_t){.fn = e20, .s = &bf0, .d = &bf1, .ee = ee20},
};