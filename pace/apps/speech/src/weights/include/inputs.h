#ifndef INPUTS_H
#define INPUTS_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed in0[1][1][14][51];
__ro_hinv mat_t mat_in0 = {.dims = {1, 1, 14, 51},
                           .len_dims = 4,
                           .strides = {714, 714, 51, 1},
                           .data = (fixed *)in0,
                           .p2quant = 6};

#endif