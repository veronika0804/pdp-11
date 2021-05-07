#pragma once
typedef unsigned char byte;          //8 бит
typedef	unsigned short int word;     //16 бит
typedef word Adress;                 //64 кб

#define MEM_SIZE (64*1024)
extern byte mem[MEM_SIZE];
extern word reg[8];
extern byte trac;
#define pc reg[7]    //адрес той ячейки памяти, в которой будет считываться очередное слово(команда)
#define sp reg[6]
#define ODATA 0177566    //регистр данных дисплея
#define OSTAT 0177564    //регистр состояния дисплея

void trace(const char * format, ...);
void test_mem();
void b_write(Adress adr, byte b);
byte b_read(Adress adr);
void w_write(Adress adr, word w);
word w_read(Adress adr);
void load_file(const char* filename);
void mem_dump(Adress start, word n);
