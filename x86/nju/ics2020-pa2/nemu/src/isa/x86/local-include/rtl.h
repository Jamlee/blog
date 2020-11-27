#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

static word_t ss_base = 0x900000;

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

// PUSH Iv
static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  // TODO();
  // 内存最大值 0xd003e8
  rtl_subi(s, &cpu.esp, &cpu.esp, 4);// esp的地址减4，栈是向下生成。这里要确认需要push的值放在那个位置
  rtl_sm(s, &ss_base, cpu.esp, src1, 4); // 写入到内存. save memory
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  // TODO();
  rtl_lm(s, dest, &ss_base, cpu.esp, 4); // 从内存读取到寄存器. load memory
  rtl_addi(s, &cpu.esp, &cpu.esp, 4);                                                                                     
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  TODO();
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  TODO();
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  TODO();
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  TODO();
}

// ZF 标志
// flag 的第 6 位，零标志位。记录相关指令执行后，结果是否为 0，是则 zf=1，反之，zf=0。在 8086CPU 的指令集种，有的指令是影响标志寄存器的逼入，add、sub、mul、div、inc、or、and 等，大多是运算指令；有的指令的执行对标志寄存器没有影响，比如，mov、push、pop等，大多是传送指令。
// PF 标志
// flag 的第 2 位，奇偶标志位。记录相关指令执行后，其结果的所有 bit 位中 1 的个数是否为偶数。若 1 的个数位偶数，pf=1，若为奇数，那么 pf=0。
// SF 标志
// flag 的第 7 位，符号标志位。记录相关指令执行后，其结果是否为负。若结果为负，sf=1；否则 sf=0。
// CF 标志
// flag 的第 0 位，进位标志位。一般情况下，在进行无符号运算时，它记录了运算结果的最高有效位向更高有效位的进位值，或从更高位的借位值。
// OF 标志
// flag 的第 11 位，标志溢出位。一般情况下，OF 记录了有符号数运算的结果是否发生了溢出。若发生了溢出，则 OF=1；反之，OF=0。
#define flag(f) cpu.eflags.f
#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    flag(f) = *src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = flag(f); \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

// ZF	零标志/Zero Flag	NZ/Not Zero/不等于零
static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  // TODO();
  rtl_shli(s, &cpu.eflags.ZF, result, 32 - width * 8);
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  // TODO();
  rtl_msb(s, &cpu.eflags.SF, result, width);
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
