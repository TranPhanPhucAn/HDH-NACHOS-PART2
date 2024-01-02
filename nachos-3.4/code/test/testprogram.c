#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 32


int main()
{
	// KHAI BAO
	int flagSuccess; // Bien co dung de kiem tra thanh cong
	SpaceId inputId, outputId, studentId, resultId;	// Bien id cho file
	int numTimes;	// Luu so luong thoi diem xet
	char cFile;	// Bien ki tu luu ki tu doc tu file
	//int flag;

	//-----------------------------------------------------------


	// Khoi tao 4 Semaphore de quan ly 3 tien trinh
	flagSuccess = CreateSemaphore("main",0);
	if(flagSuccess == -1)
		return 1;
	flagSuccess = CreateSemaphore("students", 0);
	if(flagSuccess == -1)
		return 1;
	flagSuccess = CreateSemaphore("watertap", 0);
	if(flagSuccess == -1)
		return 1;
	flagSuccess = CreateSemaphore("m_wt", 0);
	if(flagSuccess == -1)
		return 1;
	
	// Tao file output.txt de ghi ket qua cuoi cung	
	flagSuccess = Create("output.txt");
	if(flagSuccess == -1)
		return 1;
	
	// Mo file input.txt chi de doc
	inputId = Open("input.txt", 1);
	if(inputId == -1)
		return 1;
	
	// Mo file output.txt de doc va ghi
	outputId = Open("output.txt", 0);
	if(outputId == -1)
	{
		Close(inputId);
		return 1;
	}

	// Doc so luong thoi diem xet o file input.txt
	//**** Thuc hien xong doan lenh duoi thi con tro file o input.txt o dong 1
	numTimes = 0;
	while(1)
	{
		Read(&cFile, 1, inputId);
		if(cFile != '\n')
		{
			if(cFile >= '0' && cFile <= '9')
				numTimes = numTimes * 10 + (cFile - 48);
		}
		else
			break;
	}


	// Goi thuc thi tien trinh students.c
	flagSuccess = Exec("./test/students");
	if(flagSuccess == -1)
	{
		Close(inputId);
		Close(outputId);
		return 1;
	}

	// Goi thuc thi tien trinh watertap.c
	flagSuccess = Exec("./test/watertap");
	if(flagSuccess == -1)
	{
		Close(inputId);
		Close(outputId);
		return 1;
	}

	// Thuc hien xu ly khi nao het thoi diem xet thi thoi
	while(numTimes--)
	{
		// Tao file students.txt
		flagSuccess = Create("students.txt");
		if(flagSuccess == -1)
		{
			Close(inputId);
			Close(outputId);
			return 1;
		}
		
		// Mo file students.txt de ghi tung dong students tu file input.txt
		studentId = Open("students.txt", 0);
		if(studentId == -1)
		{
			Close(inputId);
			Close(outputId);
			return 1;
		}
		while(1)
		{
			if(Read(&cFile, 1, inputId) < 1)
			{
				// Doc toi cuoi file
				break;
			}
			if(cFile != '\n')
			{
				Write(&cFile, 1, studentId);				
			}
			else
				break;
						
		}
		// Dong file students.txt lai
		Close(studentId);
			
		// Goi tien trinh students hoat dong
		Signal("students");

		// Tien trinh chinh phai cho 
		Wait("main");	
		
		// Thuc hien doc file tu result va ghi vao ket qua o output.txt
		resultId = Open("result.txt", 1);
		if(resultId == -1)
		{
			Close(inputId);
			Close(outputId);
			return 1;
		}

		PrintString("\n Times: ");
		PrintInt(numTimes);
		PrintString("\n");	

		// Doc cac voi vao output.txt		
		while(1)
		{
			if(Read(&cFile, 1, resultId)  < 1)
			{
				Write("\r\n", 2, outputId);
				Close(resultId);
				Signal("m_wt");
				break;
			}
			Write(&cFile, 1, outputId);
			Write(" ", 1, outputId);
			
		}
		
	}
	Close(inputId);
	Close(outputId);
	return 0;	
}
