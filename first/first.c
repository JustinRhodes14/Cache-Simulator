#include "first.h"
#include <string.h>
#include <math.h>


struct line** cache;
struct line** cache2;
int cmisses = 0;
int chits = 0;
int mreads = 0;
int mwrites = 0;
int cmisses2 = 0;
int chits2 = 0;
int mreads2 = 0;
int mwrites2 = 0;
int type = 0;
int blockCounter = 0;
long int lruCounter = 0;
long int lruCounter2 = 0;

int main(int argc, char** argv) {

//////////////////////////////////////////
//READ IN PARAMETERS
	int cache_size = atoi(argv[1]);
	int block_size = atoi(argv[2]);
	char* cache_policy = argv[3];
	char* associativity = argv[4];
	int prefetch = atoi(argv[5]);
	char* filename = argv[6];
	int assocN = 1;
//////////////////////////////////////////
	
//////////////////////////////////////////
//ERROR CHECKS	
	if (argc != 7) {
		printf("error\n");
		exit(0);
	}
	if (!((cache_size & (cache_size - 1)) == 0) || cache_size == 0) {
		printf("error\n");
		exit(0);
	}	
	if (!((block_size & (block_size - 1)) == 0) || block_size == 0) {
		printf("error\n");
		exit(0);
	}
//////////////////////////////////////////

//////////////////////////////////////////
//COMPUTES ASSOC VALUE & CHEKCKS POW OF 2		
	if (strlen(associativity) >= 7) {
		char* temp = malloc(20 * sizeof(char));	
		if (strlen(associativity) == 7) {
			temp[0] = associativity[6];			
			assocN = atoi(temp);
		} else {		
			for (int i = 7; i <= strlen(associativity); i++) {
				temp[i - 7] = associativity[i - 1];
				
			}
			assocN = atoi(temp);
		}
		if (!((assocN & (assocN - 1)) == 0) || assocN == 0) {
			printf("error4\n");
			exit(0);
		}
	}
//////////////////////////////////////////	

//////////////////////////////////////////
//INITIALIZING CACHE FOR SCAN IN
	
	int indexBit = 1;
	int dirtyBit = compLog(block_size);
	int tagBit = 0;
	int setNum;
	int assoc;
	if (strcmp(associativity,"direct") == 0) { //assoc = 1
		type = 1;
		setNum = cache_size/block_size;
		indexBit = compLog(setNum);
		assoc = 1;
		tagBit = 48 -(indexBit + dirtyBit);
		cache = (struct line**)malloc(setNum * sizeof(struct line));
		for (int i = 0; i <  setNum; i++) {
			cache[i] = (struct line*)malloc(sizeof(struct line));
			cache[i][0].replace = 0;
			cache[i][0].valid = 0;
			cache[i][0].tag = malloc((tagBit + 1) * sizeof(char));
		}
		
		cache2 = (struct line**)malloc(setNum * sizeof(struct line));
		for (int i = 0; i <  setNum; i++) {
			cache2[i] = (struct line*)malloc(sizeof(struct line));
			cache2[i][0].replace = 0;
			cache2[i][0].valid = 0;
			cache2[i][0].tag = malloc((tagBit + 1) * sizeof(char));
		}
	} else if (strcmp(associativity,"assoc") == 0) { //set = 1
		type = 2;
		cache = (struct line**)malloc(sizeof(struct line));
		assoc = cache_size/block_size;
		setNum = 1;
		indexBit = 0;
		tagBit = 48-(dirtyBit);
		cache[0] = (struct line*)malloc(assoc * sizeof(struct line));
		for (int i = 0; i < assoc; i++) {
			cache[0][i].replace = 0;
			cache[0][i].valid = 0;
			cache[0][i].tag  = malloc((tagBit + 1) * sizeof(char));
		}

		cache2 = (struct line**)malloc(sizeof(struct line));
		cache2[0] = (struct line*)malloc(assoc * sizeof(struct line));
		for (int i = 0; i < assoc; i++) {
			cache2[0][i].replace = 0;
			cache2[0][i].valid = 0;
			cache2[0][i].tag  = malloc((tagBit + 1) * sizeof(char));
		}
	} else {
		type = 3;
		setNum = cache_size/(block_size * assocN); //everything
		indexBit = compLog(setNum);
		assoc = assocN;
		tagBit = 48 -(indexBit + dirtyBit);
		cache = (struct line**)malloc(setNum * sizeof(struct line));
		for (int i = 0; i < setNum; i++) {
			cache[i] = (struct line*) malloc(assocN * sizeof(struct line));
			for (int j = 0; j < assocN; j++) {
				cache[i][j].replace = 0;
				cache[i][j].tag = malloc((tagBit + 1) * sizeof(char));		
				cache[i][j].valid = 0;
			}
		}
		
		cache2 = (struct line**)malloc(setNum * sizeof(struct line));
		for (int i = 0; i < setNum; i++) {
			cache2[i] = (struct line*) malloc(assocN * sizeof(struct line));
			for (int j = 0; j < assocN; j++) {
				cache2[i][j].replace = 0;
				cache2[i][j].tag = malloc((tagBit + 1) * sizeof(char));		
				cache2[i][j].valid = 0;
			}
		}
	}
	
	
//////////////////////////////////////////

//////////////////////////////////////////
//SCAN IN THE VALUES (READ/WRITE & MEM ADDRESSES) AND DO STUFF WITH ACCESS & PREFETCH
	FILE *fp = fopen(filename,"r");
	if (fp == NULL) {
		printf("error\n");
		exit(0);
	}
	if (strcmp(cache_policy,"fifo") == 0) {
	int check = 1;
	while (check == 1) {
		unsigned long int addr;
		char task;
		char* str = malloc(10 * sizeof(char));
		fscanf(fp,"%s",str);
		fscanf(fp,"%c", &task);
		fscanf(fp," %lx",&addr);
		char* nbin;
		char* tag;
		char* index;
		int iNum;
		if (strcmp(str,"#eof") == 0) {
			check = 0;
			break;
		}
		char pfetch2[100000] = "";	
		char petch3[100000] = "";	
		unsigned long int taddr = addr;
		while (taddr > 0) {
			if (taddr % 2 == 0) {
				strcat(pfetch2,"0\0");
			} else {
				strcat(pfetch2,"1\0");
			}
			taddr = taddr/2;
		}
		int counter = 0;
		for (int k = (strlen(pfetch2) - 1); k >= 0; k--) {
			petch3[counter] = pfetch2[k];
			counter++;
		}
		nbin = binAdjust(petch3,cache_size);
		tag = tagBits(nbin,tagBit);
		index = indexBits(nbin,indexBit,tagBit);
		iNum = ctoBin(index,indexBit);
		cacheNofetch(setNum, assoc,tag,str,iNum);
		cachePrefetch(prefetch,assoc,tag,str,iNum,addr,block_size,tagBit,indexBit,cache_size);
		/*		
		if (strcmp(cache_policy,"fifo") == 0) {
			cacheNofetch(setNum, assoc,tag,str,iNum);
			cachePrefetch(prefetch,assoc,tag,str,iNum,addr,block_size,tagBit,indexBit,cache_size);
		} else {
			cacheNofetchLRU(setNum, assoc,tag,str,iNum);
			cachePrefetchLRU(prefetch,assoc,tag,str,iNum,addr,block_size,tagBit,indexBit,cache_size);
		}
		*/
	}
	} else {
		int check = 1;
	while (check == 1) {
		unsigned long int addr;
		char task;
		char* str = malloc(10 * sizeof(char));
		fscanf(fp,"%s",str);
		fscanf(fp,"%c", &task);
		fscanf(fp," %lx",&addr);
		char* nbin;
		char* tag;
		char* index;
		int iNum;
		if (strcmp(str,"#eof") == 0) {
			check = 0;
			break;
		}
		char pfetch2[100000] = "";	
		char petch3[100000] = "";	
		unsigned long int taddr = addr;
		while (taddr > 0) {
			if (taddr % 2 == 0) {
				strcat(pfetch2,"0\0");
			} else {
				strcat(pfetch2,"1\0");
			}
			taddr = taddr/2;
		}
		int counter = 0;
		for (int k = (strlen(pfetch2) - 1); k >= 0; k--) {
			petch3[counter] = pfetch2[k];
			counter++;
		}
		nbin = binAdjust(petch3,cache_size);
		tag = tagBits(nbin,tagBit);
		index = indexBits(nbin,indexBit,tagBit);
		iNum = ctoBin(index,indexBit);
		cacheNofetchLRU(setNum, assoc,tag,str,iNum);
		cachePrefetchLRU(prefetch,assoc,tag,str,iNum,addr,block_size,tagBit,indexBit,cache_size);
		/*		
		if (strcmp(cache_policy,"fifo") == 0) {
			cacheNofetch(setNum, assoc,tag,str,iNum);
			cachePrefetch(prefetch,assoc,tag,str,iNum,addr,block_size,tagBit,indexBit,cache_size);
		} else {
			cacheNofetchLRU(setNum, assoc,tag,str,iNum);
			cachePrefetchLRU(prefetch,assoc,tag,str,iNum,addr,block_size,tagBit,indexBit,cache_size);
		}
		*/
	}
	}
//////////////////////////////////////////
//FREE
	//cache1
	for (int i = 0; i < setNum; i++) {		
		free(cache[i]);
	}
	free(cache);
	//cache2		
	for (int i = 0; i < setNum; i++) {	
		free(cache2[i]);
	}
	free(cache2);
//////////////////////////////////////////
//////////////////////////////////////////

//////////////////////////////////////////
//PRINT TESTS
	
	printf("no-prefetch\n");
	printf("Memory reads: %d\n",mreads);
	printf("Memory writes: %d\n",mwrites);
	printf("Cache hits: %d\n",chits);
	printf("Cache misses: %d\n",cmisses);
	printf("with-prefetch\n");
	printf("Memory reads: %d\n",mreads2);
	printf("Memory writes: %d\n",mwrites2);
	printf("Cache hits: %d\n",chits2);
	printf("Cache misses: %d\n",cmisses2);
//////////////////////////////////////////

	return 0;

}

