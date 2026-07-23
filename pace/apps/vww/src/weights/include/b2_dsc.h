#ifndef B2_DSC_H
#define B2_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b2_dscw[4][1][3][3];
extern __ro_hinv fixed b2_dscs_1[1];
extern __ro_hinv fixed b2_dscb_1[1];
extern __ro_hinv fixed b2_dscs_2[2];
extern __ro_hinv fixed b2_dscb_2[2];
extern __ro_hinv fixed b2_dscs_3[3];
extern __ro_hinv fixed b2_dscb_3[3];
extern __ro_hinv fixed b2_dscs_4[4];
extern __ro_hinv fixed b2_dscb_4[4];
__ro_hinv mat_t mat_b2_dscw = {.dims = {4, 1, 3, 3},
                               .len_dims = 4,
                               .strides = {9, 9, 3, 1},
                               .data = (fixed *)b2_dscw,
                               .p2quant = 13};
__ro_hinv mat_t mat_b2_dscs_4 = {.dims = {4},
                                 .len_dims = 1,
                                 .strides = {1},
                                 .data = (fixed *)b2_dscs_4,
                                 .p2quant = 15};
__ro_hinv mat_t mat_b2_dscb_4 = {.dims = {4},
                                 .len_dims = 1,
                                 .strides = {1},
                                 .data = (fixed *)b2_dscb_4,
                                 .p2quant = 10};
__ro_hinv mat_t mat_b2_dscin = {.dims = {1, 4, 48, 48},
                                .len_dims = 4,
                                .strides = {9216, 2304, 48, 1},
                                .data = (fixed *)NULL,
                                .p2quant = 10};
__ro_hinv mat_t mat_b2_dscout = {.dims = {1, 4, 24, 24},
                                 .len_dims = 4,
                                 .strides = {2304, 576, 24, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};

#endif