#include "syscall.h"
#include "copyright.h"

#define MaxFileLength 32

int 
main()
{
	int res;
	char str[LIMIT];
	int len = 32;
	int fileID = Open("hello.txt", 1);
	
	if (fileID != -1)
		print("Success\n");
	while (1)
	{
	res = ScanLine(str, LIMIT);
	
	res = Write(str, res, ConsoleOutput);
	}
	print("\n");
	Halt();
}