char* binAdjust(char* bin, int cache_size) {
	//int bitAdj = cache_size - strlen(bin);
	int bitAdj = 48 - strlen(bin);
	char* bitA = (char*)malloc(strlen(bin) * sizeof(char) + bitAdj + 1);
	for (int i = 0; i < bitAdj; i++) {
		bitA[i] = '0';	
	}

	strcat(bitA,bin);
	return bitA;
}

char* tagBits(char* bin, int tNum) {
	char* tag = (char*)malloc((tNum + 1) * sizeof(char));
	
	for (int i = 0; i < tNum; i++) {
		tag[i] = bin[i];
	}
	
	return tag;
}

char* indexBits(char* bin, int sNum, int tNum) {

	char* index = (char*)malloc((sNum+1) * sizeof(char));
	int count = tNum;
	
	for (int i = 0; i < sNum; i++) {
		index[i] = bin[count];
		count++;
	}
	
	return index;
}

void cacheNofetch(int set, int assoc, char *tag, char *task, int index) {
		for (int j = 0; j < assoc; j++) {
			if ((cache[index][j].valid == 1) && (strcmp(cache[index][j].tag,tag) == 0)) {				
				chits++;				
				if (strcmp(task,"W") == 0) {
					mwrites++;
				}
				return;
			}
		}
		for (int j = 0; j < assoc; j++) {
			if (cache[index][j].replace == assoc) {
				cache[index][j].tag = tag;
				cache[index][j].replace = 1;
				
				for (int i = 0; i < assoc; i++) {
					if (i != j) {					
						cache[index][i].replace++;
					}		
				}
				break;

			} else if (cache[index][j].valid == 0) {
				cache[index][j].tag = tag;				
				cache[index][j].replace = 1;
				cache[index][j].valid = 1;
				for (int i = 0; i < assoc; i++) {
					if (i != j) {					
						cache[index][i].replace++;
					}
				}
				break;
							
			}
		}
		cmisses++;
		mreads++;
		if (strcmp(task,"W") == 0) {
			mwrites++;
		}
	return;
	
	
}

