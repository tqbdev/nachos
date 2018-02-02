#include "syscall.h"
#include "copyright.h"

int 
main()
{
	int fileID = -1;
	char res = '1';
	
	int len;
	char filename[MaxFileLength + 1];
	
	/* Get file name */
	print("Enter file name: ");
	len = ScanLine(filename, MaxFileLength);
	
	/* Check file exist */
	fileID = Open(filename, 1);	
	if (fileID != -1)
	{
		print("\nFile exist!");
		print("\nDo you want remove old file and create empty file?");
		print("\n0: No, 1: Yes? ");
		res = ScanChar();	
		CloseFile(fileID);
	}
		
	if (res == '1')
	{
		/* Create a file */
		if (CreateFile(filename) == -1)
		{
			print("\nCreate file ");
			print(filename);
			print(" fail.\n");
		}
		else
		{
			print("\nCreate file ");
			print(filename);
			print(" success.\n");
		}
	}
	else
	{
		print("\nExit...");
	}
	
	print("\n------------Ended-----------\n");
	Halt();
}
