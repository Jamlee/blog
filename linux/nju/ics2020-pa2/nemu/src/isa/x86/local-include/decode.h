#include <cpu/exec.h>
#include "rtl.h"


void read_ModR_M(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val);

// 在 s 中把 operand 解码的
static inline void operand_reg(DecodeExecState *s, Operand *op, bool load_val, int r, int width) {
  // op 是 s 中 Operand 的指针。往里面写东西就是解码，此外还要传输到真正的寄存器上
  op->type = OP_TYPE_REG;
  op->reg = r; // 获取到寄存器的名字 0x7 是 1110 , 位与 x07 就是 只保留 这三位

  if (width == 4) {
    op->preg = &reg_l(r); // 对应寄存器的指针，为什么是乱序排序寄存器的原因找到了。就是这里要按数字检索，寄存器的顺序不可以变
  } else {
    assert(width == 1 || width == 2); // 如果操作数的 width 是 1 或者 2。说明是 ax 或者 al 这种
    op->preg = &op->val;  // 对应寄存器的指针指向 val 内存，而不是 cpu 寄存器地址
    if (load_val) rtl_lr(s, &op->val, r, width);
  }

  print_Dop(op->str, OP_STR_SIZE, "%%%s", reg_name(r, width));
}

// 解码无符号立即数
static inline void operand_imm(DecodeExecState *s, Operand *op, bool load_val, word_t imm, int width) {
  op->type = OP_TYPE_IMM;
  op->imm = imm;
  if (load_val) {
    rtl_li(s, &op->val, imm);
    op->preg = &op->val;
  }
  print_Dop(op->str, OP_STR_SIZE, "$0x%x", imm);
}

// 解码有符号立即数
static inline void operand_simm(DecodeExecState *s, Operand *op, bool load_val, sword_t simm, int width) {
  op->type = OP_TYPE_IMM;
  op->simm = simm;
  if (load_val) {
    rtl_li(s, &op->val, simm); // li 表示 load immediate
    op->preg = &op->val;
  }
  print_Dop(op->str, OP_STR_SIZE, "$0x%x", simm);
}

// 译码操作数辅助函数(decode operand helper function)组成
// decode operand helper
#define def_DopHelper(name) void concat(decode_op_, name) (DecodeExecState *s, Operand *op, bool load_val)

/* Refer to Appendix A in i386 manual for the explanations of these abbreviations */

// static inline void decode_op_I (DecodeExecState *s, Operand *op, bool load_val) {
//   word_t imm = instr_fetch(&s->seq_pc, op->width);
//   operand_imm(s, op, load_val, imm, op->width);
// }
/* Ib, Iv */
static inline def_DopHelper(I) {
  /* pc here is pointing to the immediate */
  word_t imm = instr_fetch(&s->seq_pc, op->width);
  operand_imm(s, op, load_val, imm, op->width);
}

// 解码 读取 SI 指令 SI 是 sign immediate。有符号的立即数读取到操作数中
/* I386 manual does not contain this abbreviation, but it is different from
 * the one above from the view of implementation. So we use another helper
 * function to decode it.
 */
/* sign immediate */
static inline def_DopHelper(SI) {
  assert(op->width == 1 || op->width == 4);
  /* TODO: Use instr_fetch() to read `op->width' bytes of memory
   * pointed by 's->seq_pc'. Interpret the result as a signed immediate,
   * and call `operand_imm()` as following.
   *
   operand_imm(s, op, load_val, ???, op->width);
   */
  // TODO();
  sword_t simm = instr_fetch(&s->seq_pc, op->width);
  operand_simm(s, op, load_val, simm, op->width);
}

/* I386 manual does not contain this abbreviation.
 * It is convenient to merge them into a single helper function.
 */
/* AL/eAX */
static inline def_DopHelper(a) {
  operand_reg(s, op, load_val, R_EAX, op->width);
}

// 定义一个辅助函数用于解码寄存器
// static inline void decode_op_r (DecodeExecState *s, Operand *op, bool load_val) {
//   operand_reg(s, op, load_val, s->opcode & 0x7, op->width);
// }
/* This helper function is use to decode register encoded in the opcode. */
/* XX: AL, AH, BL, BH, CL, CH, DL, DH
 * eXX: eAX, eCX, eDX, eBX, eSP, eBP, eSI, eDI
 */
static inline def_DopHelper(r) {
  // s->opcode & 0x7 干吗？
  operand_reg(s, op, load_val, s->opcode & 0x7, op->width);
}

// 解码 modR/M 中所有字段到
/* I386 manual does not contain this abbreviation.
 * We decode everything of modR/M byte in one time.
 */
/* Eb, Ew, Ev
 * Gb, Gv
 * Cd,
 * M
 * Rd
 * Sw
 */
static inline void operand_rm(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  read_ModR_M(s, rm, load_rm_val, reg, load_reg_val);
}

/* Ob, Ov */
static inline def_DopHelper(O) {
  op->type = OP_TYPE_MEM;
  s->isa.moff = instr_fetch(&s->seq_pc, 4);
  s->isa.mbase = rz;
  if (load_val) {
    rtl_lm(s, &op->val, s->isa.mbase, s->isa.moff, op->width);
    op->preg = &op->val;
  }

  print_Dop(op->str, OP_STR_SIZE, "0x%x", s->isa.moff);
}

/* Eb <- Gb
 * Ev <- Gv
 */
static inline def_DHelper(G2E) {
  operand_rm(s, id_dest, true, id_src1, true); // id_dest 作为 Operand *rm
}

