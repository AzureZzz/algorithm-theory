#include <iostream>
#include <string>
// #include <sys/time.h>	//Linux System
#include <windows.h> //Windows System
#include <fstream>
#include <time.h>
using namespace std;
int arr[10000000];
int threshold=100;

class LargeNumberString
{
public:
	string getRandomLargeNumber(int length)
	{
		if (length <= 0)
			return NULL;
		string x(length, '0');
		x[0] = rand() % 9 + 1 + 48;
		for (int i = 1; i < length; i++)
		{
			x[i] = rand() % 10 + 48;
		}
		return x;
	}
	//x>=y
	string sub(string x, string y)
	{
		int lx = x.length(), ly = y.length();
		string res(lx, '0');
		int i = lx - 1, j = ly - 1, k = res.length() - 1;
		int subflag = 0;
		while (i >= 0 && j >= 0)
		{
			int t = x[i--] - y[j--] - subflag;
			res[k--] = t < 0 ? (t + 58) : (t + 48);
			subflag = t < 0 ? 1 : 0;
		}
		while (i >= 0 && subflag == 1)
		{
			int t = x[i--] - subflag - 48;
			res[k--] = t < 0 ? (t + 58) : (t + 48);
			subflag = t < 0 ? 1 : 0;
		}
		while (i >= 0)
			res[k--] = x[i--];
		return removeZeros(res);
	}

	//compute x*10^n+y; default x and y is positive
	string add(string x, string y, int n)
	{
		int lx = x.length(), ly = y.length();
		string res(max(lx + n, ly), '0');
		int i = lx - 1, j = ly - 1, k = res.length() - 1;
		while (j >= ly - n && j >= 0)
			res[k--] = y[j--];
		while (k > res.length() - n - 1)
			k--;
		int addflag = 0;
		while (i >= 0 && j >= 0)
		{
			int t = (x[i--] - '0') + (y[j--] - '0') + addflag;
			addflag = t / 10;
			res[k--] = t % 10 + 48;
		}
		while (i >= 0)
		{
			int t = (x[i--] - '0') + addflag;
			addflag = t / 10;
			res[k--] = t % 10 + 48;
		}
		while (j >= 0)
		{
			int t = (y[j--] - '0') + addflag;
			addflag = t / 10;
			res[k--] = t % 10 + 48;
		}
		if (addflag)
			res = '1' + res;
		return res;
	}

	string multiply(string x, string y)
	{
		int lx = x.length(), ly = y.length();
		string res(lx + ly, '0');
		for (int i = ly - 1; i >= 0; i--)
			for (int j = lx - 1; j >= 0; j--)
				arr[i + j + 1] += (y[i] - '0') * (x[j] - '0');

		for (int i = res.length() - 1; i > 0; i--)
		{
			if (arr[i] > 9)
			{
				arr[i - 1] += arr[i] / 10;
				res[i] = arr[i] % 10 + '0';
			}
			else
				res[i] = arr[i] + '0';
		}
		if (arr[0] == 0)
			res = res.substr(1, res.length());
		else
			res[0] = arr[0] + '0';
		return res;
	}

	string DACMultiply(string x, string y)
	{
		int m = x.length(), n = y.length();
		if (m <= threshold || n <= threshold)
			return multiply(x, y);
		int c = min(m, n) / 2;
		string A = x.substr(0, m - c), B = removeZeros(x.substr(m - c, m));
		string C = y.substr(0, n - c), D = removeZeros(y.substr(n - c, n));
		string res = DACMultiply(A, D);
		string t = DACMultiply(B, C);
		res = add(res, t, 0);
		t = DACMultiply(B, D);
		res = add(res, t, c);
		t = DACMultiply(A, C);
		res = add(t, res, 2 * c);
		return res;
	}

	string ImproveDACMultiply(string x, string y)
	{
		int m = x.length(), n = y.length();
		if (m <= 100 || n <= 100)
			return multiply(x, y);
		int c = min(m, n) / 2;
		string A = x.substr(0, m - c), B = removeZeros(x.substr(m - c, m));
		string C = y.substr(0, n - c), D = removeZeros(y.substr(n - c, n));
		string res = add(A, B, 0);
		string t = add(C, D, 0);
		A = ImproveDACMultiply(A, C);
		B = ImproveDACMultiply(B, D);
		res = ImproveDACMultiply(res, t);
		res = sub(res, A);
		res = sub(res, B);
		res = add(res, B, c);
		res = add(A, res, 2 * c);
		return res;
	}

	// Linux
	// void test(int n, int tag = 0)
	// {
	// 	string x, y, res;

	// 	x = getRandomLargeNumber(n);
	// 	y = getRandomLargeNumber(n);
	// 	struct timeval start, end;

	// 	//计时开始
	// 	gettimeofday(&start, NULL);
	// 	//执行程序
	// 	switch (tag)
	// 	{
	// 	case 0:
	// 		res = multiply(x, y);
	// 		break;
	// 	case 1:
	// 		res = DACMultiply(x, y);
	// 		break;
	// 	case 2:
	// 		res = ImproveDACMultiply(x, y);
	// 		break;
	// 	}
	// 	//计时结束
	// 	gettimeofday(&end, NULL);
	// 	cout << "n=" << n << ": " << getUsedTimeMs(start, end) << "ms" << endl;
	// }

