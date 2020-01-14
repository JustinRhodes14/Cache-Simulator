#include <stdio.h>
#include <stdlib.h>

typedef struct line {
	char *tag;
	int valid;
	int replace;
}line;

void prefetch(int);
void access(char*, int);
int compLog(int);
char* getTag(int);
char* hextoBin(char);
char* binAdjust(char*,int);
char* tagBits(char*,int);
void cacheNofetch(int, int, char*, char*,int);
void cacheNofetchLRU(int, int, char*, char*,int);
void cachePrefetch(int pfetch, int assoc, char *tag, char *task, int index,unsigned long int paddr,int block, int tagbits, int indexbits, int setNum);
void cachePrefetchLRU(int pfetch, int assoc, char *tag, char *task, int index,unsigned long int paddr,int block, int tagbits, int indexbits, int setNum);
int ctoBin(char*, int);
char* indexBits(char*, int, int);
char* decimaltoBin(unsigned long int,int);
unsigned long int hextoDec(char*);
int hexChartoBin(char);
char* dectoBin(unsigned long int);

