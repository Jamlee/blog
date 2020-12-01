# 显示段。段选择子
SCRN_SEL	= 0x18
# 系统段。段选择子
TSS0_SEL	= 0x20  # 0x20 = 0010 0000 右移三位 index= 0x100 = 4, DPL=0 
LDT0_SEL	= 0x28  # 0x20 = 0010 1000 右移三位 index= 0x101 = 5, DPL=0

.global startup_32
.text
startup_32:                                         # 代码被boot.s移动到 0x0。一开始加载到 0x1000, 是因为需要调用bios预设值的中断
	movl $0x10, %eax                                # 数据段选择符，0x10 = 00010000。index=10 也就是2
	mov %ax, %ds                                    # 数据段
	lss init_stack, %esp                            # 书上的代码是AT&T汇编,所以应理解为init_stack的低16位传入esp,高16位传入ss.
	
    # 重新设置 setup_idt, setup_gdt
    call setup_idt
	call setup_gdt

    # 初始化所有段寄存器
    movl $0x10, %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	lss init_stack, %esp

# 输出字符    
task0:
    call print_c
    jmp task0

# lss 指令的参数
init_stack:                          
	.long init_stack                               # esp
	.word 0x10                                     # ds

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
current: .long 0
scr_loc: .long 0

.align 2
lidt_opcode:
	.word 256*8-1		# idt contains 256 entries, 16 位的表界限（注意 -1 操作）
	.long idt		# This will be rewrite by code. 地址
lgdt_opcode:
	.word (end_gdt-gdt)-1	# so does gdt. 16 位的表界限（注意 -1 操作） 
	.long gdt		# This will be rewrite by code. 地址

.align 8
idt:	
    .fill 256,8,0		# idt is uninitialized

gdt:	
    .quad 0x0000000000000000	# NULL descriptor 空
	.quad 0x00c09a00000007ff	# 8Mb 0x08, base = 0x00000  代码段            index = 1
	.quad 0x00c09200000007ff	# 8Mb 0x10                  数据段            index = 2
	.quad 0x00c0920b80000002	# screen 0x18 - for display 显示段
	.word 0x0068, tss0, 0xe900, 0x0	# TSS0 descr 0x20 # 预先设置了 gdt 的 tss0. index = 4 . word 是 16。 4 * 16。0xe9 = 1110 改为 1000 
	.word 0x0040, ldt0, 0xe200, 0x0	# LDT0 descr 0x28 # index = 5
end_gdt:
	.fill 128, 4, 0

###########################################################################
#
# 任务数据区域
#
###########################################################################
.align 8
ldt0:	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff	# 0x0f, base = 0x00000  # 代码段 0xfa = 1111... 改为 0x9a=1001...
	.quad 0x00c0f200000003ff	# 0x17                  # 数据段
tss0:	.long 0 			/* back link */             # TSS 状态段
	.long krn_stk0, 0x10		/* esp0, ss0 */
	.long 0, 0, 0, 0, 0		/* esp1, ss1, esp2, ss2, cr3 */
	.long 0, 0, 0, 0, 0		/* eip, eflags, eax, ecx, edx */
	.long 0, 0, 0, 0, 0		/* ebx esp, ebp, esi, edi */
	.long 0, 0, 0, 0, 0, 0 		/* es, cs, ss, ds, fs, gs */
	.long LDT0_SEL, 0x8000000	/* ldt, trace bitmap */ # 注意这里的 ldt，说明 tss 切换任务会切换 ldt
	# ↑ 7 * .long = 7 * 32 = 224B
    .fill 128,4,0
krn_stk0:  # 任务的内核栈，长度为: 128 个 .long。栈空间在上面，因为它是从高到低的。

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

# 空的中断返回
ignore_int:
    // iret                            # 为什么这个指令会有问题

# call
print_c:
	movl $0xffff, %ecx                  # 执行循环，延时作用
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
