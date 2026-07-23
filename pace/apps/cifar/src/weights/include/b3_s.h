#ifndef B3_S_H
#define B3_S_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b3_sw[40][20][1][1];
extern __ro_hinv fixed b3_ss_12[12];
extern __ro_hinv fixed b3_sb_12[12];
extern __ro_hinv fixed b3_ss_16[16];
extern __ro_hinv fixed b3_sb_16[16];
extern __ro_hinv fixed b3_ss_20[20];
extern __ro_hinv fixed b3_sb_20[20];
extern __ro_hinv fixed b3_ss_24[24];
extern __ro_hinv fixed b3_sb_24[24];
extern __ro_hinv fixed b3_ss_28[28];
extern __ro_hinv fixed b3_sb_28[28];
extern __ro_hinv fixed b3_ss_32[32];
extern __ro_hinv fixed b3_sb_32[32];
extern __ro_hinv fixed b3_ss_36[36];
extern __ro_hinv fixed b3_sb_36[36];
extern __ro_hinv fixed b3_ss_40[40];
extern __ro_hinv fixed b3_sb_40[40];
__ro_nv mat_t mat_b3_sw = {.dims = {40, 20, 1, 1}, .len_dims = 4, .strides = {20, 1, 1, 1}, .data = (fixed *)b3_sw, .p2quant = 14};
__ro_nv mat_t mat_b3_ss_40 = {.dims = {40}, .len_dims = 1, .strides = {1}, .data = (fixed *)b3_ss_40, .p2quant = 15};
__ro_nv mat_t mat_b3_sb_40 = {.dims = {40}, .len_dims = 1, .strides = {1}, .data = (fixed *)b3_sb_40, .p2quant = 10};
__ro_nv mat_t mat_b3_sin = {.dims = {1, 20, 16, 16}, .len_dims = 4, .strides = {5120, 256, 16, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b3_sout = {.dims = {1, 40, 8, 8}, .len_dims = 4, .strides = {2560, 64, 8, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif