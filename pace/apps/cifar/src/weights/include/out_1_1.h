#ifndef OUT_1_1_H
#define OUT_1_1_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed out_1_1w[20][10][3][3];
extern __ro_hinv fixed out_1_1s_6[6];
extern __ro_hinv fixed out_1_1b_6[6];
extern __ro_hinv fixed out_1_1s_8[8];
extern __ro_hinv fixed out_1_1b_8[8];
extern __ro_hinv fixed out_1_1s_10[10];
extern __ro_hinv fixed out_1_1b_10[10];
extern __ro_hinv fixed out_1_1s_12[12];
extern __ro_hinv fixed out_1_1b_12[12];
extern __ro_hinv fixed out_1_1s_14[14];
extern __ro_hinv fixed out_1_1b_14[14];
extern __ro_hinv fixed out_1_1s_16[16];
extern __ro_hinv fixed out_1_1b_16[16];
extern __ro_hinv fixed out_1_1s_18[18];
extern __ro_hinv fixed out_1_1b_18[18];
extern __ro_hinv fixed out_1_1s_20[20];
extern __ro_hinv fixed out_1_1b_20[20];
__ro_nv mat_t mat_out_1_1w = {.dims = {20, 10, 3, 3}, .len_dims = 4, .strides = {90, 9, 3, 1}, .data = (fixed *)out_1_1w, .p2quant = 16};
__ro_nv mat_t mat_out_1_1s_20 = {.dims = {20}, .len_dims = 1, .strides = {1}, .data = (fixed *)out_1_1s_20, .p2quant = 15};
__ro_nv mat_t mat_out_1_1b_20 = {.dims = {20}, .len_dims = 1, .strides = {1}, .data = (fixed *)out_1_1b_20, .p2quant = 10};
__ro_nv mat_t mat_out_1_1in = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_out_1_1out = {.dims = {1, 20, 16, 16}, .len_dims = 4, .strides = {5120, 256, 16, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif