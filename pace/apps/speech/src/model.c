#include "dnn/include/dnn.h"
#include "include/inf.h"
#include "weights/include/b0.h"
#include "weights/include/b2_dsc.h"
#include "weights/include/b2_pw.h"
#include "weights/include/b3_dsc.h"
#include "weights/include/b3_pw.h"
#include "weights/include/b4_dsc.h"
#include "weights/include/b4_pw.h"
#include "weights/include/b5_dsc.h"
#include "weights/include/b5_pw.h"
#include "weights/include/inputs.h"
#include "weights/include/linear0.h"
#include "weights/include/linear1.h"
#include "weights/include/out0_dsc.h"
#include "weights/include/out0_pw.h"

/* Memory Overhead
 *
Base neurons: 23520
Early Exit +11776
EarlySlim:

| Layer   | All  | Slim Weights |
| ------- | ---- | ------------ |
| b0      | 2560 | 128          |
| b2      | 4672 | 1120         |
| b3      | 4672 | 1120         |
| b4      | 4672 | 1120         |
| b5      | 4672 | 1120         |
| linaer1 | 704  | 0            |
| linear0 | 704  | 0            |
| out0    | 4672 | 1120         |


Base:  23520 + 21952 =  45472
Early Exit: 45472 + 4672 + 704 + 11776 = 62624
Slimmable: 45472 + 1120*4+128 = 50080
EarlySlim: 62624 + 1120*5+128 = 68352

*/

#define MAX_SIZE 11776
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

__ro_hinv mat_t mat_out = {.dims = {1, 11},
                           .len_dims = 2,
                           .strides = {11, 1},
                           .data = (fixed *)_out_buf0,
                           .p2quant = 1};

void b1c(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 8;
  MAT_SAMESHAPE(output, &mat_b0out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b0w, &mat_b0b_64, &mat_b0s_64, (pad_t){0, 0, 0, 0},
         (stride_t){.cols = 2, .rows = 2});
  inplace_relu(output, 6 << output->p2quant);
}

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

__ro_hinv fixed *b2_dscscales[] = {b2_dscs_16, b2_dscs_24, b2_dscs_32,
                                   b2_dscs_40, b2_dscs_48, b2_dscs_56,
                                   b2_dscs_64};

__ro_hinv fixed *b2_dscbiases[] = {b2_dscb_16, b2_dscb_24, b2_dscb_32,
                                   b2_dscb_40, b2_dscb_48, b2_dscb_56,
                                   b2_dscb_64};

__ro_hinv fixed *b3_dscscales[] = {b3_dscs_16, b3_dscs_24, b3_dscs_32,
                                   b3_dscs_40, b3_dscs_48, b3_dscs_56,
                                   b3_dscs_64};

__ro_hinv fixed *b3_dscbiases[] = {b3_dscb_16, b3_dscb_24, b3_dscb_32,
                                   b3_dscb_40, b3_dscb_48, b3_dscb_56,
                                   b3_dscb_64};

__ro_hinv fixed *b4_dscscales[] = {b4_dscs_16, b4_dscs_24, b4_dscs_32,
                                   b4_dscs_40, b4_dscs_48, b4_dscs_56,
                                   b4_dscs_64};

__ro_hinv fixed *b4_dscbiases[] = {b4_dscb_16, b4_dscb_24, b4_dscb_32,
                                   b4_dscb_40, b4_dscb_48, b4_dscb_56,
                                   b4_dscb_64};

__ro_hinv fixed *b5_dscscales[] = {b5_dscs_16, b5_dscs_24, b5_dscs_32,
                                   b5_dscs_40, b5_dscs_48, b5_dscs_56,
                                   b5_dscs_64};

__ro_hinv fixed *b5_dscbiases[] = {b5_dscb_16, b5_dscb_24, b5_dscb_32,
                                   b5_dscb_40, b5_dscb_48, b5_dscb_56,
                                   b5_dscb_64};

