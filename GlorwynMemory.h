#pragma once
#include <stdlib.h>
#include <string.h>

//demonic typecasting :D
#define pSizePos(pointer) (int*)((int)pointer - sizeof(int*))
#define pSize *pSizePos

void* galloc(int size);
void* gcalloc(int count, int Size);
void gfree(void* memory);
void* grealloc(void* memory, int Size);