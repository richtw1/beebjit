/* Appends at the end of jit.c. */

#include "test.h"

#include "bbc.h"
#include "emit_6502.h"

static struct cpu_driver* s_p_cpu_driver = NULL;
static struct jit_struct* s_p_jit = NULL;
static struct state_6502* s_p_state_6502 = NULL;
static uint8_t* s_p_mem = NULL;
static struct interp_struct* s_p_interp = NULL;
static struct jit_compiler* s_p_compiler = NULL;

static void
jit_test_init(struct bbc_struct* p_bbc) {
  struct cpu_driver* p_cpu_driver = bbc_get_cpu_driver(p_bbc);
  assert(p_cpu_driver->p_funcs->init == jit_init);

  s_p_cpu_driver = p_cpu_driver;
  s_p_jit = (struct jit_struct*) p_cpu_driver;
  s_p_state_6502 = p_cpu_driver->abi.p_state_6502;
  s_p_mem = p_cpu_driver->p_memory_access->p_mem_read;
  s_p_interp = s_p_jit->p_interp;
  s_p_compiler = s_p_jit->p_compiler;

  jit_compiler_testing_set_optimizing(s_p_compiler, 0);
  jit_compiler_testing_set_max_ops(s_p_compiler, 4);
  jit_compiler_testing_set_max_revalidate_count(s_p_compiler, 1);
}

static void
jit_test_block_split() {
  uint8_t* p_host_address;

  struct util_buffer* p_buf = util_buffer_create();

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB00);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB01);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  util_buffer_setup(p_buf, (s_p_mem + 0xB00), 0x100);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_EXIT(p_buf);
  state_6502_set_pc(s_p_state_6502, 0xB00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB01);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  state_6502_set_pc(s_p_state_6502, 0xB01);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB00);
  /* We expect 0 because the block isn't invalidated -- the first 6502 JIT
   * instruction of the block is instead.
   */
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB01);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  state_6502_set_pc(s_p_state_6502, 0xB00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xB01);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  util_buffer_destroy(p_buf);
}

static void
jit_test_block_continuation() {
  uint8_t* p_host_address;

  struct util_buffer* p_buf = util_buffer_create();

  util_buffer_setup(p_buf, (s_p_mem + 0xC00), 0x100);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  /* Block continuation here because we set the limit to 4 opcodes. */
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_EXIT(p_buf);

  state_6502_set_pc(s_p_state_6502, 0xC00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xC00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xC01);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xC04);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  state_6502_set_pc(s_p_state_6502, 0xC01);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xC01);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xC04);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xC05);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  util_buffer_destroy(p_buf);
}

static void
jit_test_invalidation() {
  uint8_t* p_host_address;

  struct util_buffer* p_buf = util_buffer_create();

  util_buffer_setup(p_buf, (s_p_mem + 0xD00), 0x100);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  /* Block continuation here. */
  emit_NOP(p_buf);
  emit_NOP(p_buf);
  emit_EXIT(p_buf);

  state_6502_set_pc(s_p_state_6502, 0xD00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  jit_invalidate_code_at_address(s_p_jit, 0xD01);

  state_6502_set_pc(s_p_state_6502, 0xD00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD01);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  state_6502_set_pc(s_p_state_6502, 0xD00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  /* This checks that the invalidation in the middle of a block didn't create
   * a new block boundary.
   */
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD01);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD04);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  jit_invalidate_code_at_address(s_p_jit, 0xD05);

  /* This execution will create a block at 0xD05 because of the invalidation
   * but it should not be a fundamental block boundary. Also, 0xD04 must remain
   * a block continuation and not a fundamental boundary.
   */
  state_6502_set_pc(s_p_state_6502, 0xD00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  /* Execute again, should settle back to the original block boundaries and
   * continuations.
   */
  state_6502_set_pc(s_p_state_6502, 0xD00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD04);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD05);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  /* Check that no block boundaries appeared in incorrect places. */
  state_6502_set_pc(s_p_state_6502, 0xD03);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD03);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD04);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xD05);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  util_buffer_destroy(p_buf);
}

static void
jit_test_dynamic_operand() {
  uint8_t* p_host_address;

  struct util_buffer* p_buf = util_buffer_create();

  jit_compiler_testing_set_optimizing(s_p_compiler, 1);
  state_6502_set_x(s_p_state_6502, 0);

  util_buffer_setup(p_buf, (s_p_mem + 0xE00), 0x80);
  emit_LDA(p_buf, k_abx, 0x0E01);
  emit_STA(p_buf, k_abs, 0xF0);
  emit_LDX(p_buf, k_imm, 0x02);
  emit_STX(p_buf, k_abs, 0x0E01);
  emit_EXIT(p_buf);

  state_6502_set_pc(s_p_state_6502, 0xE00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  /* After the first run through, the LDA $0E01 will have been self-modified
   * to LDA $0E02 and currently status will be awaiting compilation.
   */
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xE00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_code_host_address(s_p_jit, 0xE00);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xE01);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  /* This run should trigger a compilation where the optimizer flips the
   * LDA abx operand to a dynamic one.
   * Then, the subsequent self-modification should not trigger an invalidation.
   */
  state_6502_set_pc(s_p_state_6502, 0xE00);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xE00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_code_host_address(s_p_jit, 0xE00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));
  p_host_address = jit_get_jit_block_host_address(s_p_jit, 0xE01);
  test_expect_u32(1, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  jit_invalidate_code_at_address(s_p_jit, 0xE01);
  jit_invalidate_code_at_address(s_p_jit, 0xE02);
  p_host_address = jit_get_jit_code_host_address(s_p_jit, 0xE00);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  /* Try again but with the dynamic operand opcode not at the block start. */
  util_buffer_setup(p_buf, (s_p_mem + 0xE80), 0x80);
  emit_LDY(p_buf, k_imm, 0x84);
  emit_LDA(p_buf, k_abx, 0x0E83);
  emit_STY(p_buf, k_abs, 0x0E83);
  emit_EXIT(p_buf);

  state_6502_set_pc(s_p_state_6502, 0xE80);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);
  state_6502_set_pc(s_p_state_6502, 0xE80);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  jit_invalidate_code_at_address(s_p_jit, 0xE84);
  p_host_address = jit_get_jit_code_host_address(s_p_jit, 0xE82);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  state_6502_set_pc(s_p_state_6502, 0xE80);
  jit_enter(s_p_cpu_driver);
  interp_testing_unexit(s_p_interp);

  jit_invalidate_code_at_address(s_p_jit, 0xE84);
  p_host_address = jit_get_jit_code_host_address(s_p_jit, 0xE82);
  test_expect_u32(0, jit_is_host_address_invalidated(s_p_jit, p_host_address));

  jit_compiler_testing_set_optimizing(s_p_compiler, 0);
}

void
jit_test(struct bbc_struct* p_bbc) {
  jit_test_init(p_bbc);

  jit_test_block_split();
  jit_test_block_continuation();
  jit_test_invalidation();
  jit_test_dynamic_operand();
}
