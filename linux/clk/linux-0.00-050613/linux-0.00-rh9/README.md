# 试验例子
1. 看下GDT和LDT中特权级，验证任务切换。猜测任务0的代码段、堆栈段还有初始堆栈。任务0 很特殊。
![](./01.png)

jnz: 如果 eflags 中 cf (carry flag)没有被置位。则跳转到对应地址。