	//Windows
	void test(int n, int tag, int repeat = 1)
	{
		string logfile = "./logs.txt";
		FILE *file;
		file = fopen(logfile.c_str(), "a+");
		if (!file)
		{
			cout << "logfile open filed!" << endl;
			exit(-1);
		}
		string x, y, res;

		x = getRandomLargeNumber(n);
		y = getRandomLargeNumber(n);

		LARGE_INTEGER litmp;
		LONGLONG start, end;
		double dfFreq;
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;
		time_t t = time(0);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", localtime(&t));
		fprintf(file, "\n%s:(tag=%d, n=%d, repeat=%d)\n", tmp, tag, n, repeat);
		double sum = 0.0;
		while (repeat--)
		{
			switch (tag)
			{
			case 0:
				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;
				res = multiply(x, y);
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				break;
			case 1:
				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;
				res = DACMultiply(x, y);
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				break;
			case 2:
				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;
				res = ImproveDACMultiply(x, y);
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				break;
			}
			double time = getUsedTimeMs(start, end, dfFreq);
			sum += time;
			fprintf(file, "%lf ", time);
			cout << "n=" << n << ": " << time << "ms" << endl;
		}
		fprintf(file, "\nsum:%lfms", sum);
		fclose(file);
	}

	void test_(int tag, int maxlen, int stride)
	{
		string logfile[7] = {"./points_0.txt", "./points_1.txt", "./points_2.txt", "./points_3.txt",
							 "./points_4.txt", "./points_5.txt", "./points_6.txt"};
		FILE *file;
		file = fopen(logfile[3].c_str(), "w");
		if (!file)
		{
			cout << "logfile open filed!" << endl;
			exit(-1);
		}
		LARGE_INTEGER litmp;
		LONGLONG start, end;
		double dfFreq;
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;
		time_t t = time(0);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", localtime(&t));
		fprintf(file, "%s:(tag=%d, maxlen=%d, stride=%d)\n", tmp, tag, maxlen, stride);
		// fprintf(file, "len:\n");
		for (int n = 10; n <= maxlen; n += stride)
		{
			fprintf(file, "%d ", n);
		}
		// fprintf(file, "\ntimes:\n");
		fprintf(file, "\n");
		string x, y, res;
		for (int n = 10; n <= maxlen; n += stride)
		{
			x = getRandomLargeNumber(n);
			y = getRandomLargeNumber(n);
			switch (tag)
			{
			case 0:
				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;
				res = multiply(x, y);
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				break;
			case 1:
				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;
				res = DACMultiply(x, y);
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				break;
			case 2:
				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;
				res = ImproveDACMultiply(x, y);
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				break;
			}
			double time = getUsedTimeMs(start, end, dfFreq);
			cout << "n=" << n << ": " << time << "ms" << endl;
			fprintf(file, "%lf ", time);
		}
		fclose(file);
	}

	void test_threshold(int max, int stride, int n)
	{
		string logfile[1] = {"./points_6.txt"};
		FILE *file;
		file = fopen(logfile[0].c_str(), "w");
		if (!file)
		{
			cout << "logfile open filed!" << endl;
			exit(-1);
		}
		LARGE_INTEGER litmp;
		LONGLONG start, end;
		double dfFreq;
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;
		time_t t = time(0);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", localtime(&t));
		fprintf(file, "%s:(max=%d, stride=%d)\n", tmp, max, stride);
		// fprintf(file, "threshold:\n");
		for (int i = 1; i <= max; i += stride)
		{
			fprintf(file, "%d ", i);
		}
		// fprintf(file, "\ntimes:\n");
		fprintf(file, "\n");
		string x, y, res;
		for (int i = 1; i <= max; i += stride)
		{
			threshold = i;
			x = getRandomLargeNumber(n);
			y = getRandomLargeNumber(n);
			QueryPerformanceCounter(&litmp);
			start = litmp.QuadPart;
			res = DACMultiply(x, y);
			QueryPerformanceCounter(&litmp);
			end = litmp.QuadPart;
			double time = getUsedTimeMs(start, end, dfFreq);
			cout << "threshold=" << i << ": " << time << "ms" << endl;
			fprintf(file, "%lf ", time);
		}
		fclose(file);
	}

	int compare(string x, string y)
	{
		int lx = x.size(), ly = y.size();
		if (lx < ly)
			return -1;
		if (lx > ly)
			return 1;
		for (int i = 0; i < lx; i++)
		{
			if (x[i] > y[i])
				return 1;
			if (x[i] < y[i])
				return -1;
		}
		return 0;
	}

private:
	//Linux
	// double getUsedTimeMs(struct timeval start, struct timeval end)
	// {
	// 	return ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;
	// }
	// Windows
	double getUsedTimeMs(LONGLONG start, LONGLONG end, double dfFreq)
	{
		return (double)(end - start) * 1000 / dfFreq;
	}

	string removeZeros(string str)
	{
		if (str[0] != '0')
			return str;
		int i = 0, l = str.length();
		while (str[i] == '0' && i < l - 1)
			i++;
		return str.substr(i, l);
	}
};

int main()
{
	LargeNumberString ln;
	// 测试三种方法各个长度的时间
	// cout<<"Vanillia:"<<endl;
	// for (int i = 10; i <= 10000; i *= 10)
	// {
	// 	ln.test(i, 0, 1);
	// }
	// cout<<"DAC:"<<endl;
	// for (int i = 10; i <= 10000; i *= 10)
	// {
	// 	ln.test(i, 1, 1);
	// }
	// cout<<"Improved DAC:"<<endl;
	// for (int i = 10; i <= 10000000; i *= 10)
	// {
	// 	ln.test(i, 2, 1);
	// }

	// 循环测试10,100位
	// ln.test(10,0,1000);
	// ln.test(10,1,1000);
	// ln.test(10,2,1000);
	// ln.test(100,0,100);
	// ln.test(100,1,100);
	// ln.test(100,2,100);

	// 测试各算法的增长趋势
	// ln.test_(0, 50000, 100);
	// ln.test_(1, 50000, 100);
	// ln.test_(2, 50000, 100);

	// ln.test_threshold(1000, 5, 10000);
	ln.test(10000000, 2, 1);
	return 0;
}
