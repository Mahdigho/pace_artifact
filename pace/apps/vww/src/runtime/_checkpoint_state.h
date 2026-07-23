#ifndef __CKPT_INTERM_CKPT_STATE
#define __CKPT_INTERM_CKPT_STATE
#include "_checkpoint.h"
#include "_checkpoint_cpu.h"
#include "_checkpoint_lea.h"
#include <libmsp/mspbase.h>

extern __nv uint16_t ckpt_status;

#define CKPT_STAT_START BIT0
#define CKPT_STAT_READY BIT1
#define CKPT_STAT_DONE BIT2
#define CKPT_STAT_STOP BIT3

#define CKPT_STAT_CCPU BIT4
#define CKPT_STAT_CLEA BIT5
#define CKPT_STAT_CDMA BIT6
#define CKPT_STAT_RALL (CKPT_STAT_RDMA | CKPT_STAT_RLEA | CKPT_STAT_RCPU)

/* State machine operation
 *  NOTE: the state machine will be bugged if wait spinning for code. Do not
 *  wait spin. Wait spinning can be supported but its just a headache right now.
 *  It needs special handling of IFGs and sleep states by reading the stack,
 *  which is its own headache.
 *
 * STATUS bits
 * CKPT_STAT_START: BIT0
 * CKPT_STAT_READY: BIT1
 * CKPT_STAT_DONE: BIT2
 * CKPT_STAT_STOP: BIT3
 *
 * CKPT_STAT_CCPU: BIT4
 * CKPT_STAT_CLEA: BIT5
 * CKPT_STAT_CDMA: BIT6
 *
 * CKPT_STAT_RCPU: BIT8
 * CKPT_STAT_RLEA: BIT9
 * CKPT_STAT_RDMA: BIT10
 * CKPT_STAT_RALL= CKPT_STAT_RDMA | CKPT_STAT_RLEA | CKPT_STAT_RCPU
 *
 * USER DEFINED (CPU is always done, but for now its check exists):
 * CKPT_STAT_RDO = CKPT_STAT_RDMA | CKPT_STAT_RLEA | CKPT_STAT_RCPU
 *
 *
 * Heavy Restore Operation: For direct restore from burn out.
 * By entry into this function, the CPU should have already been restored.
 * 1. if CKPT_STAT_RDMA: Restore DMA stats
 * 2. if CKPT_STAT_RLEA: Restore LEA stats
 *
 * Light restore:  For restore from high energy
 * 1. if CKPT_STAT_RDMA: set flag if need be
 * 2. if CKPT_STAT_RLEA: set flag if need be
 *
 * check_peripheral:
 * 1. If C flag for something is set but not the RFlag, check if it's done.
 *    If so, set it's R flag. If not, return.
 *
 * shutdown operation: --> Not Done RN
 * 1. Something, anything. some function at least.
 *
 *
 * ISR Operation.
 * while (true):
 * 1. if STATUS == 0:
 *    STATUS = CKPT_STAT_START | CKPT_STAT_CCPU | CKPT_STAT_RCPU
 *    Check if LEA or DMA should be also checkpointed.
 *    Next iteration should at state 2.
 *
 * 2. elif (!(status & CKPT_STAT_READY)):
 *    Wait for peripherals to finish.
 *    NOTE: Even if a STOP directive has been issued, waiting is ok.
 *    We explicitly handle code that is wait sleeping, which is no different
 *    than what is happening here.
 *
 *    check_peripheral();
 *    if ((STATUS & CKPT_STAT_RALL) == CKPT_STAT_RDO) :
 *      STATUS |= CKPT_STAT_READY;
 *      next iteration should start at state 4 or 5, pending the stop directive.
 *    else:
 *      LMP0 + GIE upon exit.
 *      next iteration should start at state 2.
 *      break;
 *
 * 3. elif (!(STATUS & CKPT_STAT_READY) && (status & CKPT_STAT_STOP)):
 *      code not ready yet BUT stop directive is given. This case should never
 *      happen.
 *
 * 4. elif ((STATUS & CKPT_STAT_READY) && (status & CKPT_STAT_STOP)):
 *    light_restore();
 *    STATUS = 0;
 *    set GIE but unset all LPM3.
 *    break;
 *
 *    Based on the code that was interrupted and our current state, several
 *    states are possible.
 *      1. Interrupted code was not sleeping,
 *      --> 1.1 Checkpoint done (the start, ready and done should have all
 *              happened in one iteration). Waiting in LPM3.
 *          --> Restore by Case 1
 *      2. Interrupted code was sleeping and waiting for an interrupt
 *      --> 2.2 Checkpoint not ready, interrupt has not occured. Impossible
 *              given the state machine.
 *          2.2 Checkpoint not raedy, interrupt has occured. Impossible given
 *              the state machine.
 *          2.3 Checkpoint ready, interrupt has occured. No LPM should be on.
 *          --> Restore by Case 2
 *          2.4 Checkpoint done, interrupt has occured. Waiting in LPM 3.
 *          --> Restore by Case 1.
 *
 *    The restore is one of two cases,
 *      1. Waiting in LPM3. Wake up by restoring from LPM3.
 *      2. No LPM should be on. No special handling of wake up.
 *
 *    In both cases, we can wake up by clearing the LPM3 bits.
 *
 * 5. elif (STATUS & CKPT_SYSTEM_READY):
 *    checkpoint system.
 *    if (CKPT_STAT_DONE & STATUS) {
        // Restored from burnout.
 *      heavy_restore.
 *      STATUS = 0;
 *      set GIE but unset all LPM3;
 *
 *      1. If the interrupted code was running, it should not start in sleep
 *         state.
 *      2. If interrupted code was sleeping on LEA or DMA, we waited on those
 *         and handled them. Code should resume and not be in sleep state
 *         anymore.
 *      3. Undefined behavior if waiting for another type of interrupt.
 *
 *    } else {
 *      // deep sleep waiting for rest.
 *      STATUS |= CKPT_STAT_DONE Set LMP3 + GIE on exit.
 *    }
 *    break.
*/

#endif // __CKPT_INTERM_CKPT_STATE