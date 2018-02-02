#include "syscall.h"
#include "copyright.h"

int 
main()
{
	char line[LIMIT];
	int sz = ScanLine(line, LIMIT);
	
	print(line);

	Halt();
}
