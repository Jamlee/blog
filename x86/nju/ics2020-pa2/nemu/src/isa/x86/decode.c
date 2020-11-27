#include <cpu/exec.h>
#include "local-include/rtl.h"
#include "local-include/reg.h"
#include "local-include/decode.h"

// x86 是小端序，所以 mod 在后面
typedef union {
  struct {
    uint8_t R_M		:3;
    uint8_t reg		:3;
    uint8_t mod		:2;
  }; // 8 位
  struct {
    uint8_t dont_care	:3;
    uint8_t opcode		:3;
  }; // 6 位
  uint8_t val; // 指令值 8
} ModR_M; // 2 3 3 总 8 个 bit。最大是 8 bit 所以占用8位，似乎可以直接解码了。这里是地位存在后面

typedef union {
  struct {
    uint8_t base	:3;
    uint8_t index	:3;
    uint8_t ss		:2;
  };
  uint8_t val;
} SIB; // 8 bit 自动解码了

// 读取内存的值到操作数中
static inline void load_addr(DecodeExecState *s, ModR_M *m, Operand *rm) {
  assert(m->mod != 3);

  sword_t disp = 0;
  int disp_size = 4;
  int base_reg = -1, index_reg = -1, scale = 0;

  if (m->R_M == R_ESP) {
    SIB sib;
    sib.val = instr_fetch(&s->seq_pc, 1);
    base_reg = sib.base;
    scale = sib.ss;

    if (sib.index != R_ESP) { index_reg = sib.index; }
  }
  else {
    /* no SIB */
    base_reg = m->R_M;
  }

  if (m->mod == 0) {
    if (base_reg == R_EBP) { base_reg = -1; }
    else { disp_size = 0; }
  }
  else if (m->mod == 1) { disp_size = 1; }

  if (disp_size != 0) {
    /* has disp */
    disp = instr_fetch(&s->seq_pc, disp_size);
    if (disp_size == 1) { disp = (int8_t)disp; }
  }

  s->isa.mbase = (base_reg != -1 ? &reg_l(base_reg) : rz);
  if (index_reg != -1) {
    rtl_shli(s, s1, &reg_l(index_reg), scale);
    rtl_add(s, &s->isa.mbr, s->isa.mbase, s1);
    s->isa.mbase = &s->isa.mbr;
  }
  s->isa.moff = disp;

#ifdef DEBUG
  char disp_buf[16];
  char base_buf[8];
  char index_buf[8];

  if (disp_size != 0) {
    /* has disp */
    sprintf(disp_buf, "%s%#x", (disp < 0 ? "-" : ""), (disp < 0 ? -disp : disp));
  }
  else { disp_buf[0] = '\0'; }

  if (base_reg == -1) { base_buf[0] = '\0'; }
  else { 
    sprintf(base_buf, "%%%s", reg_name(base_reg, 4));
  }

  if (index_reg == -1) { index_buf[0] = '\0'; }
  else { 
    sprintf(index_buf, ",%%%s,%d", reg_name(index_reg, 4), 1 << scale);
  }

  if (base_reg == -1 && index_reg == -1) {
    sprintf(rm->str, "%s", disp_buf);
  }
  else {
    sprintf(rm->str, "%s(%s%s)", disp_buf, base_buf, index_buf);
  }
#endif
  // 操作数是内存
  rm->type = OP_TYPE_MEM;
}

// 操作数是内存寻址的操作数。
void read_ModR_M(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  ModR_M m;
  m.val = instr_fetch(&s->seq_pc, 1); // 取第二位 MODR/M.放入到 val 之后自动解码
  s->isa.ext_opcode = m.opcode; // ext_code 是当前指令的扩展 code
  if (reg != NULL) operand_reg(s, reg, load_reg_val, m.reg, reg->width); // reg 解码到源或者目的操作数
  if (m.mod == 3) operand_reg(s, rm, load_rm_val, m.R_M, rm->width); // R/M 对应的是寄存器，直接寄存器寻址。另外一个寄存器在 opcode 中隐含
  else {
    load_addr(s, &m, rm); 
    if (load_rm_val) rtl_lm(s, &rm->val, s->isa.mbase, s->isa.moff, rm->width); // lea 这种指令不需要访问内存，load_rm_val = false
    rm->preg = &rm->val;
  }
}
