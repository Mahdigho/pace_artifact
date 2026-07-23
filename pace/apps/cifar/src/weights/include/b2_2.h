#ifndef B2_2_H
#define B2_2_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b2_2w[20][20][3][3];
extern __ro_hinv fixed b2_2s_6[6];
extern __ro_hinv fixed b2_2b_6[6];
extern __ro_hinv fixed b2_2s_8[8];
extern __ro_hinv fixed b2_2b_8[8];
extern __ro_hinv fixed b2_2s_10[10];
extern __ro_hinv fixed b2_2b_10[10];
extern __ro_hinv fixed b2_2s_12[12];
extern __ro_hinv fixed b2_2b_12[12];
extern __ro_hinv fixed b2_2s_14[14];
extern __ro_hinv fixed b2_2b_14[14];
extern __ro_hinv fixed b2_2s_16[16];
extern __ro_hinv fixed b2_2b_16[16];
extern __ro_hinv fixed b2_2s_18[18];
extern __ro_hinv fixed b2_2b_18[18];
extern __ro_hinv fixed b2_2s_20[20];
extern __ro_hinv fixed b2_2b_20[20];
__ro_nv mat_t mat_b2_2w = {.dims = {20, 20, 3, 3}, .len_dims = 4, .strides = {180, 9, 3, 1}, .data = (fixed *)b2_2w, .p2quant = 14};
__ro_nv mat_t mat_b2_2s_20 = {.dims = {20}, .len_dims = 1, .strides = {1}, .data = (fixed *)b2_2s_20, .p2quant = 15};
__ro_nv mat_t mat_b2_2b_20 = {.dims = {20}, .len_dims = 1, .strides = {1}, .data = (fixed *)b2_2b_20, .p2quant = 10};
__ro_nv mat_t mat_b2_2in = {.dims = {1, 20, 16, 16}, .len_dims = 4, .strides = {5120, 256, 16, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b2_2out = {.dims = {1, 20, 16, 16}, .len_dims = 4, .strides = {5120, 256, 16, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif