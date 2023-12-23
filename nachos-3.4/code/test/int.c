/*#include "syscall.h"

int main() {
	int N;
	N = ReadInt();
	PrintInt(N);
	Halt();
	return 0;
    	
}*/
#include "syscall.h"


void main()
{
	int pingPID, pongPID;
	PrintString("Ping-Pong test starting...\n\n");
	pingPID = Exec("./test/char");
	pongPID = Exec("./test/help");
	Join(pingPID);
	Join(pongPID);	
}
