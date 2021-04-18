typedef unsigned char byte;
typedef	unsigned short int word;
typedef word Adress;

#define MEM_SIZE (64*1024)
#define pc reg[7]
#define sp reg[6]
#define ODATA 0177566
#define OSTAT 0177564

void trace(const char * format, ...);
void test_mem();
void b_write(Adress adr, byte b);
byte b_read(Adress adr);
void w_write(Adress adr, word w);
word w_read(Adress adr);
void mem_dump(Adress start, word n);

typedef struct {
    word val; //значение аргумента
    word adr;//адрес аргумента
    byte type;
} Arg;