void b2d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b2_dscb_64.data = b2_dscbiases[width - 2];
  mat_b2_dscs_64.data = b2_dscscales[width - 2];
  __dsc(input, output, width, 8, &mat_b2_dscout, &mat_b2_dscw, &mat_b2_dscb_64,
        &mat_b2_dscs_64, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b3d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b3_dscb_64.data = b3_dscbiases[width - 2];
  mat_b3_dscs_64.data = b3_dscscales[width - 2];
  __dsc(input, output, width, 8, &mat_b3_dscout, &mat_b3_dscw, &mat_b3_dscb_64,
        &mat_b3_dscs_64, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b4d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b4_dscb_64.data = b4_dscbiases[width - 2];
  mat_b4_dscs_64.data = b4_dscscales[width - 2];
  __dsc(input, output, width, 8, &mat_b4_dscout, &mat_b4_dscw, &mat_b4_dscb_64,
        &mat_b4_dscs_64, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}

void b5d(mat_t *input, mat_t *output, uint16_t width) {
  mat_b5_dscb_64.data = b5_dscbiases[width - 2];
  mat_b5_dscs_64.data = b5_dscscales[width - 2];
  __dsc(input, output, width, 8, &mat_b5_dscout, &mat_b5_dscw, &mat_b5_dscb_64,
        &mat_b5_dscs_64, (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1},
        0);
  inplace_relu(output, 6 << output->p2quant);
}
__ro_hinv fixed *b2_pwscales[] = {b2_pws_16, b2_pws_24, b2_pws_32, b2_pws_40,
                                  b2_pws_48, b2_pws_56, b2_pws_64};

__ro_hinv fixed *b2_pwbiases[] = {b2_pwb_16, b2_pwb_24, b2_pwb_32, b2_pwb_40,
                                  b2_pwb_48, b2_pwb_56, b2_pwb_64};
__ro_hinv fixed *b3_pwscales[] = {b3_pws_16, b3_pws_24, b3_pws_32, b3_pws_40,
                                  b3_pws_48, b3_pws_56, b3_pws_64};

__ro_hinv fixed *b3_pwbiases[] = {b3_pwb_16, b3_pwb_24, b3_pwb_32, b3_pwb_40,
                                  b3_pwb_48, b3_pwb_56, b3_pwb_64};

__ro_hinv fixed *b4_pwscales[] = {b4_pws_16, b4_pws_24, b4_pws_32, b4_pws_40,
                                  b4_pws_48, b4_pws_56, b4_pws_64};

__ro_hinv fixed *b4_pwbiases[] = {b4_pwb_16, b4_pwb_24, b4_pwb_32, b4_pwb_40,
                                  b4_pwb_48, b4_pwb_56, b4_pwb_64};

__ro_hinv fixed *b5_pwscales[] = {b5_pws_16, b5_pws_24, b5_pws_32, b5_pws_40,
                                  b5_pws_48, b5_pws_56, b5_pws_64};

__ro_hinv fixed *b5_pwbiases[] = {b5_pwb_16, b5_pwb_24, b5_pwb_32, b5_pwb_40,
                                  b5_pwb_48, b5_pwb_56, b5_pwb_64};

void b2p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b2_pwb_64.data = b2_pwbiases[width - 2];
  mat_b2_pws_64.data = b2_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_b2_pwout, &mat_b2_pww, &mat_b2_pwb_64,
       &mat_b2_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b3p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b3_pwb_64.data = b3_pwbiases[width - 2];
  mat_b3_pws_64.data = b3_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_b3_pwout, &mat_b3_pww, &mat_b3_pwb_64,
       &mat_b3_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b4p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b4_pwb_64.data = b4_pwbiases[width - 2];
  mat_b4_pws_64.data = b4_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_b4_pwout, &mat_b4_pww, &mat_b4_pwb_64,
       &mat_b4_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

void b5p(mat_t *input, mat_t *output, uint16_t width) {
  mat_b5_pwb_64.data = b5_pwbiases[width - 2];
  mat_b5_pws_64.data = b5_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_b5_pwout, &mat_b5_pww, &mat_b5_pwb_64,
       &mat_b5_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *out0_dscscales[] = {out0_dscs_16, out0_dscs_24, out0_dscs_32,
                                     out0_dscs_40, out0_dscs_48, out0_dscs_56,
                                     out0_dscs_64};

__ro_hinv fixed *out0_dscbiases[] = {out0_dscb_16, out0_dscb_24, out0_dscb_32,
                                     out0_dscb_40, out0_dscb_48, out0_dscb_56,
                                     out0_dscb_64};

__ro_hinv fixed *out0_pwscales[] = {out0_pws_16, out0_pws_24, out0_pws_32,
                                    out0_pws_40, out0_pws_48, out0_pws_56,
                                    out0_pws_64};

__ro_hinv fixed *out0_pwbiases[] = {out0_pwb_16, out0_pwb_24, out0_pwb_32,
                                    out0_pwb_40, out0_pwb_48, out0_pwb_56,
                                    out0_pwb_64};

void e00(mat_t *input, mat_t *output, uint16_t width) {
  // move input buf_exit for later storage
  MAT_SAMESHAPE(&bfe, input);
  mat_move(input, &bfe);
}

void ee00(mat_t *input, mat_t *output, uint16_t width) {
  input = &bfe;
  mat_out0_dscb_64.data = out0_dscbiases[width - 2];
  mat_out0_dscs_64.data = out0_dscscales[width - 2];
  __dsc(input, output, width, 8, &mat_out0_dscout, &mat_out0_dscw,
        &mat_out0_dscb_64, &mat_out0_dscs_64, (pad_t){1, 1, 1, 1},
        (stride_t){.cols = 1, .rows = 1}, 0);
  MAT_SAMESHAPE(input, output);
  relu(output, input, 6 << input->p2quant);

  mat_out0_pwb_64.data = out0_pwbiases[width - 2];
  mat_out0_pws_64.data = out0_pwscales[width - 2];
  __pw(input, output, width, 8, &mat_out0_pwout, &mat_out0_pww,
       &mat_out0_pwb_64, &mat_out0_pws_64, (stride_t){.cols = 1, .rows = 1}, 0);
  MAT_SAMESHAPE(input, output);
  relu(output, input, 6 << input->p2quant);

  int width_seperation = 8;
  MAT_SAMESHAPE(output, &mat_linear0in);
  MAT_SAMESHAPE(&mat_out, &mat_linear0out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(input, output, (stride_t){.cols = 21, .rows = 6}, 260);
  linear(output, &mat_out, &mat_linear0w, &mat_linear0b);
}

void e10(mat_t *input, mat_t *output, uint16_t width) { return; }
void ee10(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 8;
  MAT_SAMESHAPE(output, &mat_linear1in);
  MAT_SAMESHAPE(&mat_out, &mat_linear1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(input, output, (stride_t){.cols = 21, .rows = 6}, 260);
  linear(output, &mat_out, &mat_linear1w, &mat_linear1b);
}

__ro_hinv op_t dnn_layers[LAYER_LENGHT] = {
    (op_t){.fn = b1c, .s = &mat_in0, .d = &bf0},
    (op_t){.fn = e00, .s = &bf0, .d = &bf1, .ee = ee00, .v = 6},
    (op_t){.fn = b2d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b2p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b3d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b3p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b4d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b4p, .s = &bf1, .d = &bf0},
    (op_t){.fn = b5d, .s = &bf0, .d = &bf1},
    (op_t){.fn = b5p, .s = &bf1, .d = &bf0},
    (op_t){.fn = e10, .s = &bf0, .d = &bf1, .ee = ee10},
};