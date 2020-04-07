# 配置所有的测试子目录
SUBDIRS= \
	libevent/src/third-party/libevent-2.0.22-stable \
	c/declaration \
	epoll \
	c++/c++11 \
	c++/c++14

define cleanup
	@cd $(1); make clean
endef

clean:
	$(foreach subDir, $(SUBDIRS), $(call cleanup, $(subDir))) 

.PHONY: clean