#include "syscall.h"

int main()
{
    OpenFileID res;
    res = OpenF("hello.txt", 0);
    CloseF(res);
    Halt();

}
