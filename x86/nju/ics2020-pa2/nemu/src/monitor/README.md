# 试验报告

`monitor` 只使用到 `src/memeory`. `include/isa.h` `include/isa.h`。和`include下`的其他的源代码完全没有联系。但是这没道理呀？monitor 是启动入口，入口不和底下的内核打交道不是很奇怪吗。
靠这段代码和底层的 x86 的 exec 进行交互。

````
__attribute__((unused)) vaddr_t seq_pc = isa_exec_once(); // 这里就是 isa 的价值了，因为是引入的 x86 这里会是 x86.h 文件的方法
````

````
static inline void exec_gp5 (DecodeExecState *s) {
  switch (s->isa.ext_opcode) {
    case 0: set_width(s, 0); ; exec_inv(s); break; case 1: set_width(s, 0); ; exec_inv(s); break; case 2: set_width(s, 0); ; exec_inv(s); break; case 3: set_width(s, 0); ; exec_inv(s); break;
    case 4: set_width(s, 0); ; exec_inv(s); break; case 5: set_width(s, 0); ; exec_inv(s); break; case 6: set_width(s, 0); ; exec_inv(s); break; case 7: set_width(s, 0); ; exec_inv(s); break;
  }
}
````
## RSIC 是指令定长
由于CISC指令变长的特性, x86指令长度和指令形式需要一边取指一边译码来确定, 而不像RISC指令集那样可以泾渭分明地处理取指和译码阶段, 因此你会在x86的译码操作数辅助函数中看到instr_fetch()的操作.