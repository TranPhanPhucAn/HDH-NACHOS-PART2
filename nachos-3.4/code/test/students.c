#include "syscall.h"
#include "copyright.h"

void main()
{
    // Khai bao
	int flagSuccess; // Bien co dung de kiem tra thanh cong
	SpaceId studentId, watertapId;	// Bien id cho file
	char cFile;	// Bien ki tu luu ki tu doc tu file
	int flagWatertap;		// Bien co de nhay den tien trinh voinuoc
	int flagMain;		// Bien co de nhay den tien trinh main
	int lengthFile;		// Luu do dai file
	int iFile;		// Luu con tro file
	//-----------------------------------------------------------
	Signal("m_wt");	

	while(1)
	{
		lengthFile = 0;

		Wait("students");
		

		// Tao file result.txt de ghi voi nao su dung
		flagSuccess = Create("result.txt");
		if(flagSuccess == -1)
		{
			Signal("main"); // tro ve tien trinh chinh
			return;
		}

		// Mo file students.txt len de doc
		studentId = Open("students.txt", 1);
		if(studentId == -1)
		{
			Signal("main"); // tro ve tien trinh chinh
			return;
		}
		
		lengthFile = Seek(-1, studentId);
		Seek(0, studentId);
		iFile = 0;
	
		// Tao file watertap.txt
		flagSuccess = Create("watertap.txt");
		if(flagSuccess == -1)
		{
			Close(studentId);
			Signal("main"); // tro ve tien trinh chinh
			return;
		}
		

		// Mo file watertap.txt de ghi tung dung tich nuoc cua sinhvien
		watertapId = Open("watertap.txt", 0);
		if(watertapId == -1)
		{
			Close(studentId);
			Signal("main"); // tro ve tien trinh chinh
			return;
		}
		
		// Ghi dung tich vao file watertap.txt tu file sinhvien.txt
		while(iFile < lengthFile)
		{
			flagWatertap = 0;
			Read(&cFile, 1, studentId);
			if(cFile != ' ')
			{
				Write(&cFile, 1, watertapId);
			}
			else
			{
				flagWatertap = 1;
			}
			if(iFile == lengthFile - 1)
			{
				Write("*", 1, watertapId);
				flagWatertap = 1;
			}
			
				
			if(flagWatertap == 1)
			{
				Close(watertapId);
				Signal("watertap");
				// Dung chuong trinh sinhvien lai de voinuoc thuc thi
				Wait("students");
				
				// Tao file watertap.txt
				flagSuccess = Create("watertap.txt");
				if(flagSuccess == -1)
				{
					Close(studentId);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
		

				// Mo file watertap.txt de ghi tung dung tich nuoc cua sinhvien
				watertapId = Open("watertap.txt", 0);
				if(watertapId == -1)
				{
					Close(studentId);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
				
			}
			iFile++;			
							
		}				
		// Ket thuc tien trinh sinhvien va voinuoc quay lai ham SvVn
		Signal("main");			
	}
	// Quay lai ham Svvn	
}
