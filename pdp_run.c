#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"

word reg[8];
byte b, r;
Arg ss, dd;
word nn;
char xx;
byte N, Z, C;

void print_regs() {
    for (int i= 0; i < 8; i++)
        trace("R%o = %06o\n", i, reg[i]);
    trace("\n");
}

void print_flags() {
    trace("N = %d\n Z= %d\n C= %d\n", N, Z, C);
}

void do_halt() {
    printf("THE END!!!\n");
    print_regs();
    exit(0);
}
void do_mov(word w) {
    b = (w >> 15) & 1;
    ss = get_mr(w >> 6);
    dd = get_mr(w);
    if (dd.type == 0)
        reg[dd.adr] = ss.val;
    else
        if (b == 0)
            w_write(dd.adr, ss.val);
        else
            b_write(dd.adr, (byte)(ss.val & 0xFF));
    C = 0;
    Z = (ss.val == 0);
    N = (ss.val >> 15) & 1;
    trace("\n");
}
void do_add(word w) {
    b = (w >> 15) & 1;
    ss = get_mr(w >> 6);
    dd = get_mr(w);
    if (dd.type == 0)
        reg[dd.adr] = dd.val + ss.val;
    else
        if (b == 0)
            w_write(dd.adr, dd.val + ss.val);
        else
            b_write(dd.adr, dd.val + ss.val);

    Z = (dd.val + ss.val ==0);
    N = ((dd.val + ss.val) >> 15) & 1;
    C = ((dd.val + ss.val) >= MEM_SIZE);
    trace("\n");
}

void sob(word w) {
    r = (w >> 6) & 7;
    nn = w & 63;
    if (--reg[r] != 0)
        pc = pc - 2 * nn;
    trace("R%o %06o", r, pc);
    trace("\n");
}

void br(word w) {
    xx = (char)(w & 0377);
    pc = pc + 2 * xx;
    trace("%06o\n", pc);
}

void do_nothing(word w) {
    trace("\n");
}

typedef struct {
    word mask;
    word opcode;
    char * name;
    void (*do_func)(word w);
} Command;

Command cmd[] = {
        {0170000, 0010000, "mov", do_mov},
        {0170000, 0060000, "add", do_add},
        {0177777, 0000000, "halt", do_halt},
        {0177000, 0077000, "sob", sob},
        {0177400, 0000400, "br", br},
        {0177777, 0177777, "nothing", do_nothing}
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
        int i = 0;
        while(1) {
            if ((w & cmd[i].mask) == cmd[i].opcode) {
                trace("%s ", cmd[i].name);
                cmd[i].do_func(w);
                break;
            }
            i++;
            if ((cmd[i].mask == 0177777) && (cmd[i].opcode == 0177777) ) {
                trace("%s ", cmd[i].name);
                cmd[i].do_func(w);
                break;
            }
        }
        if (trac == 2) {
            print_regs();
            print_flags();
        }
    }
}
