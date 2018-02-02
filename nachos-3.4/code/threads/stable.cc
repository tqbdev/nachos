#include "stable.h"
#include "system.h"

STable::STable()
{
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		semTab[i] = NULL;
	bm = new BitMap(MAX_SEMAPHORE);
}

STable::~STable()
{
	if (bm != NULL)
		delete bm;
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		if (semTab[i] != NULL)
			delete semTab[i];
}

int STable::Create(char* name, int init)
{
	int slot = bm->Find();
	int i;
	if (slot < 0)
	{
		printf ("Stable::Create-> no free slot.\n");
		return -1;
	}
	for (i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (semTab[i] != NULL)
			if (strcmp(semTab[i]->getName(), name) == 0)
			{
				printf("STable::Create-> name existed.\n");
				return -1;
			}
	}
	semTab[slot] = new Semaphore(name, init);	
	return slot;
}

int STable::Wait(char* name)
{
	int i;
	for (i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (semTab[i] != NULL)
			if (strcmp(semTab[i]->getName(), name) == 0)
			{
				semTab[i]->P();
				return i;
			}
	}
	printf("STable::Wait-> not exists semaphore.\n");
	return -1;
	
}

int STable::Signal(char* name)
{
	int i;
	for (i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (semTab[i] != NULL)
			if (strcmp(semTab[i]->getName(), name) == 0)
			{
				semTab[i]->V();
				return i;
			}
	}
	printf("STable::Wait-> not exists semaphore.\n");
	return -1;
}

int STable::FindFreeSlot(int id)
{
	return bm->Find();
}
