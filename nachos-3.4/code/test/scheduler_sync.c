#include "syscall.h"
int main()
{
	int pingPID, pongPID;
	CreateSemaphore("A", 1);
	CreateSemaphore("B", 0);
	
	print("Ping-Pong test starting ...\n\n");
	pingPID = Exec("./test/ping_sync");
	pongPID = Exec("./test/pong_sync");
	
	Join(pingPID);
	Join(pongPID);
}
