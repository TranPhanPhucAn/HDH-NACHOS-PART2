// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "directory.h"
#define MaxFileLength 32
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

// Change program counter of system after 4 byte to continue load next calls.
void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    // printf("\n Filename u2s:");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        // printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

void ExceptionHandlerReadInt()
{
    int maxLenBuffer = 11;
    char *buffer = new char[maxLenBuffer + 1];
    int res = 0;
    // Read input buffer and return number of byte read
    int len = gSynchConsole->Read(buffer, maxLenBuffer);
    int i = (buffer[0] == '-') ? 1 : 0;
    bool checkDot = false;
    for (i; i < len; i++)
    {
        if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            if (buffer[i] - '0' && checkDot)
                goto next;
            if (!checkDot)
                res = res * 10 + (buffer[i] - '0');
        }
        else if (buffer[i] == '.')
        {
            checkDot = true;
        }
        else
        {
        next:
            DEBUG('a', "\nInvalid input.");
            // Return 0 if invalid input
            machine->WriteRegister(2, 0);
            printf("\n Invalid input.");
            delete buffer;
            return;
        }
    }
    res = (buffer[0] == '-') ? (-1 * res) : res;
    machine->WriteRegister(2, res);
    delete buffer;
    return;
}

void ExceptionHandlerPrintInt()
{
    int number = machine->ReadRegister(4);
    int maxLen = 11;
    char *buffer = new char[maxLen + 1];
    char *temp_buffer = new char[maxLen + 1];
    int len = 0;
    int i = 0;
    if (number < 0)
    {
        number *= -1;
        buffer[i++] = '-';
        len = 1;
    }
    do
    {
        temp_buffer[len++] = number % 10;
        number /= 10;
    } while (number);
    for (int j = len - 1; j >= 0; j--)
        buffer[i++] = '0' + (char)temp_buffer[j];
    gSynchConsole->Write(buffer, len);
    delete buffer, temp_buffer;
}

void ExceptionHandlerReadChar()
{
    char c = 0;
    gSynchConsole->Read(&c, 1);
    machine->WriteRegister(2, (int)c);
}
void ExceptionHandlerPrintChar()
{
    char c = (char)machine->ReadRegister(4);
    gSynchConsole->Write(&c, 1);
    machine->WriteRegister(2, 0);
}

void ExceptionHandlerReadString()
{
    int buffeAddress = machine->ReadRegister(4);
    int maxLength = machine->ReadRegister(5);

    char *buffer = new char[maxLength + 1];
    int len = gSynchConsole->Read(buffer, maxLength);

    buffer[len] = '\0';

    System2User(buffeAddress, len + 1, buffer);
    delete buffer;
}

void ExceptionHandlerPrintString()
{
    int bufferAddress = machine->ReadRegister(4);
    char *buffer = User2System(bufferAddress, MAX_STRING_SIZE);

    int len = strlen(buffer);

    for (int i = 0; i < len; i++)
    {
        gSynchConsole->Write(&buffer[i], 1);
    }

    char nullChar = '\0';
    gSynchConsole->Write(&nullChar, 1);

    delete buffer;
}
// Output: Loi tra ve -1, thanh cong tra ve 0
// Chuc nang: tao ra file moi voi tham so la ten file
void ExceptionHandleCreateFile()
{
    // Load file name
    int virtAddr = machine->ReadRegister(4);
    char *fileName = User2System(virtAddr, MaxFileLength + 1);
    // Check valid file name and memory enough
    if (!strlen(fileName) || fileName == NULL)
    {
        printf("\n Unable to read filename!");
        DEBUG('a', "\n Unable to read filename!");
        machine->WriteRegister(2, -1);
        return;
    }
    // Create new file
    bool success = fileSystem->Create(fileName, 0);
    if (success)
        machine->WriteRegister(2, 0);
    else
        machine->WriteRegister(2, -1);
    delete[] fileName;
}

// Input: arg1: Dia chi cua chuoi name, arg2: type
// Output: Tra ve OpenFileID neu thanh cong, -1 neu loi
// Chuc nang: Tra ve ID cua file.

