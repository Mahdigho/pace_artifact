#ifndef B5_DSC_H
#define B5_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b5_dscw[64][1][3][3];
extern __ro_hinv fixed b5_dscs_16[16];
extern __ro_hinv fixed b5_dscb_16[16];
extern __ro_hinv fixed b5_dscs_24[24];
extern __ro_hinv fixed b5_dscb_24[24];
extern __ro_hinv fixed b5_dscs_32[32];
extern __ro_hinv fixed b5_dscb_32[32];
extern __ro_hinv fixed b5_dscs_40[40];
extern __ro_hinv fixed b5_dscb_40[40];
extern __ro_hinv fixed b5_dscs_48[48];
extern __ro_hinv fixed b5_dscb_48[48];
extern __ro_hinv fixed b5_dscs_56[56];
extern __ro_hinv fixed b5_dscb_56[56];
extern __ro_hinv fixed b5_dscs_64[64];
extern __ro_hinv fixed b5_dscb_64[64];
__ro_nv mat_t mat_b5_dscw = {.dims = {64, 1, 3, 3}, .len_dims = 4, .strides = {9, 9, 3, 1}, .data = (fixed *)b5_dscw, .p2quant = 11};
__ro_nv mat_t mat_b5_dscs_64 = {.dims = {64}, .len_dims = 1, .strides = {1}, .data = (fixed *)b5_dscs_64, .p2quant = 15};
__ro_nv mat_t mat_b5_dscb_64 = {.dims = {64}, .len_dims = 1, .strides = {1}, .data = (fixed *)b5_dscb_64, .p2quant = 10};
__ro_nv mat_t mat_b5_dscin = {.dims = {1, 64, 6, 21}, .len_dims = 4, .strides = {8064, 126, 21, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_b5_dscout = {.dims = {1, 64, 6, 21}, .len_dims = 4, .strides = {8064, 126, 21, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif