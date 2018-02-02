#include "ptable.h"
#include "system.h"

PTable::PTable(int size)
{
	psize = size;
	
	for (int i = 0; i < psize; i++)
	{
		pcb[i] = NULL;
	}
	
	bm = new BitMap(psize);
	bmsem = new Semaphore("bmsem", 1);
	
	// processID = 0 for main process
	bm->Mark(0);
	pcb[0] = new PCB(0);
	pcb[0]->parentID = -1;
}

PTable::~PTable()
{
	for (int i = 0; i < psize; i++)
	{
		if (pcb[i] != NULL) delete pcb[i];
	}
	
	delete bm;
	delete bmsem;
	psize = 0;
}

int PTable::ExecUpdate(char* filename)
{
	// Ngan khong cho nap 2 tien tinh cung 1 luc
	bmsem->P();
	
	// Kiem tra tinh hop le cua chuong trinh filename
	if(filename == NULL || filename == "")
	{
		ASSERT(0);
		printf("PTable:ExecUpdate-> name is NULL.\n");
		bmsem->V();
		return -1;
	}

	
	// Kiem tra su ton tai cua chuong trinh filename
 	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) 
	{
		ASSERT(0);
		printf("PTable::ExecUpdate-> name isn't a filename.\n");
		bmsem->V();
		return -1;

	}
	delete executable;
	
	// Kiem tra co goi chinh no hay khong?
	if(strcmp(filename, currentThread->getName()) == 0)
	{
		ASSERT(0);
		printf("PTable::ExecUpdate-> process call itself.\n");
		bmsem->V();
		return -1;
	}
	
	// Tim slot trong trong PTable
	int pID = GetFreeSlot();
	
	// Kiem tra slot vua tim
	if (pID < 0)
	{
		ASSERT(0);
		printf("PTable::ExecUpdate-> no free slot.\n");
		bmsem->V();
		return -1;
	}
	
	pcb[pID] = new PCB(pID);
	pcb[pID]->parentID = currentThread->processID; // see in PCB(int id)
	int res = pcb[pID]->Exec(filename, pID); // name is passed into Exec.
	bmsem->V();
	
	return res;
}

int PTable::ExitUpdate(int ec)
{
	int pid = currentThread->processID;
		
	if (pid == 0) // exit main process
	{
		interrupt->Halt();
		return -1;
	}
	
	if (!IsExist(pid))
	{
		printf("PTable::ExitUpdate : not exists id.\n");
		return -1;
	}
	
	pcb[pid]->SetExitCode(ec);
	pcb[pid]->JoinRelease();
	pcb[pid]->ExitWait();
	pcb[pcb[pid]->parentID]->DecNumWait();
	
	Remove(pid);
	return ec;
}

int PTable::JoinUpdate(int pID)
{
	if (pID < 0 || !bm->Test(pID)) // allocated for thread.
	{
		printf("PTable::JoinUpdate-> pID<0 or bm->Test(pID)==0.\n");
		return -1;
	}
	
	if (currentThread->processID != pcb[pID]->parentID)
	{
		printf("PTable::JoinUpdate-> currentThread->processID != pcb[id]->parentID\n");
		return -1;
	}
	
	pcb[pcb[pID]->parentID]->IncNumWait();
	pcb[pID]->JoinWait(); // wait until process finishes
	 
	int ExitCode = pcb[pID]->GetExitCode();
	pcb[pID]->ExitRelease();// when finish, allow it to exit
	return ExitCode;
}

int PTable::GetFreeSlot()
{
	return bm->Find();
}

bool PTable::IsExist(int pID)
{
	return bm->Test(pID);
}

void PTable::Remove(int pID)
{
	if (bm->Test(pID))
	{
		delete pcb[pID];
		pcb[pID] = NULL;
		bm->Clear(pID);
	}
}

char* PTable::GetName(int pID)
{
	if (pID < 0 || pID >= psize || pcb[pID] == NULL) return NULL;
	return pcb[pID]->GetNameThread();
}
