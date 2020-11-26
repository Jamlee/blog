#include <isa/x86.h>

// 测试用
// word_t x = 0x1;

// static inline void exec_mov (DecodeExecState *s) {
//   operand_write(s, (&s->dest), ((&s->src1)->preg));
//   print_asm_template2(mov);
// }
// I2r 里：目标值 op->preg 指向 eax。立即数 op->preg 指向 op->val.
// 也有可能 目标值 op->preg 指向 op->val。立即数 op->preg 指向 op->val.
static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

// 自己定义 push 指令，解码时已经确定了源操作数和目标操作数. 解码器是 push_SI
static inline def_EHelper(push) {
  // TODO();
  // print_asm_template1(push);
  rtl_push(s, dsrc1);

  print_asm_template1(push);
}

// 自己定义 pop 指令
static inline def_EHelper(pop) {
  TODO();
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  TODO();
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  TODO();
  print_asm("popa");
}

static inline def_EHelper(leave) {
  TODO();
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}
