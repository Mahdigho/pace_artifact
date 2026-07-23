#ifndef B2_PW_H
#define B2_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b2_pww[8][4][1][1];
extern __ro_hinv fixed b2_pws_2[2];
extern __ro_hinv fixed b2_pwb_2[2];
extern __ro_hinv fixed b2_pws_3[3];
extern __ro_hinv fixed b2_pwb_3[3];
extern __ro_hinv fixed b2_pws_4[4];
extern __ro_hinv fixed b2_pwb_4[4];
extern __ro_hinv fixed b2_pws_5[5];
extern __ro_hinv fixed b2_pwb_5[5];
extern __ro_hinv fixed b2_pws_6[6];
extern __ro_hinv fixed b2_pwb_6[6];
extern __ro_hinv fixed b2_pws_7[7];
extern __ro_hinv fixed b2_pwb_7[7];
extern __ro_hinv fixed b2_pws_8[8];
extern __ro_hinv fixed b2_pwb_8[8];
__ro_hinv mat_t mat_b2_pww = {.dims = {8, 4, 1, 1},
                              .len_dims = 4,
                              .strides = {4, 1, 1, 1},
                              .data = (fixed *)b2_pww,
                              .p2quant = 11};
__ro_hinv mat_t mat_b2_pws_8 = {.dims = {8},
                                .len_dims = 1,
                                .strides = {1},
                                .data = (fixed *)b2_pws_8,
                                .p2quant = 15};
__ro_hinv mat_t mat_b2_pwb_8 = {.dims = {8},
                                .len_dims = 1,
                                .strides = {1},
                                .data = (fixed *)b2_pwb_8,
                                .p2quant = 10};
__ro_hinv mat_t mat_b2_pwin = {.dims = {1, 4, 24, 24},
                               .len_dims = 4,
                               .strides = {2304, 576, 24, 1},
                               .data = (fixed *)NULL,
                               .p2quant = 10};
__ro_hinv mat_t mat_b2_pwout = {.dims = {1, 8, 24, 24},
                                .len_dims = 4,
                                .strides = {4608, 576, 24, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};

#endif