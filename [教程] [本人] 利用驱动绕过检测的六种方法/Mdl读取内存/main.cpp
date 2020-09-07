#include <Windows.h>
#include <iostream>

/* 读取内存 */
#define Mdl_Read CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ALL_ACCESS)

/* 写入内存 */
#define Mdl_Write CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ALL_ACCESS)

/* 连接符 */
const  char* g_link = "\\??\\{F90B1129-715C-4F84-A069-FEE12E2AFB48}";

/* 传递信息的结构 */
typedef struct _UserData
{
	DWORD Pid;							//要读写的进程ID
	DWORD64 Address;				//要读写的地址
	DWORD Size;							//读写长度
	PBYTE Data;								//要读写的数据
}UserData, *PUserData;

HANDLE hDriver = NULL;

template<class T>
T read(DWORD pid, DWORD64 addr)
{
	T result{};

	UserData buf{ 0 };
	buf.Pid = pid;
	buf.Address = addr;
	buf.Data = (PBYTE)&result;
	buf.Size = sizeof(T);

	DWORD dwSize = 0;
	DeviceIoControl(hDriver, Mdl_Read, &buf, sizeof(buf), &buf, sizeof(buf), &dwSize, NULL);

	return result;
}

template<class T>
void write(DWORD pid, DWORD64 addr, T value)
{
	UserData buf{ 0 };
	buf.Pid = pid;
	buf.Address = addr;
	buf.Data = (PBYTE)&value;
	buf.Size = sizeof(T);

	DWORD dwSize = 0;
	DeviceIoControl(hDriver, Mdl_Write, &buf, sizeof(buf), &buf, sizeof(buf), &dwSize, NULL);
}

void test()
{
	hDriver = CreateFileA(g_link,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDriver == INVALID_HANDLE_VALUE)
	{
		printf("[-] 驱动打开失败 %d \n", GetLastError());
		return;
	}

	int data = read<int>(14332, 0x88c000);
	printf("[+] 读取到数据 : %d \n", data);

	write<int>(14332, 0x88c000, 30);

	CloseHandle(hDriver);
}

int main(int argc, char* argv[])
{
	test();

	system("pause");
	return 0;
}