void cachePrefetch(int pfetch, int assoc, char *tag, char *task, int index,unsigned long int paddr,int block, int tagbits, int indexbits, int cache_size) {
		for (int j = 0; j < assoc; j++) {
			if ((cache2[index][j].valid == 1) && (strcmp(cache2[index][j].tag,tag) == 0)) {				
				chits2	++;				
				if (strcmp(task,"W") == 0) {
					mwrites2++;
				}
				return;
			}
		}
		for (int j = 0; j < assoc; j++) {
			if (cache2[index][j].replace == assoc) {
				cache2[index][j].tag = tag;
				cache2[index][j].replace = 1;
				
				for (int i = 0; i < assoc; i++) {
					if (i != j) {					
						cache2[index][i].replace++;
					}
				}
				
				break;

			} else if (cache2[index][j].valid == 0) {
				cache2[index][j].tag = tag;				
				cache2[index][j].replace = 1;
				cache2[index][j].valid = 1;
				for (int i = 0; i < assoc; i++) {
					if (i != j) {					
						cache2[index][i].replace++;
					}
				}
				break;
							
			}
		}
		cmisses2++;
		mreads2++;
		if (strcmp(task,"W") == 0) {
			mwrites2++;
		}
		for (int i = 0; i < pfetch; i++) {
			paddr = paddr + block;
			char pfetch2[100000] = "";	
			unsigned long int tpaddr = paddr;
			while (tpaddr > 0) {
				if (tpaddr % 2 == 0) {
					strcat(pfetch2,"0");
				} else {
					strcat(pfetch2,"1");
				}
				tpaddr = tpaddr/2;
			}
			char petch2[100000] = "";
			int counter = 0;
			for (int k = (strlen(pfetch2) - 1); k >= 0; k--) {
				petch2[counter] = pfetch2[k];
				counter++;
			}			
			char* fin;
			fin = binAdjust(petch2,48);
			char* tag2;
			tag2 = tagBits(fin,tagbits);
			char* indexB;
			indexB = indexBits(fin, indexbits,tagbits);
			int in = ctoBin(indexB,indexbits);
			int tvar = 0;
			for (int j = 0; j < assoc; j++) {
			if ((cache2[in][j].valid == 1) && (strcmp(cache2[in][j].tag,tag2) == 0)) {				
				tvar = 1;				
				break;
			}
			}		
			if (tvar == 1) {
				continue;
			}	
			mreads2++;
			for (int j = 0; j < assoc; j++) {
			if (cache2[in][j].replace == assoc) {
				cache2[in][j].tag = tag2;
				cache2[in][j].replace = 1;
				
				for (int i = 0; i < assoc; i++) {
					if (i != j) {					
						cache2[in][i].replace++;		
					}
				}
				
				break;

			} else if (cache2[in][j].valid == 0) {
				cache2[in][j].tag = tag2;				
				cache2[in][j].replace = 1;
				cache2[in][j].valid = 1;
				for (int i = 0; i < assoc; i++) {
					if (i != j) {					
						cache2[in][i].replace++;		
					}
				}
				break;
							
			}
			}
			

			
		}	
	return;
}

