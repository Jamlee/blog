#  head.s contains the 32-bit startup code.
#  Two L3 task multitasking. The code of tasks are in kernel area, 
#  just like the Linux. The kernel code is located at 0x10000. 
SCRN_SEL	= 0x18
TSS0_SEL	= 0x20
LDT0_SEL	= 0x28
TSS1_SEL	= 0X30
LDT1_SEL	= 0x38

# AT&T 语法手册 https://sourceware.org/binutils/docs/as/
# 伪指令查询: http://www.sourceware.org/binutils/docs-2.12/as.info/Pseudo-Ops.html#Pseudo%20Ops
# .global关键字用来让一个符号对链接器可见，可以供其他链接对象模块使用。
# .extern XXXX 说明xxxx为外部函数，调用的时候可以遍访所有文件找到该函数并且使用它。
.global startup_32
.text
startup_32: # 代码boot.s被移动到 0x0
	movl $0x10,%eax # 数据段选择符，0x10 = 00010000。index=10 也就是2
	mov %ax,%ds
#	mov %ax,%es
	lss init_stack,%esp # 书上的代码是AT&T汇编,所以应理解为init_stack的低16位传入esp,高16位传入ss.

# setup base fields of descriptors.
	call setup_idt
	call setup_gdt

	# 为什么这里是  0x10?
	movl $0x10,%eax		# reload all the segment registers。 l = long (32 bit integer or 64-bit floating point). 
	mov %ax,%ds		# after changing gdt. 
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	lss init_stack,%esp

# 端口地址的设置方法一般有两种:统一编址和独立编址。
# IBM PC及其兼容微机主要使用独立编址方式，采用了一个独立的I/O地址空间对控制设备中的寄存 器进行寻址和访问。使用 ISA 总线结构的传统 PC 机其 I/O 地址空间范围是 0x000 -- 0x3FF，有 1024 个 I/O 端口地址可供使用。
# IBM PC机也部分地使用了统一编址方式。例如，CGA 显示卡上显示内存的地址就直接占用了 存储器地址空间 0xB800 -- 0xBC00 范围。因此若要让一个字符显示在屏幕上，可以直接使用内存操作指令 往这个内存区域执行写操作。
# 设置 8253 定时芯片。把计数器通道 0 设置成每隔 10 毫秒向中断控制器发送一个中断请求信号。
# setup up timer 8253 chip. 
	movb $0x36, %al # al 中写入控制字 https://zhuanlan.zhihu.com/p/24356107。0x36 = 0011 0110。 (00)(11)(011)(0)B，选择计数器0，计数值为65536，工作方式3，二进制计数
	movl $0x43, %edx # 0x43 io 端口寻址。https://wiki.osdev.org/I/O_Ports。定义在 PCH: https://stackoverflow.com/questions/14194798/is-there-a-specification-of-x86-i-o-port-assignment
	outb %al, %dx   # 已知IBM PC内8253的接口地址采用部分译码方式，占用的设备端口地址为40H-5FH。选择该8253芯片。控制字在AL中。 由于软件兼容性的原因，最新的PC系统都还支持PC/AT机的总线结构。
	movl $11930, %eax        # timer frequency 100 HZ 
	movl $0x40, %edx
	outb %al, %dx
	movb %ah, %al
	outb %al, %dx

# 0000 0000 0000 0000 1000 1110 0000 0000  0x8e P=1, DPL = 00. S -- 描述符类型(0-系统;1-代码或数据) = 0. TYPE=1110. 查表得知 32 位中断门
# 在 IDT表第8和第 128(0x80)项处分别设置定时中断门描述符和系统调用陷阱门描述符。
# setup timer & system call interrupt descriptors.
	movl $0x00080000, %eax              # 基地址是 8 。段的 selector index=8
	movw $timer_interrupt, %ax          # ax 是 函数地址，也就是偏移地址。 
	movw $0x8E00, %dx                   # 定义了段的属性
	movl $0x08, %ecx              # The PC default timer int. 第8个中断
	lea idt(,%ecx,8), %esi        # 载入有效地址到 esi， %ecx * 8 + idt 
	movl %eax,(%esi)              # 在 esi 位置中写入 eax
	movl %edx,4(%esi)			  # 在 esi 偏移4字节的位置中写入 edx。edx这里高16位没有写入过。难道不会变有影响吗？这里暂时还不明白
	movw $system_interrupt, %ax
	movw $0xef00, %dx
	movl $0x80, %ecx
	lea idt(,%ecx,8), %esi
	movl %eax,(%esi) 
	movl %edx,4(%esi)

# unmask the timer interrupt.
#	movl $0x21, %edx
#	inb %dx, %al
#	andb $0xfe, %al
#	outb %al, %dx

# 我们为移动到任务 0(任务 A)中执行来操作堆栈内容，在堆栈中人工建立中断返回时的场景。
# Move to user mode (task 0)
	pushfl  # pushfl 指令是把标志寄存器的值压栈
	andl $0xffffbfff, (%esp) # eflag 是 32 位。 1111 1111 1111 1111 1011 1111 1111 1111 。置为初始状态
	popfl   # 标志寄存器出栈
	movl $TSS0_SEL, %eax
	ltr %ax
	movl $LDT0_SEL, %eax
	lldt %ax 
	movl $0, current
	sti
	pushl $0x17
	pushl $init_stack # 把堆栈指针入栈(也可以直接把 ESP 入栈)。
	pushfl
	pushl $0x0f
	pushl $task0
	iret

/****************************************/
setup_gdt:
	lgdt lgdt_opcode
	ret

