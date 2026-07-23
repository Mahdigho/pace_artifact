#ifndef B13_PW_H
#define B13_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b13_pww[64][32][1][1];
extern __ro_hinv fixed b13_pws_16[16];
extern __ro_hinv fixed b13_pwb_16[16];
extern __ro_hinv fixed b13_pws_24[24];
extern __ro_hinv fixed b13_pwb_24[24];
extern __ro_hinv fixed b13_pws_32[32];
extern __ro_hinv fixed b13_pwb_32[32];
extern __ro_hinv fixed b13_pws_40[40];
extern __ro_hinv fixed b13_pwb_40[40];
extern __ro_hinv fixed b13_pws_48[48];
extern __ro_hinv fixed b13_pwb_48[48];
extern __ro_hinv fixed b13_pws_56[56];
extern __ro_hinv fixed b13_pwb_56[56];
extern __ro_hinv fixed b13_pws_64[64];
extern __ro_hinv fixed b13_pwb_64[64];
__ro_hinv mat_t mat_b13_pww = {.dims = {64, 32, 1, 1},
                               .len_dims = 4,
                               .strides = {32, 1, 1, 1},
                               .data = (fixed *)b13_pww,
                               .p2quant = 14};
__ro_hinv mat_t mat_b13_pws_64 = {.dims = {64},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b13_pws_64,
                                  .p2quant = 15};
__ro_hinv mat_t mat_b13_pwb_64 = {.dims = {64},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b13_pwb_64,
                                  .p2quant = 10};
__ro_hinv mat_t mat_b13_pwin = {.dims = {1, 32, 6, 6},
                                .len_dims = 4,
                                .strides = {1152, 36, 6, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};
__ro_hinv mat_t mat_b13_pwout = {.dims = {1, 64, 6, 6},
                                 .len_dims = 4,
                                 .strides = {2304, 36, 6, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};

#endif