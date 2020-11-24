#ifndef __ISA_X86_H__
#define __ISA_X86_H__

#include <common.h>


// 定义了起始地址，尤其是 0x0 用来做物理地址和线性地址的转换来用的
// memory
#define x86_IMAGE_START 0x100000
#define x86_PMEM_BASE 0x0

// reg

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  union {
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];

    /* Do NOT change the order of the GPRs' definitions. */

    /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
    * in PA2 able to directly access these registers.
    */
    struct {
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };
  };
  vaddr_t pc;
} x86_CPU_state;

// decode
typedef struct {
  bool is_operand_size_16; // 动态决定 width
  uint8_t ext_opcode;  // 这是干么的用的 gp1 之类的就是它。扩展 opcode
  const rtlreg_t *mbase; // 内存基地址
  rtlreg_t mbr; // base reg 
  word_t moff; // offset
} x86_ISADecodeInfo; // 解码时用到的信息

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))
#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define x86_has_mem_exception() (false)

#endif
