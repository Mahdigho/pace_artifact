#ifndef B7_PW_H
#define B7_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b7_pww[32][16][1][1];
extern __ro_hinv fixed b7_pws_8[8];
extern __ro_hinv fixed b7_pwb_8[8];
extern __ro_hinv fixed b7_pws_12[12];
extern __ro_hinv fixed b7_pwb_12[12];
extern __ro_hinv fixed b7_pws_16[16];
extern __ro_hinv fixed b7_pwb_16[16];
extern __ro_hinv fixed b7_pws_20[20];
extern __ro_hinv fixed b7_pwb_20[20];
extern __ro_hinv fixed b7_pws_24[24];
extern __ro_hinv fixed b7_pwb_24[24];
extern __ro_hinv fixed b7_pws_28[28];
extern __ro_hinv fixed b7_pwb_28[28];
extern __ro_hinv fixed b7_pws_32[32];
extern __ro_hinv fixed b7_pwb_32[32];
__ro_hinv mat_t mat_b7_pww = {.dims = {32, 16, 1, 1},
                              .len_dims = 4,
                              .strides = {16, 1, 1, 1},
                              .data = (fixed *)b7_pww,
                              .p2quant = 14};
__ro_hinv mat_t mat_b7_pws_32 = {.dims = {32},
                                 .len_dims = 1,
                                 .strides = {1},
                                 .data = (fixed *)b7_pws_32,
                                 .p2quant = 15};
__ro_hinv mat_t mat_b7_pwb_32 = {.dims = {32},
                                 .len_dims = 1,
                                 .strides = {1},
                                 .data = (fixed *)b7_pwb_32,
                                 .p2quant = 10};
__ro_hinv mat_t mat_b7_pwin = {.dims = {1, 16, 12, 12},
                               .len_dims = 4,
                               .strides = {2304, 144, 12, 1},
                               .data = (fixed *)NULL,
                               .p2quant = 10};
__ro_hinv mat_t mat_b7_pwout = {.dims = {1, 32, 12, 12},
                                .len_dims = 4,
                                .strides = {4608, 144, 12, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};

#endif