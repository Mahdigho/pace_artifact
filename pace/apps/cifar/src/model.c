#include "dnn/include/dnn.h"
#include "include/inf.h"
#include "weights/include/b0.h"
#include "weights/include/b1_1.h"
#include "weights/include/b1_2.h"
#include "weights/include/b2_1.h"
#include "weights/include/b2_2.h"
#include "weights/include/b2_s.h"
#include "weights/include/b3_1.h"
#include "weights/include/b3_2.h"
#include "weights/include/b3_s.h"
#include "weights/include/inputs.h"
#include "weights/include/linear0.h"
#include "weights/include/linear1.h"
#include "weights/include/linear2.h"
#include "weights/include/mat_defs.h"
#include "weights/include/out_0_1.h"
#include "weights/include/out_0_2.h"
#include "weights/include/out_1_1.h"
#include "weights/include/out_1_2.h"
#include "weights/include/out_1_s.h"

/* Memory Overhead
 *
Base neurons: 11560 \* 2 + 10240
Early Exit +11560 \* 2
EarlySlim:

| Layer   | All   | Slim Weights |
| ------- | ----- | ------------ |
| b0      | 270   | 20           |
| b11     | 900   | 104          |
| b12     | 900   | 104          |
| b21     | 1800  | 208          |
| b22     | 3600  | 208          |
| b2s     | 1800  | 208          |
| b31     | 7200  | 416          |
| b32     | 11400 | 416          |
| b3s     | 7200  | 416          |
| linear0 | 400   | 0            |
| linear1 | 800   | 0            |
| linaer2 | 400   | 0            |
| out01   | 900   | 104          |
| out02   | 900   | 104          |
| out11   | 1800  | 216          |
| out12   | 3600  | 216          |
| out1s   | 1800  | 216          |


Base:  35070 + 33360 + 400 =  68830
Early Exit: 68830 + 23120 + 10200 = 102150
Slimmable: 68830 + 2100 = 70930
EarlySlim: 102150 + 856 + 2100 = 105106

*/
__ro_hinv fixed _padb0[11560];
__ro_hinv fixed _padb1[11560];
__ro_hinv fixed _padb_exit[11560];
__ro_hinv fixed _padb_exit2[11560];
__ro_hinv fixed _resb0[10240];
__ro_hinv fixed _out_bf0[10];

__ro_nv mat_t bf0 = {.dims = {1, MAX_ACTIVE_SIZE},
                     .len_dims = 1,
                     .strides = {MAX_ACTIVE_SIZE, 1},
                     .data = (fixed *)_padb0,
                     .p2quant = 1};

__ro_nv mat_t bf1 = {.dims = {1, MAX_ACTIVE_SIZE},
                     .len_dims = 1,
                     .strides = {MAX_ACTIVE_SIZE, 1},
                     .data = (fixed *)_padb1,
                     .p2quant = 1};

__ro_nv mat_t bfe = {.dims = {1, MAX_ACTIVE_SIZE},
                     .len_dims = 1,
                     .strides = {MAX_ACTIVE_SIZE, 1},
                     .data = (fixed *)_padb_exit,
                     .p2quant = 1};

__ro_nv mat_t bfe2 = {.dims = {1, MAX_ACTIVE_SIZE},
                      .len_dims = 1,
                      .strides = {MAX_ACTIVE_SIZE, 1},
                      .data = (fixed *)_padb_exit2,
                      .p2quant = 1};

__ro_nv mat_t res = {.dims = {1, MAX_ACTIVE_SIZE},
                     .len_dims = 1,
                     .strides = {MAX_ACTIVE_SIZE, 1},
                     .data = (fixed *)_resb0,
                     .p2quant = 1};

__ro_nv mat_t mat_out = {.dims = {1, 10},
                         .len_dims = 2,
                         .strides = {10, 1},
                         .data = (fixed *)_out_bf0,
                         .p2quant = 1};

