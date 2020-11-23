#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__

#include <isa.h>

#define OP_STR_SIZE 40
enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM };

// 译码解码定义统一的共享代码

typedef struct {
  uint32_t type;
  int width;
  union {
    uint32_t reg;    // reg或者立即
    word_t imm;      // 立即数
    sword_t simm;    // 无符号立即数
  };
  rtlreg_t *preg;    // 寄存器地址
  rtlreg_t val;      // 寄存器内容
  char str[OP_STR_SIZE];
} Operand; // 操作数或称为运行对象

typedef struct {
  uint32_t opcode;
  vaddr_t seq_pc;  // sequential pc
  uint32_t is_jmp;
  vaddr_t jmp_pc;
  Operand src1, dest, src2;  // 三个操作对象
  int width;
  rtlreg_t tmp_reg[4]; // 内部使用的临时寄存器四个
  ISADecodeInfo isa;
} DecodeExecState;

// 译码辅助函数(decode helper function)来完成的. 用于生成译码的函数
#define def_DHelper(name) void concat(decode_, name) (DecodeExecState *s)

#ifdef DEBUG
#define print_Dop(...) snprintf(__VA_ARGS__)
#else
#define print_Dop(...)
#endif

#endif
