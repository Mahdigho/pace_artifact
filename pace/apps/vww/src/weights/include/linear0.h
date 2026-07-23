#ifndef LINEAR0_H
#define LINEAR0_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed linear0w[2][64];
extern __ro_hinv fixed linear0b[2];
__ro_hinv mat_t mat_linear0w = {.dims = {2, 64},
                                .len_dims = 2,
                                .strides = {64, 1},
                                .data = (fixed *)linear0w,
                                .p2quant = 15};
__ro_hinv mat_t mat_linear0b = {.dims = {2},
                                .len_dims = 1,
                                .strides = {1},
                                .data = (fixed *)linear0b,
                                .p2quant = 10};
__ro_hinv mat_t mat_linear0in = {.dims = {1, 64},
                                 .len_dims = 2,
                                 .strides = {64, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};
__ro_hinv mat_t mat_linear0out = {.dims = {1, 2},
                                  .len_dims = 2,
                                  .strides = {2, 1},
                                  .data = (fixed *)NULL,
                                  .p2quant = 10};

#endif