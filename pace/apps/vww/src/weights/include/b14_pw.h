#ifndef B14_PW_H
#define B14_PW_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b14_pww[64][64][1][1];
extern __ro_hinv fixed b14_pws_16[16];
extern __ro_hinv fixed b14_pwb_16[16];
extern __ro_hinv fixed b14_pws_24[24];
extern __ro_hinv fixed b14_pwb_24[24];
extern __ro_hinv fixed b14_pws_32[32];
extern __ro_hinv fixed b14_pwb_32[32];
extern __ro_hinv fixed b14_pws_40[40];
extern __ro_hinv fixed b14_pwb_40[40];
extern __ro_hinv fixed b14_pws_48[48];
extern __ro_hinv fixed b14_pwb_48[48];
extern __ro_hinv fixed b14_pws_56[56];
extern __ro_hinv fixed b14_pwb_56[56];
extern __ro_hinv fixed b14_pws_64[64];
extern __ro_hinv fixed b14_pwb_64[64];
__ro_hinv mat_t mat_b14_pww = {.dims = {64, 64, 1, 1},
                               .len_dims = 4,
                               .strides = {64, 1, 1, 1},
                               .data = (fixed *)b14_pww,
                               .p2quant = 13};
__ro_hinv mat_t mat_b14_pws_64 = {.dims = {64},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b14_pws_64,
                                  .p2quant = 15};
__ro_hinv mat_t mat_b14_pwb_64 = {.dims = {64},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b14_pwb_64,
                                  .p2quant = 10};
__ro_hinv mat_t mat_b14_pwin = {.dims = {1, 64, 6, 6},
                                .len_dims = 4,
                                .strides = {2304, 36, 6, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};
__ro_hinv mat_t mat_b14_pwout = {.dims = {1, 64, 6, 6},
                                 .len_dims = 4,
                                 .strides = {2304, 36, 6, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};

#endif