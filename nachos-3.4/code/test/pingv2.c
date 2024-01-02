#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 1000; i++) {
        Wait("sem_ping");
        PrintChar('A');
        Signal("sem_pong");
    }

    Exit(0);
}
