#ifndef B11_DSC_H
#define B11_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b11_dscw[32][1][3][3];
extern __ro_hinv fixed b11_dscs_8[8];
extern __ro_hinv fixed b11_dscb_8[8];
extern __ro_hinv fixed b11_dscs_12[12];
extern __ro_hinv fixed b11_dscb_12[12];
extern __ro_hinv fixed b11_dscs_16[16];
extern __ro_hinv fixed b11_dscb_16[16];
extern __ro_hinv fixed b11_dscs_20[20];
extern __ro_hinv fixed b11_dscb_20[20];
extern __ro_hinv fixed b11_dscs_24[24];
extern __ro_hinv fixed b11_dscb_24[24];
extern __ro_hinv fixed b11_dscs_28[28];
extern __ro_hinv fixed b11_dscb_28[28];
extern __ro_hinv fixed b11_dscs_32[32];
extern __ro_hinv fixed b11_dscb_32[32];
__ro_hinv mat_t mat_b11_dscw = {.dims = {32, 1, 3, 3},
                                .len_dims = 4,
                                .strides = {9, 9, 3, 1},
                                .data = (fixed *)b11_dscw,
                                .p2quant = 14};
__ro_hinv mat_t mat_b11_dscs_32 = {.dims = {32},
                                   .len_dims = 1,
                                   .strides = {1},
                                   .data = (fixed *)b11_dscs_32,
                                   .p2quant = 15};
__ro_hinv mat_t mat_b11_dscb_32 = {.dims = {32},
                                   .len_dims = 1,
                                   .strides = {1},
                                   .data = (fixed *)b11_dscb_32,
                                   .p2quant = 10};
__ro_hinv mat_t mat_b11_dscin = {.dims = {1, 32, 12, 12},
                                 .len_dims = 4,
                                 .strides = {4608, 144, 12, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};
__ro_hinv mat_t mat_b11_dscout = {.dims = {1, 32, 12, 12},
                                  .len_dims = 4,
                                  .strides = {4608, 144, 12, 1},
                                  .data = (fixed *)NULL,
                                  .p2quant = 10};

#endif