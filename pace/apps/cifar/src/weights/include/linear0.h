#ifndef LINEAR0_H
#define LINEAR0_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed linear0w[10][40];
extern __ro_hinv fixed linear0b[10];
__ro_nv mat_t mat_linear0w = {.dims = {10, 40}, .len_dims = 2, .strides = {40, 1}, .data = (fixed *)linear0w, .p2quant = 14};
__ro_nv mat_t mat_linear0b = {.dims = {10}, .len_dims = 1, .strides = {1}, .data = (fixed *)linear0b, .p2quant = 10};
__ro_nv mat_t mat_linear0in = {.dims = {1, 40}, .len_dims = 2, .strides = {40, 1}, .data = (fixed *)NULL, .p2quant = 10};
__ro_nv mat_t mat_linear0out = {.dims = {1, 10}, .len_dims = 2, .strides = {10, 1}, .data = (fixed *)NULL, .p2quant = 10};


#endif