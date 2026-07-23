#ifndef B5_PW_H
#define B5_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b5_pww[64][64][1][1];
extern __ro_hinv fixed b5_pws_16[16];
extern __ro_hinv fixed b5_pwb_16[16];
extern __ro_hinv fixed b5_pws_24[24];
extern __ro_hinv fixed b5_pwb_24[24];
extern __ro_hinv fixed b5_pws_32[32];
extern __ro_hinv fixed b5_pwb_32[32];
extern __ro_hinv fixed b5_pws_40[40];
extern __ro_hinv fixed b5_pwb_40[40];
extern __ro_hinv fixed b5_pws_48[48];
extern __ro_hinv fixed b5_pwb_48[48];
extern __ro_hinv fixed b5_pws_56[56];
extern __ro_hinv fixed b5_pwb_56[56];
extern __ro_hinv fixed b5_pws_64[64];
extern __ro_hinv fixed b5_pwb_64[64];
__ro_nv mat_t mat_b5_pww = {.dims = {64, 64, 1, 1}, .len_dims = 4, .strides = {64, 1, 1, 1}, .data = (fixed *)b5_pww, .p2quant = 12};
__ro_nv mat_t mat_b5_pws_64 = {.dims = {64}, .len_dims = 1, .strides = {1}, .data = (fixed *)b5_pws_64, .p2quant = 15};
__ro_nv mat_t mat_b5_pwb_64 = {.dims = {64}, .len_dims = 1, .strides = {1}, .data = (fixed *)b5_pwb_64, .p2quant = 10};
__ro_nv mat_t mat_b5_pwin = {.dims = {1, 64, 6, 21}, .len_dims = 4, .strides = {8064, 126, 21, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b5_pwout = {.dims = {1, 64, 6, 21}, .len_dims = 4, .strides = {8064, 126, 21, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif