#ifndef B11_PW_H
#define B11_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b11_pww[32][32][1][1];
extern __ro_hinv fixed b11_pws_8[8];
extern __ro_hinv fixed b11_pwb_8[8];
extern __ro_hinv fixed b11_pws_12[12];
extern __ro_hinv fixed b11_pwb_12[12];
extern __ro_hinv fixed b11_pws_16[16];
extern __ro_hinv fixed b11_pwb_16[16];
extern __ro_hinv fixed b11_pws_20[20];
extern __ro_hinv fixed b11_pwb_20[20];
extern __ro_hinv fixed b11_pws_24[24];
extern __ro_hinv fixed b11_pwb_24[24];
extern __ro_hinv fixed b11_pws_28[28];
extern __ro_hinv fixed b11_pwb_28[28];
extern __ro_hinv fixed b11_pws_32[32];
extern __ro_hinv fixed b11_pwb_32[32];
__ro_hinv mat_t mat_b11_pww = {.dims = {32, 32, 1, 1},
                               .len_dims = 4,
                               .strides = {32, 1, 1, 1},
                               .data = (fixed *)b11_pww,
                               .p2quant = 14};
__ro_hinv mat_t mat_b11_pws_32 = {.dims = {32},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b11_pws_32,
                                  .p2quant = 15};
__ro_hinv mat_t mat_b11_pwb_32 = {.dims = {32},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b11_pwb_32,
                                  .p2quant = 10};
__ro_hinv mat_t mat_b11_pwin = {.dims = {1, 32, 12, 12},
                                .len_dims = 4,
                                .strides = {4608, 144, 12, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};
__ro_hinv mat_t mat_b11_pwout = {.dims = {1, 32, 12, 12},
                                 .len_dims = 4,
                                 .strides = {4608, 144, 12, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};

#endif