#ifndef LINEAR1_H
#define LINEAR1_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed linear1w[10][80];
extern __ro_hinv fixed linear1b[10];
__ro_nv mat_t mat_linear1w = {.dims = {10, 80}, .len_dims = 2, .strides = {80, 1}, .data = (fixed *)linear1w, .p2quant = 13};
__ro_nv mat_t mat_linear1b = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)linear1b, .p2quant = 10};
__ro_nv mat_t mat_linear1in = {.dims = {1, 80}, .len_dims = 2, .strides = {80, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_linear1out = {.dims = {1, 10}, .len_dims = 2, .strides = {10, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif