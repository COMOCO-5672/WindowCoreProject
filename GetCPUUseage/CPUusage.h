#pragma once


#include <Windows.h> 
//原理：调用GetProcessTimes()，并与上次调用得到的结果相减，即得到某段时间内CPU的使用时间
//C++ 获取特定进程规定CPU使用率  原文：http://blog.csdn.net/liuqx97bb/article/details/52058657
class CPUusage {
private:
	typedef long long          int64_t;
	typedef unsigned long long uint64_t;
	HANDLE _hProcess;
	int _processor;    //cpu数量  
	int64_t _last_time;         //上一次的时间  
	int64_t _last_system_time;


	// 时间转换  
	uint64_t file_time_2_utc(const FILETIME *ftime);

	// 获得CPU的核数  
	int get_processor_number();

	//初始化
	void init()
	{
		_last_system_time = 0;
		_last_time = 0;
		_hProcess = 0;
	}

	//关闭进程句柄
	void clear()
	{
		if (_hProcess) {
			CloseHandle(_hProcess);
			_hProcess = 0;
		}
	}

public:
	CPUusage(DWORD ProcessID)
	{
		init();
		_processor = get_processor_number();
		setpid(ProcessID);
	}
	CPUusage() { init(); _processor = get_processor_number(); }
	~CPUusage() { clear(); }

	//返回值为进程句柄，可判断OpenProcess是否成功
	HANDLE setpid(DWORD ProcessID)
	{
		clear();    //如果之前监视过另一个进程，就先关闭它的句柄
		init();
		return _hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, ProcessID);
	}

	//-1 即为失败或进程已退出； 如果成功，首次调用会返回-2（中途用setpid更改了PID后首次调用也会返回-2）
	float get_cpu_usage();
};