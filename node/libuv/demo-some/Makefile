demo01:
	gcc -Iinclude libs/libuv.a -framework CoreFoundation -framework CoreServices -o demo01 demo01.c

demo02:
	gcc -Iinclude libs/libuv.a -framework CoreFoundation -framework CoreServices -o demo02 demo02.c

libuv_socket:
	gcc -Iinclude libs/libuv.a -framework CoreFoundation -framework CoreServices -o libuv_socket libuv_socket.c


socket:
	gcc  -o socket socket.c

kqueue:
	gcc  -o kqueue kqueue.c
	
clean:
	rm -f demo01