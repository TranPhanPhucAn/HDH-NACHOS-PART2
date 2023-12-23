/* ascii.c
 *	A simple program to print out printable ascii characters
 */

#include "syscall.h"

int main() {
	int c;
    	PrintString("=============ASCII TABLE=============\n");
	for (c = 32; c < 64; ++c) {
		PrintInt(c);
		PrintString(": ");
		PrintChar(c);

		PrintString("\t\t");
		PrintInt(c + 32);
		PrintString(": ");
		PrintChar(c + 32);

		PrintString("\t\t");
		if (c + 64 != 127) {
			PrintInt(c + 64);
			PrintString(": ");
			PrintChar(c + 64);
		}
		PrintChar('\n');
	}
	Halt();
}

