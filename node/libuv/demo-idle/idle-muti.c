#include <stdio.h>
#include <uv.h>

int64_t counter = 0;

void hello_001(uv_idle_t* handle) {
    counter++;
    if (counter >= 10e6) {
        printf("Hello...\n");
        uv_idle_stop(handle);
    }
}

void hello_002(uv_idle_t* handle) {
    counter++;
    if (counter >= 10e6) {
        printf("World...\n");
        uv_idle_stop(handle);
    }
}

int main() {
    uv_idle_t idler001;
    uv_idle_t idler002;

    uv_idle_init(uv_default_loop(), &idler001);
    uv_idle_init(uv_default_loop(), &idler002);
    uv_idle_start(&idler001, hello_001);
    uv_idle_start(&idler002, hello_002);

    printf("Idling...\n");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_loop_close(uv_default_loop());
    return 0;
}
