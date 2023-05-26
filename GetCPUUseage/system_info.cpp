// system_info.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
//#include "CPUusage.h"
#include "cpu2.h"


int main()
{
	_NtQuerySystemInformation = (fnNtQuerySystemInformation)GetProcAddress(LoadLibrary(L"ntdll.dll"), "NtQuerySystemInformation");
    std::cout << "Hello World!\n";

	/*CPUusage usg(21420);
	for (int i = 0; i < 10; i++) {
		float cpu = usg.get_cpu_usage();
		printf("Taskmgr.exe: %.2f%%\n", cpu);
		Sleep(500);
	}

	usg.setpid(11084);
	for (int i = 0; i < 10; i++) {
		float cpu = usg.get_cpu_usage();
		printf("devenv.exe: %.2f%%\n", cpu);
		Sleep(1000);
	}*/

	for (int i = 0; i < 1000; i++) {
		int n = GetCpuUsage();
		printf("Taskmgr.exe: %d\n", n);
		Sleep(500);
	}

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
