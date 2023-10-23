#ifndef SPU_H
#define SPU_H

#include "/Users/bulatmotygullin/Documents/Processor/Processor/stack/stack.h"
#include "/Users/bulatmotygullin/Documents/Processor/Processor/buffer_process/buffer_process.h"
#include "../assembly/logging.h"

const size_t amount_of_regs = 4;

struct SPU
{
	Stack stk;
	elem_t regs[amount_of_regs];
	Buf_w_carriage_n_len byte_code;
};

enum Registers
{
	RAX = 1,
	RBX = 2,
	RCX = 3,
	RDX = 4,
};


// a) static lib
//
// 1.
// stack_1.o
// stack_2.o
//
// -> libstack.a
//
// 2.
// g++ main.o libstack.a -o main.exe
//
// b) dynamic lib
//
// с) header only lib
// проблема нескольких инкклюдов

//объектники без маейна и хедеры

#endif
