#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "pdp11.h"
#include "run.h"

byte mem[MEM_SIZE];
word reg[8];
byte trac;


void trace(const char* format, ...) {
    if(trac == 1 || trac == 2) {
        va_list ap;
        va_start(ap, format);
        vprintf(format, ap);
        va_end(ap);

    }
}

void test_mem() {
    //пишем байт, читаем байт
    byte b1 = 0x0a;
    b_write(0, b1);
    byte bres = b_read(0);
    trace("%02hhx = %02hhx\n", b1, bres);
    assert(bres == b1);

    //пишем слово, читаем слово
    word w1 = 0x0a0b;
    w_write(0, w1);
    word wres = w_read(0);
    trace("%04hx = %04hx\n", w1, wres);
    assert(wres == w1);

    //пишем 2 байта, читаем слово
    b1 = 0x0a;
    byte b2 = 0x0b;
    b_write(0, b1);
    b_write(1, b2);
    wres = w_read(0);
    trace("%02hhx%02hhx = %04hx\n", b2, b1, wres);
    assert(wres == 0x0b0a);

    //пишем слово, читаем 2 байта
    w1 = 0x0b0a;
    w_write(0, w1);
    byte bres1 = b_read(0);
    byte bres2 = b_read(1);
    trace("%02hhx = 0a\n", bres1);
    trace("%02hhx = 0b\n", bres2);
    assert(bres1 == 0x0a);
    assert(bres2 == 0x0b);
}

word w_read(Adress adr) {              //читаем слово по адресу adr
    word res = ((word)mem[adr+1]) << 8;
    //printf("w = %x\n", w);
    res = res | mem[adr];
    return res;
}
void b_write(Adress adr, byte b) {   //пишем байт b по адресу adr
    mem[adr] = b;
    if (adr == ODATA)
        printf("%c", b);
}
byte b_read(Adress adr) {            //читаем байт по адресу adr
    return mem[adr];
}

void w_write(Adress adr, word w) {   //пишем слово по адресу adr
    mem[adr] = w & 0x00FF;
    mem[adr + 1] = (w >> 8) & 0x00FF;
    if (adr == ODATA)
        printf("%c", w);
}

void load_file(const char* filename) {
    Adress a, n;
    byte b, c;
    FILE* input_file;
    input_file = fopen(filename, "r");

    if (input_file == NULL) {
        perror(filename);
        exit(1);
    }
    while (fscanf(input_file, "%hx %hx", &a, &n) == 2) {
        for (Adress i = 0; i < n; i++) {
            fscanf(input_file, "%hhx", &c);
            b = c & 0xFF;
            b_write(a + i, b);
        }
    }
    //mem_dump(01000, n);
    fclose(input_file);
}

void mem_dump(Adress start, word n) {
    printf("-----------------------------mem_dump-----------------------------\n");
    for (Adress i = 0; i <= n; i += 2) {
        trace("%06o : %06o\n", start + i, w_read(start + i));
        printf("-----------------------------mem_dump-----------------------------\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Трассировка: ./(...) -t -T  <path>\n");
        return 0;
    }
    test_mem();
    if (!strcmp(argv[1], "-t")) {
        trac = 1;
        load_file(argv[2]);
    }
    else if (!strcmp(argv[1], "-T")) {
        trac = 2;
        load_file(argv[2]);
    }
    else {
        load_file(argv[1]);
    }
    run();
    return 0;
}