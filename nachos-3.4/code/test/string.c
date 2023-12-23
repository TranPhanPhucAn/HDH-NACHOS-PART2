#include "syscall.h"

#define MAX_LENGTH 100

int main()
{
    char buffer[MAX_LENGTH];

    PrintString("Enter your string (max length = 100): ");

    ReadString(buffer, MAX_LENGTH);
    PrintString("You entered: ");
    PrintString(buffer);
    PrintString("\n");

    Halt();
}
