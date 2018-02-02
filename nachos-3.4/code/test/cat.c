#include "syscall.h"
#include "copyright.h"

int 
main()
{
	int res = 0;
	char str[LIMIT];
	
	int len;
	char filename[MaxFileLength + 1];
	int fileID;
	int fMoreLine = 0;
	
	print("Enter file name: ");
	len = ScanLine(filename, MaxFileLength);
	fileID = Open(filename, 1);
	
	/* Open a file */
	if (fileID != -1)
	{
		while (res >= 0)
		{
			res = Read(str, LIMIT, fileID);
			if (res <= 0) break;
			
			//if (fMoreLine) print("\n");
			print(str);
			
			fMoreLine = 1;
		} 
		CloseFile(fileID);
	}
	else
	{
		print("Open file ");
		print(filename);
		print(" failure.\n");
	}
	
	print("\n------------Ended-----------\n");
	Halt();
}
