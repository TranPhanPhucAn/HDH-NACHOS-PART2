#include "syscall.h"

int main() {
    int pingPID, pongPID;
    
    CreateSemaphore("sem_ping", 1);
    CreateSemaphore("sem_pong", 0);

    pingPID = Exec("./test/pingv2");
    pongPID = Exec("./test/pongv2");

    Join(pingPID);
    Join(pongPID);

    Exit(0);
}