#ifndef B0_H
#define B0_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b0w[4][3][3][3];
extern __ro_hinv fixed b0s_1[1];
extern __ro_hinv fixed b0b_1[1];
extern __ro_hinv fixed b0s_2[2];
extern __ro_hinv fixed b0b_2[2];
extern __ro_hinv fixed b0s_3[3];
extern __ro_hinv fixed b0b_3[3];
extern __ro_hinv fixed b0s_4[4];
extern __ro_hinv fixed b0b_4[4];
__ro_hinv mat_t mat_b0w = {.dims = {4, 3, 3, 3},
                           .len_dims = 4,
                           .strides = {27, 9, 3, 1},
                           .data = (fixed *)b0w,
                           .p2quant = 13};
__ro_hinv mat_t mat_b0s_4 = {.dims = {4},
                             .len_dims = 1,
                             .strides = {1},
                             .data = (fixed *)b0s_4,
                             .p2quant = 15};
__ro_hinv mat_t mat_b0b_4 = {.dims = {4},
                             .len_dims = 1,
                             .strides = {1},
                             .data = (fixed *)b0b_4,
                             .p2quant = 10};
__ro_hinv mat_t mat_b0in = {.dims = {1, 3, 96, 96},
                            .len_dims = 4,
                            .strides = {27648, 9216, 96, 1},
                            .data = (fixed *)NULL,
                            .p2quant = 10};
__ro_hinv mat_t mat_b0out = {.dims = {1, 4, 48, 48},
                             .len_dims = 4,
                             .strides = {9216, 2304, 48, 1},
                             .data = (fixed *)NULL,
                             .p2quant = 10};

#endif