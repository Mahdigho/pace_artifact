#include "_checkpoint_lea.h"
#include <libdsp/DSPLib.h>
#include <libmsp/dma.h>
#include <libmspprintf/mspprintf.h>

__nv uint32_t *lea_sptr_copy = (uint32_t *)CKPT_LEASPTR;
__nv uint32_t *lea_pdst_copy = (uint32_t *)CKPT_LEAPDST;
__nv uint32_t *lea_pms0_copy = (uint32_t *)CKPT_LEAPMS0;
__nv uint32_t *lea_pms1_copy = (uint32_t *)CKPT_LEAPMS1;

__nv uint16_t *lea_sifg_copy = (uint16_t *)CKPT_LEASIFG;
__nv uint16_t *lea_lock_copy = (uint16_t *)CKPT_LEASIFG;

void _ckpt_make_lea() {
  // Save the approrpiate registers
  *lea_sptr_copy = LEACNF2;
  *lea_pms0_copy = LEAPMS0;
  *lea_pms1_copy = LEAPMS1;
  *lea_pdst_copy = LEAPMDST;
  *lea_sifg_copy = msp_lea_ifg;
  *lea_lock_copy = msp_lea_locked;
  __disable_interrupt();
  // Copying words, so divide size in half
  DMA_WW(1, MSP_LEA_BOTTOM, CKPT_LEA, CKPT_LEA_SIZE >> 1);
  __bis_SR_register(GIE + LPM0_bits);
  __disable_interrupt();
}

void _ckpt_restore_lea() {
  LEACNF2 = *lea_sptr_copy;
  LEAPMS0 = *lea_pms0_copy;
  LEAPMS1 = *lea_pms1_copy;
  LEAPMDST = *lea_pdst_copy;
  msp_lea_ifg = *lea_sifg_copy;
  msp_lea_locked = *lea_lock_copy;
  // Copying words, so divide size in half
  __disable_interrupt();
  DMA_WW(1, CKPT_LEA, MSP_LEA_BOTTOM, CKPT_LEA_SIZE >> 1);
  __bis_SR_register(GIE + LPM0_bits);
  __disable_interrupt();
}