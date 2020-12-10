#include <iostream>
#include <string>
#include <windows.h> //Windows System
#include <fstream>
#include <time.h>
using namespace std;

	double getUsedTimeMs(LONGLONG start, LONGLONG end, double dfFreq)
	{
		return (double)(end - start) * 1000 / dfFreq;
	}
	//Windows
	void test(int repeat = 1)
	{
		LARGE_INTEGER litmp;
		LONGLONG start, end;
        int a = rand();
        int b = rand();
        int c;
		double dfFreq;
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;
		double sum = 0.0;
		QueryPerformanceCounter(&litmp);
		start = litmp.QuadPart;
		while (repeat--)
		{
			a+b;
        }
		QueryPerformanceCounter(&litmp);
		end = litmp.QuadPart;
		cout << getUsedTimeMs(start,end,dfFreq) << "ms" << endl;
	}



int main()
{
	test(100000000);
	return 0;
}
