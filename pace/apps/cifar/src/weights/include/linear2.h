#ifndef LINEAR2_H
#define LINEAR2_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed linear2w[10][40];
extern __ro_hinv fixed linear2b[10];
__ro_nv mat_t mat_linear2w = {.dims = {10, 40}, .len_dims = 2, .strides = {40, 1}, .data = (fixed *)linear2w, .p2quant = 12};
__ro_nv mat_t mat_linear2b = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)linear2b, .p2quant = 10};
__ro_nv mat_t mat_linear2in = {.dims = {1, 40}, .len_dims = 2, .strides = {40, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_linear2out = {.dims = {1, 10}, .len_dims = 2, .strides = {10, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif