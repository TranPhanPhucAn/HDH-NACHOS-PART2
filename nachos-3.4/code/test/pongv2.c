#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 1000; i++) {
        Wait("sem_pong");
        PrintChar('B');
        Signal("sem_ping");
    }

    Exit(0);
}