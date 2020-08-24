#pragma once
#include "type.h"
class buddyallocator
{
public:
	buddyallocator(PVOID fr, PVOID bd, int tl);
	~buddyallocator();
	PVOID alloc(size_t s);
	void showlevel();
	void clean_fragmentation(size_t level, linkedlist* t);
	void free(PVOID addr, size_t x);
	void inserttail(int level, linkedlist* c);
	void deleteentry(int level, linkedlist* c);
	size_t findlevel(size_t x);
private:
	PVOID freeregion;
	PVOID buddylists;
	int TOPLEVEL;
};

