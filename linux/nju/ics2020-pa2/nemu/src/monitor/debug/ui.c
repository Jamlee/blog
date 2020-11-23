#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include "memory/vaddr.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>


#define CLOSE "\033[0m"                 // 关闭所有属性
#define BLOD  "\033[1m"                 // 强调、加粗、高亮
#define BEGIN(x,y) "\033["#x";"#y"m"    // x: 背景，y: 前景

// 执行几个指令
void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline(BEGIN(31, 33)"nemu$ "CLOSE);

  if (line_read && *line_read) {
    // 单独的使用readline()并没有上下键切换补全的功能，实现这个需要用到另一个函数 - add_history()

    add_history(line_read);
  }

  return line_read;
}

// 指令命令，-1代表n多条指令
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_help(char *args);

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  if (args == NULL) {
    cpu_exec(1);
  } else {
    cpu_exec(atoi(args));
  }
  return 0;
}

static int cmd_info(char *args) {
   if (strcmp("w", args) == 0 ) {
      printf("Unknown info sub command\n");
   } else if (strcmp("r", args) == 0) {
      printf("eax: %#010x ebx: %#010x ecx: %#010x edx: %#010x edi: %#010x esi: %#010x  esp: %#010x ebp: %#010x \n", 
      cpu.eax, cpu.ebx, cpu.ecx, cpu.edx, cpu.edi, cpu.esi, cpu.esp, cpu.ebp);
   } else {
      printf("Unknown info sub command\n");
   }
  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("must have expr arg\n");
  } else {
    bool success = false;
    int result;
    result = expr(args, &success);
    printf("your expr is %s, result %d\n", args, result);
  }
  return 0;
}

static int cmd_x(char *args) {
  char *arg1=strtok(NULL," ");  
  char *arg2=strtok(NULL," ");  
  int len;  
  vaddr_t address;  
    
  sscanf(arg1,"%d",&len);  
  sscanf(arg2,"%x",&address);  
    
  printf("0x%x:",address);  
  for(int i=0;i<len;i++){  
      printf("%x ",vaddr_read(address,4));  
      address+=4;  
  }  
  printf("\n");  
  return 0;  
}

static int cmd_w(char *args) {
  return -1;
}

static int cmd_d(char *args) {
  return -1;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "打印命令的帮助信息", cmd_help },
  { "c", "继续运行被暂停的程序", cmd_c },
  { "q", "退出NEMU", cmd_q },
  { "si", "让程序单步执行N条指令后暂停执行,当N没有给出时, 缺省为1", cmd_si },
  { "info", "打印寄存器状态;打印监视点信息", cmd_info },
  { "p", "求出表达式EXPR的值, EXPR支持的,运算请见调试中的表达式求值小节", cmd_p },
  { "x", "求出表达式EXPR的值, 将结果作为起始内存地址, 以十六进制形式输出连续的N个4字节", cmd_x },
  { "w", "当表达式EXPR的值发生变化时, 暂停程序执行", cmd_w },
  { "d", "删除序号为N的监视点", cmd_d },

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%-6s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%-6s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  // 永远循环取数据
  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
