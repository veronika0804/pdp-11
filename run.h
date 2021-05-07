#pragma once
#include "pdp11.h"

typedef struct {
    word mask;
    word opcode;
    char * name;
    void (*do_func)(word w);
} Command;

typedef struct { //аргумент ss dd
    word val; //значение аргумента  mem[adr]
    word adr;//адрес аргумента  reg[n]
    byte type; //в регистре или памяти
} Arg;

extern Command cmd[];
extern byte b, r;
extern Arg ss, dd;
extern word nn;
extern char xx;
extern byte N, Z, C;  //N = 1, если результат команды <0, иначе 0 (знаковый бит)
              //Z = 1, если результат команды 0, иначе = 0
              //C отвечает за знаковое переполнение(если после операции из '+'числа получилось '-'число)

Arg get_mr(word w);

void run();
void print_regs();
void print_flags();

