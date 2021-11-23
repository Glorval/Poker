#include "GlorwynMemory.h"


void* galloc(int Size) {
	int* newMemory = (int*)malloc((Size * sizeof(char)) + sizeof(int));
	int* size = newMemory;
	*size = Size;
	void* returns = &newMemory[1];
	return(returns);
}

void* gcalloc(int count, int Size) {
	int* newMemory = (int*)calloc(count + 4,  Size);
	int* size = newMemory;
	*size = Size * count;
	void* returns = &newMemory[1];
	return(returns);
}

void gfree(void* memory) {
	void* actualPos = (int*)memory - 1;
	free(actualPos);
}

void* grealloc(void* memory, int Size) {
	if (memory == NULL) {
		return(galloc(Size));
	}
	int prevSize = pSize((char*)memory);
	if (prevSize > Size) {
		char* newMemory = (char*)galloc(Size);
		memcpy(newMemory, memory, Size);
		gfree(memory);
		return(newMemory);
	}
	int* newMemory = (int*)memory - 1;
	newMemory = (int*)realloc(newMemory, Size + 4);
	//gfree(memory);
	int* size = newMemory;
	*size = Size;
	void* returns = &newMemory[1];
	return(returns);
}