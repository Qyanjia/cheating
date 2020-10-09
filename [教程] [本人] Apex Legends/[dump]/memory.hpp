#pragma once

#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <winioctl.h>
#include <stdint.h>

//连接符
const wchar_t* symbols = L"\\\\.\\{ED2761FC-91F4-4E1E-A441-19117D9FAC59}";

//设备句柄
HANDLE g_device = INVALID_HANDLE_VALUE;

//读取结构
struct KB_READ_PROCESS_MEMORY_IN
{
	UINT64 ProcessId;
	PVOID BaseAddress;
	PVOID Buffer;
	ULONG Size;
};

//写入结构
struct KB_WRITE_PROCESS_MEMORY_IN
{
	UINT64 ProcessId;
	PVOID BaseAddress;
	PVOID Buffer;
	ULONG Size;
	BOOLEAN PerformCopyOnWrite;
};

/* 发送IOCTL */
BOOL SendIOCTL(
	IN HANDLE hDevice,
	IN DWORD Ioctl,
	IN PVOID InputBuffer,
	IN ULONG InputBufferSize,
	IN PVOID OutputBuffer,
	IN ULONG OutputBufferSize,
	OPTIONAL OUT PDWORD BytesReturned = NULL,
	OPTIONAL IN DWORD Method = 3)
{
	DWORD RawIoctl = CTL_CODE(0x8000, Ioctl, Method, FILE_ANY_ACCESS);
	DWORD Returned = 0;
	BOOL Status = DeviceIoControl(hDevice, RawIoctl, InputBuffer, InputBufferSize, OutputBuffer, OutputBufferSize, &Returned, NULL);
	if (BytesReturned) *BytesReturned = Returned;
	return Status;
}

/* 发送请求 */
BOOL KbSendRequest(
	int Index,
	IN PVOID Input = NULL,
	ULONG InputSize = 0,
	OUT PVOID Output = NULL,
	ULONG OutputSize = 0)
{
	if (g_device) return SendIOCTL(g_device, 0x800 + Index, Input, InputSize, Output, OutputSize);
	else MessageBoxA(0, 0, 0, 0);
	return 0;
}

/* 写入内存 */
BOOL KbWriteProcessMemory(
	ULONG ProcessId,
	OUT PVOID BaseAddress,
	IN PVOID Buffer,
	ULONG Size,
	BOOLEAN PerformCopyOnWrite = FALSE)
{
	if (!ProcessId || !BaseAddress || !Buffer || !Size) return FALSE;
	KB_WRITE_PROCESS_MEMORY_IN Input = {};
	Input.ProcessId = ProcessId;
	Input.BaseAddress = BaseAddress;
	Input.Buffer = reinterpret_cast<PVOID>(Buffer);
	Input.Size = Size;
	Input.PerformCopyOnWrite = PerformCopyOnWrite;
	return KbSendRequest(65, &Input, sizeof(Input));
}

/* 读取内存 */
BOOL KbReadProcessMemory(
	ULONG ProcessId,
	IN PVOID BaseAddress,
	OUT PVOID Buffer,
	ULONG Size)
{
	if (!ProcessId || !BaseAddress || !Buffer || !Size) return FALSE;
	KB_READ_PROCESS_MEMORY_IN Input = {};
	Input.ProcessId = ProcessId;
	Input.BaseAddress = BaseAddress;
	Input.Buffer = reinterpret_cast<PVOID>(Buffer);
	Input.Size = Size;
	return KbSendRequest(64, &Input, sizeof(Input));
}

/* 读取内存 */
template<class T>
T read(DWORD32 process_id, DWORD64 addr, DWORD32 size = sizeof(T))
{
	T result{};

	KbReadProcessMemory((ULONG)process_id, (PVOID)addr, (PVOID)&result, (ULONG)size);
	return result;
}

/* 读取数组 */
uint8_t* read_array(DWORD32 process_id, DWORD64 addr, DWORD32 size)
{
	uint8_t* data = new uint8_t[size];
	if (data)
	{
		memset(data, 0, size);
		KbReadProcessMemory((ULONG)process_id, (PVOID)addr, (PVOID)data, (ULONG)size);
	}
	return data;
}

/* 写入内存 */
template<class T>
void write(DWORD32 process_id, DWORD64 addr, T buf, DWORD32 size = sizeof(T))
{
	KbWriteProcessMemory((ULONG)process_id, (PVOID)addr, (PVOID)&buf, (ULONG)size, FALSE);
}

/* 打开设备 */
HANDLE open_device(LPCWSTR NativeDeviceName = symbols)
{
	g_device = CreateFileW(NativeDeviceName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	return g_device;
}