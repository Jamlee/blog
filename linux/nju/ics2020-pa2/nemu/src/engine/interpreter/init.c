void ui_mainloop();
void init_device();

void engine_start() {
  // 开启了 ioe 才会有这个内容。
  /* Initialize devices. */
  init_device();

  /* Receive commands from user. */
  ui_mainloop();
}
