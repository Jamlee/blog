# 试验例子
实现内核级别的进程, TSS 的特权为 DPL=00.

```
# 测试函数
print_c:
// 	movl $0xffff, %ecx                  # 执行循环，延时作用
// 1:	loop 1b
// 	push %ds                           # 保存 ds eax, 执行完再恢复
// 	pushl %eax
// 	movl $0x10, %eax                   # 0x10 = 0001 0000 右移三位 10 = 2.也就是当前 gdt 的 index=2。数据段，其实默认值也是它呀
// 	mov %ax, %ds                       # 数据段
// 	movl $67, %eax                     # print 'C' 其实是在 AL 中, print 'C'
// 	call write_char
// 	popl %eax
// 	pop %ds
	ret
```