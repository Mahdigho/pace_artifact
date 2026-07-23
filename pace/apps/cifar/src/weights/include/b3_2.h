#ifndef B3_2_H
#define B3_2_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b3_2w[40][40][3][3];
extern __ro_hinv fixed b3_2s_12[12];
extern __ro_hinv fixed b3_2b_12[12];
extern __ro_hinv fixed b3_2s_16[16];
extern __ro_hinv fixed b3_2b_16[16];
extern __ro_hinv fixed b3_2s_20[20];
extern __ro_hinv fixed b3_2b_20[20];
extern __ro_hinv fixed b3_2s_24[24];
extern __ro_hinv fixed b3_2b_24[24];
extern __ro_hinv fixed b3_2s_28[28];
extern __ro_hinv fixed b3_2b_28[28];
extern __ro_hinv fixed b3_2s_32[32];
extern __ro_hinv fixed b3_2b_32[32];
extern __ro_hinv fixed b3_2s_36[36];
extern __ro_hinv fixed b3_2b_36[36];
extern __ro_hinv fixed b3_2s_40[40];
extern __ro_hinv fixed b3_2b_40[40];
__ro_nv mat_t mat_b3_2w = {.dims = {40, 40, 3, 3}, .len_dims = 4, .strides = {360, 9, 3, 1}, .data = (fixed *)b3_2w, .p2quant = 14};
__ro_nv mat_t mat_b3_2s_40 = {.dims = {40}, .len_dims = 1, .strides = {1}, .data = (fixed *)b3_2s_40, .p2quant = 15};
__ro_nv mat_t mat_b3_2b_40 = {.dims = {40}, .len_dims = 1, .strides = {1}, .data = (fixed *)b3_2b_40, .p2quant = 10};
__ro_nv mat_t mat_b3_2in = {.dims = {1, 40, 8, 8}, .len_dims = 4, .strides = {2560, 64, 8, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b3_2out = {.dims = {1, 40, 8, 8}, .len_dims = 4, .strides = {2560, 64, 8, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif