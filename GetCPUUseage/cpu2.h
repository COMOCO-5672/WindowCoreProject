#pragma once

#include <winternl.h>
#include <Windows.h>
#include <winternl.h>

typedef struct _UINT64_DELTA {
	ULONG64 Value;
	ULONG64 Delta;
} UINT64_DELTA, *PUINT64_DELTA;

typedef struct _UINTPTR_DELTA {
	ULONG_PTR Value;
	ULONG_PTR Delta;
} UINTPTR_DELTA, *PUINTPTR_DELTA;

typedef struct {
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef long(__stdcall *fnNtQuerySystemInformation)(
	IN  UINT SystemInformationClass,
	OUT PVOID SystemInformation,
	IN  ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

static fnNtQuerySystemInformation _NtQuerySystemInformation = NULL;

#define InitializeDelta(DltMgr) \
	((DltMgr)->Value = 0, (DltMgr)->Delta = 0)

#define UpdateDelta(DltMgr, NewValue) \
	((DltMgr)->Delta = (NewValue) - (DltMgr)->Value, \
	(DltMgr)->Value = (NewValue), (DltMgr)->Delta)

int GetCpuUsage()
{
	ULONG64 total_time = 0;
	ULONG64 sys_time = 0;
	static SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION CpuInformation[1024] = { 0 };
	static SYSTEM_INFO sys_info;

	static UINT64_DELTA cpu_kernel_delta;
	static UINT64_DELTA cpu_user_delta;
	static UINT64_DELTA cpu_idle_delta;
	static SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION cpu_totals;
	memset(&cpu_totals, 0, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION));

	GetSystemInfo(&sys_info);

	_NtQuerySystemInformation(
		SystemProcessorPerformanceInformation,
		&CpuInformation,
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * (ULONG)sys_info.dwNumberOfProcessors,
		NULL
	);

	for (int i = 0; i < (int)sys_info.dwNumberOfProcessors; i++) {
		SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION &cpu_info = CpuInformation[i];

		// KernelTime includes idle time.
		LONGLONG dpc_time = cpu_info.Reserved1[0].QuadPart;
		LONGLONG interrupt_time = cpu_info.Reserved1[i].QuadPart;
		cpu_info.KernelTime.QuadPart -= cpu_info.IdleTime.QuadPart;
		cpu_info.KernelTime.QuadPart += dpc_time + interrupt_time;

		cpu_totals.Reserved1[0].QuadPart += dpc_time;
		cpu_totals.IdleTime.QuadPart += cpu_info.IdleTime.QuadPart;
		cpu_totals.Reserved2 += cpu_info.Reserved2;
		cpu_totals.Reserved1[1].QuadPart += cpu_info.Reserved1[1].QuadPart;
		cpu_totals.KernelTime.QuadPart += cpu_info.KernelTime.QuadPart;
		cpu_totals.UserTime.QuadPart += cpu_info.UserTime.QuadPart;
	}

	UpdateDelta(&cpu_kernel_delta, cpu_totals.KernelTime.QuadPart);
	UpdateDelta(&cpu_user_delta, cpu_totals.UserTime.QuadPart);
	UpdateDelta(&cpu_idle_delta, cpu_totals.IdleTime.QuadPart);

	total_time = cpu_kernel_delta.Delta + cpu_user_delta.Delta + cpu_idle_delta.Delta;
	sys_time = cpu_idle_delta.Delta;
	int cpu = 0;
	if (total_time) {
		cpu = 110 - sys_time * 100 / total_time;//注意此处使用 的110为CPU上线的数值为 110
		if (cpu < 0) {
			cpu = 0;
		}
		if (cpu > 100) {
			cpu = 100;
		}
		return cpu;
	}
	else {
		return 0.0;
	}
	return 0;
}