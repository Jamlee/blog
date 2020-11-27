void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  // 显示 monitor （类似于串口连接的界面）
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  // init_monitor 中加载完毕了镜像。在这里开始取指、译码、执行。engine 是通过解释执行的方式执行
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
