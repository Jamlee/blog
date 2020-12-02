# 内核完全剖析 试验例子

linux-0.00-050613 是简易内核。用于验证GDT，IDT，LDT提供出来的 任务切换、异常处理能力

- linux-0.00-050613。 i386 任务管理试验
- linux-0.12。 linux-0.12 源码
- linux-0.12-080324。

## 磁盘

有了扇区（sector），有了柱面（cylinder），有了 磁头（head），显然可以定位数据了，这就是数据定位(寻址)方式之一，CHS（也称3D），对早期的磁盘（上图所示）非常有效，知道用哪个 磁头，读取哪个柱面上的第几扇区就OK了。CHS模式支持的硬盘容量有限，用8bit来存储 磁头地址，用10bit来存储柱面地址，用6bit来存储扇区地址，而一个扇区共有512Byte，这样使用CHS寻址一块硬盘最大容量为256 * 1024 * 63 * 512B = 8064 MB(1MB = 1048576B)（若按1MB=1000000B来算就是8.4GB）

```
磁头数 × 磁道(柱面)数 × 每道扇区数 × 每扇区字节数
```

- 磁头（head）数：每个盘片一般有上下两面，分别对应1个 磁头，共2个 磁头；
- 磁道（track）数：磁道是从盘片外圈往内圈编号0磁道，1磁道...，靠近主轴的同心圆用于停靠 磁头，不存储数据；
- 柱面（cylinder）数：同磁道数量；
- 扇区（sector）数：每个磁道都别切分成很多扇形区域，每道的扇区数量相同；
- 圆盘（platter）数：就是盘片的数量。

[mbr分区结构](./mbr.png)

逻辑扇区号LBA的公式：

```
LBA(逻辑扇区号)=磁头数 × 每磁道扇区数 × 当前所在柱面号 + 每磁道扇区数 × 当前所在磁头号 + 当前所在扇区号 – 1
```

例如：CHS=0/0/1，则根据公式LBA=255 × 63 × 0 + 63 × 0 + 1 – 1= 0

[gpt分区结构](./gpt.png)

## ioport
http://bochs.sourceforge.net/techspec/PORTS.LST
https://stackoverflow.com/questions/14194798/is-there-a-specification-of-x86-i-o-port-assignment
[ioport结构](./ioport.png)

[ioport spec](./ioport-spec.png)

## 资料
硬盘LBA 和CHS的关系 https://blog.csdn.net/zztan/article/details/70861021

8086-insformat: 8086也有其指令结构的
8086-opcodes: 汇编快速查询