#include <stdio.h>
#include <uv.h>

int64_t counter = 0;

// 将 handle 停止
void wait_for_a_while(uv_idle_t* handle) {
    counter++;

    // handle 可以被关闭
    // if (counter >= 10e6)
    //     uv_idle_stop(handle);
}

int main() {
    uv_idle_t idler;

    // 初始化一个 idler handle。将其插入到 loop->handle 队列中，且handle和loop关联。
    uv_idle_init(uv_default_loop(), &idler);

    // uv__active_handle_add 将 handle 加入进来
    uv_idle_start(&idler, wait_for_a_while);

    printf("Idling...\n");

    // 启动事件循环
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);


    uv_loop_close(uv_default_loop());
    return 0;
}