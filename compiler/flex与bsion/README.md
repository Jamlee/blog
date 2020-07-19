# 编译器

测试有关编译原理的内容：Flex(2.5.x)、Bison(2.4) 等

## 安装 Flex 与 Bsion
```
# https://stackoverflow.com/questions/31805431/how-to-install-bison-on-mac-osx
# mac 中的 bison 默认的版本是 2.3 的
brew link bison --force
echo 'export PATH="/usr/local/opt/bison/bin:$PATH"' >> ~/.zshrc
```