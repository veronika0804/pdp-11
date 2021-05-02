#include <stdio.h>
#include <stdlib.h>
#include "funcs.h"
#include "run.h"
#include "pdp11.h"


void do_halt(word w) {
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

void do_sob(word w) {
    r = (w >> 6) & 7;
    nn = w & 63;
    if (--reg[r] != 0)
        pc = pc - 2 * nn;
    trace("R%o %06o", r, pc);
    trace("\n");
}

void do_br(word w) {
    xx = (char)(w & 0377);
    pc = pc + 2 * xx;
    trace("%06o\n", pc);
}

void do_bpl(word w) {
    if (N == 0)
        do_br(w);
    else
        trace("FALSE\n");
}

void do_beq(word w) {
    if (Z == 1)
        do_br(w);
    else
        trace("FALSE\n");
}

void do_cmp(word w) {
    b = (w >> 15) & 1;
    ss = get_mr(w >> 6);
    dd = get_mr(w);
    Z = ((ss.val - dd.val) == 0);
    if (b == 0) {
        N = ((ss.val - dd.val) >> 15) & 1;
        C = ((ss.val + 1 + ~dd.val) >= MEM_SIZE);
    }
    else {
        N = (((byte)ss.val - (byte)dd.val) >> 7) & 1;
        C = (((byte)ss.val + 1 + ~((byte)dd.val)) >= 256);
    }
    trace("\n");
}

void do_clr(word w) {
    b = (w >> 15) & 1;
    dd = get_mr(w);
    if (dd.type = 0)
        reg[dd.adr] = 0;
    else
    if (b == 0)
        w_write(dd.adr, 0);
    else
        b_write(dd.adr, 0);

    Z = 1;
    N = 0;
    C = 0;
    trace("\n");
}

void do_tst(word w) {
    C = 0;
    b = (w >> 15) & 1;
    dd = get_mr(w);
    if (dd.type == 0) {
        Z = (reg[dd.adr] == 0);
        if (b == 0)
            N = ((reg[dd.adr] >> 15) & 1);
        else
            N = ((reg[dd.adr] >> 7) & 1);
    } else {
        if (b == 0) {
            Z = (w_read(dd.adr) == 0);
            N = ((w_read(dd.adr) >> 15) & 1);
        } else {
            Z = (b_read(dd.adr) == 0);
            N = ((b_read(dd.adr) >> 7) & 1);
        }
    }
    C = 0;
    trace("\n");
}

void do_nothing(word w) {
    trace("\n");
}