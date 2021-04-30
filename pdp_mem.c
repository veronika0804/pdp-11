#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
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
    byte b0 = 0x0a;
    b_write(2, b0);
    byte bres1 = b_read(2);
    printf("%02hhx = %02hhx\n", b0, bres1);
    assert(b0 == bres1);

    //пишем слово читаем слово
    word w1 = 0x0a0b;
    w_write(0, w1);
    word wres1 = w_read(0);
    assert(wres1 == w1);

    //пишем 2 байта, читаем слово
    Adress a = 4;
    byte b1 = 0x0b;
    b0 = 0x0a;
    word w = 0x0b0a;
    b_write(a, b0);
    b_write(a + 1, b1);
    word wres2 = w_read(a);
    printf("ww/br \t %04hx=%02hhx%02hhx\n", wres2, b1, b0);
    assert(w == wres2);

    //пишем слово, читаем 2 байта
    word w0 = 0x0a0b;
    w_write(2, w0);
    b0 = b_read(2);
    b1 = b_read(3);
    printf("%04hx\n%02hhx%02hhx\n", w0, b1, b0);
    assert(b0 == (byte)0x0b);
    assert(b1 == (byte)0x0a);

}

word w_read(Adress a) {
    word w = ((word)mem[a+1]) << 8;
    //printf("w = %x\n", w);
    w = w | mem[a];
    return w;
}
void b_write(Adress adr, byte b) {
    mem[adr]=b;
    if (adr == ODATA)
        printf("%c", b);
}
byte b_read(Adress adr) {
    return mem[adr];
}

void w_write(Adress adr, word w) {
    mem[adr] = (byte)(w & 0xFF);
    mem[adr + 1] = (byte)((w >> 8) & 0xFF);
    if (adr == ODATA)
        printf("%c", w);
}

void load_file(const char* filename) {
    Adress a, n;
    byte b, c;
    FILE* input_file;
    input_file = fopen(filename, "r");
    while (fscanf(input_file, "%hx %hx", &a, &n) == 2) {
        for (Adress i = 0; i < n; i++) {
            fscanf(input_file, "%hhx", &c);
            b = c & 0xFF;
            b_write(a + i, b);
        }
    }

    fclose(input_file);
}

void mem_dump(Adress start, word n) {
    for (Adress i = 0; i <= n; i += 2) {
        trace("%06o : %06o\n", start + i * 2 , w_read(start + i*2));
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Трассировка -t -T \n");
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