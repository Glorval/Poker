#include "GlorwynMemory.h"


listItem* fillList(char* data, int datalength) {
	listItem* newlist = (listItem*)gcalloc(datalength, sizeof(listItem));
	for (int fill = 0; fill < datalength; fill++) {
		newlist[fill].data = (void*)data[fill];
		newlist[fill].flag = 0;
	}
	return(newlist);
}

int* fillPseudolist(int datalength) {
	return((int*)gcalloc(datalength, sizeof(int*)));
}


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
		return(gcalloc(1, Size));
	}
	int prevSize = pSize((char*)memory);
	if (prevSize > Size) {
		char* newMemory = (char*)gcalloc(1, Size);
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
