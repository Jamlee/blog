#ifndef __CPU_EXEC_H__
#define __CPU_EXEC_H__

#include <isa.h>
#include <rtl/rtl.h>
#include <memory/vaddr.h>
#include <cpu/decode.h>

// 译码辅助函数(decode helper function)来完成的。concat 也是宏
#define def_EHelper(name) void concat(exec_, name) (DecodeExecState *s)

// 用于译码的帮助宏
// empty decode helper
#define decode_empty(s)

#define IDEXW(idx, id, ex, w) CASE_ENTRY(idx, concat(decode_, id), concat(exec_, ex), w)
#define IDEX(idx, id, ex)     IDEXW(idx, id, ex, 0)
#define EXW(idx, ex, w)       IDEXW(idx, empty, ex, w)
#define EX(idx, ex)           EXW(idx, ex, 0)
#define EMPTY(idx)            EX(idx, inv)

// set_width() is defined in src/isa/$isa/exec/exec.c
#define CASE_ENTRY(idx, id, ex, w) case idx: set_width(s, w); id(s); ex(s); break;

static inline uint32_t instr_fetch(vaddr_t *pc, int len) {
  // 从 pc 的位置中取指令, pc 指向还是模拟内存的线性
  uint32_t instr = vaddr_ifetch(*pc, len); // len 是指令额大小。目前都是 one byte 指令
#ifdef DEBUG
  uint8_t *p_instr = (void *)&instr;
  int i;
  for (i = 0; i < len; i ++) {
    extern char log_bytebuf[];
    strcatf(log_bytebuf, "%02x ", p_instr[i]);
  }
#endif
  // 取值之后指向下一个字节.第一次取opcode，第二次取 modr/m 
  (*pc) += len;
  return instr;
}

static inline void update_pc(DecodeExecState *s) {
  // seq_pc 同步到 cpu.pc 中
  cpu.pc = (s->is_jmp ? s->jmp_pc : s->seq_pc);
}

#ifdef DEBUG
#define print_asm(...) \
  do { \
    extern char log_asmbuf[]; \
    strcatf(log_asmbuf, __VA_ARGS__); \
  } while (0)
#else
#define print_asm(...)
#endif

#ifndef suffix_char
#define suffix_char(width) ' '
#endif

#define print_asm_template1(instr) \
  print_asm(str(instr) "%c %s", suffix_char(id_dest->width), id_dest->str)

#define print_asm_template2(instr) \
  print_asm(str(instr) "%c %s,%s", suffix_char(id_dest->width), id_src1->str, id_dest->str)

#define print_asm_template3(instr) \
  print_asm(str(instr) "%c %s,%s,%s", suffix_char(id_dest->width), id_src1->str, id_src2->str, id_dest->str)

#endif
