#include <stdio.h>
#include <assert.h>


typedef unsigned char byte;
typedef unsigned int word;
typedef word Adress;

#define MEMSIZE (64*1024)

byte mem[MEMSIZE];

void b_write(Adress adr, byte b);
byte b_read(Adress adr);
void w_write(Adress adr, word w);
word w_read(Adress adr);

void test_mem() {
    byte b0 = 0x0a;
    //пишем байт, читаем байт
    b_write(2, b0);
    byte bres = b_read(2);
    printf("%02hhx = %02hhx\n", b0, bres);
    assert(b0 == bres);
    //пишем 2 байта, читаем слово

    Adress a = 4;
    byte b1 = 0x0b;
    b0 = 0x0a;
    word w = 0x0b0a;
    b_write(a, b0);
    b_write(a + 1, b1);
    word wres = w_read(a);
    printf("ww/br \t %04hx=%02hhx%02hhx\n", wres, b1, b0);
    assert(w == wres);

    word w0 = 0x0a0b;
    w_write(2, w0);
    b0 = b_read(2);
    b1 = b_read(3);
    printf("%04hx\n%02hhx%02hhx\n", w0, b1, b0);
    assert(b0 == (byte)0x0b);
    assert(b1 == (byte)0x0a);
    
}

int main() {
    test_mem();
    return 0;
}

word w_read(Adress a) {
    word w = ((word)mem[a+1]) << 8;
    //printf("w = %x\n", w);
    w = w | mem[a];
    return w;
}
void b_write(Adress adr, byte b) {
    mem[adr]=b;
}
byte b_read(Adress adr) {
    return mem[adr];
}

void w_write(Adress adr, word w) {
    mem[adr] = (byte)(w & 0xFF);
    mem[adr + 1] = (byte)((w >> 8) & 0xFF);
}
