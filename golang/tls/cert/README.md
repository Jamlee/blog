# 生成 ca 根证书

MacOS 使用 brew 安装的 openssl
```
brew install openssl
export PATH="/usr/local/opt/openssl@1.1/bin:$PATH"  

mkdir demoCA
mkdir demoCA/csr demoCA/private demoCA/jks demoCA/newcerts
touch demoCA/index.txt
echo 03 > ./demoCA/serial
```
cd gen_ca_root
openssl genrsa -out ca.key 2048
# 生成一个 csr 用于给 key 来签发
openssl req -new -key ca.key -out ca.csr
# 签发一个 ca 证书
openssl x509 -req -days 365 -in ca.csr -signkey ca.key -out ca.crt
```

```
# 签发一个服务器证书，并验证
openssl genrsa -des3 -out server.key 1024
openssl req -new -key server.key -out server.csr    
openssl ca -in server.csr -out server.crt -cert ca.crt -keyfile ca.key
openssl verify -CAfile ca.crt server.crt
```