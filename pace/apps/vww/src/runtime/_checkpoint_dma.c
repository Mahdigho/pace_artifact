#ifndef __CKPT_INTERM_DMA_CKPT
#define __CKPT_INTERM_DMA_CKPT
#include "_checkpoint.h"
/*
 * Need to save DMA registers in case death happened
 * whilst setting DMA registers but before it was turned on.
 *
 * Here we save a limited set of DMA registers.
 * We preserve only those used by the DMA move.
 */

#endif // __CKPT_INTERM_LEA_CKPT