void ExceptionHandlerOpenFile()
{
    // Load file name
    int virtAddr = machine->ReadRegister(4);
    int type = machine->ReadRegister(5);
    char *fileName = User2System(virtAddr, MaxFileLength + 1);

    // Open file
    OpenFile *pFile = fileSystem->Open(fileName);
    if (pFile == NULL)
    {
        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    // Add open file to ftable
    int ret = gFTable->Open(pFile, type);
    if (ret == -1)
    {
        delete pFile;
        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }
    else
    {
        machine->WriteRegister(2, ret + 2);
        delete[] fileName;
        return;
    }
}
// Input: Id cua file
// Output: tra ve 1 neu thanh cong, 0 neu that bai
// Dong file co idFile tu tham so
void ExceptionHandlerCloseFile()
{
    // Load fileID
    int fileID = machine->ReadRegister(4);

    if (fileID == ConsoleInput || fileID == ConsoleOutput)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    // fid exception
    fileID -= 2;
    int ret = gFTable->Close(fileID);
    machine->WriteRegister(2, ret);
}

void ExceptionHandlerReadFile()
{
    int buf = machine->ReadRegister(4);
    int count = machine->ReadRegister(5);
    int fileID = machine->ReadRegister(6);
    if (count < 0)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    int i = 0;
    if (fileID == ConsoleInput)
    {
        /*read from console input*/
        while (i < count)
        {
            char c = 0;
            int result = gSynchConsole->Read(&c, 1);
            if (result == -1)
            {
                machine->WriteRegister(2, -2);
                return;
            }
            else if (result == 0)
                break;
            machine->WriteMem(buf + i, 1, (int)c);
            ++i;
        }
        machine->WriteRegister(2, i);
        return;
    }
    /*read from file*/
    fileID -= 2;
    if (gFTable->getType(fileID) == -1)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    while (i < count)
    {
        char c = 0;
        if (gFTable->ReadChar(c, fileID) == 0)
            break;
        machine->WriteMem(buf + i, 1, (int)c);
        ++i;
    }
    machine->WriteRegister(2, i);
}
void ExceptionHandlerWriteFile()
{
    int buf = machine->ReadRegister(4);
    int count = machine->ReadRegister(5);
    int fileID = machine->ReadRegister(6);
    if (count < 0)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    int i = 0;
    if (fileID == ConsoleOutput)
    {
        /*write to console output*/
        while (i < count)
        {
            int c = 0;
            bool result = machine->ReadMem(buf + i, 1, &c);
            if (!result)
            {
                machine->WriteRegister(2, -1);
                return;
            }
            char cr = (char)c;
            gSynchConsole->Write(&cr, 1);
            ++i;
        }
        machine->WriteRegister(2, i);
        return;
    }
    /*write to file*/
    fileID -= 2;
    if (gFTable->getType(fileID) != 0)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    while (i < count)
    {
        int c = 0;
        bool result = machine->ReadMem(buf + i, 1, &c);
        if (!result)
        {
            machine->WriteRegister(2, -1);
            return;
        }
        char cr = (char)c;
        gFTable->WriteChar(cr, fileID);
        ++i;
    }
    machine->WriteRegister(2, i);
}

void ExceptionHandleExec()
{
    // Input: vi tri int
    // Output: Fail return -1, Success: return id cua thread dang chay
    // SpaceId Exec(char *name);
    int virtAddr;
    virtAddr = machine->ReadRegister(4); // doc dia chi ten chuong trinh tu thanh ghi r4
    char *name;
    name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel

    if (name == NULL)
    {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        machine->WriteRegister(2, -1);
        // IncreasePC();
        return;
    }
    OpenFile *oFile = fileSystem->Open(name);
    if (oFile == NULL)
    {
        printf("\nExec:: Can't open this file.");
        machine->WriteRegister(2, -1);
        // IncreasePC();
        return;
    }

    delete oFile;

    // Return child process id
    int id = pTab->ExecUpdate(name);
    machine->WriteRegister(2, id);

    delete[] name;
    return;
}
void ExceptionHandleJoin()
{
    int id = machine->ReadRegister(4);

    int res = pTab->JoinUpdate(id);

    machine->WriteRegister(2, res);
    return;
}
void ExceptionHandleExit()
{
    int exitStatus = machine->ReadRegister(4);

    if (exitStatus != 0)
    {
        return;
    }

    int res = pTab->ExitUpdate(exitStatus);
    // machine->WriteRegister(2, res);

    currentThread->FreeSpace();
    currentThread->Finish();
    return;
}
void ExceptionHandlerCreateSemaphore()
{
    int name = machine->ReadRegister(4);
    int semval = machine->ReadRegister(5);
    if (name == 0 || semval < 0)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    /*use SEM_MAXNAMESIZE to get name*/
    char s[SEM_MAXNAMESIZE] = {0};
    for (int i = 0; i < SEM_MAXNAMESIZE - 1; ++i)
    {
        int oneChar = 0;
        machine->ReadMem(name + i, 1, &oneChar);
        if (oneChar == 0)
            break;
        s[i] = (char)oneChar;
    }
    /*create semaphore*/
    int ret = semTab->Create(s, semval);
    machine->WriteRegister(2, ret);
}
void ExceptionHandlerWait()
{
    int name = machine->ReadRegister(4);
    if (name == 0)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    /*use SEM_MAXNAMESIZE to get name*/
    char s[SEM_MAXNAMESIZE] = {0};
    for (int i = 0; i < SEM_MAXNAMESIZE - 1; ++i)
    {
        int oneChar = 0;
        machine->ReadMem(name + i, 1, &oneChar);
        if (oneChar == 0)
            break;
        s[i] = (char)oneChar;
    }
    /*wait semaphore*/
    int ret = semTab->Wait(s);
    machine->WriteRegister(2, ret);
}
void ExceptionHandlerSignal()
{
    int name = machine->ReadRegister(4);
    if (name == 0)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    /*use SEM_MAXNAMESIZE to get name*/
    char s[SEM_MAXNAMESIZE] = {0};
    for (int i = 0; i < SEM_MAXNAMESIZE - 1; ++i)
    {
        int oneChar = 0;
        machine->ReadMem(name + i, 1, &oneChar);
        if (oneChar == 0)
            break;
        s[i] = (char)oneChar;
    }
    /*signal semaphore*/
    int ret = semTab->Signal(s);
    machine->WriteRegister(2, ret);
}
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException:
        return;
    case PageFaultException:
        DEBUG('a', "\nPage Fault Exception.");
        printf("\n\nPage Fault Exception.");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "\nRead Only Exception.");
        printf("\n\nRead Only Exception.");
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "\nBus Error Exception.");
        printf("\n\nBus Error Exception.");
        interrupt->Halt();
        break;
    case AddressErrorException:
        DEBUG('a', "\nAddress Error Exception.");
        printf("\n\nAddress Error Exception.");
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "\nOverflow Exception.");
        printf("\n\nOverflow Exception.");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "\nIllegal Instruction Exception.");
        printf("\n\nIllegal Instruction Exception.");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "\nNum Exception Types.");
        printf("\n\nNum Exception Types.");
        interrupt->Halt();
        break;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG('a', "\nShutdown, initiated by user program.");
            printf("\n\nShutdown, initiated by user program.");
            interrupt->Halt();
            break;
        case SC_ReadChar:
            ExceptionHandlerReadChar();
            IncreasePC();
            break;
        case SC_PrintChar:
            ExceptionHandlerPrintChar();
            IncreasePC();
            break;
        case SC_ReadInt:
            ExceptionHandlerReadInt();
            IncreasePC();
            break;
        case SC_PrintInt:
            ExceptionHandlerPrintInt();
            IncreasePC();
            break;
        case SC_ReadString:
            ExceptionHandlerReadString();
            IncreasePC();
            break;
        case SC_PrintString:
            ExceptionHandlerPrintString();
            IncreasePC();
            break;
        case SC_Create:
            ExceptionHandleCreateFile();
            IncreasePC();
            break;
        case SC_Open:
            ExceptionHandlerOpenFile();
            IncreasePC();
            break;
        case SC_Close:
            ExceptionHandlerCloseFile();
            IncreasePC();
            break;
        case SC_Read:
            ExceptionHandlerReadFile();
            IncreasePC();
            break;
        case SC_Write:
            ExceptionHandlerWriteFile();
            IncreasePC();
            break;
        case SC_Exec:
            ExceptionHandleExec();
            IncreasePC();
            break;
        case SC_Join:
            ExceptionHandleJoin();
            IncreasePC();
            break;
        case SC_Exit:
            ExceptionHandleExit();
            IncreasePC();
            break;
        case SC_CreateSemaphore:
            ExceptionHandlerCreateSemaphore();
            IncreasePC();
            break;
        case SC_Wait:
            ExceptionHandlerWait();
            IncreasePC();
            break;
        case SC_Signal:
            ExceptionHandlerSignal();
            IncreasePC();
            break;
        }

        break;

    default:
        printf("\n Unexpected user mode exception (%d %d)", which,
               type);
        interrupt->Halt();
    }
}
