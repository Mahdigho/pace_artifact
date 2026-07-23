#ifndef OUT_0_1_H
#define OUT_0_1_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed out_0_1w[10][10][3][3];
extern __ro_hinv fixed out_0_1s_3[3];
extern __ro_hinv fixed out_0_1b_3[3];
extern __ro_hinv fixed out_0_1s_4[4];
extern __ro_hinv fixed out_0_1b_4[4];
extern __ro_hinv fixed out_0_1s_5[5];
extern __ro_hinv fixed out_0_1b_5[5];
extern __ro_hinv fixed out_0_1s_6[6];
extern __ro_hinv fixed out_0_1b_6[6];
extern __ro_hinv fixed out_0_1s_7[7];
extern __ro_hinv fixed out_0_1b_7[7];
extern __ro_hinv fixed out_0_1s_8[8];
extern __ro_hinv fixed out_0_1b_8[8];
extern __ro_hinv fixed out_0_1s_9[9];
extern __ro_hinv fixed out_0_1b_9[9];
extern __ro_hinv fixed out_0_1s_10[10];
extern __ro_hinv fixed out_0_1b_10[10];
__ro_nv mat_t mat_out_0_1w = {.dims = {10, 10, 3, 3}, .len_dims = 4, .strides = {90, 9, 3, 1}, .data = (fixed *)out_0_1w, .p2quant = 15};
__ro_nv mat_t mat_out_0_1s_10 = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)out_0_1s_10, .p2quant = 15};
__ro_nv mat_t mat_out_0_1b_10 = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)out_0_1b_10, .p2quant = 10};
__ro_nv mat_t mat_out_0_1in = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_out_0_1out = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif