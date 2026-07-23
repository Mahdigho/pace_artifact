#ifndef LINEAR1_H
#define LINEAR1_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed linear1w[11][64];
extern __ro_hinv fixed linear1b[11];
__ro_nv mat_t mat_linear1w = {.dims = {11, 64},
                              .len_dims = 2,
                              .strides = {64, 1},
                              .data = (fixed *)linear1w,
                              .p2quant = 12};
__ro_nv mat_t mat_linear1b = {.dims = {11},
                              .len_dims = 1,
                              .strides = {1},
                              .data = (fixed *)linear1b,
                              .p2quant = 10};
__ro_nv mat_t mat_linear1in = {.dims = {1, 64},
                               .len_dims = 2,
                               .strides = {64, 1},
                               .data = (fixed *)NULL,
                               .p2quant = 10};
__ro_nv mat_t mat_linear1out = {.dims = {1, 11},
                                .len_dims = 2,
                                .strides = {11, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};

#endif