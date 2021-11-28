#pragma once
#include <stdlib.h>
#include <string.h>

//demonic typecasting :D
#define pSizePos(pointer) (int*)((int)pointer - sizeof(int*))
#define pSize *pSizePos

struct listitem {
	void* data;
	int flag;
};
typedef struct listitem listItem;

listItem* fillList(void* data, int datalength);
int* fillPseudolist(int datalength);

void* galloc(int size);
void* gcalloc(int count, int Size);
void gfree(void* memory);
void* grealloc(void* memory, int Size);