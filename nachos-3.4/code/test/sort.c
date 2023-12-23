/* sort.c 
 *  Sort user-input array with bubble sort algorithm
 */

#include "syscall.h"

int main() {
	int n, i, j, tmp;
	int a[100];
	PrintString("=====SORTING=====\n");
	n = 0;
	while (n < 1 || n > 100) {
		PrintString("Input array size (0 < n <= 100): ");
		n = ReadInt();
		if (n < 1 || n > 100) {
			PrintString("Invalid input\n");
		}
	}
	for (i = 0; i < n; ++i) {
        	PrintString("a[");
        	PrintInt(i);
        	PrintString("] = ");
       		a[i] = 0;
        	a[i] = ReadInt();
	}
	PrintString("Before sorting: ");
	PrintChar('[');
	for (i = 0; i < n; ++i) {
        	PrintInt(a[i]);
		if (i != n - 1) PrintChar(' ');
		else PrintChar(']');
	}
	PrintChar('\n');
    	for (i = 0; i < n - 1; ++i) {
        	for (j = 0; j < (n - 1 - i); ++j) {
			if (a[j] > a[j + 1]) {
	      			tmp = a[j];
	     			a[j] = a[j + 1];
     			 	a[j + 1] = tmp;
    	   		}
		}
	}
	
	PrintString("After sorting: ");
	PrintChar('[');
	for (i = 0; i < n; ++i) {
        	PrintInt(a[i]);
		if (i != n - 1) PrintChar(' ');
		else PrintChar(']');
	}
	Halt();
}

