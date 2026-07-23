#ifndef B5_PW_H
#define B5_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b5_pww[16][16][1][1];
extern __ro_hinv fixed b5_pws_4[4];
extern __ro_hinv fixed b5_pwb_4[4];
extern __ro_hinv fixed b5_pws_6[6];
extern __ro_hinv fixed b5_pwb_6[6];
extern __ro_hinv fixed b5_pws_8[8];
extern __ro_hinv fixed b5_pwb_8[8];
extern __ro_hinv fixed b5_pws_10[10];
extern __ro_hinv fixed b5_pwb_10[10];
extern __ro_hinv fixed b5_pws_12[12];
extern __ro_hinv fixed b5_pwb_12[12];
extern __ro_hinv fixed b5_pws_14[14];
extern __ro_hinv fixed b5_pwb_14[14];
extern __ro_hinv fixed b5_pws_16[16];
extern __ro_hinv fixed b5_pwb_16[16];
__ro_hinv mat_t mat_b5_pww = {.dims = {16, 16, 1, 1},
                              .len_dims = 4,
                              .strides = {16, 1, 1, 1},
                              .data = (fixed *)b5_pww,
                              .p2quant = 15};
__ro_hinv mat_t mat_b5_pws_16 = {.dims = {16},
                                 .len_dims = 1,
                                 .strides = {1},
                                 .data = (fixed *)b5_pws_16,
                                 .p2quant = 15};
__ro_hinv mat_t mat_b5_pwb_16 = {.dims = {16},
                                 .len_dims = 1,
                                 .strides = {1},
                                 .data = (fixed *)b5_pwb_16,
                                 .p2quant = 10};
__ro_hinv mat_t mat_b5_pwin = {.dims = {1, 16, 24, 24},
                               .len_dims = 4,
                               .strides = {9216, 576, 24, 1},
                               .data = (fixed *)NULL,
                               .p2quant = 10};
__ro_hinv mat_t mat_b5_pwout = {.dims = {1, 16, 24, 24},
                                .len_dims = 4,
                                .strides = {9216, 576, 24, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};

#endif