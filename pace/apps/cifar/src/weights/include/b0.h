#ifndef B0_H
#define B0_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b0w[10][3][3][3];
extern __ro_hinv fixed b0s_3[3];
extern __ro_hinv fixed b0b_3[3];
extern __ro_hinv fixed b0s_4[4];
extern __ro_hinv fixed b0b_4[4];
extern __ro_hinv fixed b0s_5[5];
extern __ro_hinv fixed b0b_5[5];
extern __ro_hinv fixed b0s_6[6];
extern __ro_hinv fixed b0b_6[6];
extern __ro_hinv fixed b0s_7[7];
extern __ro_hinv fixed b0b_7[7];
extern __ro_hinv fixed b0s_8[8];
extern __ro_hinv fixed b0b_8[8];
extern __ro_hinv fixed b0s_9[9];
extern __ro_hinv fixed b0b_9[9];
extern __ro_hinv fixed b0s_10[10];
extern __ro_hinv fixed b0b_10[10];
__ro_nv mat_t mat_b0w = {.dims = {10, 3, 3, 3}, .len_dims = 4, .strides = {27, 9, 3, 1}, .data = (fixed *)b0w, .p2quant = 13};
__ro_nv mat_t mat_b0s_10 = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)b0s_10, .p2quant = 15};
__ro_nv mat_t mat_b0b_10 = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)b0b_10, .p2quant = 10};
__ro_nv mat_t mat_b0in = {.dims = {1, 3, 32, 32}, .len_dims = 4, .strides = {3072, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b0out = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif