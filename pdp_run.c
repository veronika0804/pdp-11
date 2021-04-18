#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"

word reg[8];
byte b;
Arg ss, dd;

void do_halt() {
    printf("THE END!!!\n");
    exit(0);
}
void do_mov() {
    w_write(dd.adr, ss.val);
}
void do_add();
void do_nothing();

typedef struct {
    word mask;
    word opcode;
    char * name;
    void (*do_func)(void);
} Command;
Command cmd[] = {
        {0170000, 0010000, "mov", do_mov},
        {0170000, 0060000, "add", do_add},
        {0177777, 0000000, "halt", do_halt},
};

Arg get_mr(word w) {
    Arg res;
    int r = w & 7; //номер регистра
    int mode = (w >> 3) & 7; //номер моды
    switch(mode) {
        case 0:     // R3
            res.adr = r;
            res.val = reg[r];
            res.type = 0;
            trace("R%o ", r);
            break;
        case 1:     // (R3)
            res.type = 1;
            res.adr = reg[r];
            res.val = b ? b_read(res.adr) : w_read(res.adr);   // b_read
            trace("(R%o) ", r);
            break;
        case 2:               // (R3)+   #3
            res.type = 1;
            if (r == 7) {
                res.adr = reg[r];
                res.val = w_read(res.adr);   // b_read
                reg[r] += 2;
                trace("#%o ", res.val);
            }
            else if ( r == 6) {
                res.adr = reg[r];
                res.val = w_read(res.adr);   // b_read
                reg[r] += 2;
                trace("(SP)+ ");
            }
            else {
                res.adr = reg[r];
                if (b == 0) {
                    res.val = w_read(res.adr);
                    reg[r] += 2;
                } else {
                    res.val = ((b_read(res.adr) >> 7) * 0177400) | b_read(res.adr);
                    reg[r] += 1;
                }
                trace("(R%o)+ ", r);
            }
            break;
        default:
            fprintf(stderr,
                    "Mode %o NOT IMPLEMENTED YET!\n", mode);
            exit(1);
    }

    return res;
}
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
            ss = get_mr();
            dd = get_mr(w);
            do_mov();
        }
    }
}