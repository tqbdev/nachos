#include "syscall.h"
#include "copyright.h"

int 
main()
{
	char choose = '1';
	
	int res = 0;
	char str[LIMIT];
	
	char fileSrc[MaxFileLength + 1];
	char fileDes[MaxFileLength + 1];
	
	int fileSrcID;
	int fileDesID;
	int fMoreLine = 0;
	
	print("Enter file src name: ");
	ScanLine(fileSrc, MaxFileLength);
	fileSrcID = Open(fileSrc, 1);
	
	/* Open a file src */
	if (fileSrcID != -1)
	{
		print("Enter file des name: ");
		ScanLine(fileDes, MaxFileLength);
		fileDesID = Open(fileDes, 0);
		
		if (fileDesID != -1)
		{
			print("File des - ");
			print(fileDes);
			print(" - exist.\n");
			print("Do you want to overwrite?\n");
			print("\n0: No, 1: Yes? ");
			choose = ScanChar();
			
			CloseFile(fileDesID);
		}
		
		if (choose == '1')
		{
			CreateFile(fileDes);
			fileDesID = Open(fileDes, 0);
			
			while (res >= 0)
			{
				res = Read(str, LIMIT, fileSrcID);
				if (res < 0) break;
			
				//if (fMoreLine) Write("\n", 1, fileDesID);
				Write(str, res - 1, fileDesID);
			
				fMoreLine = 1;
			} 
			CloseFile(fileSrcID);
			CloseFile(fileDesID);
		}
	}
	else
	{
		print("Open file ");
		print(fileSrc);
		print(" failure.\n");
	}
	
	print("\n------------Ended-----------\n");
	Halt();
}
