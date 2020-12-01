# 显示段。段选择子
SCRN_SEL	= 0x18
# 系统段。段选择子
TSS0_SEL	= 0x20  # 0x20 = 0010 0000 右移三位 index= 100 = 4, DPL=0 
LDT0_SEL	= 0x28  # 0x28 = 0010 1000 右移三位 index= 101 = 5, DPL=0
TSS1_SEL	= 0X30  # 0x30 = 0011 0000 右移三位 index= 110 = 6, DPL=0 
LDT1_SEL	= 0x38  # 0x38 = 0011 1000 右移三位 index= 111 = 7, DPL=0 

.global startup_32
.text
startup_32:                                         # 代码被boot.s移动到 0x0。一开始加载到 0x1000, 是因为需要调用bios预设值的中断
	movl $0x10, %eax                                # 数据段选择符，0x10 = 00010000。index=10 也就是2.在boot里是 0x07c0.
	mov %ax, %ds                                    # 数据段

	# 这里会用到数据段寻址
	# 格式如下 lss stack, %esp 把stack指向的内存内容装入到SS:ESP中。 
	# 这里要注意， stack 是一个内存地址，lss 指令会把stack指向的内存地址的前四字节装入ESP寄存器，后两字节装入SS段寄存器，
	# 而不是把add这个值装入ESP寄存器。 如：内存0x1000地址的内容为0x0000F000,0x0010，则lss指令会把0x0000F000装入ESP，0x0010装入SS段寄存器。
	lss init_stack, %esp                           

    # 重新设置 setup_idt, setup_gdt
    call setup_idt
	call setup_gdt

    # 初始化所有数据段寄存器
    movl $0x10, %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	lss init_stack, %esp

	// ljmp $TSS1_SEL, $0

	####################################################################
	# 任务切换与定时器
	####################################################################
	# 端口地址的设置方法一般有两种:统一编址和独立编址。
    # IBM PC及其兼容微机主要使用独立编址方式，采用了一个独立的I/O地址空间对控制设备中的寄存 器进行寻址和访问。使用 ISA 总线结构的传统 PC 机其 I/O 地址空间范围是 0x000 -- 0x3FF，有 1024 个 I/O 端口地址可供使用。
    # IBM PC机也部分地使用了统一编址方式。例如，CGA 显示卡上显示内存的地址就直接占用了 存储器地址空间 0xB800 -- 0xBC00 范围。因此若要让一个字符显示在屏幕上，可以直接使用内存操作指令 往这个内存区域执行写操作。
    # 设置 8253 定时芯片。把计数器通道 0 设置成每隔 10 毫秒向中断控制器发送一个中断请求信号。
	# setup up timer 8253 chip.
	movb $0x36, %al          # al 中写入控制字 https://zhuanlan.zhihu.com/p/24356107。0x36 = 0011 0110。 (00)(11)(011)(0)B，选择计数器0，计数值为65536，工作方式3，二进制计数
	movl $0x43, %edx         # 0x43 io 端口寻址。https://wiki.osdev.org/I/O_Ports。定义在 PCH: https://stackoverflow.com/questions/14194798/is-there-a-specification-of-x86-i-o-port-assignment
	outb %al, %dx            # 已知IBM PC内8253的接口地址采用部分译码方式，占用的设备端口地址为40H-5FH。选择该8253芯片。控制字在AL中。 由于软件兼容性的原因，最新的PC系统都还支持PC/AT机的总线结构。
	movl $11930, %eax        # timer frequency 100 HZ 
	movl $0x40, %edx
	outb %al, %dx
	movb %ah, %al
	outb %al, %dx

	# INT 08H：时钟中断 。时钟的中断默认是 0x08
	# 注意计算的的时候不要错看成“段”，这里是“门”而不是“段”。虽然都是 64 位但是结构有些不同
	# 0000 0000 0000 0000 1000 1110 0000 0000. 0x8e P=1, DPL = 00. S -- 描述符类型(0-系统;1-代码或数据) = 0. TYPE=1110. 查表得知 32位中断门
	# 在 IDT表第8和第 128(0x80)项处分别设置定时中断门描述符和系统调用陷阱门描述符。
	# setup timer & system call interrupt descriptors.
	movl $0x00080000, %eax	           # 基地址是 8。段的 selector index=8
	movw $timer_interrupt, %ax         # ax 是函数地址，也就是偏移地址。
	movw $0x8E00, %dx                  # 定义了段的属性
	movl $0x08, %ecx                   # The PC default timer int. 8 * 8 = 64，即一个表项的大小
	lea idt(,%ecx,8), %esi             # 载入有效地址到 esi， idt + %ecx * 8 也就是 第 8 个的位置
	movl %eax,(%esi)                   # 低 32。esi 位置中写入 eax。
	movl %edx,4(%esi)                  # 高 32。esi 偏移 4 字节的位置中写入 edx。edx这里高16位没有写入过。难道不会变有影响吗？这里暂时还不明白
	movw $system_interrupt, %ax        # ax 低位是函数的偏移（函数的入口） 
	movw $0xef00, %dx                  # dx 低位是“门”的属性，属性 为 ef 。 1110 1111 陷阱门
	movl $0x80, %ecx                   # 0x80 就是  index=126
	lea idt(,%ecx,8), %esi             # 陷阱门载入到 idt 中表项（index=126）
	movl %eax,(%esi) 
	movl %edx,4(%esi)

	# 我们为移动到任务 0(任务 A)中执行来操作堆栈内容，在堆栈中人工建立中断返回时的场景。
	pushfl                             # pushfl 指令是把内核当前的标志寄存器的值压内核自己的栈
	andl $0xffffbfff, (%esp)           # eflag 是 32 位。 复位标志寄存器 EFLAGS 中的嵌套任务标志, NT标志。复位就是清0，置位就是置1。
	popfl                              # 清理后，标志寄存器出栈
	movl $TSS0_SEL, %eax               # 任务 0 的 TSS 段。TSS0_SEL = 0x20 = 0010 0000,右移三位，index=00100 = 4
	ltr %ax                            # tr 的值是 任务 0
	movl $LDT0_SEL, %eax               # 任务 0 的 LDT 段 TSS0_SEL = 0x28 = 0010 1000,右移三位，index=00101 = 5
	lldt %ax                           # ldtr 寄存器是 index=5 的值, 这里把task0的内容都手动建好了，其实就是 tss 里的内容
	movl $0, current                   # 内存32位。当前任务的id是 0
	sti                                # Set Interrupt Flag。开启中断，还有就是关闭中断  Clear Interrupt Flag
	# DS, SS 入栈似乎没有作用呀，iret 不弹出它。这里因为目前构造tss的任务其实就是任务0前身了。放到栈里面
	pushl $0x17                        # DS, 任务 0 的数据段选择子入栈。0x17 = 0001 0111 . 右移三位，index=0010 = 2             
	pushl $init_stack                  # SS, 任务 0 的堆栈指针入栈(也可以直接把 ESP 入栈)。
	# 中断返回栈。
	pushfl                             # 标志寄存器入栈
	pushl $0x0c                        # 0x0c = 0000 1001, 右移三位 index = 0x1. TI=1 DPL=00. CS 段选择子
	pushl $task0                       # EIP. eip 的位置。这里配置笔记里的中断返回图看（图4-29）
	iret                               # 自己造个栈然后中断返回。0 任务模拟一个被中断的状态。这里就到 task0 和 它的 ldt 了

