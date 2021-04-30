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

void do_nothing(word w) {
    trace("\n");
}