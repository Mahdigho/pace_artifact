#ifndef B1_2_H
#define B1_2_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b1_2w[10][10][3][3];
extern __ro_hinv fixed b1_2s_3[3];
extern __ro_hinv fixed b1_2b_3[3];
extern __ro_hinv fixed b1_2s_4[4];
extern __ro_hinv fixed b1_2b_4[4];
extern __ro_hinv fixed b1_2s_5[5];
extern __ro_hinv fixed b1_2b_5[5];
extern __ro_hinv fixed b1_2s_6[6];
extern __ro_hinv fixed b1_2b_6[6];
extern __ro_hinv fixed b1_2s_7[7];
extern __ro_hinv fixed b1_2b_7[7];
extern __ro_hinv fixed b1_2s_8[8];
extern __ro_hinv fixed b1_2b_8[8];
extern __ro_hinv fixed b1_2s_9[9];
extern __ro_hinv fixed b1_2b_9[9];
extern __ro_hinv fixed b1_2s_10[10];
extern __ro_hinv fixed b1_2b_10[10];
__ro_nv mat_t mat_b1_2w = {.dims = {10, 10, 3, 3}, .len_dims = 4, .strides = {90, 9, 3, 1}, .data = (fixed *)b1_2w, .p2quant = 15};
__ro_nv mat_t mat_b1_2s_10 = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)b1_2s_10, .p2quant = 15};
__ro_nv mat_t mat_b1_2b_10 = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)b1_2b_10, .p2quant = 10};
__ro_nv mat_t mat_b1_2in = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b1_2out = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif