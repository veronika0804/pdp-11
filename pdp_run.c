#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"
#include "run.h"
#include "funcs.h"

 byte b, r;
Arg ss, dd;
word nn;
char xx;
byte N, Z, C;

void value(word* val1, word adr1, byte b) {
    if (b == 0)
        *val1 = w_read(adr1);
    else
        *val1 = ((b_read(adr1) >> 7) * 0177400 | b_read(adr1));
}

void print_regs() {
    for (int i = 0; i < 8; i++)
        trace("R%o = %06o\n", i, reg[i]);
    trace("\n");
}

void print_flags() {
    trace("N = %d\n Z= %d\n C= %d\n", N, Z, C);
}


Arg get_mr(word w) {
    Arg res;
    int r = w & 7; //номер регистра
    int mode = (w >> 3) & 7; //номер моды

    switch(mode) {
        case 0:     // R3
            res.type = 0;
            res.adr = r;
            res.val = reg[r];
            trace("R%o ", r);
            break;
        case 1:     // (R3)
            res.type = 1;
            res.adr = reg[r];
            value(&(res.val), res.adr, b);   // b_read
            trace("(R%o) ", r);
            break;
        case 2:               // (R3)+   #3
            res.type = 1;
            if (r == 7) {
                res.adr = reg[r];
                res.val = w_read(res.adr);   // b_read
                reg[r] += 2;
                trace("#%06o ", res.val);
            }
            else if (r == 6) {
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
        case 3:
            res.type = 1;
            res.adr = w_read(reg[r]);
            value(&(res.val), res.adr, b);
            reg[r] += 2;
            if (r == 7)
                trace("@#%o", res.adr);
            else
                trace("@(R%o)+ ", r);
            break;
        case 4:
            res.type = 1;
            if (r == 7 || r == 6 || b == 0) {
                reg[r] -= 2;
                res.adr = reg[r];
                res.val = w_read(res.adr);
            }
            else {
                reg[r]--;
                res.adr = reg[r];
                res.val = b_read(res.adr);
            }
            trace("-(R%d)", r);
            break;

        case 5:
            res.type = 1;
            trace ("@-(R%o)", r);
            reg[r] -= 2;
            res.adr = w_read(reg[r]);
            res.val = w_read (res.adr);
            break;
        case 6:
            res.type = 1;
            pc += 2;
            res.adr = reg[r] + w_read(pc - 2);
            value(&(res.val), res.adr, b);
            if (r == 7)
                trace("%06o", res.adr);
            else
                trace("%o(R%o)", w_read(pc - 2), r);
            break;
        case 7:
            res.type = 1;
            pc += 2;
            res.adr = reg[r] + w_read(pc - 2);
            res.adr = w_read(res.adr);
            value(&(res.val), res.adr, b);
            if (r == 7)
                trace("@%06o", reg[r] + w_read(pc - 2));
            else
                trace("@%o(R%o)", w_read(pc - 2), r);
            break;
        default:
            fprintf(stderr,
                    "Mode %o NOT IMPLEMENTED YET!\n", mode);
            exit(1);
    }

    return res;
}


Command cmd[] = {
        {0070000, 0010000, "movb", do_mov},
        {0170000, 0060000, "add", do_add},
        {0177777, 0000000, "halt", do_halt},
        {0177000, 0077000, "sob", do_sob},
        {0177400, 0000400, "br", do_br},
        {0070000, 0020000, "cmpb", do_cmp},
        {0177400, 0001400, "beq", do_beq},
        {0177400, 0100000, "bpl", do_bpl},
        {0177700, 0005000, "clrb", do_clr},
        {0077700, 0005700, "TSTb", do_tst},
        {0177000, 0004000, "JSR", do_jsr},
        {0177770, 0000200, "RTS", do_rts},
        {0177777, 0177777, "nothing", do_nothing}
};


void run() {
    pc = 01000;
    w_write(OSTAT, 0177777);
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

