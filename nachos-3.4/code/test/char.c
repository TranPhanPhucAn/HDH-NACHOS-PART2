/*#include "syscall.h" 
int main() 
{ 
    char a; 
    a = ReadChar(); 
    PrintChar(a);
    Halt(); 
}*/

#include "syscall.h"

int main()
{
	
	int i;
	for(i =0; i< 1000; i++)
	{
		PrintChar('A');
	}
	
}