setup_idt: # 循环设置256个中断统一为为 ignore_int
	lea ignore_int,%edx
	movl $0x00080000,%eax  # gdt中索引为8的段
	movw %dx,%ax		/* selector = 0x0008 = cs */
	movw $0x8E00,%dx	/* interrupt gate - dpl=0, present */
	lea idt,%edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx     # 这条指令执行结果影响AF、OF、PF、SF、ZF标志位
	jne rp_sidt  # 递减循环。 当零标志 Z=0 则跳转； 否则 零标志 Z=1 则顺序执行下一条指令。
	lidt lidt_opcode
	ret

# -----------------------------------
write_char:
	push %gs
	pushl %ebx
#	pushl %eax
	mov $SCRN_SEL, %ebx
	mov %bx, %gs
	movl scr_loc, %ebx
	shl $1, %ebx
	movb %al, %gs:(%ebx)
	shr $1, %ebx
	incl %ebx
	cmpl $2000, %ebx
	jb 1f
	movl $0, %ebx
1:	movl %ebx, scr_loc	
#	popl %eax
	popl %ebx
	pop %gs
	ret

/***********************************************/
/* This is the default interrupt "handler" :-) */
.align 2
ignore_int:
	push %ds
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds
	movl $67, %eax            /* print 'C' */
	call write_char
	popl %eax
	pop %ds
	iret

/* Timer interrupt handler */ 
.align 2
timer_interrupt:
	push %ds
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds
	movb $0x20, %al
	outb %al, $0x20
	movl $1, %eax
	cmpl %eax, current
	je 1f
	movl %eax, current
	ljmp $TSS1_SEL, $0
	jmp 2f
1:	movl $0, current
	ljmp $TSS0_SEL, $0
2:	popl %eax
	pop %ds
	iret

/* system call handler */
.align 2
system_interrupt:
	push %ds
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10, %edx
	mov %dx, %ds
	call write_char
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	pop %ds
	iret

/*********************************************/
current:.long 0
scr_loc:.long 0

.align 2
lidt_opcode:
	.word 256*8-1		# idt contains 256 entries
	.long idt		# This will be rewrite by code. 
lgdt_opcode:
	.word (end_gdt-gdt)-1	# so does gdt 
	.long gdt		# This will be rewrite by code.

	.align 8
idt:	.fill 256,8,0		# idt is uninitialized

gdt:	.quad 0x0000000000000000	/* NULL descriptor */
	.quad 0x00c09a00000007ff	/* 8Mb 0x08, base = 0x00000 */
	.quad 0x00c09200000007ff	/* 8Mb 0x10 */
	.quad 0x00c0920b80000002	/* screen 0x18 - for display */

	.word 0x0068, tss0, 0xe900, 0x0	# TSS0 descr 0x20   # 预先设置了 gdt 的 tss0
	.word 0x0040, ldt0, 0xe200, 0x0	# LDT0 descr 0x28
	.word 0x0068, tss1, 0xe900, 0x0	# TSS1 descr 0x30   # 预先设置了 gdt 的 tss1
	.word 0x0040, ldt1, 0xe200, 0x0	# LDT1 descr 0x38
end_gdt:
	.fill 128,4,0
init_stack:                          # Will be used as user stack for task0.
	.long init_stack                 # 32 位写入 init_stack 的地址
	.word 0x10

/*************************************/
.align 8
ldt0:	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff	# 0x0f, base = 0x00000
	.quad 0x00c0f200000003ff	# 0x17

tss0:	.long 0 			/* back link */      # TSS 状态段
	.long krn_stk0, 0x10		/* esp0, ss0 */
	.long 0, 0, 0, 0, 0		/* esp1, ss1, esp2, ss2, cr3 */
	.long 0, 0, 0, 0, 0		/* eip, eflags, eax, ecx, edx */
	.long 0, 0, 0, 0, 0		/* ebx esp, ebp, esi, edi */
	.long 0, 0, 0, 0, 0, 0 		/* es, cs, ss, ds, fs, gs */
	.long LDT0_SEL, 0x8000000	/* ldt, trace bitmap */

	.fill 128,4,0
krn_stk0:
#	.long 0

/************************************/
.align 8
ldt1:	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff	# 0x0f, base = 0x00000
	.quad 0x00c0f200000003ff	# 0x17

tss1:	.long 0 			/* back link */ # TSS 状态段
	.long krn_stk1, 0x10		/* esp0, ss0 */
	.long 0, 0, 0, 0, 0		/* esp1, ss1, esp2, ss2, cr3 */
	.long task1, 0x200		/* eip, eflags */
	.long 0, 0, 0, 0		/* eax, ecx, edx, ebx */
	.long usr_stk1, 0, 0, 0		/* esp, ebp, esi, edi */
	.long 0x17,0x0f,0x17,0x17,0x17,0x17 /* es, cs, ss, ds, fs, gs */
	.long LDT1_SEL, 0x8000000	/* ldt, trace bitmap */

	.fill 128,4,0
krn_stk1:

/************************************/
task0:
	movl $0x17, %eax
	movw %ax, %ds
	movb $65, %al              /* print 'A' */
	int $0x80
	movl $0xfff, %ecx
1:	loop 1b
	jmp task0 

task1:
	movl $0x17, %eax
	movw %ax, %ds
	movb $66, %al              /* print 'B' */
	int $0x80
	movl $0xfff, %ecx
1:	loop 1b
	jmp task1

	.fill 128,4,0 
usr_stk1:
