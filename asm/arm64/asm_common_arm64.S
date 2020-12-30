#include "asm_defs_registers_arm64.h"

#include "../asm_defs_host.h"
#include "../asm_inturbo_defs.h"
#include "asm_calling_convention_arm64.h"

.file "asm/arm64/asm_common_arm64.S"
.section rodata
.text


.globl asm_instruction_CLD
.globl asm_instruction_CLD_END
asm_instruction_CLD:

  bic REG_6502_ID_F, REG_6502_ID_F, #0x08

asm_instruction_CLD_END:
  ret


.globl asm_instruction_CLI
.globl asm_instruction_CLI_END
asm_instruction_CLI:

  bic REG_6502_ID_F, REG_6502_ID_F, #0x04

asm_instruction_CLI_END:
  ret


.globl asm_instruction_DEX
.globl asm_instruction_DEX_END
asm_instruction_DEX:

  sub REG_6502_X, REG_6502_X, #1
  adds xzr, xzr, REG_6502_X, lsl #56
  and REG_6502_X, REG_6502_X, #0xFF

asm_instruction_DEX_END:
  ret


.globl asm_instruction_INX
.globl asm_instruction_INX_END
asm_instruction_INX:

  add REG_6502_X, REG_6502_X, #1
  adds xzr, xzr, REG_6502_X, lsl #56
  and REG_6502_X, REG_6502_X, #0xFF

asm_instruction_INX_END:
  ret


.globl asm_instruction_INY
.globl asm_instruction_INY_END
asm_instruction_INY:

  add REG_6502_Y, REG_6502_Y, #1
  adds xzr, xzr, REG_6502_Y, lsl #56
  and REG_6502_Y, REG_6502_Y, #0xFF

asm_instruction_INY_END:
  ret


.globl asm_instruction_PHA
.globl asm_instruction_PHA_END
asm_instruction_PHA:

  strb REG_6502_A_32, [REG_6502_S]
  sub REG_6502_S, REG_6502_S, #1
  bic REG_6502_S, REG_6502_S, #0x100

asm_instruction_PHA_END:
  ret


.globl asm_instruction_SED
.globl asm_instruction_SED_END
asm_instruction_SED:

  orr REG_6502_ID_F, REG_6502_ID_F, #0x08

asm_instruction_SED_END:
  ret


.globl asm_instruction_SEI
.globl asm_instruction_SEI_END
asm_instruction_SEI:

  orr REG_6502_ID_F, REG_6502_ID_F, #0x04

asm_instruction_SEI_END:
  ret


.globl asm_instruction_TAX
.globl asm_instruction_TAX_END
asm_instruction_TAX:

  mov REG_6502_X, REG_6502_A

asm_instruction_TAX_END:
  ret


.globl asm_instruction_TAY
.globl asm_instruction_TAY_END
asm_instruction_TAY:

  mov REG_6502_Y, REG_6502_A

asm_instruction_TAY_END:
  ret


.globl asm_instruction_TXS
.globl asm_instruction_TXS_END
asm_instruction_TXS:

  mov REG_6502_S, REG_6502_X
  orr REG_6502_S, REG_6502_S, REG_MEM_READ
  add REG_6502_S, REG_6502_S, 0x100

asm_instruction_TXS_END:
  ret


.globl asm_instruction_A_NZ_flags
.globl asm_instruction_A_NZ_flags_END
asm_instruction_A_NZ_flags:

  adds xzr, xzr, REG_6502_A, lsl #56

asm_instruction_A_NZ_flags_END:
  ret


.globl asm_instruction_X_NZ_flags
.globl asm_instruction_X_NZ_flags_END
asm_instruction_X_NZ_flags:

  adds xzr, xzr, REG_6502_X, lsl #56

asm_instruction_X_NZ_flags_END:
  ret


.globl asm_instruction_Y_NZ_flags
.globl asm_instruction_Y_NZ_flags_END
asm_instruction_Y_NZ_flags:

  adds xzr, xzr, REG_6502_Y, lsl #56

asm_instruction_Y_NZ_flags_END:
  ret


