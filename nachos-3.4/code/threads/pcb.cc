#include "pcb.h"
#include "addrspace.h"
#include "system.h"

extern void StartProcess_2(int id); // in progtest.cc

PCB::PCB(int id)
{
	pid = id;
	parentID = currentThread->processID;
	exitcode = 0;
	numwait = 0;
	
	thread = NULL;	
	
	joinsem = new Semaphore("joinsem", 0);
	exitsem = new Semaphore("exitsem", 0);
	mutex = new Semaphore("mutex", 1);	
}

PCB::~PCB()
{
	delete joinsem;
	delete exitsem;
	delete mutex;
	
	if (thread != NULL)
	{
		delete thread->space;
		thread->Finish();
	}
}

int PCB::Exec(char *filename, int pID)
{
	// Ngan khong cho nap 2 tien tinh cung luc
	mutex->P();
	
	// Khoi tao thread
	thread = new Thread(filename);
	
	if (thread == NULL)
	{
		ASSERT(FALSE);
		printf("Error: not enough memory.\n");
		mutex->V();
		return -1;
	}
	
	// Set lai cac tham so
	pid = pID;
	thread->processID = pID;
	parentID = currentThread->processID;
	
	thread->Fork(StartProcess_2, pID);
	
	mutex->V();
	return pID;

}

int PCB::GetID()
{
	return pid;
}

int PCB::GetNumWait()
{
	return numwait;
}

void PCB::JoinWait()
{
	joinsem->P();
}

void PCB::ExitWait()
{
	exitsem->P();
}

void PCB::JoinRelease()
{
	joinsem->V();
}

void PCB::ExitRelease()
{
	exitsem->V();
}

void PCB::IncNumWait()
{
	mutex->P();
	numwait = numwait + 1;
	mutex->V();
}

void PCB::DecNumWait()
{
	mutex->P();
	numwait = numwait - 1;
	mutex->V();
}

void PCB::SetExitCode(int ec)
{
	exitcode = ec;
}

int PCB::GetExitCode()
{
	return exitcode;
}

char* PCB::GetNameThread()
{
	if (thread != NULL) return thread->getName();
	return NULL;
}
