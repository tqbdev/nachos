// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
	
    switch (which)
    {
	case NoException:
		return;
	case SyscallException:
		switch (type)
		{
			case SC_Halt:
				DEBUG('a', "Shutdown, initiated by user program.\n");
   				interrupt->Halt();
				break;
				
			case SC_Create:
			case SC_CreateFile:
			{
				int virtAddr;
				char* filename;

				DEBUG('a', "\n SC_Create or SC_CreateFile call ...");
				DEBUG('a', "\n Reading virtual address of filename.");

				// Lay tham so ten tap tin tu thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				DEBUG('a', "\n Reading filename.");

				// MaxFileLength la 32
				filename = machine->User2System(virtAddr, MaxFileLength + 1);
				if (filename == NULL)
				{
					//printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] filename;
					break;
				}

				DEBUG('a', "\n Finish reading filename.");
				
				if (fileSystem->Create(filename, 0) == FALSE)
				{
					//printf("\n Error create file '%s'", filename);
					DEBUG('a', "\n Error file");
					machine->WriteRegister(2, -1);
					delete[] filename;
					break;
				}
				
				machine->WriteRegister(2, 0); // tra ve thong bao cho chuong trinh nguoi dung
				delete[] filename;
				break;
			}			
				
			case SC_Open:
			{
				int fileID = fileSystem->EmptyEntryInTable();

				if (fileID == -1)
				{
					DEBUG('a', "\n Not enough memory in file table.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
								
				int virtAddr;
				char* filename;
				int type;
				
				DEBUG('a', "\n SC_Open call ...");
				DEBUG('a', "\n Reading type (int).");
				type = machine->ReadRegister(5);
				DEBUG('a', "\n Finish reading type.");
				
				DEBUG('a', "\n Reading virtual address of filename.");

				// Lay tham so ten tap tin tu thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				DEBUG('a', "\n Reading filename.");

				// MaxFileLength la 32
				filename = machine->User2System(virtAddr, MaxFileLength + 1);
				if (filename == NULL)
				{
					printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] filename;
					break;
				}

				DEBUG('a', "\n Finish reading filename.");
				
				DEBUG('a', "\n Starting open file with filename.");
				OpenFile* file = fileSystem->Open(filename, type);
				if (file == NULL)
				{
					DEBUG('a', "\n Error open file.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] filename;
					break;
				}
				
				fileSystem->SetFileByID(file, fileID);
				machine->WriteRegister(2, fileID); // tra ve OpenFileID cho chuong trinh nguoi dung
				delete[] filename;
				break;
			}
			
			case SC_CloseFile:
			{
				int fileID = machine->ReadRegister(4);
				
				DEBUG('a', "\n SC_CloseFile call ...");
				DEBUG('a', "\n Checking fileID ...");
				
				if (fileID < 0 || fileID >= MAX_TABLE_FILE || fileID == 0 || fileID == 1)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
				
				OpenFile* file = fileSystem->GetFileByID(fileID);
				if (file == NULL)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
				
				DEBUG('a', "\n Closing opened file with fileID ...");
				
				delete file;
				fileSystem->SetFileByID(NULL, fileID);
				fileSystem->DecreaseAmount(); // Giam so luong file dang mo
				machine->WriteRegister(2, 0); // tra ve thong bao thanh cong cho chuong trinh nguoi dung
				
				break;
			}
			
			case SC_Read:
			{
				int virtAddr = machine->ReadRegister(4);
				int charcount = machine->ReadRegister(5);
				int fileID = machine->ReadRegister(6);
				
				DEBUG('a', "\n SC_Read call ...");
				DEBUG('a', "\n Checking fileID ...");
				
				if (fileID < 0 || fileID >= MAX_TABLE_FILE || fileID == ConsoleOutput)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
				
				char *str = new char[charcount];
				
				if (str == NULL)
				{
					printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				if (fileID == ConsoleInput)
				{
					DEBUG('a', "\nConsoleInput mode ...");
					int sz = gSynchConsole->Read(str, charcount);
					str[sz] = '\0';
					//sz++;
					
					machine->System2User(virtAddr, sz + 1, str);
					machine->WriteRegister(2, sz);
					
					break;
				}
				
				OpenFile* file = fileSystem->GetFileByID(fileID);
				if (file == NULL)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				DEBUG('a', "\n Checking current pos ...");			
				int oldPos = file->GetCurrentPos();
				if (oldPos >= file->Length())
				{
					DEBUG('a', "\n Current pos in file is EOF ...");
					machine->WriteRegister(2, -2); // tra ve loi cuoi file cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				DEBUG('a', "\n Reading from file ...");
								
				if (file->Read(str, charcount) > 0 )
				{
					int newPos = file->GetCurrentPos();
					machine->System2User(virtAddr, newPos - oldPos + 1, str);
					DEBUG('a', "\n Return actual amount char read from file ...");
					machine->WriteRegister(2, newPos - oldPos + 1);
				}
				else
				{
					DEBUG('a', "\n Error reading from file ...");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				break;
			}
			
			case SC_Write:
			{
				int virtAddr = machine->ReadRegister(4);
				int charcount = machine->ReadRegister(5);
				int fileID = machine->ReadRegister(6);
				
				DEBUG('a', "\nSC_Write call ...");
				DEBUG('a', "\nChecking fileID ...");
				
				if (fileID < 0 || fileID >= MAX_TABLE_FILE || fileID == ConsoleInput)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
				
				char *str = new char[charcount];
				DEBUG('a', "\nGetting string from agrument (user) ...");
				str = machine->User2System(virtAddr, charcount);
				
				if (str == NULL)
				{
					printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				if (fileID == ConsoleOutput)
				{
					DEBUG('a', "\nConsoleOutput mode ...");
					int i = 0;
					while (str[i] != 0 && str[i] != '\n')
					{
						gSynchConsole->Write(str+i, 1);
						i++;
					}
					
					str[i] = '\n';
					
					gSynchConsole->Write(str+i,1);
					machine->WriteRegister(2, i-1);
					delete[] str;
					break;
				}
				
				OpenFile* file = fileSystem->GetFileByID(fileID);
				if (file == NULL)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				if (file->GetType() == 1)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;
					break;
				}
				
				int oldPos = file->GetCurrentPos();
				
				if (file->Write(str, charcount) > 0 )
				{
					int newPos = file->GetCurrentPos();
					DEBUG('a', "\nReturn actual amount char write to file ...");
					machine->WriteRegister(2, newPos - oldPos + 1); // tra ve so ky tu thuc su cho chuong trinh nguoi dung
				}
				else
				{
					DEBUG('a', "\nError writing to file ...");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
				}
				
				delete[] str;
				break;
			}
			
			case SC_Seek:
			{
				int pos = machine->ReadRegister(4);
				int fileID = machine->ReadRegister(5);
				
				DEBUG('a', "\nSC_Seek call...");
				DEBUG('a', "\nChecking OpenFileId...");
				
				if (fileID < 0 || fileID >= MAX_TABLE_FILE)
				{
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
				
				if (fileID == 0 || fileID == 1)
				{
					printf("\nCannot seek in console!!!");
					DEBUG('a', "\nCannot seek in console!!!");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break; 
				}
				
				OpenFile* file = fileSystem->GetFileByID(fileID);
				if (file == NULL)
				{
					printf("\nSeek fail \n");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					break;
				}
				
				DEBUG('a', "\nChecking pos...");
				if (pos == -1)
				{
					file->Length();
				}
				
				DEBUG('a', "\nSeeking...");
				if (pos > file->Length() || pos < 0) 
				{
					DEBUG('a', "\npos invalid...");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
				} 
				else 
				{
					file->Seek(pos);
					machine->WriteRegister(2, pos); // tra ve vi tri con tro hien tai cho chuong trinh nguoi dung
				}
				
				break;
			}
			
			case SC_Print:
			{
				int virtAddr;
				char* str;

				DEBUG('a', "\n SC_Print call ...");
				DEBUG('a', "\n Reading virtual address of string.");

				// Lay tham so chuoi tu thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				DEBUG('a', "\n Reading string.");

				// LIMIT la 255
				str = machine->User2System(virtAddr, LIMIT);
				if (str == NULL)
				{
					printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					delete[] str;					
					break;
				}
				
				DEBUG('a', "\n Finish reading string.");
				DEBUG('a', "\n Writing to console by gSynchConsole.");
				gSynchConsole->Write(str, LIMIT);				
				delete[] str;
				break;
			}
			
			case SC_ScanChar:
			{
				char* res = new char;
				
				DEBUG('a', "\n SC_ScanChar call ...");
				
				if (res == NULL)
				{
					printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					//delete res;
					break;
				}
				
				DEBUG('a', "\n Reading char from console by gSynchConsole.");
				gSynchConsole->Read(res, 1);
				machine->WriteRegister(2, *res);
				delete res;
				break;
			}
			
			case SC_PrintChar:
			{
				char c;
				c = machine->ReadRegister(4);	// read arg char
				gSynchConsole->Write(&c, 1);	// print char
				break;
			}
			
			case SC_ScanLine:
			{
				int virtAddr;
				char* str = new char[LIMIT];
				int length;

				DEBUG('a', "\n SC_ScanLine call ...");

				// Lay tham so chuoi tu thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				
				// Lay tham so length tu thanh ghi r5
				length = machine->ReadRegister(5);
				
				if (length > LIMIT || str == NULL)
				{
					printf("\n Not enough memory in system.");
					DEBUG('a', "\n Not enough memory in system.");
					machine->WriteRegister(2, -1); // tra ve loi cho chuong trinh nguoi dung
					delete[] str;					
					break;
				}
				
				DEBUG('a', "\n Reading line from console by gSynchConsole.");
				
				int sz = gSynchConsole->Read(str, length);
				str[sz] = '\0';
				//sz++;
				machine->System2User(virtAddr, sz + 1, str);
				machine->WriteRegister(2, sz); // tra ve so ky tu lay duoc cho chuong trinh nguoi dung
				
				delete[] str;
				break;
			}
			
			case SC_Join:
			{
				int id, res;
				
				// Doc pid tu thanh ghi r4 len
				id = machine->ReadRegister(4);
				
				// Goi JoinUpdate cua PTable
				res = pTab->JoinUpdate(id);
				
				// Tra ket qua ve cho nguoi dung
				machine->WriteRegister(2,res);
				break;
			}
			
			case SC_Exit:
			{
				int exitStatus, res;
				
				// Doc exitcode tu thanh ghi r4 len
				exitStatus = machine->ReadRegister(4);
				
				// Goi ExitUpdate cua PTable
				res = pTab->ExitUpdate(exitStatus);
				
				// Giai phong frame
				delete currentThread->space;
				// Giai phong ten thread
				delete currentThread->getName();

				// Ket thuc thread
				currentThread->Finish();
				
				// Tra ket qua ve cho nguoi dung
				machine->WriteRegister(2, res);
				break;
			}
			
			case SC_Exec:
			{
				// Get address of the file name
				int buffAddr;
				buffAddr = machine->ReadRegister(4);
				
				// Lay duong dan - name thread
				char* name = machine->User2System(buffAddr, 100);
				
				OpenFile *file = fileSystem->Open(name);
	   			if (file == NULL)
				{
					printf("\nExec:: Can't open this file.");
					machine->WriteRegister(2,-1);
					break;
				}

				delete file;

				// Goi ExecUpdate cua PTable. Return child process id
				int id = pTab->ExecUpdate(name);
				
				// Bien name se duoc thread su dung, shallow copy, nen khong giai phong vung nho name o day
				machine->WriteRegister(2,id);
				break;
			}
							
			default:
				printf("Unexpected user mode exception %d %d\n", which, type);
				//ASSERT(FALSE);
				interrupt->Halt();
		}
		// Increase PC Register
		machine->registers[PrevPCReg] = machine->registers[PCReg];
		machine->registers[PCReg] = machine->registers[NextPCReg];
		machine->registers[NextPCReg] += 4;
		break;
		    
    case  PageFaultException:    // No valid translation found
		printf("No valid translation found %d %d\n", which, type);
		//ASSERT(FALSE);
		interrupt->Halt();
		break;
		
	case  ReadOnlyException:     // Write attempted to page marked 
		printf("Write attempted tp page marked %d %d\n", which, type);
		//ASSERT(FALSE);
		interrupt->Halt();
		break;
		
	case  BusErrorException:     // Translation resulted in an 
		printf("Translaion resulted in an %d %d\n", which, type);
		//ASSERT(FALSE);
		interrupt->Halt();
		break;
		
	case  AddressErrorException: // Unaligned reference or one that
		printf("Unaligned reference or one that %d %d\n", which, type);
		//ASSERT(FALSE);
		interrupt->Halt();
		break;
		
	case  OverflowException:     // Integer overflow in add or sub.
		printf("Integer overflow in add or sub %d %d\n", which, type);
		//ASSERT(FALSE);
		interrupt->Halt();
		break;
		
	case  IllegalInstrException: // Unimplemented or reserved instr.
		printf("Unimplemented or reserved instr %d %d\n", which, type);
		//ASSERT(FALSE);
		interrupt->Halt();
		break;
   }
}
