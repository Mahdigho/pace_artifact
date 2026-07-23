#include "_checkpoint_cpu.h"
#include "include/runtime.h"
#include <libmspio/uartio.h>
#include <libmspprintf/mspprintf.h>

// General purpose information
__nv uint16_t chkpt_complete = 0;
__nv uint16_t heavy_restore_count = 0;

// CPU globals
__nv uint32_t *prog_counter = (uint32_t *)CKPT_PC;
__nv const uint16_t *stack_start = (uint16_t *)(&__stack); // OR just 0x2C00
__nv const uint16_t *stack_copy_top = (uint16_t *)CKPT_STK;
__nv uint16_t **stack_top_addr = (uint16_t **)CKPT_SP;
__nv uint16_t **stack_copy_start_addr = (uint16_t **)CKPT_STCK_START;

void _print_stack(uint16_t *start, uint16_t *end) {
  while (start >= end) {
    msp_printf("0x%x ---> 0x%x\n\r", (uint16_t)end, *end);
    end++;
  }
}

/* Saves the stack, from stack_start until but excluding stack_top.
 *
 * Memory overview, given that stack grows down
 *
 * 0st LEAstack --> 0x3BFE
 * ....
 * LEAstack end --> 0x3AC8
 * ....
 * LEARAM       --> 0x2C00
 * 0th STACK    --> 0x2BFE
 * 1st stack    --> 0x2BFB
 *  .....
 *
 * With the DMA, assuming everything should get copied
 * we can copy from R1 until 3BFD (inclusive of both).
 */
void _stack_save_cpu() {
  uint16_t *stack_top = (*stack_top_addr);
#if CKPT_DEBUG_SAVE_STK
  msp_printf("SAVE: Stack Real Range: %x-%x\n\r", (uint16_t)stack_start,
             (uint16_t)stack_top);
  msp_printf("SAVE: Stack Real\n\r");
  _print_stack(stack_start - 1, stack_top);
#endif
  if (((uint16_t)stack_start - (uint16_t)stack_top) > CKPT_STK_SIZE) {
    msp_printf("Stack overloaded");
    gpio_set(1, 1);
    while (1)
      ;
  }
  uint16_t *save_loc = stack_copy_top;
  // Only equals because the first address (0x2C00) is not part of the stack.
  while ((unsigned int)stack_start > (unsigned int)stack_top) {
    *save_loc = *stack_top;
    save_loc++;
    stack_top++;
  }
  *stack_copy_start_addr = (save_loc - 1);

#if CKPT_DEBUG_SAVE_STK
  msp_printf("SAVE: Stack Copy\n\r");
  _print_stack(*stack_copy_start_addr, stack_copy_top);
#endif
}

void _stack_restore_cpu() {
  uint16_t *stack_top = (*stack_top_addr);
#if CKPT_DEBUG_REST_STK
  msp_printf("RESTORE: Stack Real Range: %x-%x\n\r", (uint16_t)stack_start,
             (uint16_t)stack_top);
  msp_printf("RESTORE: Stack Copy\n\r");
  _print_stack(*stack_copy_start_addr, stack_copy_top);
  msp_printf("Pre-RESTORE: Stack Real\n\r");
  _print_stack(stack_start - 1, *(stack_top_addr));
#endif

  uint16_t *save_loc = stack_copy_top;
  while ((unsigned int)stack_start > (unsigned int)stack_top) {
#if CKPT_DEBUG_REST_STK
    msp_printf("Copying value 0x%x\t", *save_loc);
#endif
    *stack_top = *save_loc;
#if CKPT_DEBUG_REST_STK
    msp_printf("Stored value 0x%x\t", *stack_top);
    msp_printf("addr is 0x%x\n\r", stack_top);
#endif
    save_loc++;
    stack_top++;
  }
#if CKPT_DEBUG_REST_STK
  msp_printf("Post-RESTORE: Stack Real\n\r");
  _print_stack(stack_start - 1, *(stack_top_addr));
#endif
}

