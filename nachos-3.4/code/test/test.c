#include "syscall.h"

int main()
{
    int id,res;
    id = Open("Hello.txt", 0);

    res =  ReadF("abc",4,id);
    PrintInt(res);
    Halt();
}
