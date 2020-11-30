# x11 原理
## x11 是什么
X窗口系统（X Window System，也常称为X11或X，天窗口系统）是一种以位图方式显示的软件窗口系统。最初是1984年麻省理工学院的研究，之后变成UNIX、类UNIX、以及OpenVMS等操作系统所一致适用的标准化软件工具包及显示架构的运作协议。X窗口系统通过软件工具及架构协议来创建操作系统所用的图形用户界面，此后则逐渐扩展适用到各形各色的其他操作系统上。现在几乎所有的操作系统都能支持与使用X。更重要的是，今日知名的桌面环境——GNOME和KDE也都是以X窗口系统为基础建构成的。

x11 转发是转发xlib调用的接口内容到 xserver. 以前有一种 xterml （x 终端机）仅仅装 xserver的。

## 验证实验
- 实验一: 转发一个画方形的程序

### 实验一
转发一个画方形的程序。参考: https://github.com/QMonkey/Xlib-demo/blob/master/src/simple-drawing.c

```
sudo apt install libx11-dev
gcc -lX11 simple-drawing.c -o simple-drawing

# ssh 用户是谁，就要在谁下面执行图形化程序
ssh -X -vvv  ics@127.0.0.1 -p 2223
X11Forwarding yes
X11DisplayOffset 10

# 下面这个不需要的。直接 localhost 就好了 因为 x11 会转发的
# export DISPLAY=192.168.131.203:10.0
```
仅仅装了 libx11-dev 就有下面的效果 
![效果图](./x11-02.jpg)

调错图  
![调错图](./x11-01.jpg)

https://www.cyberciti.biz/faq/x11-connection-rejected-because-of-wrong-authentication/

xquartz 要用 `2.7.8`. 新版本 `2.7.11`不能使用。
https://unix.stackexchange.com/questions/497967/how-can-i-enable-graphics-on-local-macos-from-remote-linux-using-ssh

运行图  
![运行图](./x11-03.jpg)

mac docker 播放声音
```
# mac 安装 pulseaudio 服务端
brew install pulseaudio
brew services start  pulseaudio
brew services restart  pulseaudio
# 或用于调试
pulseaudio --load=module-native-protocol-tcp --exit-idle-time=-1 -v

# 容器内部安装 pulseaudio 客户端
apt install vim pulseaudio
# /usr/local/Cellar/pulseaudio/13.0_1/etc/pulse/default.pa
# 默认的播放设备有误，所以播放不了， https://wiki.archlinux.org/index.php/PulseAudio/Examples#Set_the_default_output_sink
pacmd list-sources | grep -e 'index:' -e device.string -e 'name:'
set-default-sink Channel_1__Channel_2.5
```

修改服务启动参数
```
# /usr/local/Cellar/pulseaudio/13.0_1/homebrew.mxcl.pulseaudio.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>homebrew.mxcl.pulseaudio</string>
  <key>ProgramArguments</key>
  <array>
    <string>/usr/local/opt/pulseaudio/bin/pulseaudio</string>
    <string>--load=module-native-protocol-tcp</string>
    <string>--exit-idle-time=-1</string>
    <string>--verbose</string>
  </array>
  <key>RunAtLoad</key>
  <true/>
  <key>KeepAlive</key>
  <true/>
  <key>StandardErrorPath</key>
  <string>/usr/local/var/log/pulseaudio.log</string>
  <key>StandardOutPath</key>
  <string>/usr/local/var/log/pulseaudio.log</string>
</dict>
</plist>
```

声音配置图  
![声音配置图](./x11-04.jpg)