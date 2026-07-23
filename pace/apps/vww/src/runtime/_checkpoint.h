#ifndef __CKPT_INTERM_CKPT
#define __CKPT_INTERM_CKPT
#include "include/defs.h"
#include <libmsp/mspbase.h>
#include <libmspprintf/mspprintf.h>

/*
 * The checkpoint module operates by saving a snapshot of the NON volatile
 *   state of the program and restoring said non-volatile state.
 *   As such, it is assumed that between checkpoint and restoration, the
 *   application does not actually run anything else since doing so would lead
 *   to unpredictable behaviour.
 *
 * Our goal is to support checkpointing the following,
 *  1. CPU and STACK state.
 *  2. DMA0 block transfer ops (we let the operation complete before saving the
 *     state).
 *  3. LEA and LEARAM (we let the operation complete before saving the state).
 *  4. TODO: MPY module.
 *
 *  DMA1 is used by this module. Do not use.
 *
 * NOTE:
 * 1. Checkpoint resumes immidietly after the checkpoint function. Write code
 *    to handle variable behavior if checkpointed code and resumed code should
 *    behave differently.
 * 2. It is assumed that the checkpoint make logic is called in an interrupt.
 *    The restore should be called at system start. The code should ideally work
 *    as normal, but just in case some assumptions were made.
 */

/*
 * We altered the MSP430FR5994 linker to include a CKPT section in the memory,
 *   reducing the total space available to low FRAM. This segment is in total
 *   0x14C0 bytes long and is divided into the following sections.
 *     1. 0xEAC0 -- 0xEB7F (length of 192 bytes) for all manners of registers
 *     2. 0xEB80 -- 0xEF7F (length of 1024 bytes) for checkpointCPU stack
 *     3. 0xEF80 -- 0xFF80 (length of 4096 bytes) for LEA RAM
 *
 */

#define CKPT_START 0xEAC0 // inclusive
#define CKPT_END 0xFF80   // exclusive

/*
 * For registers,
 * EAC0 - EAFC are used by CPU, 64 bytes
 * EB00 - EB10 are used by LEA, 32 bytes
 *
 */
#define CKPT_REG ((uint16_t *)0xEAC0)
#define CKPT_REG_SIZE 0x00C0
#define CKPT_STK ((uint16_t *)0xEB80)
#define CKPT_STK_SIZE 0x0400
#define CKPT_LEA ((uint16_t *)0xEF80)
#define CKPT_LEA_SIZE 0x1000

/* MAIN API */
void ckpt_restore_cpu() __attribute__((section(".lower.text.restore_chkpt")));
void ckpt_stop();
void ckpt_init();
extern __nv uint16_t ckpt_status;

#define CKPT_STATE_DEBUG 0

#if CKPT_STATE_DEBUG
#define CKPT_PRINTF(msg)                                                       \
  {                                                                            \
    uint16_t old = P8IE;                                                       \
    uint16_t oldc = (CEINT & (CEIIE | CEIE));                                  \
    P8IE = 0;                                                                  \
    CEINT &= ~(CEIIE | CEIE);                                                  \
    msp_printf(msg);                                                           \
    P8IE = old;                                                                \
    CEINT |= oldc;                                                             \
  }
#else
#define CKPT_PRINTF(msg)                                                       \
  {}
#endif // CKPT_STATE_DEBUG

#define ckpt_make()                                                            \
  {                                                                            \
    P8IE = BIT0;                                                               \
    P8IFG = BIT0;                                                              \
  }

// Check & awaken (Only to be used by LEA and DMA, which both would use LPM0).
#define ckpt_intr_check()                                                      \
  {                                                                            \
    if (ckpt_status & CKPT_STAT_START) {                                       \
      P8IE = BIT0;                                                             \
      P8IFG = BIT0;                                                            \
      _bis_SR_register_on_exit(GIE);                                           \
    }                                                                          \
  }

#endif // __CKPT_INTERM_CKPT