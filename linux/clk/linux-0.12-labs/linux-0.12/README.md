2020年10月份阅读
2021年4月份阅读

## 新版gcc编译代码修改
```
sed -i 's/align 2/align 4/g' boot/head.s
sed -i 's/align 3/align 8/g' boot/head.s
```
放弃了， https://www.cnblogs.com/welhzh/p/4521196.html。 东西改的太多


使用泰晓科技的docker环境：
```
cd /home/jamlee/Work/cloud-lab/labs/linux-0.11-lab
mkdir 0.12
mount -o bind /home/jamlee/Work/blog/linux/clk/linux-0.12-labs/linux-0.12 0.12

cd /home/jamlee/Work/cloud-lab
tools/docker/run linux-0.11-lab
```