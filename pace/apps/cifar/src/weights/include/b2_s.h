#ifndef B2_S_H
#define B2_S_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b2_sw[20][10][1][1];
extern __ro_hinv fixed b2_ss_6[6];
extern __ro_hinv fixed b2_sb_6[6];
extern __ro_hinv fixed b2_ss_8[8];
extern __ro_hinv fixed b2_sb_8[8];
extern __ro_hinv fixed b2_ss_10[10];
extern __ro_hinv fixed b2_sb_10[10];
extern __ro_hinv fixed b2_ss_12[12];
extern __ro_hinv fixed b2_sb_12[12];
extern __ro_hinv fixed b2_ss_14[14];
extern __ro_hinv fixed b2_sb_14[14];
extern __ro_hinv fixed b2_ss_16[16];
extern __ro_hinv fixed b2_sb_16[16];
extern __ro_hinv fixed b2_ss_18[18];
extern __ro_hinv fixed b2_sb_18[18];
extern __ro_hinv fixed b2_ss_20[20];
extern __ro_hinv fixed b2_sb_20[20];
__ro_nv mat_t mat_b2_sw = {.dims = {20, 10, 1, 1}, .len_dims = 4, .strides = {10, 1, 1, 1}, .data = (fixed *)b2_sw, .p2quant = 15};
__ro_nv mat_t mat_b2_ss_20 = {.dims = {20}, .len_dims = 1, .strides = {1}, .data = (fixed *)b2_ss_20, .p2quant = 15};
__ro_nv mat_t mat_b2_sb_20 = {.dims = {20}, .len_dims = 1, .strides = {1}, .data = (fixed *)b2_sb_20, .p2quant = 10};
__ro_nv mat_t mat_b2_sin = {.dims = {1, 10, 32, 32}, .len_dims = 4, .strides = {10240, 1024, 32, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b2_sout = {.dims = {1, 20, 16, 16}, .len_dims = 4, .strides = {5120, 256, 16, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif