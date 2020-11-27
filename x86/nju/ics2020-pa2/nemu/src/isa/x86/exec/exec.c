#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

// 被包含在宏里面. 为一次执行状态机设置 width
static inline void set_width(DecodeExecState *s, int width) {
  if (width == -1) return;
  if (width == 0) {
    // typedef struct {
    //   bool is_operand_size_16;
    //   uint8_t ext_opcode;
    //   const rtlreg_t *mbase;
    //   rtlreg_t mbr;
    //   word_t moff;
    // } x86_ISADecodeInfo;
    // isa 的结构，应算是整体 DecodeExecState 这个的配置吧
    // 默认操作数的位宽都设置为后面指定的 w
    width = s->isa.is_operand_size_16 ? 2 : 4;
  }
  // 默认操作数的位宽都设置为后面指定的 w。解码时会用到 op 的位宽
  s->src1.width = s->dest.width = s->src2.width = width;
}

//////////////////////////////////////////////////////
// 定了 gp1-7 这种莫名奇妙的东西
//////////////////////////////////////////////////////
/* 0x80, 0x81, 0x83 */
static inline def_EHelper(gp1) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
static inline def_EHelper(gp2) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0xf6, 0xf7 */
static inline def_EHelper(gp3) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0xfe */
static inline def_EHelper(gp4) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

// 宏展开结果大概是这样
// static inline void exec_gp5 (DecodeExecState *s) {
//   switch (s->isa.ext_opcode) {
//     case 0: set_width(s, 0); ; exec_inv(s); break; case 1: set_width(s, 0); ; exec_inv(s); break; case 2: set_width(s, 0); ; exec_inv(s); break; case 3: set_width(s, 0); ; exec_inv(s); break;
//     case 4: set_width(s, 0); ; exec_inv(s); break; case 5: set_width(s, 0); ; exec_inv(s); break; case 6: set_width(s, 0); ; exec_inv(s); break; case 7: set_width(s, 0); ; exec_inv(s); break;
//   }
// }
/* 0xff */
static inline def_EHelper(gp5) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0x0f 0x01*/
static inline def_EHelper(gp7) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

static inline def_EHelper(2byte_esc) {
  uint8_t opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
  /* TODO: Add more instructions!!! */
    IDEX (0x01, gp7_E, gp7)
    default: exec_inv(s); // 不知道是一个什么特殊指令，似乎是空
  }
}

// #define IDEXW(idx, id, ex, w) CASE_ENTRY(idx, concat(decode_, id), concat(exec_, ex), w)
// #define IDEX(idx, id, ex)     IDEXW(idx, id, ex, 0)
// #define EXW(idx, ex, w)       IDEXW(idx, empty, ex, w)
// #define EX(idx, ex)           EXW(idx, ex, 0)
// #define EMPTY(idx)            EX(idx, inv)
// set_width() is defined in src/isa/$isa/exec/exec.c
// #define CASE_ENTRY(idx, id, ex, w) case idx: set_width(s, w); id(s); ex(s); break;
// idx: 指令码（16进制）
// ex: exec_ex 应该对应的执行
// id: decode_id 对应的指令读取解码工具
// w: 设置指令的宽度
static inline void fetch_decode_exec(DecodeExecState *s) {
  uint8_t opcode;
again:
  // 取指令，很简单从指令表匹配就是了。指令都是 1byte 的，所以是 1。i386 中指令是3byte的
  opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
    EX   (0x0f, 2byte_esc) // Instructions that begin with 0FH can be found in the two-byte opcode maps given in Table A-3.
    IDEXW(0x80, I2E, gp1, 1) // idex=0x80, id=decode_I2E, ex=exec_gp1, w=1。这个需要读后面MOD才知道具体之类
    IDEX (0x81, I2E, gp1)    // w=2或者4。I2E 中的 I 表示立即数
    IDEX (0x83, SI2E, sub)        // 减法实现
    IDEXW(0x88, mov_G2E, mov, 1)
    IDEX (0x89, mov_G2E, mov)
    IDEXW(0x8a, mov_E2G, mov, 1)
    IDEX (0x8b, mov_E2G, mov)
    IDEXW(0xa0, O2a, mov, 1)
    IDEX (0xa1, O2a, mov)
    IDEXW(0xa2, a2O, mov, 1)
    IDEX (0xa3, a2O, mov)
    IDEXW(0xb0, mov_I2r, mov, 1) // mov al, 1 都是这种
    IDEXW(0xb1, mov_I2r, mov, 1) // mov ax, 1
    IDEXW(0xb2, mov_I2r, mov, 1)
    IDEXW(0xb3, mov_I2r, mov, 1)
    IDEXW(0xb4, mov_I2r, mov, 1)
    IDEXW(0xb5, mov_I2r, mov, 1)
    IDEXW(0xb6, mov_I2r, mov, 1)
    IDEXW(0xb7, mov_I2r, mov, 1)
    IDEX (0xb8, mov_I2r, mov)
    IDEX (0xb9, mov_I2r, mov)
    IDEX (0xba, mov_I2r, mov)
    IDEX (0xbb, mov_I2r, mov)
    IDEX (0xbc, mov_I2r, mov)
    IDEX (0xbd, mov_I2r, mov)
    IDEX (0xbe, mov_I2r, mov)
    IDEX (0xbf, mov_I2r, mov)
    IDEXW(0xc0, gp2_Ib2E, gp2, 1)
    IDEX (0xc1, gp2_Ib2E, gp2)
    IDEXW(0xc6, mov_I2E, mov, 1)
    IDEX (0xc7, mov_I2E, mov)
    IDEXW(0xd0, gp2_1_E, gp2, 1)
    IDEX (0xd1, gp2_1_E, gp2)
    IDEXW(0xd2, gp2_cl2E, gp2, 1)
    IDEX (0xd3, gp2_cl2E, gp2)
    EX   (0xd6, nemu_trap) // 在表格里 d6 是个空白的
    IDEXW(0xf6, E, gp3, 1)
    IDEX (0xf7, E, gp3)
    IDEXW(0xfe, E, gp4, 1)
    IDEX (0xff, E, gp5)

    // 我添加的指令
    IDEX   (0xe8, call_J, call)
    IDEX   (0x68, push_SI, push)  // 目的目标数都是内存这里就不写了
    IDEX   (0x55, push_G, push)
    IDEX   (0x31, E2G, xor)
    EX     (0xc3, ret)

  case 0x66: s->isa.is_operand_size_16 = true; goto again;
  default: exec_inv(s);
  }
}

vaddr_t isa_exec_once() {
  DecodeExecState s; // 统一定义
  s.is_jmp = 0; // 非调转指令
  s.isa = (ISADecodeInfo) { 0 }; // 初始化ISADecodeInfo为全0的空值. is_operand_size_16 默认是 0 也就是 false。默认位宽微微
  s.seq_pc = cpu.pc;  // 执行当前 pc 寄存器的位置
  // s 每条指令一次执行的状态

  fetch_decode_exec(&s);

  // jump_pc 会在这里更新那么下一句要跳转的内容了
  update_pc(&s);

  // 返回当前指令的尾部指针
  return s.seq_pc;
}
