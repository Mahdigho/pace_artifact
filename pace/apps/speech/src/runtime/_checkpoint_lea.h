#ifndef __CKPT_INTERM_LEA_CKPT
#define __CKPT_INTERM_LEA_CKPT
#include "_checkpoint.h"
/*
 * Not all LEA reigsters are needed.
 * The checkpoint operates by letting a command finish before checkpointing the
 * command. Hence, only variables that maintain state between lea invocations
 * are saved.
 *
 * Register link: https://www.ti.com/lit/ug/slau853/slau853.pdf
 *
 * NOTE: The LEA technically support suspending and resuming.
 * If operations are taking too long; suspend; copy; restore; resume would be a
 * good alternative.
 *
 * Here is what I could figure out.
 *
 * Save Registers:
 * LEACNF2:     Used as stack pointer.
 * LEAPMDST:    Used in a very spcial case (for auto scaled fft).
 * LEAPMS0:     Input register 0, need to save in case sleep is caused after set
 *                    but before invocation
 * LEAPMS1:     Input register 1, need to save in case sleep is cuased afte rset
 *                    but before invocation
 *
 * Variables used by the dsplib to be saved:
 * msp_lea_ifg: a variable used by the LEA library.
 * msp_lea_locked: a variable used by the LEA library.
 *
 *
 * Set at init (and unchanged so we don't save):
 * LEACNF0:     A variety of flags.
 * LEACNF1:     Mostly information about status of the LEA.
 * LEAPMCTL:    Enables the LEA (triggered by writing LEAPMCB)
 * LEAIE:       Enables interrupts.
 * LEACMCTL:    Manages the memory access? set to 0, so the LEARAM remains on in
 *                LPM3&4.
 *
 * Set at each invocation, do not save:
 * LEAPMCB:     Invokes the command. Do not restore this bit at all (it will
 *                cause another invocation).
 *
 * Do not Save anyways:
 * The rest.
 */
#define LEA_BUSY (LEACNF1 & LEABUSY)
#define CKPT_LEASPTR 0xEB00 // Stack Pointer
#define CKPT_LEAPDST 0xEB04 // Potential results register
#define CKPT_LEAPMS0 0xEB08 // Input Reg 0
#define CKPT_LEAPMS1 0xEB0C // Input Reg 1
#define CKPT_LEALOCK 0xEB10 // Software defined lea lock
#define CKPT_LEASIFG 0xEB12 // Software defined ifg register

extern __nv uint32_t *lea_sptr_copy;
extern __nv uint32_t *lea_pdst_copy;
extern __nv uint32_t *lea_pms0_copy;
extern __nv uint32_t *lea_pms1_copy;
extern __nv uint16_t *lea_sifg_copy;
extern __nv uint16_t *lea_lock_copy;

void _ckpt_make_lea();
void _ckpt_restore_lea();

#endif // __CKPT_INTERM_LEA_CKPT