#include <common.h>
#include <stdarg.h>

FILE *log_fp = NULL;

void init_log(const char *log_file) {
  if (log_file == NULL) return;
  log_fp = fopen(log_file, "w");
  Assert(log_fp, "Can not open '%s'", log_file);
}

char log_bytebuf[80] = {};
char log_asmbuf[80] = {};
static char tempbuf[256] = {};

// 字符串追加， strcatf(log_bytebuf, "%02x ", p_instr[i]); 格式化后的字符串向目标字符串添加
void strcatf(char *buf, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tempbuf, sizeof(tempbuf), fmt, ap);
  va_end(ap);
  // C 库函数 char *strcat(char *dest, const char *src) 把 src 所指向的字符串追加到 dest 所指向的字符串的结尾。
  strcat(buf, tempbuf);
}

// 每次执行一条指令，就把当前指令的汇编都显示出来
void asm_print(vaddr_t this_pc, int instr_len, bool print_flag) {
  snprintf(tempbuf, sizeof(tempbuf), FMT_WORD ":   %s%*.s%s", this_pc, log_bytebuf,
      50 - (12 + 3 * instr_len), "", log_asmbuf);
  log_write("%s\n", tempbuf);
  if (print_flag) {
    puts(tempbuf);
  }

  log_bytebuf[0] = '\0';
  log_asmbuf[0] = '\0';
}
