// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"

typedef int OpenFileID;

#define MAX_TABLE_FILE 10

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {

  private:
  	int amount_;
  	OpenFile** fileTable_; // store OpenFile*
  	
  public:
    
    FileSystem(bool format) 
    {
    	fileTable_ = new OpenFile*[MAX_TABLE_FILE]; // Khoi tao bang mo ta file
		amount_ = 0;
		for (int i = 0; i < MAX_TABLE_FILE; i++)
		{
			fileTable_[i] = NULL;
		}
    }
    
    ~FileSystem()
    {
    	for (int i = 0; i < MAX_TABLE_FILE; i++)
		{
			if (fileTable_[i] != NULL) delete fileTable_[i];
		}
		delete[] fileTable_;
    }
    
    int EmptyEntryInTable()
    {
    	for (int i = 2; i < MAX_TABLE_FILE; i++) 
    		if (fileTable_[i] == NULL) return i;
		return -1;
    }

    bool Create(char *name, int initialSize) 
    { 
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1) return FALSE;
		Close(fileDescriptor); 
		return TRUE; 
    }

    OpenFile* Open(char *name) 
    {
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1) return NULL;
	  
		amount_++;
	  
		return new OpenFile(fileDescriptor);
    }
    
    OpenFile* Open(char *name, int type) 
    {
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1) return NULL;
	
		amount_++;
	
		return new OpenFile(fileDescriptor, type);
    }

    bool Remove(char *name) { return Unlink(name) == 0; }
	
	// Inscrese amount of opened file
	void IncreaseAmount() { amount_++; }
	
	// Descrese amount of opened file
	void DecreaseAmount() { amount_--; }
	
	// Get amount of opened file
	int GetAmount() { return amount_; }
	
	// Get OpenFile* by fileID
	OpenFile* GetFileByID(OpenFileID fileID)
	{
		if (fileID < 2 || fileID >= MAX_TABLE_FILE) return NULL;
		
		return fileTable_[fileID];
	}
	
	void SetFileByID(OpenFile* file, OpenFileID fileID)
	{
		if (fileID < 2 || fileID >= MAX_TABLE_FILE) return;
		
		fileTable_[fileID] = file;
	}
};

#else // FILESYS
class FileSystem {
  public:
 
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.
    					
     ~FileSystem();

    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)
    OpenFile* Open(char *name, int type); // Open a file with type (UNIX open)

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

	int EmptyEntryInTable();
	
	void IncreaseAmount(); // Inscrese amount of opened file
	void DecreaseAmount(); // Descrese amount of opened file
	int GetAmount();	// Get amount of opened file
	
	OpenFile* GetFileByID(OpenFileID fileID);
	void SetFileByID(OpenFile* file, OpenFileID fileID);
  private:
	OpenFile* freeMapFile;			// Bit map of free disk blocks,
					// represented as a file
	OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
	int amount_; //  amount of file opened in file table
	OpenFile** fileTable_; // store OpenFile*
};

#endif // FILESYS

#endif // FS_H
