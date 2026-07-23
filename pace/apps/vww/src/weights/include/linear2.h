#ifndef LINEAR2_H
#define LINEAR2_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed linear2w[2][64];
extern __ro_hinv fixed linear2b[2];
__ro_hinv mat_t mat_linear2w = {.dims = {2, 64},
                                .len_dims = 2,
                                .strides = {64, 1},
                                .data = (fixed *)linear2w,
                                .p2quant = 15};
__ro_hinv mat_t mat_linear2b = {.dims = {2},
                                .len_dims = 1,
                                .strides = {1},
                                .data = (fixed *)linear2b,
                                .p2quant = 10};
__ro_hinv mat_t mat_linear2in = {.dims = {1, 64},
                                 .len_dims = 2,
                                 .strides = {64, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};
__ro_hinv mat_t mat_linear2out = {.dims = {1, 2},
                                  .len_dims = 2,
                                  .strides = {2, 1},
                                  .data = (fixed *)NULL,
                                  .p2quant = 10};

#endif