# lss 指令的参数. 栈的增长方向是相反的
.align 8
.fill 128, 4, 0
init_stack:                          
	.long init_stack                               # esp
	.word 0x10                                     # 数据段选择子

setup_gdt:
	lgdt lgdt_opcode                   # 使用 6 字节操作数 lgdt_opcode 设置 GDT 表位置和长度。
	ret

setup_idt:                             # 循环设置256个中断统一为为 ignore_int
	lea ignore_int,%edx                # gdt中索引为8的段
	movl $0x00080000,%eax
	movw %dx,%ax		               /* selector = 0x0008 = cs */
	movw $0x8E00,%dx	               /* interrupt gate - dpl=0, present */
	lea idt,%edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx                           # 这条指令执行结果影响AF、OF、PF、SF、ZF标志位
	jne rp_sidt                        # 递减循环。当零标志 Z=0 则跳转； 否则 零标志 Z=1 则顺序执行下一条指令。
	lidt lidt_opcode                   # 使用 6 字节操作数 lidt_opcode 设置 IDT 表位置和长度。
	ret

###########################################################################
#
# 全局数据区域
#
###########################################################################
# 当前运行的任务ID
current: .long 0
# 屏幕写入位置
scr_loc: .long 0

.align 2
lidt_opcode:
	.word 256*8-1		# idt contains 256 entries, 16 位的表界限（注意 -1 操作）
	.long idt		    # This will be rewrite by code. 地址
