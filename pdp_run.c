#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"

word reg[8];

void do_halt() {
    printf("THE END!!!\n");
    exit(0);
}
void do_mov();
void do_add();
void do_nothing();

typedef struct {
    word mask;
    word opcode;
    char * name;
    void (do_func)(void);
} Command;
Command cmd[] = {
        {0170000, 0010000, "mov", do_mov},
        {0170000, 0060000, "add", do_add},
        {0177777, 0000000, "halt", do_halt},
};
void run() {
    pc = 01000;
    while(1) {
        word w = w_read(pc);
        trace("%06o %06o: ", pc, w);
        pc += 2;
        if (w == 0) {
            trace("halt ");
            do_halt();
        }
        else if ((w & 0170000) == 0010000) { //01SSDD
            trace("mov ");
            do_mov();
        }
    }
}