void _ckpt_make_cpu() {
  // Save the status register
  __asm__ volatile("MOVX.A R2, &" ASMS(CKPT_R2));
  // Save the stack
  __asm__ volatile("MOVX.A R1, &" ASMS(CKPT_SP));
  // R3 is not saved

  // Save the Gen purpose
  __asm__ volatile("MOVX.A R4, &" ASMS(CKPT_R4));
  __asm__ volatile("MOVX.A R5, &" ASMS(CKPT_R5));
  __asm__ volatile("MOVX.A R6, &" ASMS(CKPT_R6));
  __asm__ volatile("MOVX.A R7, &" ASMS(CKPT_R7));
  __asm__ volatile("MOVX.A R8, &" ASMS(CKPT_R8));
  __asm__ volatile("MOVX.A R9, &" ASMS(CKPT_R9));
  __asm__ volatile("MOVX.A R10, &" ASMS(CKPT_R10));
  __asm__ volatile("MOVX.A R11, &" ASMS(CKPT_R11));
  __asm__ volatile("MOVX.A R12, &" ASMS(CKPT_R12));
  __asm__ volatile("MOVX.A R13, &" ASMS(CKPT_R13));
  __asm__ volatile("MOVX.A R14, &" ASMS(CKPT_R14));
  __asm__ volatile("MOVX.A R15, &" ASMS(CKPT_R15));

  __asm__ volatile("MOVX.A 0(R1), R4");           // R4 = RET PC
  __asm__ volatile("MOVX.A R4, &" ASMS(CKPT_PC)); // CKPT_PC = RET PC

  // Save the stack
  _stack_save_cpu();

  // restore registers
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R4) ", R4");
  chkpt_complete = 1;
}

void ckpt_restore_cpu() {
  if (!chkpt_complete) {
#if CKPT_DEBUG_REST_STK
    uartio_open(0);
    __delay_cycles(10000);
    msp_printf("Error! Broken checkpoint\r\n");
#endif
    return; // Error
  }
  chkpt_complete = 0;
  // In case the restore stack uses the stack, prevent corruption.
  __asm__ volatile("ADDA #20, R1");
  _stack_restore_cpu();

  __asm__ volatile("MOVX.A &" ASMS(CKPT_R4) ", R4");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R5) ", R5");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R6) ", R6");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R7) ", R7");

  __asm__ volatile("MOVX.A &" ASMS(CKPT_R8) ", R8");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R9) ", R9");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R10) ", R10");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R11) ", R11");

  __asm__ volatile("MOVX.A &" ASMS(CKPT_R12) ", R12");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R13) ", R13");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R14) ", R14");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R15) ", R15");

  __asm__ volatile("MOVX.A &" ASMS(CKPT_SP) ", R1");
  __asm__ volatile("MOVX.A &" ASMS(CKPT_R2) ", R2");
  // Have to manually return here as the compiler will
  // pop some values from the stack and mess up everything.
  __asm__ volatile("reta");
}

void _ckpt_status_cpu() {
#if CKPT_DEBUG_SAVE_STK
  msp_printf("STATUS: Stack Copy\n\r");
  _print_stack(*stack_copy_start_addr, stack_copy_top);
  msp_printf("STATUS: Reg   Copy\n\r");
  msp_printf("PC     ---> 0x%x\n\r", *((uint16_t *)CKPT_PC));
  msp_printf("SP     ---> 0x%x\n\r", *((uint16_t *)CKPT_SP));
  msp_printf("R2     ---> 0x%x\n\r", *((uint16_t *)CKPT_R2));
  msp_printf("R4     ---> 0x%x\n\r", *((uint16_t *)CKPT_R4));
  msp_printf("R5     ---> 0x%x\n\r", *((uint16_t *)CKPT_R5));
  msp_printf("R6     ---> 0x%x\n\r", *((uint16_t *)CKPT_R6));
  msp_printf("R7     ---> 0x%x\n\r", *((uint16_t *)CKPT_R7));
  msp_printf("R8     ---> 0x%x\n\r", *((uint16_t *)CKPT_R8));
  msp_printf("R9     ---> 0x%x\n\r", *((uint16_t *)CKPT_R9));
  msp_printf("R10    ---> 0x%x\n\r", *((uint16_t *)CKPT_R10));
  msp_printf("R11    ---> 0x%x\n\r", *((uint16_t *)CKPT_R11));
  msp_printf("R12    ---> 0x%x\n\r", *((uint16_t *)CKPT_R12));
  msp_printf("R13    ---> 0x%x\n\r", *((uint16_t *)CKPT_R13));
  msp_printf("R14    ---> 0x%x\n\r", *((uint16_t *)CKPT_R14));
  msp_printf("R15    ---> 0x%x\n\r", *((uint16_t *)CKPT_R15));
#endif
}