static inline def_DHelper(mov_G2E) {
  operand_rm(s, id_dest, false, id_src1, true);
}

/* Gb <- Eb
 * Gv <- Ev
 */
static inline def_DHelper(E2G) {
  operand_rm(s, id_src1, true, id_dest, true);
}

static inline def_DHelper(mov_E2G) {
  operand_rm(s, id_src1, true, id_dest, false);
}

static inline def_DHelper(lea_M2G) {
  operand_rm(s, id_src1, false, id_dest, false); // 加载内存地址则不需要访问内存了
}

/* AL <- Ib
 * eAX <- Iv
 */
static inline def_DHelper(I2a) {
  decode_op_a(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}

/* Gv <- EvIb
 * Gv <- EvIv
 * use for imul */
static inline def_DHelper(I_E2G) {
  operand_rm(s, id_src2, true, id_dest, false);
  decode_op_I(s, id_src1, true);
}

/* Eb <- Ib
 * Ev <- Iv
 */
static inline def_DHelper(I2E) {
  operand_rm(s, id_dest, true, NULL, false);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(mov_I2E) {
  operand_rm(s, id_dest, false, NULL, false);
  decode_op_I(s, id_src1, true);
}

/* XX <- Ib
 * eXX <- Iv
 */
static inline def_DHelper(I2r) {
  decode_op_r(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}

// 立即数到寄存器的 mov 解码
// static inline void decode_op_r (DecodeExecState *s, Operand *op, bool load_val) {
//   operand_reg(s, op, load_val, s->opcode & 0x7, op->width); // widtd 在 local-include 里的  set_width 设置了
// }
// static inline void decode_mov_I2r (DecodeExecState *s) {
//   decode_op_r(s, (&s->dest), false);
//   decode_op_I(s, (&s->src1), true);
// }
// op 的width 设置的
// id_dest 是宏。编辑器里颜色是深蓝色
// 解码只是把值解码到 operand 中存储了。具体操作还要 exec 中来操作
static inline def_DHelper(mov_I2r) {
  decode_op_r(s, id_dest, false); // 目标值，因为这里已经用的是 id_dest
  decode_op_I(s, id_src1, true);  // 源。true 意味着将值load入 operand里的val中，operand->preg 指向 operand->val。operand 中是有位宽的在表中已经有了。
}

/* used by unary operations */
static inline def_DHelper(I) {
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(r) {
  decode_op_r(s, id_dest, true);
}

static inline def_DHelper(E) {
  operand_rm(s, id_dest, true, NULL, false);
}

static inline def_DHelper(setcc_E) {
  operand_rm(s, id_dest, false, NULL, false);
}

static inline def_DHelper(gp7_E) {
  operand_rm(s, id_dest, false, NULL, false);
}

/* used by test in group3 */
static inline def_DHelper(test_I) {
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(SI2E) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 2) {
    *dsrc1 &= 0xffff;
  }
}

static inline def_DHelper(SI_E2G) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_src2, true, id_dest, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 2) {
    *dsrc1 &= 0xffff;
  }
}

static inline def_DHelper(gp2_1_E) {
  operand_rm(s, id_dest, true, NULL, false);
  operand_imm(s, id_src1, true, 1, 1);
}

static inline def_DHelper(gp2_cl2E) {
  operand_rm(s, id_dest, true, NULL, false);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}

static inline def_DHelper(gp2_Ib2E) {
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}

/* Ev <- GvIb
 * use for shld/shrd */
static inline def_DHelper(Ib_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}

/* Ev <- GvCL
 * use for shld/shrd */
static inline def_DHelper(cl_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}

static inline def_DHelper(O2a) {
  decode_op_O(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(a2O) {
  decode_op_a(s, id_src1, true);
  decode_op_O(s, id_dest, false);
}

//  The instruction contains a relative offset to be added to the instruction pointer register
// (for example, JMP (0E9), LOOP).
static inline def_DHelper(J) {
  decode_op_SI(s, id_dest, false);
  // the target address can be computed in the decode stage
  s->jmp_pc = id_dest->simm + s->seq_pc;
}

static inline def_DHelper(call_J) {
  decode_op_SI(s, id_dest, false);
  // the target address can be computed in the decode stage
  s->jmp_pc = id_dest->simm + cpu.pc + 5;
}

// 定义 PUSH。push的参数是有符号立即数(SI)
static inline def_DHelper(push_SI) {
  decode_op_SI(s, id_src1, true);
}

// 定义 PUSH 的 r 参数解码
static inline def_DHelper(push_G) {
  // id_src1 的默认位宽是 4
  // 解码这条指令时
  decode_op_r(s, id_src1, true); // 解码到 src 中。 true 表示寄存器里存的值放到 操作数的 val 中
}

static inline def_DHelper(in_I2a) {
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(in_dx2a) {
  operand_reg(s, id_src1, true, R_DX, 2);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(out_a2I) {
  decode_op_a(s, id_src1, true);
  id_dest->width = 1;
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(out_a2dx) {
  decode_op_a(s, id_src1, true);
  operand_reg(s, id_dest, true, R_DX, 2);
}

static inline void operand_write(DecodeExecState *s, Operand *op, rtlreg_t* src) {
  if (op->type == OP_TYPE_REG) { rtl_sr(s, op->reg, src, op->width); }
  else if (op->type == OP_TYPE_MEM) { rtl_sm(s, s->isa.mbase, s->isa.moff, src, op->width); }
  else { assert(0); }
}
