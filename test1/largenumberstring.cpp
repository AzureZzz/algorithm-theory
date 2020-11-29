#include <iostream>
#include <string>
#include <sys/time.h>	//Linux System
// #include <windows.h> //Windows System
using namespace std;

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
		{
			int mulflag = 0, addflag = 0;
			for (int j = lx - 1; j >= 0; j--)
			{
				int t1 = (y[i] - '0') * (x[j] - '0') + mulflag;
				mulflag = t1 / 10;
				t1 = t1 % 10;
				int t2 = res[i + j + 1] - '0' + t1 + addflag;
				res[i + j + 1] = t2 % 10 + 48;
				addflag = t2 / 10;
			}
			res[i] += mulflag + addflag;
		}
		if (res[0] == '0')
			res = res.substr(1, res.length());
		return res;
	}
	
	string DACMultiply(string x, string y)
	{
		int m = x.length(), n = y.length();
		if (m <= 100 || n <= 100)
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
		if (m <= 30 || n <= 30)
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
	void test(int n, int tag = 0)
	{
		string x, y, res;

		x = getRandomLargeNumber(n);
		y = getRandomLargeNumber(n);
		struct timeval start, end;

		//计时开始
		gettimeofday(&start, NULL);
		//执行程序
		switch (tag)
		{
		case 0:
			res = multiply(x, y);
			break;
		case 1:
			res = DACMultiply(x, y);
			break;
		case 2:
			res = ImproveDACMultiply(x, y);
			break;
		}
		//计时结束
		gettimeofday(&end, NULL);
		cout << "n=" << n << ": " << getUsedTimeMs(start, end) << "ms" << endl;
	}

	//Windows
	// void test(int n, int tag)
	// {
	// 	string x, y, res;
	// 	x = getRandomLargeNumber(n);
	// 	y = getRandomLargeNumber(n);

	// 	LARGE_INTEGER litmp;
	// 	LONGLONG start, end;
	// 	double dfFreq;
	// 	QueryPerformanceFrequency(&litmp);
	// 	dfFreq = (double)litmp.QuadPart;

	// 	//计时开始
	// 	QueryPerformanceCounter(&litmp);
	// 	start = litmp.QuadPart;
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
	// 	QueryPerformanceCounter(&litmp);
	// 	end = litmp.QuadPart;
	// 	cout << "n=" << n << ": " << getUsedTimeMs(start, end, dfFreq) << "ms" << endl;
	// }

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
	double getUsedTimeMs(struct timeval start, struct timeval end)
	{
		return ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;
	}
	// Windows
	// double getUsedTimeMs(LONGLONG start, LONGLONG end, double dfFreq)
	// {
	// 	return (double)(end - start) * 1000 / dfFreq;
	// }

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
	int maxlen = 100000;
	for (int i = 10; i <= 1000; i *= 10)
		ln.test(i, 0);
	cout<<endl;
	cout << "Vanilla:" << endl;
	for (int i = 10; i <= maxlen; i *= 10)
		ln.test(i, 0);
	cout<<endl;
	cout << "DAC:" << endl;
	for (int i = 10; i <= maxlen; i *= 10)
		ln.test(i, 1);
	cout<<endl;
	cout << "Improved DAC:" << endl;
	for (int i = 10; i <= maxlen*10; i *= 10)
		ln.test(i, 2);
	return 0;
}