.globl asm_save_AXYS_PC_flags
.globl asm_save_AXYS_PC_flags_END
asm_save_AXYS_PC_flags:
  stp x29, x30, [sp, #-16]!
  ldr REG_SCRATCH2, [REG_CONTEXT, #K_CONTEXT_OFFSET_STATE_6502]
  str REG_6502_A_32, [REG_SCRATCH2, #K_STATE_6502_OFFSET_REG_A]
  str REG_6502_X_32, [REG_SCRATCH2, #K_STATE_6502_OFFSET_REG_X]
  str REG_6502_Y_32, [REG_SCRATCH2, #K_STATE_6502_OFFSET_REG_Y]
  str REG_6502_S_32, [REG_SCRATCH2, #K_STATE_6502_OFFSET_REG_S]
  sub REG_6502_PC, REG_6502_PC, REG_MEM_READ
  str REG_6502_PC_32, [REG_SCRATCH2, #K_STATE_6502_OFFSET_REG_PC]

  bl asm_emit_arm64_flags_to_scratch
  str REG_SCRATCH1_32, [REG_SCRATCH2, #K_STATE_6502_OFFSET_REG_FLAGS]

  ldp x29, x30, [sp], #16
  ret

asm_save_AXYS_PC_flags_END:
  ret


.globl asm_restore_AXYS_PC_flags
.globl asm_restore_AXYS_PC_flags_END
asm_restore_AXYS_PC_flags:
  stp x29, x30, [sp, #-16]!
  ldr REG_SCRATCH1, [REG_CONTEXT, #K_CONTEXT_OFFSET_STATE_6502]
  ldr REG_6502_A_32, [REG_SCRATCH1, #K_STATE_6502_OFFSET_REG_A]
  ldr REG_6502_X_32, [REG_SCRATCH1, #K_STATE_6502_OFFSET_REG_X]
  ldr REG_6502_Y_32, [REG_SCRATCH1, #K_STATE_6502_OFFSET_REG_Y]
  ldr REG_6502_S_32, [REG_SCRATCH1, #K_STATE_6502_OFFSET_REG_S]
  ldr REG_6502_PC_32, [REG_SCRATCH1, #K_STATE_6502_OFFSET_REG_PC]
  add REG_6502_PC, REG_MEM_READ, REG_6502_PC

  ldr REG_SCRATCH1_32, [REG_SCRATCH1, #K_STATE_6502_OFFSET_REG_FLAGS]
  bl asm_set_arm64_flags_from_scratch

  ldp x29, x30, [sp], #16
  ret

asm_restore_AXYS_PC_flags_END:
  ret


.globl asm_emit_arm64_flags_to_scratch
.globl asm_emit_arm64_flags_to_scratch_END
asm_emit_arm64_flags_to_scratch:
  mov REG_SCRATCH1, REG_6502_ID_F
  movz REG_SCRATCH3, #2
  csel REG_SCRATCH3, REG_SCRATCH3, xzr, eq
  orr REG_SCRATCH1, REG_SCRATCH1, REG_SCRATCH3
  ret

asm_emit_arm64_flags_to_scratch_END:
  ret


.globl asm_set_arm64_flags_from_scratch
.globl asm_set_arm64_flags_from_scratch_END
asm_set_arm64_flags_from_scratch:
  and REG_6502_ID_F, REG_SCRATCH1, 0x3C
  and REG_SCRATCH1, REG_SCRATCH1, #2
  cmp REG_SCRATCH1, #2
  ret

asm_set_arm64_flags_from_scratch_END:
  ret


.globl asm_enter
.globl asm_enter_END
asm_enter:
  stp x29, x30, [sp, #-16]!

  # param1, x0 == context
  # param2, x1 == x64 start address
  # param3, x2 == countdown
  # param4, x3 == mem base

  # Save callee save registers.
  stp x27, x28, [sp, #-16]!
  stp x25, x26, [sp, #-16]!
  stp x23, x24, [sp, #-16]!
  stp x21, x22, [sp, #-16]!

  mov REG_CONTEXT, REG_PARAM1
  mov REG_SCRATCH3, REG_PARAM2
  mov REG_COUNTDOWN, REG_PARAM3
  mov REG_MEM_READ, REG_PARAM4

  movz REG_SCRATCH1, #(K_BBC_MEM_OFFSET_READ_TO_WRITE >> 16), lsl #16
  movk REG_SCRATCH1, #(K_BBC_MEM_OFFSET_READ_TO_WRITE & 0xFFFF)
  add REG_MEM_WRITE, REG_MEM_READ, REG_SCRATCH1
  movz REG_INTURBO_CODE, #(K_INTURBO_OPCODES >> 16), lsl #16
  movk REG_INTURBO_CODE, #(K_INTURBO_OPCODES & 0xFFFF)
  adr REG_DEBUG_FUNC, asm_debug
  adr REG_INTERP_FUNC, asm_inturbo_call_interp

  bl asm_restore_AXYS_PC_flags

  br REG_SCRATCH3

.asm_enter_END:
  ret


.globl asm_exit
asm_exit:
  # Restore callee save registers.
  ldp x21, x22, [sp], #16
  ldp x23, x24, [sp], #16
  ldp x25, x26, [sp], #16
  ldp x27, x28, [sp], #16
  # Restore stack frame.
  ldp x29, x30, [sp], #16
  ret

.asm_exit_END:
  ret


.globl asm_debug
.globl asm_debug_END
asm_debug:
  stp x29, x30, [sp, #-16]!

  bl asm_save_AXYS_PC_flags

  mov REG_PARAM1, REG_CONTEXT
  movz REG_PARAM2, #0
  ldr REG_SCRATCH1, [REG_CONTEXT, #K_CONTEXT_OFFSET_DEBUG_CALLBACK]
  blr REG_SCRATCH1

  bl asm_restore_AXYS_PC_flags

  # TODO: handle different 6502 PC!
  ldp x29, x30, [sp], #16
  ret

asm_debug_end:
  ret