package main

import (
	"fmt"
	"net"
	"log"
)

func process(conn net.Conn) {
	defer conn.Close()
	//
	for {

	}
}

func main() {
	listen, err := net.Listen("tcp", "127.0.0.1:20000")
	if err != nil {
		log.Fatal(err)
	}
	// 循环
	for {
		// 每次建立链接时。返回一个 conn
		conn, err := listen.Accept() // 建立连接
		if err != nil {
			fmt.Println("accept failed, err:", err)
			continue
		}
		//  不要阻塞后续的链接
		go process(conn)
	}
}