lgdt_opcode:
	.word (end_gdt-gdt)-1	# so does gdt. 16 位的表界限（注意 -1 操作） 
	.long gdt		        # This will be rewrite by code. 地址

.align 8
idt:	
    .fill 256,8,0		# idt is uninitialized

gdt:	
    .quad 0x0000000000000000	# NULL descriptor 空
	.quad 0x00c09a00000007ff	# 8Mb 0x08, base = 0x00000  代码段            index = 1
	.quad 0x00c09200000007ff	# 8Mb 0x10                  数据段            index = 2
	.quad 0x00c0920b80000002	# screen 0x18 - for display 显示段
	.word 0x0068, tss0, 0x8900, 0x0	# TSS0 descr 0x20 , 预先设置了 gdt 的 tss0. index = 4. word 是 16。 4 * 16。0x89 = 1000 
	.word 0x0040, ldt0, 0x8200, 0x0	# LDT0 descr 0x28 , index = 5.
	.word 0x0068, tss1, 0x8900, 0x0	# TSS1 descr 0x30 , index = 6.
	.word 0x0040, ldt1, 0x8200, 0x0	# LDT1 descr 0x38 , index = 7.
end_gdt:

# 任务管理数据
.align 8
ldt0:	.quad 0x0000000000000000
	.quad 0x00c09a00000003ff	# 0x0f, base = 0x00000, 代码段 0xfa = 1111... 改为 0x9a=1001...
	.quad 0x00c09200000003ff	# 0x17                  # 数据段
tss0:	.long 0 			/* back link */             # TSS 状态段
	.long krn_stk0, 0x10		/* esp0, ss0 */
	.long 0, 0, 0, 0, 0		/* esp1, ss1, esp2, ss2, cr3 */
	.long 0, 0, 0, 0, 0		/* eip, eflags, eax, ecx, edx */
	.long 0, 0, 0, 0, 0		/* ebx esp, ebp, esi, edi */
	.long 0, 0, 0, 0, 0, 0 		/* es, cs, ss, ds, fs, gs */
	.long LDT0_SEL, 0x8000000	/* ldt, trace bitmap */ # 注意这里的 ldt，说明 tss 切换任务会切换 ldt
	# ↑ 7 * .long = 7 * 32 = 224B

# 任务的内核栈，长度为: 128 个 .long。栈空间在上面，因为它是从高到低的。
	.fill 128,4,0
krn_stk0:  

.align 8
ldt1:	.quad 0x0000000000000000
	.quad 0x00c09a00000003ff	# 0x0f, base = 0x00000. 代码段 0xfa = 1111... 改为 0x9a=1001...
	.quad 0x00c09200000003ff	# 0x17

tss1:	.long 0 			/* back link */
	.long krn_stk1, 0x10		/* esp0, ss0 */
	.long 0, 0, 0, 0, 0		/* esp1, ss1, esp2, ss2, cr3 */
	.long task1, 0x200		/* eip, eflags */
	.long 0, 0, 0, 0		/* eax, ecx, edx, ebx */
	.long usr_stk1, 0, 0, 0		/* esp, ebp, esi, edi */
	.long 0x14,0x0c,0x14,0x14,0x14,0x14 /* es, cs, ss, ds, fs, gs */ # cs = 0x0f = 0000 1111 改为 0000 1100 = 0x0c. 0x17 = 0001 0111 改为 0001 0100
	.long LDT1_SEL, 0x8000000	/* ldt, trace bitmap */
