#ifndef INPUTS_H
#define INPUTS_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed in0[1][3][34][34];
__ro_nv mat_t mat_in0 = {.dims = {1, 3, 34, 34}, .len_dims = 4, .strides = {3468, 1156, 34, 1}, .data = (fixed *)in0, .p2quant = 10};


#endif