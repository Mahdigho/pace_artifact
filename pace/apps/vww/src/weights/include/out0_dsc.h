#ifndef OUT0_DSC_H
#define OUT0_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed out0_dscw[8][1][3][3];
extern __ro_hinv fixed out0_dscs_2[2];
extern __ro_hinv fixed out0_dscb_2[2];
extern __ro_hinv fixed out0_dscs_3[3];
extern __ro_hinv fixed out0_dscb_3[3];
extern __ro_hinv fixed out0_dscs_4[4];
extern __ro_hinv fixed out0_dscb_4[4];
extern __ro_hinv fixed out0_dscs_5[5];
extern __ro_hinv fixed out0_dscb_5[5];
extern __ro_hinv fixed out0_dscs_6[6];
extern __ro_hinv fixed out0_dscb_6[6];
extern __ro_hinv fixed out0_dscs_7[7];
extern __ro_hinv fixed out0_dscb_7[7];
extern __ro_hinv fixed out0_dscs_8[8];
extern __ro_hinv fixed out0_dscb_8[8];
__ro_hinv mat_t mat_out0_dscw = {.dims = {8, 1, 3, 3},
                                 .len_dims = 4,
                                 .strides = {9, 9, 3, 1},
                                 .data = (fixed *)out0_dscw,
                                 .p2quant = 13};
__ro_hinv mat_t mat_out0_dscs_8 = {.dims = {8},
                                   .len_dims = 1,
                                   .strides = {1},
                                   .data = (fixed *)out0_dscs_8,
                                   .p2quant = 15};
__ro_hinv mat_t mat_out0_dscb_8 = {.dims = {8},
                                   .len_dims = 1,
                                   .strides = {1},
                                   .data = (fixed *)out0_dscb_8,
                                   .p2quant = 10};
__ro_hinv mat_t mat_out0_dscin = {.dims = {1, 8, 24, 24},
                                  .len_dims = 4,
                                  .strides = {4608, 576, 24, 1},
                                  .data = (fixed *)NULL,
                                  .p2quant = 10};
__ro_hinv mat_t mat_out0_dscout = {.dims = {1, 8, 24, 24},
                                   .len_dims = 4,
                                   .strides = {4608, 576, 24, 1},
                                   .data = (fixed *)NULL,
                                   .p2quant = 10};

#endif