# 内核栈和用户栈
	.fill 128,4,0
krn_stk1:
	.fill 128,4,0
usr_stk1:

###########################################################################
#
# 中断函数
#
###########################################################################
.align 2
system_interrupt:
	iret

.align 2
timer_interrupt:
	push %ds                           # 保存用到的寄存器，用完后恢复
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds                       # 设置 数据段 index = 2。似乎没有必要，因为没有函数改过
	movb $0x20, %al                    # 立刻允许其他硬件中断。即向 8259A 发送 EOI 命令
	outb %al, $0x20
	movl $1, %eax                      # 假设当前任务是 1
	cmpl %eax, current                 # 比较如果不是 1 就是跳到 1
	je 1f
	movl %eax, current                 # 是 0 就运行 1
	ljmp $TSS1_SEL, $0                 # 代码段选择子， 偏移 1
	jmp 2f
1:	movl $0, current
	ljmp $TSS0_SEL, $0
2:	popl %eax
	pop %ds
	iret                               # 中断返回

ignore_int:
    iret

###########################################################################
#
# 功能函数
#
###########################################################################
write_char:
	push %gs                           # 保存 gs 和 ebx 到栈
	pushl %ebx
	mov $SCRN_SEL, %ebx                # 显示段选择。这个空间是显示器的空间
	mov %bx, %gs                       # 显示段给 gs
	movl scr_loc, %ebx                 # 从变量 scr_loc 中取目前字符显示位置值
	shl $1, %ebx                       # 因为在屏幕上每个字符还有一个属性字节，因此字符实际显示位置对应的显示内存偏移地址要乘 2
	movb %al, %gs:(%ebx)               # al 中是要显示的字符。把字符放到显示内存后把位置值除 2 加 1，此时位置值对应下一个显示位置。如果该位置大于 2000，则复位成 0
	shr $1, %ebx
	incl %ebx
	cmpl $2000, %ebx                   # 大于 2000 则复位成 0
	jb 1f
	movl $0, %ebx                      # 复位成 0
1:	movl %ebx, scr_loc	               # 下一个显示位置存储
	popl %ebx                          # 恢复 gs ebx
	pop %gs
	ret

print_c:
	movl $0xffff, %ecx                 # 执行循环，延时作用
1:	loop 1b
	push %ds                           # 保存 ds eax, 执行完再恢复
	pushl %eax
	movl $0x10, %eax                   # 0x10 = 0001 0000 右移三位 10 = 2.也就是当前 gdt 的 index=2。数据段，其实默认值也是它呀
	mov %ax, %ds                       # 数据段
	movl $67, %eax                     # print 'C' 其实是在 AL 中, print 'C'
	call write_char
	popl %eax
	pop %ds
	ret

# 输出字符    
task0:
	movl $0xffff, %ecx                 # 执行循环，延时作用
1:	loop 1b
	push %ds                           # 保存 ds eax, 执行完再恢复
	pushl %eax
	movl $0x10, %eax                   # 0x10 = 0001 0000 右移三位 10 = 2.也就是当前 gdt 的 index=2。数据段，其实默认值也是它呀
	mov %ax, %ds                       # 数据段
	movl $67, %eax                     # print 'C' 其实是在 AL 中, print 'C'
	call write_char
	popl %eax
	pop %ds
    jmp task0

task1:
	movl $0xffff, %ecx                 # 执行循环，延时作用
1:	loop 1b
	push %ds                           # 保存 ds eax, 执行完再恢复
	pushl %eax
	movl $0x10, %eax                   # 0x10 = 0001 0000 右移三位 10 = 2.也就是当前 gdt 的 index=2。数据段，其实默认值也是它呀
	mov %ax, %ds                       # 数据段
	movl $68, %eax                     # print 'D' 其实是在 AL 中, print 'D'
	call write_char
	popl %eax
	pop %ds
    jmp task1