void b00(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 1;
  MAT_SAMESHAPE(output, &mat_b0out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b0w, &mat_b0b_10, &mat_b0s_10, (pad_t){0, 0, 0, 0},
         (stride_t){.cols = 1, .rows = 1});
  inplace_relu(output, 6 << output->p2quant);
}

__ro_nv fixed *b1_1scales[] = {
    b1_1s_3, b1_1s_4, b1_1s_5, b1_1s_6, b1_1s_7, b1_1s_8, b1_1s_9, b1_1s_10,
};

__ro_nv fixed *b1_1biases[] = {
    b1_1b_3, b1_1b_4, b1_1b_5, b1_1b_6, b1_1b_7, b1_1b_8, b1_1b_9, b1_1b_10,
};

void b11(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 1;
  MAT_SAMESHAPE(&res, &mat_b1_2out);
  MAT_SAMESHAPE(output, &mat_b1_1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  UPDATE_WIDTH((&res), width_seperation, width, 0);
  mat_move(input, &res);

  mat_b1_1b_10.data = b1_1biases[width - 3];
  mat_b1_1s_10.data = b1_1scales[width - 3];
  convNN(input, output, &mat_b1_1w, &mat_b1_1b_10, &mat_b1_1s_10,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  inplace_relu(output, 6 << output->p2quant);
}

__ro_nv fixed *b1_2scales[] = {
    b1_2s_3, b1_2s_4, b1_2s_5, b1_2s_6, b1_2s_7, b1_2s_8, b1_2s_9, b1_2s_10,
};

__ro_nv fixed *b1_2biases[] = {
    b1_2b_3, b1_2b_4, b1_2b_5, b1_2b_6, b1_2b_7, b1_2b_8, b1_2b_9, b1_2b_10,
};

void b12(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 1;
  mat_b1_2b_10.data = b1_2biases[width - 3];
  mat_b1_2s_10.data = b1_2scales[width - 3];
  MAT_SAMESHAPE(output, &mat_b1_2out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b1_2w, &mat_b1_2b_10, &mat_b1_2s_10,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  mat_add(output, &res, output);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b2_sscales[] = {
    b2_ss_6,  b2_ss_8,  b2_ss_10, b2_ss_12,
    b2_ss_14, b2_ss_16, b2_ss_18, b2_ss_20,
};

__ro_hinv fixed *b2_sbiases[] = {
    b2_sb_6,  b2_sb_8,  b2_sb_10, b2_sb_12,
    b2_sb_14, b2_sb_16, b2_sb_18, b2_sb_20,
};

void b2s(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 2;
  mat_b2_sb_20.data = b2_sbiases[width - 3];
  mat_b2_ss_20.data = b2_sscales[width - 3];
  MAT_SAMESHAPE(&res, &mat_b2_sout);
  UPDATE_WIDTH(&res, width_seperation, width, 0);
  pwNN(input, &res, &mat_b2_sw, &mat_b2_sb_20, &mat_b2_ss_20,
       (stride_t){.cols = 2, .rows = 2});
}

__ro_hinv fixed *b2_1scales[] = {
    b2_1s_6,  b2_1s_8,  b2_1s_10, b2_1s_12,
    b2_1s_14, b2_1s_16, b2_1s_18, b2_1s_20,
};

__ro_hinv fixed *b2_1biases[] = {
    b2_1b_6,  b2_1b_8,  b2_1b_10, b2_1b_12,
    b2_1b_14, b2_1b_16, b2_1b_18, b2_1b_20,
};

void b21(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 2;
  mat_b2_1b_20.data = b2_1biases[width - 3];
  mat_b2_1s_20.data = b2_1scales[width - 3];
  MAT_SAMESHAPE(output, &mat_b2_1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b2_1w, &mat_b2_1b_20, &mat_b2_1s_20,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 2, .rows = 2});
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b2_2scales[] = {
    b2_2s_6,  b2_2s_8,  b2_2s_10, b2_2s_12,
    b2_2s_14, b2_2s_16, b2_2s_18, b2_2s_20,
};

__ro_hinv fixed *b2_2biases[] = {
    b2_2b_6,  b2_2b_8,  b2_2b_10, b2_2b_12,
    b2_2b_14, b2_2b_16, b2_2b_18, b2_2b_20,
};

void b22(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 2;
  mat_b2_2b_20.data = b2_2biases[width - 3];
  mat_b2_2s_20.data = b2_2scales[width - 3];
  MAT_SAMESHAPE(output, &mat_b2_2out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b2_2w, &mat_b2_2b_20, &mat_b2_2s_20,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  mat_add(output, &res, output);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_hinv fixed *b3_sscales[] = {
    b3_ss_12, b3_ss_16, b3_ss_20, b3_ss_24,
    b3_ss_28, b3_ss_32, b3_ss_36, b3_ss_40,
};

__ro_hinv fixed *b3_sbiases[] = {
    b3_sb_12, b3_sb_16, b3_sb_20, b3_sb_24,
    b3_sb_28, b3_sb_32, b3_sb_36, b3_sb_40,
};

void b3s(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 4;
  mat_b3_sb_40.data = b3_sbiases[width - 3];
  mat_b3_ss_40.data = b3_sscales[width - 3];
  MAT_SAMESHAPE(&res, &mat_b3_sout);
  UPDATE_WIDTH(&res, width_seperation, width, 0);
  pwNN(input, &res, &mat_b3_sw, &mat_b3_sb_40, &mat_b3_ss_40,
       (stride_t){.cols = 2, .rows = 2});
}

__ro_nv fixed *b3_1scales[] = {
    b3_1s_12, b3_1s_16, b3_1s_20, b3_1s_24,
    b3_1s_28, b3_1s_32, b3_1s_36, b3_1s_40,
};

__ro_nv fixed *b3_1biases[] = {
    b3_1b_12, b3_1b_16, b3_1b_20, b3_1b_24,
    b3_1b_28, b3_1b_32, b3_1b_36, b3_1b_40,
};

void b31(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 4;
  mat_b3_1b_40.data = b3_1biases[width - 3];
  mat_b3_1s_40.data = b3_1scales[width - 3];
  MAT_SAMESHAPE(output, &mat_b3_1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b3_1w, &mat_b3_1b_40, &mat_b3_1s_40,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 2, .rows = 2});
  inplace_relu(output, 6 << output->p2quant);
}

__ro_nv fixed *b3_2scales[] = {
    b3_2s_12, b3_2s_16, b3_2s_20, b3_2s_24,
    b3_2s_28, b3_2s_32, b3_2s_36, b3_2s_40,
};

__ro_nv fixed *b3_2biases[] = {
    b3_2b_12, b3_2b_16, b3_2b_20, b3_2b_24,
    b3_2b_28, b3_2b_32, b3_2b_36, b3_2b_40,
};

void b32(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 4;
  mat_b3_2b_40.data = b3_2biases[width - 3];
  mat_b3_2s_40.data = b3_2scales[width - 3];
  MAT_SAMESHAPE(output, &mat_b3_2out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(input, output, &mat_b3_2w, &mat_b3_2b_40, &mat_b3_2s_40,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  mat_add(output, &res, output);
  inplace_relu(output, 6 << output->p2quant);
}

__ro_nv fixed *out_0_1scales[] = {
    out_0_1s_3, out_0_1s_4, out_0_1s_5, out_0_1s_6,
    out_0_1s_7, out_0_1s_8, out_0_1s_9, out_0_1s_10,
};

__ro_nv fixed *out_0_1biases[] = {
    out_0_1b_3, out_0_1b_4, out_0_1b_5, out_0_1b_6,
    out_0_1b_7, out_0_1b_8, out_0_1b_9, out_0_1b_10,
};

__ro_nv fixed *out_0_2scales[] = {
    out_0_2s_3, out_0_2s_4, out_0_2s_5, out_0_2s_6,
    out_0_2s_7, out_0_2s_8, out_0_2s_9, out_0_2s_10,
};

__ro_nv fixed *out_0_2biases[] = {
    out_0_2b_3, out_0_2b_4, out_0_2b_5, out_0_2b_6,
    out_0_2b_7, out_0_2b_8, out_0_2b_9, out_0_2b_10,
};

void e00(mat_t *input, mat_t *output, uint16_t width) {
  // move input buf_exit for later storage
  MAT_SAMESHAPE(&bfe, input);
  mat_move(input, &bfe);
  ;
}
void ee00(mat_t *input, mat_t *output, uint16_t width) {
  input = &bfe;
  output = &bfe2;

  // residual --> res
  MAT_SAMESHAPE(&res, &bfe);
  mat_move(&bfe, &res);

  int width_seperation = 1;

  // input  to out_0_1 --> buf_exit
  // output to out_0_1 --> output

  mat_out_0_1b_10.data = out_0_1biases[width - 3];
  mat_out_0_1s_10.data = out_0_1scales[width - 3];
  MAT_SAMESHAPE(output, &mat_out_0_1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(&bfe, output, &mat_out_0_1w, &mat_out_0_1b_10, &mat_out_0_1s_10,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  inplace_relu(output, 6 << output->p2quant);

  // input  to out_0_2 --> output
  // output to out_0_2 --> buf_exit
  mat_out_0_2b_10.data = out_0_2biases[width - 3];
  mat_out_0_2s_10.data = out_0_2scales[width - 3];
  MAT_SAMESHAPE((&bfe), &mat_out_0_2out);
  UPDATE_WIDTH((&bfe), width_seperation, width, 0);
  convNN(output, (&bfe), &mat_out_0_2w, &mat_out_0_2b_10, &mat_out_0_2s_10,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  // add residiual
  mat_add((&bfe), &res, (&bfe));
  inplace_relu((&bfe), 6 << bfe.p2quant);

  // input  to avg_pool --> buf_exit
  // output to avg_pool --> output
  width_seperation = 4;
  MAT_SAMESHAPE(output, &mat_linear0in);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(&bfe, output, (stride_t){.cols = 16, .rows = 16}, 128);

  MAT_SAMESHAPE(&mat_out, &mat_linear0out);
  linear(output, &mat_out, &mat_linear0w, &mat_linear0b);
}

__ro_nv fixed *out_1_sscales[] = {
    out_1_ss_6,  out_1_ss_8,  out_1_ss_10, out_1_ss_12,
    out_1_ss_14, out_1_ss_16, out_1_ss_18, out_1_ss_20,
};

__ro_nv fixed *out_1_sbiases[] = {
    out_1_sb_6,  out_1_sb_8,  out_1_sb_10, out_1_sb_12,
    out_1_sb_14, out_1_sb_16, out_1_sb_18, out_1_sb_20,
};

__ro_nv fixed *out_1_1scales[] = {
    out_1_1s_6,  out_1_1s_8,  out_1_1s_10, out_1_1s_12,
    out_1_1s_14, out_1_1s_16, out_1_1s_18, out_1_1s_20,
};

__ro_nv fixed *out_1_1biases[] = {
    out_1_1b_6,  out_1_1b_8,  out_1_1b_10, out_1_1b_12,
    out_1_1b_14, out_1_1b_16, out_1_1b_18, out_1_1b_20,
};

__ro_nv fixed *out_1_2scales[] = {
    out_1_2s_6,  out_1_2s_8,  out_1_2s_10, out_1_2s_12,
    out_1_2s_14, out_1_2s_16, out_1_2s_18, out_1_2s_20,
};

__ro_nv fixed *out_1_2biases[] = {
    out_1_2b_6,  out_1_2b_8,  out_1_2b_10, out_1_2b_12,
    out_1_2b_14, out_1_2b_16, out_1_2b_18, out_1_2b_20,
};

void e10(mat_t *input, mat_t *output, uint16_t width) {
  // move input buf_exit for later storage
  MAT_SAMESHAPE(&bfe, input);
  mat_move(input, &bfe);
}
void ee10(mat_t *input, mat_t *output, uint16_t width) {
  input = &bfe;
  output = &bfe2;

  // input is in bfe
  int width_seperation = 2;

  // residual --> mat_res0
  mat_out_1_sb_20.data = out_1_sbiases[width - 3];
  mat_out_1_ss_20.data = out_1_sscales[width - 3];
  MAT_SAMESHAPE(&res, &mat_out_1_sout);
  UPDATE_WIDTH(&res, width_seperation, width, 0);
  pwNN(&bfe, &res, &mat_out_1_sw, &mat_out_1_sb_20, &mat_out_1_ss_20,
       (stride_t){.cols = 2, .rows = 2});

  // input  to out_1_1 --> buf_exit
  // output to out_1_1 --> output

  mat_out_1_1b_20.data = out_1_1biases[width - 3];
  mat_out_1_1s_20.data = out_1_1scales[width - 3];
  MAT_SAMESHAPE(output, &mat_out_1_1out);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  convNN(&bfe, output, &mat_out_1_1w, &mat_out_1_1b_20, &mat_out_1_1s_20,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 2, .rows = 2});
  inplace_relu(output, 6 << output->p2quant);

  // input  to out_0_2 --> output
  // output to out_0_2 --> buf_exit
  mat_out_1_2b_20.data = out_1_2biases[width - 3];
  mat_out_1_2s_20.data = out_1_2scales[width - 3];
  MAT_SAMESHAPE((&bfe), &mat_out_1_2out);
  UPDATE_WIDTH((&bfe), width_seperation, width, 0);
  convNN(output, (&bfe), &mat_out_1_2w, &mat_out_1_2b_20, &mat_out_1_2s_20,
         (pad_t){1, 1, 1, 1}, (stride_t){.cols = 1, .rows = 1});
  // add residiual
  mat_add((&bfe), &res, (&bfe));
  inplace_relu((&bfe), 6 << bfe.p2quant);

  // input  to avg_pool --> buf_exit
  // output to avg_pool --> output
  width_seperation = 8;
  MAT_SAMESHAPE(output, &mat_linear1in);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(&bfe, output, (stride_t){.cols = 8, .rows = 8}, 512);

  MAT_SAMESHAPE(&mat_out, &mat_linear1out);
  linear(output, &mat_out, &mat_linear1w, &mat_linear1b);
}

void e20(mat_t *input, mat_t *output, uint16_t width) { return; }
void ee20(mat_t *input, mat_t *output, uint16_t width) {
  int width_seperation = 4;
  MAT_SAMESHAPE(output, &mat_linear2in);
  UPDATE_WIDTH(output, width_seperation, width, 0);
  avg_pool(input, output, (stride_t){.cols = 8, .rows = 8}, 512);
  MAT_SAMESHAPE(&mat_out, &mat_linear2out);
  linear(output, &mat_out, &mat_linear2w, &mat_linear2b);
}

__ro_hinv op_t dnn_layers[LAYER_LENGHT] = {
    (op_t){.fn = b00, .s = &mat_in0, .d = &bf0, .ee = NULL},
    (op_t){.fn = e00, .s = &bf0, .d = &bf1, .ee = ee00, .v = 2},
    (op_t){.fn = b11, .s = &bf0, .d = &bf1, .ee = NULL},
    (op_t){.fn = b12, .s = &bf1, .d = &bf0, .ee = NULL},
    (op_t){.fn = e10, .s = &bf0, .d = &bf1, .ee = ee10, .v = 3},
    (op_t){.fn = b2s, .s = &bf0, .d = NULL, .ee = NULL},
    (op_t){.fn = b21, .s = &bf0, .d = &bf1, .ee = NULL},
    (op_t){.fn = b22, .s = &bf1, .d = &bf0, .ee = NULL},
    (op_t){.fn = b3s, .s = &bf0, .d = NULL, .ee = NULL},
    (op_t){.fn = b31, .s = &bf0, .d = &bf1, .ee = NULL},
    (op_t){.fn = b32, .s = &bf1, .d = &bf0, .ee = NULL},
    (op_t){.fn = e20, .s = &bf0, .d = &bf1, .ee = ee20},
};