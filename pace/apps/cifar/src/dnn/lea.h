#ifndef __LIBDNN_LEA_H
#define __LIBDNN_LEA_H
#include "libmatAbstract/mat.h"
#include "libmsp/macro_basics.h"
#include <libdsp/DSPLib.h>
#include <libmsp/dma.h>
#include <libmsp/nv.h>

#define MAKE_ALIGN_2(s) ((s) + ((s) & 0x1))

/* Lea RAM size defined such that it takes most of the LEARAM
 * + some extra spaces for extra params.
 *
 * LEARAM has 0xEC8 bytes.
 * 0xE00 is used general buffer space and the rest for other lea variables.
 */
#define LEA_RAM_SIZE (0xE00 >> 1)
extern _q15 lea_ram[LEA_RAM_SIZE];

extern _iq31 mac_res;
extern MSP_LEA_MAC_PARAMS lea_mac_params;

extern MSP_LEA_MPYMATRIX_PARAMS lea_mpy_params;
extern MSP_LEA_MPYMATRIX_PARAMS lea_const_mpy_params;
extern _q15 lea_const_mpy_vector[2];

extern MSP_LEA_MPYMATRIX_PARAMS lea_const_rshift_params;
extern _q15 lea_const_rshift_vector[2];

extern MSP_LEA_ADDMATRIX_PARAMS lea_add_params;
extern MSP_LEA_ADDMATRIX_PARAMS lea_add_self_params;
extern MSP_LEA_ADDMATRIX_PARAMS lea_const_add_params;
extern _q15 lea_const_add_vector[2];

extern MSP_LEA_DEINTERLEAVE_PARAMS lea_deinter_params;

extern MSP_LEA_FIR_PARAMS lea_fir_params;

extern MSP_LEA_ADDMATRIX_PARAMS lea_fill_params;
extern _q15 lea_fill_vector[2];

extern MSP_LEA_MPYMATRIXROW_PARAMS lea_mpymatrixrow_params;

/* Performs a left shift by adding the vectors */
void _left_shift_and_bias(mat_t *dst, mat_t *bias, int16_t shift_count);
void _right_shift_and_bias(mat_t *dst, mat_t *bias, int16_t shift_count);

/* figures out x and y, where sum = mult * count + y. */
void _count(uint16_t sum, uint16_t mult, uint16_t *count, uint16_t *rem);

void print_range(fixed *src, uint16_t start, uint16_t end, uint16_t cut);

#define FRAM_MOVE(SRC, DST, COUNT)                                             \
  {                                                                            \
    __bic_SR_register(GIE);                                                    \
    DMA_WW(0, (uintptr_t)SRC, (uintptr_t)DST, COUNT);                          \
    __bis_SR_register(GIE + LPM0_bits);                                        \
  }

#define LOOP_MOVE(SRC, DST, COUNT)                                             \
  {                                                                            \
    for (size_t i = 0; i < COUNT; i++) {                                       \
      DST[i] = SRC[i];                                                         \
    }                                                                          \
  }

#define FRAM_SET(SRC, DST, COUNT)                                              \
  {                                                                            \
    __bic_SR_register(GIE);                                                    \
    DMA_SETW(0, (uintptr_t)SRC, (uintptr_t)DST, COUNT);                        \
    __bis_SR_register(GIE + LPM0_bits);                                        \
  }

#define ADDR_WHILE(NAME, START, COUNT)                                         \
  uintptr_t STICH(NAME, _end) = (uintptr_t)(START + COUNT);                    \
  fixed *NAME = START;                                                         \
  while ((uintptr_t)NAME < STICH(NAME, _end))

#define ADDR_FOR(NAME, START, COUNT, STEP)                                     \
  uintptr_t STICH(NAME, _e) = (uintptr_t)(START + COUNT);                      \
  fixed *NAME = START;                                                         \
  for (; (uintptr_t)NAME < STICH(NAME, _e); NAME += STEP)

#endif // !__LIBDNN_LEA_H