#include <isa.h>
#include <memory/paddr.h>
#include "local-include/reg.h"

// static const uint8_t img []  = {
//   0xb8, 0x34, 0x12, 0x00, 0x00,        // 100000:  movl  $0x1234,%eax      // x 打印内存看到是的倒序的 0x1234b8
//   0xb9, 0x27, 0x00, 0x10, 0x00,        // 100005:  movl  $0x100027,%ecx
//   0x89, 0x01,                          // 10000a:  movl  %eax,(%ecx)
//   0x66, 0xc7, 0x41, 0x04, 0x01, 0x00,  // 10000c:  movw  $0x1,0x4(%ecx)
//   0xbb, 0x02, 0x00, 0x00, 0x00,        // 100012:  movl  $0x2,%ebx
//   0x66, 0xc7, 0x84, 0x99, 0x00, 0xe0,  // 100017:  movw  $0x1,-0x2000(%ecx,%ebx,4)
//   0xff, 0xff, 0x01, 0x00,
//   0xb8, 0x00, 0x00, 0x00, 0x00,        // 100021:  movl  $0x0,%eax
//   0xd6,                                // 100026:  nemu_trap
// };

// 用于测试  push 指令
static const uint8_t img []  = {
  0xbd, 0x91, 0x81, 0x71, 0x61,       	  // 100000: mov     $0x61718191, %ebp
  0x55,                                   // 100005: push    %ebp. 保存 ebp. gdb: p /x *s
  0x68, 0x2a, 0x00, 0x10, 0x00,           // 100006: push    $0x10002a
  0xe8, 0x13, 0x00, 0x10, 0x00,       	  // 100011: call   100013 <_trm_init>
  0xd6,                                   // 100012
  0xd6,                                   // 100013
};

static void restart() {
  /* Set the initial instruction pointer. */
  cpu.pc = PMEM_BASE + IMAGE_START;

  // 内存最大值 0xd003e8
  // 我把寄存器都复制为 0
  for (int i = R_EAX; i <= R_EDI; i ++) {
    reg_l(i)  = 0;
  }
  reg_l(R_ESP) = 0x100;
}

void init_isa() {
  /* Test the implementation of the `CPU_state' structure. */
  void reg_test();
  reg_test();

  // 读取是数组中的内存地址位置。并返回
  // 宏会展开: IMAGE_START -> x86_IMAGE_START
  /* Load built-in image. */
  memcpy(guest_to_host(IMAGE_START), img, sizeof(img));

  /* Initialize this virtual computer system. */
  restart();
}
