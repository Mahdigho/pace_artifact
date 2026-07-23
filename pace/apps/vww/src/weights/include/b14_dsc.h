#ifndef B14_DSC_H
#define B14_DSC_H
#include <libfixedAbstract/fixed.h>
#include <libmatAbstract/mat.h>
#include <libmsp/nv.h>

extern __ro_hinv fixed b14_dscw[64][1][3][3];
extern __ro_hinv fixed b14_dscs_16[16];
extern __ro_hinv fixed b14_dscb_16[16];
extern __ro_hinv fixed b14_dscs_24[24];
extern __ro_hinv fixed b14_dscb_24[24];
extern __ro_hinv fixed b14_dscs_32[32];
extern __ro_hinv fixed b14_dscb_32[32];
extern __ro_hinv fixed b14_dscs_40[40];
extern __ro_hinv fixed b14_dscb_40[40];
extern __ro_hinv fixed b14_dscs_48[48];
extern __ro_hinv fixed b14_dscb_48[48];
extern __ro_hinv fixed b14_dscs_56[56];
extern __ro_hinv fixed b14_dscb_56[56];
extern __ro_hinv fixed b14_dscs_64[64];
extern __ro_hinv fixed b14_dscb_64[64];
__ro_hinv mat_t mat_b14_dscw = {.dims = {64, 1, 3, 3},
                                .len_dims = 4,
                                .strides = {9, 9, 3, 1},
                                .data = (fixed *)b14_dscw,
                                .p2quant = 13};
__ro_hinv mat_t mat_b14_dscs_64 = {.dims = {64},
                                   .len_dims = 1,
                                   .strides = {1},
                                   .data = (fixed *)b14_dscs_64,
                                   .p2quant = 15};
__ro_hinv mat_t mat_b14_dscb_64 = {.dims = {64},
                                   .len_dims = 1,
                                   .strides = {1},
                                   .data = (fixed *)b14_dscb_64,
                                   .p2quant = 10};
__ro_hinv mat_t mat_b14_dscin = {.dims = {1, 64, 6, 6},
                                 .len_dims = 4,
                                 .strides = {2304, 36, 6, 1},
                                 .data = (fixed *)NULL,
                                 .p2quant = 10};
__ro_hinv mat_t mat_b14_dscout = {.dims = {1, 64, 6, 6},
                                  .len_dims = 4,
                                  .strides = {2304, 36, 6, 1},
                                  .data = (fixed *)NULL,
                                  .p2quant = 10};

#endif