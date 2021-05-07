#include <stdio.h>
#include <stdlib.h>
#include "funcs.h"
#include "run.h"
#include "pdp11.h"

void do_halt(word w) {
    printf("\n\n----------halted----------\n");
    print_regs();
    exit(0);
}
void do_mov(word w) {
    b = (w >> 15) & 1;
    ss = get_mr(w >> 6);
    dd = get_mr(w);
    if (dd.type == 0)             //если мода 0,
        reg[dd.adr] = ss.val;     //значение пишу в регистр
    else
        if (b == 0)
            w_write(dd.adr, ss.val);     //по адресу
        else
            b_write(dd.adr, (byte)(ss.val & 0xFF)); //в память
    C = 0;                                //нет переполнения
    Z = (ss.val == 0);                    //результат 0 или не 0
    N = (ss.val >> 15) & 1;               //отр. результат или нет
    trace("\n");
}
void do_add(word w) {
    b = (w >> 15) & 1;
    ss = get_mr(w >> 6);
    dd = get_mr(w);
    if (dd.type == 0)                        //если мода 0,
        reg[dd.adr] = dd.val + ss.val;       //записываю значение в регистр
    else
    if (b == 0)
        w_write(dd.adr, dd.val + ss.val);
    else                                      //иначе по адресу
        b_write(dd.adr, dd.val + ss.val);  //в зависимости от слова/байта

    Z = (dd.val + ss.val == 0);
    N = ((dd.val + ss.val) >> 15) & 1;
    C = ((dd.val + ss.val) >= MEM_SIZE);     //проверяеем на переполнение
    trace("\n");
}

void do_sob(word w) {
    r = (w >> 6) & 7;
    nn = w & 077;                           //111111
    if (--reg[r] != 0)                      // если содержимое регистра при уменьшении на один != 0, то
        pc = pc - 2 * nn;                   // идти по адресу со сдвигом назад в nn
    trace("R%o %06o", r, pc);
    trace("\n");
}

void do_br(word w) {
    xx = (char)(w & 0377);    //8 бит, знаковое
    pc = pc + 2 * xx;
    trace("%06o\n", pc);
}

void do_bpl(word w) {
    if (N == 0)
        do_br(w);
    trace("\n");
}

void do_beq(word w) {
    if (Z == 1)
        do_br(w);
    trace("\n");
}

void do_cmp(word w) {
    b = (w >> 15) & 1;
    ss = get_mr(w >> 6);
    dd = get_mr(w);
    Z = ((ss.val - dd.val) == 0);      //z устанавливается, если результат = 0, иначе сбрасывается
    if (b == 0) {
        N = ((ss.val - dd.val) >> 15) & 1;        //содержимое приёмника-содержимое источника
        C = ((ss.val + 1 + ~dd.val) >= MEM_SIZE); //с устанавливается, если не было переноса из самого
    }                                             //старшего бита результата, иначе сбрасывается
    else {
        N = (((byte)ss.val - (byte)dd.val) >> 7) & 1;    //аналогично для byte
        C = (((byte)ss.val + 1 + ~((byte)dd.val)) >= 256);
    }
    trace("\n");
}

void do_clr(word w) {       //присваивание слову приёмника значение 0
    b = (w >> 15) & 1;
    dd = get_mr(w);
    if (dd.type = 0)
        reg[dd.adr] = 0;
    else
    if (b == 0)
        w_write(dd.adr, 0);
    else
        b_write(dd.adr, 0);

    Z = 1;                  //устанавливается
    N = 0;                  //сброс
    C = 0;                  //сброс
    trace("\n");
}

void do_tst(word w) {
    C = 0;
    b = (w >> 15) & 1;
    dd = get_mr(w);
    if (dd.type == 0) {
        Z = (reg[dd.adr] == 0);                     //устанавливается, при 0
        if (b == 0)
            N = ((reg[dd.adr] >> 15) & 1);         //устанавливается при <0
        else
            N = ((reg[dd.adr] >> 7) & 1);          //иначе сбрасывается
    }
    else {
        if (b == 0) {
            Z = (w_read(dd.adr) == 0);
            N = ((w_read(dd.adr) >> 15) & 1);
        } else {
            Z = (b_read(dd.adr) == 0);
            N = ((b_read(dd.adr) >> 7) & 1);        //byte
        }
    }
    C = 0;                                  //всегда сброс
    trace("\n");
}


void do_nothing(word w) {
    trace("\n");
}