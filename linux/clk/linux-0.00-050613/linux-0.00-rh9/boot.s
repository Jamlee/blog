!	boot.s
!
! It then loads the system at 0x10000, using BIOS interrupts. Thereafter
! it disables all interrupts, changes to protected mode, and calls the 

BOOTSEG = 0x07c0
SYSSEG  = 0x1000			! system loaded at 0x10000 (65536).
SYSLEN  = 17				! sectors occupied. 需要装配的扇区的长度。硬盘ZDR技术处理外部磁道扇区多的问题。

# as86 https://manpages.debian.org/jessie/bin86/as86.1.en.html

entry start
start:
	jmpi	go,#BOOTSEG
go:	mov	ax,cs
	mov	ds,ax
	mov	ss,ax  # ds 和 ss 是 0x07c0
	mov	sp,#0x400		! arbitrary value >>512

# 利用 BIOS 中断 int 0x13 功能 2 从启动盘读取 head 代码
# DH - 磁头号;DL - 驱动器号;CH - 10位磁道号低8位
# CL- 位7、6是磁道号高2位，位5-0起始扇区号(从1计)。
# AH - 读扇区功能号; AL - 需读的扇区数(17)。
! ok, we've written the message, now
load_system:
	mov	dx,#0x0000 # 0 号磁头 0 号驱动器
	mov	cx,#0x0002 # 15-6 是磁道号， 5-0 是 扇区号，扇区号为 2（从1计数）。cx 和 dx 共同搞定分区定位
	mov	ax,#SYSSEG # 目标地址 0x1000，不直接加载到 0x0 是因为 bios 在那里有中断。加载磁盘是需要bios中断的。
	mov	es,ax
	xor	bx,bx # bx 全部置位 0
	mov	ax,#0x200+SYSLEN  # 功能号 和 17 个扇区
	# 调用 biso 中断， https://en.wikipedia.org/wiki/BIOS_interrupt_call
	int 	0x13
	jnc	ok_load # 中断执行没问题就跳转到 ok_load 或者到 下一句死循环
die:	jmp	die

! now we want to move to protected mode ...
ok_load:
	cli			! no interrupts allowed ! # 关闭中断
	mov	ax, #SYSSEG
	mov	ds, ax
	xor	ax, ax
	mov	es, ax
	mov	cx, #0x2000
	sub	si,si
	sub	di,di
	rep
	movw # 内存中的 0x1000 移到 0x0，不再需要 bios 中断了
	mov	ax, #BOOTSEG
	mov	ds, ax # ds 设置到 0x07c0
	lidt	idt_48		! load idt with 0,0
	lgdt	gdt_48		! load gdt with whatever appropriate

! absolute address 0x00000, in 32-bit protected mode.
	mov	ax,#0x0001	! protected mode (PE) bit
	lmsw	ax		! This is it! # https://blog.csdn.net/epluguo/article/details/9260429, 切换保护模式
	jmpi	0,8		! jmp offset 0 of segment 8 (cs) # 最终切换到head.s的那一句，! 注意此时段值已是段选择符。该段的线性基地址是 0。
	# 8 = 0x1000 , index = 1, https://blog.csdn.net/qq_35733751/article/details/91331552#3.%20%E6%AE%B5%E9%80%89%E6%8B%A9%E5%AD%90

# 这里是设置的 gdt， 默认在 0x7c00+gdt 的位置。里面有三个段 基地址都是 0x00000
# 一个 .word 是 2个字节。段的限制长度是 8mb。
gdt:	.word	0,0,0,0		! dummy
	# https://www.cnblogs.com/cdwodm/archive/2012/12/14/2818477.html
	# 抽象出，当段限长为n的时候，段的长度是(n+1)个长度单位。
	.word	0x07FF		! 8Mb - limit=2047 (2048*4096=8Mb)  0x07ff =  2047。4096 是颗粒度。定位后颗粒度里面再读取
	.word	0x0000		! base address=0x00000
	.word	0x9A00		! code read/exec
	.word	0x00C0		! granularity=4096, 386

	.word	0x07FF		! 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		! base address=0x00000
	.word	0x9200		! data read/write
	.word	0x00C0		! granularity=4096, 386

idt_48: .word	0		! idt limit=0
	.word	0,0		! idt base=0L
gdt_48: .word	0x7ff		! gdt limit=2048, 256 GDT entries
	.word	0x7c00+gdt,0	! gdt base = 07xxx
.org 510
	.word   0xAA55 #  # 扇区结尾的魔术

