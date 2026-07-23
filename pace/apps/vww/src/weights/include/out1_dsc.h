#ifndef OUT1_DSC_H
#define OUT1_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed out1_dscw[16][1][3][3];
extern __ro_hinv fixed out1_dscs_4[4];
extern __ro_hinv fixed out1_dscb_4[4];
extern __ro_hinv fixed out1_dscs_6[6];
extern __ro_hinv fixed out1_dscb_6[6];
extern __ro_hinv fixed out1_dscs_8[8];
extern __ro_hinv fixed out1_dscb_8[8];
extern __ro_hinv fixed out1_dscs_10[10];
extern __ro_hinv fixed out1_dscb_10[10];
extern __ro_hinv fixed out1_dscs_12[12];
extern __ro_hinv fixed out1_dscb_12[12];
extern __ro_hinv fixed out1_dscs_14[14];
extern __ro_hinv fixed out1_dscb_14[14];
extern __ro_hinv fixed out1_dscs_16[16];
extern __ro_hinv fixed out1_dscb_16[16];
__ro_hinv mat_t mat_out1_dscw = {.dims = {16, 1, 3, 3},
                                 .len_dims = 4,
                                 .strides = {9, 9, 3, 1},
                                 .data = (fixed *)out1_dscw,
                                 .p2quant = 13};
__ro_hinv mat_t mat_out1_dscs_16 = {.dims = {16},
                                    .len_dims = 1,
                                    .strides = {1},
                                    .data = (fixed *)out1_dscs_16,
                                    .p2quant = 15};
__ro_hinv mat_t mat_out1_dscb_16 = {.dims = {16},
                                    .len_dims = 1,
                                    .strides = {1},
                                    .data = (fixed *)out1_dscb_16,
                                    .p2quant = 10};
__ro_hinv mat_t mat_out1_dscin = {.dims = {1, 16, 24, 24},
                                  .len_dims = 4,
                                  .strides = {9216, 576, 24, 1},
                                  .data = (fixed *)NULL,
                                  .p2quant = 10};
__ro_hinv mat_t mat_out1_dscout = {.dims = {1, 16, 24, 24},
                                   .len_dims = 4,
                                   .strides = {9216, 576, 24, 1},
                                   .data = (fixed *)NULL,
                                   .p2quant = 10};

#endif