void cacheNofetchLRU(int set, int assoc, char *tag, char *task, int index) {
		for (int j = 0; j < assoc; j++) {
			if ((cache[index][j].valid == 1) && (strcmp(cache[index][j].tag,tag) == 0)) {				
				chits++;				
				if (strcmp(task,"W") == 0) {
					mwrites++;
				}
				cache[index][j].replace = lruCounter;
				lruCounter++;
				return;
			}
		}
		int success = 0;
		for (int j = 0; j < assoc; j++) {
			if (cache[index][j].valid == 0) {
				cache[index][j].tag = tag;				
				cache[index][j].replace = lruCounter;
				lruCounter++;
				cache[index][j].valid = 1;
				success = 1;
				break;
							
			}
		}
		if (success == 0) {
			int holder = cache[index][0].replace;
			int jdex = 0;
			for (int j = 0; j < assoc; j++) {
				if (cache[index][j].replace < holder) {
					holder = cache[index][j].replace;
					jdex = j;
				}
			}
		
			cache[index][jdex].replace = lruCounter;
			lruCounter++;
			cache[index][jdex].tag = tag;
		}		
		cmisses++;
		mreads++;
		if (strcmp(task,"W") == 0) {
			mwrites++;
		}
	return;
	
	
}
void cachePrefetchLRU(int pfetch, int assoc, char *tag, char *task, int index,unsigned long int paddr,int block, int tagbits, int indexbits, int cache_size) {
		for (int j = 0; j < assoc; j++) {
			if ((cache2[index][j].valid == 1) && (strcmp(cache2[index][j].tag,tag) == 0)) {				
				chits2	++;				
				if (strcmp(task,"W") == 0) {
					mwrites2++;
				}
				cache2[index][j].replace = lruCounter2;
				lruCounter2++;
				return;
			}
		}
		int success = 0;
		for (int j = 0; j < assoc; j++) {
			if (cache2[index][j].valid == 0) {
				cache2[index][j].tag = tag;				
				cache2[index][j].replace = lruCounter2;
				lruCounter2++;
				cache2[index][j].valid = 1;
				success = 1;
				break;
							
			}
		}
		if (success == 0) {
			int holder = cache2[index][0].replace;
			int jdex = 0;
			for (int j = 0; j < assoc; j++) {
				if (cache2[index][j].replace < holder) {
					holder = cache2[index][j].replace;
					jdex = j;
				}
			}
		
			cache2[index][jdex].replace = lruCounter2;
			lruCounter2++;
			cache2[index][jdex].tag = tag;
		}
		cmisses2++;
		mreads2++;
		if (strcmp(task,"W") == 0) {
			mwrites2++;
		}
		for (int i = 0; i < pfetch; i++) {
			paddr = paddr + block;
			char pfetch2[100000] = "";	
			unsigned long int tpaddr = paddr;
			while (tpaddr > 0) {
				if (tpaddr % 2 == 0) {
					strcat(pfetch2,"0");
				} else {
					strcat(pfetch2,"1");
				}
				tpaddr = tpaddr/2;
			}
			char petch2[100000] = "";
			int counter = 0;
			for (int k = (strlen(pfetch2) - 1); k >= 0; k--) {
				petch2[counter] = pfetch2[k];
				counter++;
			}			
			char* fin;
			fin = binAdjust(petch2,48);
			char* tag2;
			tag2 = tagBits(fin,tagbits);
			char* indexB;
			indexB = indexBits(fin, indexbits,tagbits);
			int in = ctoBin(indexB,indexbits);
			int tvar = 0;
			for (int j = 0; j < assoc; j++) {
			if ((cache2[in][j].valid == 1) && (strcmp(cache2[in][j].tag,tag2) == 0)) {				
				tvar = 1;
				cache2[in][j].replace = lruCounter2;
				lruCounter2++;			
				break;
			}
			}		
			if (tvar == 1) {
				continue;
			}	
			mreads2++;
			int success2 = 0;
			for (int j = 0; j < assoc; j++) {
				if (cache2[in][j].valid == 0) {
					cache2[in][j].tag = tag2;				
					cache2[in][j].replace = lruCounter2;
					lruCounter2++;
					cache2[in][j].valid = 1;
					success2 = 1;
					break;
							
				}
			}
			if (success2 == 0) {
				int holder = cache2[in][0].replace;
				int jdex = 0;
				for (int j = 0; j < assoc; j++) {
					if (cache2[in][j].replace < holder) {
						holder = cache2[in][j].replace;
						jdex = j;
					}
				}
		
				cache2[in][jdex].replace = lruCounter2;
				lruCounter2++;
				cache2[in][jdex].tag = tag2;
			}
			

			
		}	
	return;
}
int ctoBin(char* index, int sNum) {
	if (sNum == 0) {
		return 0;
	}
	
	int b = 1;
	int sum = 0;
	int v = 0;
		
	for (int i = sNum - 1; i >= 0; i--) {
		if (index[i] == '0') {
			v = 0;
		} else {
			v = 1;
		}
	
		sum = sum + (v * b);
		b = b * 2;
	}

	return sum;
}

int compLog(int x) {
	if (x == 0) {
		return 0;
	}
	int result = -1;
	while (x) {
		result++;
		x = x >> 1;
	}
	
	return result;
	
}










