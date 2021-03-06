#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  rtl_jr(s, ddest);

  print_asm("jmp *%s", id_dest->str);
}

// 实现 call 指令, 压栈 eip（也就是 cpu.pu）然后 jmp
// Call near, relative, displacement relative to next instruction
// call 的值是下一个指令的地址
static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  // TODO();
  // print_asm("call %x", s->jmp_pc);
  *t0 = cpu.pc + 5;
  rtl_push(s, (rtlreg_t*)t0);
  rtl_j(s, s->jmp_pc);

  print_asm("call %x", s->jmp_pc);
}

static inline def_EHelper(ret) {
  // TODO();
  rtl_pop(s, (rtlreg_t*)t0);
  rtl_j(s, *t0);
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();
  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  TODO();
  print_asm("call *%s", id_dest->str);
}
