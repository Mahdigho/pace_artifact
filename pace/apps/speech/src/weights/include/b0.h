#ifndef B0_H
#define B0_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b0w[64][1][4][10];
extern __ro_hinv fixed b0s_16[16];
extern __ro_hinv fixed b0b_16[16];
extern __ro_hinv fixed b0s_24[24];
extern __ro_hinv fixed b0b_24[24];
extern __ro_hinv fixed b0s_32[32];
extern __ro_hinv fixed b0b_32[32];
extern __ro_hinv fixed b0s_40[40];
extern __ro_hinv fixed b0b_40[40];
extern __ro_hinv fixed b0s_48[48];
extern __ro_hinv fixed b0b_48[48];
extern __ro_hinv fixed b0s_56[56];
extern __ro_hinv fixed b0b_56[56];
extern __ro_hinv fixed b0s_64[64];
extern __ro_hinv fixed b0b_64[64];
__ro_nv mat_t mat_b0w = {.dims = {64, 1, 4, 10},
                         .len_dims = 4,
                         .strides = {40, 40, 10, 1},
                         .data = (fixed *)b0w,
                         .p2quant = 19};
__ro_nv mat_t mat_b0s_64 = {.dims = {64},
                            .len_dims = 1,
                            .strides = {1},
                            .data = (fixed *)b0s_64,
                            .p2quant = 15};
__ro_nv mat_t mat_b0b_64 = {.dims = {64},
                            .len_dims = 1,
                            .strides = {1},
                            .data = (fixed *)b0b_64,
                            .p2quant = 10};
__ro_nv mat_t mat_b0in = {.dims = {1, 1, 12, 49},
                          .len_dims = 4,
                          .strides = {588, 588, 49, 1},
                          .data = (fixed *)NULL,
                          .p2quant = 10};
__ro_nv mat_t mat_b0out = {.dims = {1, 64, 6, 21},
                           .len_dims = 4,
                           .strides = {8064, 126, 21, 1},
                           .data = (fixed *)NULL,
                           .p2quant = 10};

#endif