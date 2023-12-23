/* help.c
 *	A simple program to output name of each members
 */

#include "syscall.h"

int main()
{
    	PrintString("==================================================\n");
	PrintString("\tOS - NachOS Project\n");
	PrintString("\n");
	PrintString("1/ Members\n");
	PrintString("\t21120407 - Tran Phan Phuc An\n");
	PrintString("\t21120409 - Nguyen Duc Duy Anh\n");
	PrintString("\t21120423 - Pham Manh Cuong\n");
	PrintString("\t21120451 - Le Bao Hieu\n");
	PrintString("\t21120496 - Chu Hai Linh\n");
        PrintString("2/ Available programs\n");
	PrintString("\tascii: Print the ASCII TABLE.\n");
	PrintString("\tsort: The user enter an array of integers.\n");
	PrintString("\t      Use bubble sort to sort that array.\n");
	PrintString("==================================================\n");
	Halt();
}

