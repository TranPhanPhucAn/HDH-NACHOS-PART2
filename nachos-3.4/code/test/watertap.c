#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int flagSuccess;
	SpaceId  watertapId, resultId;	// Bien id cho file
	char cFile;	// Bien ki tu luu ki tu doc tu file
	int tap1, tap2;		// Voi 1, voi 2
	int volume;			// Dung tich nuoc cua sinh vien
	int flag;	// Bien co luu dau hieu doc xong file result

	//-----------------------------------------------------------
	
	tap1 = tap2 = 0;
	// Xu ly voi nuoc
	// WHILE(11111111111111111111111111111111111111)
	while(1)
	{
		Wait("m_wt");

		// Mo file result.txt de ghi voi nao su dung
		resultId = Open("result.txt", 0);
		if(resultId == -1)
		{		//?
			Signal("students");
			return;
		}
		// WHILE(2222222222222222222222222222222222222222)
		while(1)
		{
			Wait("watertap");
			cFile = 0;			
			// Mo file voi nuoc .txt de doc dung tich
			watertapId = Open("watertap.txt", 1);
			if(watertapId == -1)
			{
				//?
				Close(resultId);
				Signal("students");
			
				return;
			}
		
			volume = 0;
			flag = 0;
			// WHILE(3333333333333333333333333333333333333333333333)
			while(1)
			{			
				if(Read(&cFile, 1, watertapId)  == -2)
				{	
					Close(watertapId);			
					break;
				}
				if(cFile != '*')
				{
					volume = volume* 10 + (cFile - 48);
				}
				else
				{
					flag = 1;				
					Close(watertapId);
					break;			
				}
			
			}
			// WHILE(3333333333333333333333333333333333333333333333)
			if(volume!= 0)
			{
				// Dung voi 1
				if(tap1 <= tap2 )
				{
					tap1 += volume;
					Write("1", 1, resultId);
				}
				else	// Dung voi 2
				{					
					tap2 += volume;
					Write("2", 1, resultId);
					
				}
			}
		
			if(flag == 1)
			{
				tap1 = tap2 = 0;
				Close(resultId);				
				Signal("students");
				break;				
			}

			Signal("students");
		}
		// WHILE(2222222222222222222222222222222222222222)	
	}
	// WHILE(11111111111111111111111111111111111111)			
}
