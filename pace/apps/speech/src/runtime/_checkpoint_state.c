#include "_checkpoint_state.h"
#include "_energy.h"
#include "_scheduler.h"
#include "include/runtime.h"
#include <libdsp/DSPLib.h>
// #include <libmsp/mspbase.h>

/* Port 8 interrupt vector is used to cause a checkpoint (& restore).
 * This method allows for creating checkpoints from different ISRs by enabling
 * this interrupt.
 *
 * Why Port 8? Limited conflict with other functions and lowest interrupt
 * priority besides LEA.
 */

extern __nv uint16_t chkpt_complete;
__nv uint16_t ckpt_status = 0;
__nv uint16_t light_restore_count = 0;
__nv uint16_t failed_to_light = 0;
__nv uint32_t pre_ckpt_ticks;
__nv uint32_t post_ckpt_ticks;
__nv uint16_t pre_ckpt_cap_read;
__nv uint16_t post_ckpt_cap_read;

void ckpt_init() { ckpt_low_energy(MIN_THRESH); }

void ckpt_stop() {
  if (ckpt_status & CKPT_STAT_START) {
    CKPT_PRINTF("Sending Stop Directive\n\r")
    ckpt_status |= CKPT_STAT_STOP;
    P8IE = BIT0;
    P8IFG = BIT0;
  }
}

static void _heavy_restore() {
  CKPT_PRINTF("Heavy Restore\n\r");
  if (ckpt_status & CKPT_STAT_RDMA) {
    // Nothing to do for DMA heavy restore
  }
  if (ckpt_status & CKPT_STAT_RLEA) {
    _ckpt_restore_lea();
  }
}

static void _light_restore() {
  CKPT_PRINTF("Light Restore\n\r");
  // cpu should not be restored from this point on
  chkpt_complete = 0;
  if (ckpt_status & CKPT_STAT_RDMA) {
    // Nothing to do for DMA light restore
  }
  if (ckpt_status & CKPT_STAT_RLEA) {
    // Nothing to do for LEA light restore
  }
}

static void _check_back_peripheral() {
  // Check if module is ready for checkpointing.
  // Module is ready for checkpointing if
  //   1. Module is not active
  //   2. Module has no pending interrupts
  // Check 1 first. If it is true, the flag should have already been set.
  // NOTE: I'm assuming either that the setting of interrupt does not happen
  // more than 1-2 cycles after the device is set free
  // If this is not the case, there is no option but to wait spin between
  // checking for interrupt.
  if ((ckpt_status & CKPT_STAT_CDMA) && !(ckpt_status & CKPT_STAT_RDMA)) {
    if (!(DMA0CTL & DMAEN) && !(DMA0CTL & DMAIFG)) {
      ckpt_status |= CKPT_STAT_RDMA;
      CKPT_PRINTF("DMA READY\n\r");
    }
  }
  if ((ckpt_status & CKPT_STAT_CLEA) && !(ckpt_status & CKPT_STAT_RLEA)) {
    if (!(LEACNF1 & LEABUSY) && (LEAIFG == 0)) {
      ckpt_status |= CKPT_STAT_RLEA;
      CKPT_PRINTF("LEA READY\n\r");
    }
  }
}

/* Detect low energy, exit properly */
static inline __attribute__((always_inline)) void resume_sequence() {
  _disable_interrupts();
  ckpt_low_energy(MIN_THRESH);
  _bis_SR_register_on_exit(GIE);
  _bic_SR_register_on_exit(LPM3_bits);
  if (CECTL1 & CEOUT) {
    // TODO: Do something special here?
    failed_to_light += 1;
  }
}

__attribute__((interrupt(PORT8_VECTOR))) void CKPTport8ISR(void) {
  // No more P8 interrupts upon exit from here.
  P8IE = 0;
  P8IFG = 0;

  while (true) {
    if (ckpt_status == 0) {
      // Start of CKPT.
      CKPT_PRINTF("Start of checkpoint\n\r");
      ckpt_status = CKPT_STAT_START | CKPT_STAT_CCPU | CKPT_STAT_RCPU;
      if (CKPT_STAT_RDO & CKPT_STAT_RDMA) {
        ckpt_status |= CKPT_STAT_CDMA;
      }
      if (CKPT_STAT_RDO & CKPT_STAT_RLEA) {
        ckpt_status |= CKPT_STAT_CLEA;
      }
    } else if (!(ckpt_status & CKPT_STAT_READY)) {
      // Waiting for peripherals.
      CKPT_PRINTF("Waiting for peripherals\n\r");
      _check_back_peripheral();
      if ((ckpt_status & CKPT_STAT_RALL) == CKPT_STAT_RDO) {
        CKPT_PRINTF("ALL READY\n\r");
        ckpt_status |= CKPT_STAT_READY;
      } else {
        __bis_SR_register_on_exit(LPM0_bits + GIE);
        return;
      }
    } else if ((ckpt_status & CKPT_STAT_READY) &&
               (ckpt_status & CKPT_STAT_STOP)) {
      // In the midst of waiting for LEA or DMA, energy might become high.
      // If so, light restore and enable GIE.
      CKPT_PRINTF("Stopping checkpoint\n\r");
      _light_restore();
      light_restore_count += 1;
      wakeup_sequence(WAKE_SOFT);
      resume_sequence();
      ckpt_status = 0;
      return;
    } else if (ckpt_status & CKPT_STAT_READY) {
      // Ready for checkpointing
      CKPT_PRINTF("Making checkpoints\n\r");
      pre_ckpt_ticks = get_ticks();
      pre_ckpt_cap_read = read_cap();
      _ckpt_make_lea();
      _ckpt_make_cpu();
      if (ckpt_status & CKPT_STAT_DONE) {
        // Restored from burnout.
        _heavy_restore();
        heavy_restore_count += 1;
        wakeup_sequence(WAKE_HARD);
        resume_sequence();
        ckpt_status = 0;
      } else {
        // Post checkpoint
        ckpt_status |= CKPT_STAT_DONE;
        _ckpt_status_cpu();

        CKPT_PRINTF("Shutdown, waiting for reset or high energy\n\r");
        __bis_SR_register_on_exit(LPM3_bits + GIE);
        _disable_interrupts();
        post_ckpt_ticks = get_ticks();
        post_ckpt_cap_read = read_cap();
        ckpt_high_energy(MAX_THRESH);
      }
      return;
    }
  }
}

/* Wake up CPU once DMA is complete */
void __attribute__((interrupt(DMA_VECTOR))) dma_isr_handler(void) {
  switch (__even_in_range(DMAIV, DMAIV_DMA2IFG)) {
  case DMAIV_DMA0IFG:
    // Channel 0 is used for LEA transfers
    ckpt_intr_check();
    break;
  case DMAIV_DMA1IFG:
    break;
  case DMAIV_DMA2IFG:
    break;
  default:
    break;
  }
  __bic_SR_register_on_exit(LPM0_bits);
}

void __attribute__((interrupt(LEA_VECTOR))) msp_lea_isr(void) {
  /* Save the interrupt flags, clear interrupt and exit LPM0. */
  uint16_t flags = LEAIFG;
  LEAIFG |= flags;
  msp_lea_ifg = flags;
  ckpt_intr_check();
  __bic_SR_register_on_exit(LPM0_bits);
}