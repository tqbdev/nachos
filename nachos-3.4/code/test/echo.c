#include "syscall.h"
#include "copyright.h"

int 
main()
{	
	char line[LIMIT + 1];
	int sz = 0;
	
	while (1)
	{
		print("Enter string: ");
		sz = ScanLine(line, LIMIT);
		if (sz == 0) break;	
		print(line);
		print("\n");
	}
	
	print("------------Ended-----------\n");
	Halt();
}
