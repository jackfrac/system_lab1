#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <conio.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;//робимо повертаючий описувач наслідуваним
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* MutexName_1 = "Mutex1";
	wchar_t wtext1[20];
	mbstowcs(wtext1, MutexName_1, strlen(MutexName_1) + 1);
	LPWSTR MutexPtr1 = wtext1;//конвертуємо багатобайтову стрічку в стрічку широких символів

	HANDLE hMutex = CreateMutex(NULL, FALSE, MutexPtr1);//створюємо перший мютекс

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* SemaphoreName_1 = "Semaphore1";
	mbstowcs(wtext1, SemaphoreName_1, strlen(SemaphoreName_1) + 1);
	LPWSTR SemaphorePtr1 = wtext1;

	HANDLE hSemaphore_1 = CreateSemaphore(NULL, 1, 1, SemaphorePtr1);//створюємо перший семафор

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* SemaphoreName_2 = "Semaphore2";
	mbstowcs(wtext1, SemaphoreName_2, strlen(SemaphoreName_2) + 1);
	LPWSTR SemaphorePtr2	 = wtext1;

	HANDLE hSemaphore_2 = CreateSemaphore(NULL, 1, 1, SemaphorePtr2);//створюємо другий семафор

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* Timer_Name = "Timer";
	mbstowcs(wtext1, Timer_Name, strlen(Timer_Name) + 1);
	LPWSTR TimerPtr = wtext1;

	HANDLE hTimer = CreateWaitableTimer(NULL, true, TimerPtr);//створюємо таймер
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HANDLE hSemaphores[2];
	hSemaphores[0] = hSemaphore_1;
	hSemaphores[1] = hSemaphore_2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -500000LL;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (GetLastError() == ERROR_ALREADY_EXISTS) //відкритий опис існуючого обєкта, (FALSE) ігнорується
	{
		puts("Process #2:");
		puts("///////////////////////////////////////////////////////////////");
		printf("Mutex:\n %s already exists!\n", MutexName_1);
		printf("Semaphores:\n `%s` and `%s` already exists!\n", SemaphoreName_1, SemaphoreName_2);
		printf("Timer:\n %s already exists!\n", Timer_Name);
		puts("///////////////////////////////////////////////////////////////");

		puts("\nMutex:");
		
		HANDLE hMyMutexInherited = (HANDLE)atoi(argv[1]);//перетворює перший аргумент в інт

		printf("%s (%d) : 0x%X\n", argv[0], argc, hMyMutexInherited);

		///////////////////////////////////////////////////////////////////////////////////////////
		

		switch (WaitForSingleObject(hMutex, INFINITE))
		{
		case WAIT_OBJECT_0://процес завершується
		{
			puts("Got Mutex!");
			puts("Mutex to be released...");
			puts("///////////////////////////////////////////////////////////////");
			ReleaseMutex(hMyMutexInherited);//звільняємо мютекс(зменшуємо лічильник рекурсії в мютексі на 1)
		} break;

		////////////////////////////////////////////////////////////////////////////////////////////

		case WAIT_FAILED://неправильний виклик функції
		{
			puts("Don`t catch mutex!");
		} break;

		//////////////////////////////////////////////////////////////////////////////////////////////////

		case WAIT_ABANDONED://відмова від мютекса(при завершенні потака мютекса,не встигнувши його звільнити)
		{
			puts("Wait abandoned!");
		} break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		default:
			break;
		}

		puts("\nSemaphore:");
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		HANDLE hSemaphoreInherited_1 = (HANDLE)atoi(argv[2]);//другий аргумент
		printf("%s (%d) : 0x%X\n", argv[0], argc, hSemaphoreInherited_1);
		HANDLE hSemaphoreInherited_2 = (HANDLE)atoi(argv[3]);//третій аргумент
		printf("%s (%d) : 0x%X\n", argv[0], argc, hSemaphoreInherited_2);
		HANDLE hSemaphoresInherited[2];
		hSemaphoresInherited[0] = hSemaphoreInherited_1;
		hSemaphoresInherited[1] = hSemaphoreInherited_2;

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (WaitForMultipleObjects(2, hSemaphoresInherited, true, INFINITE) == WAIT_OBJECT_0)
		{
			std::cout << "Caught semaphores." << std::endl;
			ReleaseSemaphore(hSemaphoreInherited_1, 1, NULL);
			ReleaseSemaphore(hSemaphoreInherited_2, 1, NULL);
		}
		else
		{
			std::cout << "Error in waiting semaphores." << GetLastError() << std::endl;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::cout << "\nTimer:" << std::endl;
		
		HANDLE hTimerInherited = (HANDLE)atoi(argv[4]);
		printf("%s (%d) : 0x%X\n", argv[0], argc, hTimerInherited);

		std::cout << "Timer start for " << (long)&liDueTime << std::endl;

		SetWaitableTimer(hTimerInherited, &liDueTime, 0, NULL, NULL, 0);//встановлюємо таймер (коли має спрацювати вперше, як часто це має бути,...)

		if (WaitForSingleObject(hTimerInherited, INFINITE) != WAIT_OBJECT_0)
		{
			std::cout << "SetWaitableTimer failed: " << GetLastError() << std::endl;
		}
		else
		{
			std::cout << "\nTimer was signaled.\n\a";
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CloseHandle(hMyMutexInherited);
		CloseHandle(hMutex);

		CloseHandle(hSemaphoresInherited[0]);
		CloseHandle(hSemaphoresInherited[1]);
		CloseHandle(hSemaphores[0]);
		CloseHandle(hSemaphores[1]);

		CloseHandle(hTimerInherited);
		CloseHandle(hTimer);

		_getch();
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else
	{
		puts("Process #1:");
		puts("///////////////////////////////////////////////////////////////");

		HANDLE hMyMutexInherited = CreateMutex(&sa, FALSE, NULL);

		HANDLE hSemaphoreInherited_1 = CreateSemaphore(&sa, 1, 1, NULL);
		HANDLE hSemaphoreInherited_2 = CreateSemaphore(&sa, 1, 1, NULL);

		HANDLE hSemaphoresInherited[2];
		hSemaphoresInherited[0] = hSemaphoreInherited_1;
		hSemaphoresInherited[1] = hSemaphoreInherited_2;

		HANDLE hTimerInherited = CreateWaitableTimer(&sa, true, NULL);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (hMyMutexInherited != NULL || hSemaphoreInherited_1 != NULL || hSemaphoreInherited_2 != NULL || hTimerInherited != NULL)
		{
			printf("Mutex 0x%X was created.\n", hMyMutexInherited);
			printf("Semaphores 0x%X and 0x%X were created.\n", hSemaphoresInherited[0], hSemaphoresInherited[1]);
			printf("Timer 0x%X was created.\n", hTimerInherited);
			puts("///////////////////////////////////////////////////////////////");

			//створюємо другорядний процес для тестування
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			char cmdLn[255];
			sprintf_s(cmdLn, "SystemProgramming_#1 %d %d %d %d\0", hMyMutexInherited, hSemaphoreInherited_1, hSemaphoreInherited_2, hTimerInherited);

			wchar_t wtext[255];
			mbstowcs(wtext, cmdLn, strlen(cmdLn) + 1);
			LPWSTR ptr = wtext;

			_getch();

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if (CreateProcess(TEXT("D:\\системка\\ConsoleApplication1\\Debug\\ConsoleApplication1.exe"), ptr, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
			{
				puts("Process 2 was created.");
				puts("///////////////////////////////////////////////////////////////");
			}
			else
			{
				puts("Failed to create process!");
				puts("///////////////////////////////////////////////////////////////");
			}

			puts("Mutex is free");
			puts("Semaphores is free");
			puts("///////////////////////////////////////////////////////////////");

			system("pause>>void");

			ReleaseMutex(hMyMutexInherited);

			ReleaseSemaphore(hSemaphoresInherited[0], 1, NULL);
			ReleaseSemaphore(hSemaphoresInherited[1], 1, NULL);

			//////////////////////////////////////////////////////////////////////////////////////////////////
			
			CloseHandle(hMyMutexInherited);
			CloseHandle(hSemaphoresInherited[0]);
			CloseHandle(hSemaphoresInherited[1]);
			CloseHandle(hTimerInherited);
		}
		else
		{
			puts("Error with creating ");
		}

		CloseHandle(hMutex);
		CloseHandle(hSemaphores[0]);
		CloseHandle(hSemaphores[1]);
		CloseHandle(hTimer);

	}
	return 0;
}