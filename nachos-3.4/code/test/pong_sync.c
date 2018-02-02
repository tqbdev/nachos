#include "syscall.h"

void main()
{
	int i;
	for (i = 0; i<1000; i++)
	{
		Down("B");
		PrintChar('B');
		Up("A");
	}
}
