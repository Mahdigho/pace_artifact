#ifndef B3_1_H
#define B3_1_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b3_1w[40][20][3][3];
extern __ro_hinv fixed b3_1s_12[12];
extern __ro_hinv fixed b3_1b_12[12];
extern __ro_hinv fixed b3_1s_16[16];
extern __ro_hinv fixed b3_1b_16[16];
extern __ro_hinv fixed b3_1s_20[20];
extern __ro_hinv fixed b3_1b_20[20];
extern __ro_hinv fixed b3_1s_24[24];
extern __ro_hinv fixed b3_1b_24[24];
extern __ro_hinv fixed b3_1s_28[28];
extern __ro_hinv fixed b3_1b_28[28];
extern __ro_hinv fixed b3_1s_32[32];
extern __ro_hinv fixed b3_1b_32[32];
extern __ro_hinv fixed b3_1s_36[36];
extern __ro_hinv fixed b3_1b_36[36];
extern __ro_hinv fixed b3_1s_40[40];
extern __ro_hinv fixed b3_1b_40[40];
__ro_nv mat_t mat_b3_1w = {.dims = {40, 20, 3, 3}, .len_dims = 4, .strides = {180, 9, 3, 1}, .data = (fixed *)b3_1w, .p2quant = 15};
__ro_nv mat_t mat_b3_1s_40 = {.dims = {40}, .len_dims = 1, .strides = {1}, .data = (fixed *)b3_1s_40, .p2quant = 15};
__ro_nv mat_t mat_b3_1b_40 = {.dims = {40}, .len_dims = 1, .strides = {1}, .data = (fixed *)b3_1b_40, .p2quant = 10};
__ro_nv mat_t mat_b3_1in = {.dims = {1, 20, 16, 16}, .len_dims = 4, .strides = {5120, 256, 16, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b3_1out = {.dims = {1, 40, 8, 8}, .len_dims = 4, .strides = {2560, 64, 8, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif