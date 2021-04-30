#include "pdp11.h"

typedef struct {
    word mask;
    word opcode;
    char * name;
    void (*do_func)(word w);
} Command;

typedef struct {
    word val; //значение аргумента
    word adr;//адрес аргумента
    byte type;
} Arg;

extern Command cmd[];
extern byte b, r;
extern Arg ss, dd;
extern word nn;
extern char xx;
extern byte N, Z, C;

Arg get_mr(word w);

void run();
void print_regs();
void print_flags();

