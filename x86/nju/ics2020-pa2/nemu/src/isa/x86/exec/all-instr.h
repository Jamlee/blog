#include "../local-include/rtl.h"

#include "arith.h"
#include "control.h"
#include "data-mov.h"
#include "logic.h"
#include "string.h"
#include "system.h"

def_EHelper(nop);
def_EHelper(inv);
def_EHelper(nemu_trap);

// 定义额外的指令
def_EHelper(call);			// control.c
def_EHelper(push); 		//data-mov.c
def_EHelper(sub); 			//arith.c
def_EHelper(xor); 			//logic.c
def_EHelper(pop);			//data-mov.c
def_EHelper(ret);  		//control.c