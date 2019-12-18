// transdataexe.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <windows.h>
#include <iostream>
#include <WinIoCtl.h>

#include "ctl_code.h"

using namespace std;
#pragma warning(disable: 4789)

#pragma message( "Compiling " __FILE__ ) 
#pragma message( "Last modified on " __TIMESTAMP__ ) 
#pragma message("Remember to write some actual code in this sample app!!"))

int add_bufferred(HANDLE hDevice, int a, int b)
{
	int port[2] = {0x00};
	int buffret;
	ULONG dwWrite;
	port[0] = a;
	port[1] = b;
	::DeviceIoControl(hDevice, add_code, &port, 8, &buffret, 4, &dwWrite, NULL);
	return buffret;
}
int add_in_direct(HANDLE hDevice, int a, int b)
{
	int port[2] = { 0x00 };
	int buffret;
	ULONG dwWrite;
	port[0] = a;
	port[1] = b;
	::DeviceIoControl(hDevice, add_code1, &port, 8, &buffret, 4, &dwWrite, NULL);
	return buffret;
}

int sub_bufferred(HANDLE hDevice, int a, int b)
{
	int port[2] = { 0x00 };
	int buffret;
	ULONG dwWrite;
	port[0] = a;
	port[1] = b;
	::DeviceIoControl(hDevice, sub_code, &port, 8, &buffret, 4, &dwWrite, NULL);
	return buffret;
}

int sub_direct(HANDLE hDevice, int a, int b)
{
	int port[2] = { 0x00 };
	int buffret;
	ULONG dwWrite;
	port[0] = a;
	port[1] = b;
	::DeviceIoControl(hDevice, sub_code1, &port, 8, &buffret, 4, &dwWrite, NULL);
	return buffret;
}

int main()
{
	HANDLE hDevice = ::CreateFileA("\\\\.\\MySysData",
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		cout << "获取驱动句柄失败" <<GetLastError() <<endl;
	}
	int a=0, b=0;
	while ((a+b)!=100)
	{
		cin >> a >> b;
		int r = add_bufferred(hDevice, a, b);
		cout << a << " + " << b << " = " << r << endl;
		getchar();
	}
	CloseHandle(hDevice);
    std::cout << "Hello World!\n"; 
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
