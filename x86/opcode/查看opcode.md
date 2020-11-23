# opcode
opcode的格式能够决定汇编语言的语法。x86 汇编指令大约总共有  586 个。分布如下 P 和 F 开头的指令最多：
```
A: 47
B: 15
C: 78
D: 13
E: 4
F: 157
G: 2
H: 22
I: 27
J: 38
K: 0
L: 25
M: 96
N: 7
O: 19
P: 239
Q: 0
R: 45
S: 95
T: 8
U: 8
V: 12
W: 2
X: 21
Y: 0
Z: 0
```

## Mac 上编译 asm
```
nasm -f macho64 hello_world.asm
ld -macosx_version_min 10.7.0 -o hello_world hello_world.o
```

## 验证指令数含有指示哪列寄存器
```
nasm -f macho64 01.s
ld -macosx_version_min 10.7.0 -o test 01.o
```
[](./01.jpg)

## 文件
- 02.s 测试 mac 编译 asm