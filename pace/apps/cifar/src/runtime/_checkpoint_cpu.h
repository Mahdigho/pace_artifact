#ifndef __CKPT_INTERM_CPU_CKPT
#define __CKPT_INTERM_CPU_CKPT
#include "_checkpoint.h"
#include <libmsp/mspbase.h>

#define CKPT_DEBUG_SAVE_STK 0
#define CKPT_DEBUG_REST_STK 0

// For stringification inside the __asm__ instructions
#define __ASMS(X) #X
#define ASMS(X) __ASMS(X)

// Unfortunately the preprocessor cannot do math before stringification
#define CKPT_PC 0xEAC0         // Also R0 --> used by not actually restored
#define CKPT_R0 0xEAC0         // ALSO PC
#define CKPT_SP 0xEAC4         // Also R1
#define CKPT_R1 0xEAC4         // Also SP
#define CKPT_R2 0xEAC8         // Status Reg
#define CKPT_R3 0              // R3 is a constant generator, don't save
#define CKPT_STCK_START 0xEACC // Repurpose this space
#define CKPT_R4 0xEAD0         // Gen Purpose
#define CKPT_R5 0xEAD4         // Gen Purpose
#define CKPT_R6 0xEAD8         // Gen Purpose
#define CKPT_R7 0xEADC         // Gen Purpose
#define CKPT_R8 0xEAE0         // Gen Purpose
#define CKPT_R9 0xEAE4         // Gen Purpose
#define CKPT_R10 0xEAE8        // Gen Purpose
#define CKPT_R11 0xEAEC        // Gen Purpose
#define CKPT_R12 0xEAF0        // Gen Purpose
#define CKPT_R13 0xEAF4        // Gen Purpose
#define CKPT_R14 0xEAF8        // Gen Purpose
#define CKPT_R15 0xEAFC        // Gen Purpose

extern int __stack;
extern __nv uint32_t *prog_counter;
extern __nv const uint16_t *stack_start;
extern __nv const uint16_t *stack_copy_top;
extern __nv uint16_t **stack_top_addr;
extern __nv uint16_t **stack_copy_start_addr;

void _ckpt_make_cpu() __attribute__((section(".lower.text.checkpoint")));
void _ckpt_status_cpu();
void _stack_save_cpu();
void _stack_restore_cpu();
#define print_stack_value()                                                    \
  {                                                                            \
    __asm__ volatile("MOVX.A R1, &" ASMS(CKPT_SP));                            \
    msp_printf("Stack value is %x", (*stack_top_addr));                        \
  }

#endif // __CKPT_INTERM_CPU_CKPT