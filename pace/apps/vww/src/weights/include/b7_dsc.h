#ifndef B7_DSC_H
#define B7_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b7_dscw[16][1][3][3];
extern __ro_hinv fixed b7_dscs_4[4];
extern __ro_hinv fixed b7_dscb_4[4];
extern __ro_hinv fixed b7_dscs_6[6];
extern __ro_hinv fixed b7_dscb_6[6];
extern __ro_hinv fixed b7_dscs_8[8];
extern __ro_hinv fixed b7_dscb_8[8];
extern __ro_hinv fixed b7_dscs_10[10];
extern __ro_hinv fixed b7_dscb_10[10];
extern __ro_hinv fixed b7_dscs_12[12];
extern __ro_hinv fixed b7_dscb_12[12];
extern __ro_hinv fixed b7_dscs_14[14];
extern __ro_hinv fixed b7_dscb_14[14];
extern __ro_hinv fixed b7_dscs_16[16];
extern __ro_hinv fixed b7_dscb_16[16];
__ro_hinv mat_t mat_b7_dscw = {.dims = {16, 1, 3, 3},
                               .len_dims = 4,
                               .strides = {9, 9, 3, 1},
                               .data = (fixed *)b7_dscw,
                               .p2quant = 14};
__ro_hinv mat_t mat_b7_dscs_16 = {.dims = {16},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b7_dscs_16,
                                  .p2quant = 15};
__ro_hinv mat_t mat_b7_dscb_16 = {.dims = {16},
                                  .len_dims = 1,
                                  .strides = {1},
                                  .data = (fixed *)b7_dscb_16,
                                  .p2quant = 10};
__ro_hinv mat_t mat_b7_dscin = {.dims = {1, 16, 24, 24},
                                .len_dims = 4,
                                .strides = {9216, 576, 24, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};
__ro_hinv mat_t mat_b7_dscout = {.dims = {1, 16, 12, 12},
                                 .len_dims = 4,
                                 .strides = {2304, 144, 12, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};

#endif