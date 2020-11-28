#include <iostream>
#include <ctime>
#include <algorithm>
// #include <sys/time.h>	//Linux System
#include <windows.h> //Windows System
using namespace std;

typedef struct L_NUM
{
	int len;
	int *num;
} * LNUM;

void deleteLNUM(LNUM x)
{
	delete[] x->num;
	delete x;
}

void printLargeNum(LNUM x)
{
	for (int i = 0; i < x->len; i++)
		cout << x->num[i] << " ";
	cout << endl;
}

void removeZeros(LNUM x)
{
	while (x->num[0] == 0 && x->len > 0)
	{
		x->num++;
		x->len--;
	}
}

LNUM getLargeNum(int len)
{
	Sleep(1000);
	srand((int)time(0));
	LNUM x = new L_NUM;
	x->len = len;
	x->num = new int[len];
	x->num[0] = rand() % 9 + 1;
	for (int i = 1; i < len; i++)
		x->num[i] = rand() % 10;
	return x;
}

LNUM add(LNUM x, LNUM y, int n)
{
	LNUM res = new L_NUM;
	res->len = max(x->len + n, y->len) + 1;
	res->num = new int[res->len];
	res->num[0] = 0;

	fill(res->num, res->num + res->len, 0);
	int i = x->len - 1, j = y->len - 1, k = res->len - 1;
	while (j >= y->len - n && j >= 0)
		res->num[k--] = y->num[j--];
	while (k > res->len - n - 1)
		k--;
	int addflag = 0;
	while (i >= 0 && j >= 0)
	{
		int t = x->num[i--] + y->num[j--] + addflag;
		addflag = t / 10;
		res->num[k--] = t % 10;
	}
	while (i >= 0)
	{
		int t = x->num[i--] + addflag;
		addflag = t / 10;
		res->num[k--] = t % 10;
	}
	while (j >= 0)
	{
		int t = y->num[j--] + addflag;
		addflag = t / 10;
		res->num[k--] = t % 10;
	}
	if (addflag)
		res->num[0] = 1;
	else
	{
		res->num++;
		res->len--;
	}
	return res;
}

LNUM sub(LNUM x, LNUM y)
{
	LNUM res = new L_NUM;
	res->len = x->len;
	res->num = new int[res->len];
	fill(res->num, res->num + res->len, 0);
	int i = x->len - 1, j = y->len - 1, k = res->len - 1;
	int subflag = 0;
	while (i >= 0 && j >= 0)
	{
		int t = x->num[i--] - y->num[j--] - subflag;
		res->num[k--] = t < 0 ? (t + 10) : t;
		subflag = t < 0 ? 1 : 0;
	}
	while (i >= 0 && subflag == 1)
	{
		int t = x->num[i--] - subflag;
		res->num[k--] = t < 0 ? (t + 10) : t;
		subflag = t < 0 ? 1 : 0;
	}
	while (i >= 0)
		res->num[k--] = x->num[i--];
	res->len = res->len - k - 1;
	res->num = res->num + k + 1;
	removeZeros(res);
	return res;
}

LNUM mul(LNUM x, LNUM y)
{
	LNUM res = new L_NUM;
	res->len = x->len + y->len;
	res->num = new int[res->len];
	fill(res->num, res->num + res->len, 0);
	for (int i = y->len - 1; i >= 0; i--)
	{
		int mulflag = 0, addflag = 0;
		for (int j = x->len - 1; j >= 0; j--)
		{
			int t1 = y->num[i] * x->num[j] + mulflag;
			mulflag = t1 / 10;
			t1 = t1 % 10;
			int t2 = res->num[i + j + 1] + t1 + addflag;
			res->num[i + j + 1] = t2 % 10;
			addflag = t2 / 10;
		}
		res->num[i] += mulflag + addflag;
	}
	if (res->num[0] == 0)
	{
		res->num++;
		res->len--;
	}
	return res;
}

void copy(LNUM x, LNUM y)
{
	for (int i = 0; i < x->len; i++)
		x->num[i] = y->num[i];
}

//在position处将大数串断为两个子串
LNUM *cutLargeNum(LNUM x, int position)
{
	LNUM *lnums = new LNUM[2];
	lnums[0] = new L_NUM;
	lnums[1] = new L_NUM;
	lnums[0]->len = x->len - position;
	lnums[0]->num = x->num;
	lnums[1]->len = position;
	lnums[1]->num = x->num + x->len - position;
	return lnums;
}

LNUM DACMul(LNUM x, LNUM y)
{
	if (x->len <= 2 || y->len <= 2)
		return mul(x, y);
	int c = (min(x->len, y->len) + 1) / 2;
	LNUM *xs = cutLargeNum(x, c); //A:xs[0] B:xs[1]
	LNUM *ys = cutLargeNum(y, c); //C:ys[0] D:ys[1]
	LNUM A_B = add(xs[0], xs[1], 0);
	LNUM C_D = add(ys[0], ys[1], 0);
	LNUM t1 = DACMul(A_B, C_D);
	LNUM AC = DACMul(xs[0], ys[0]);
	LNUM BD = DACMul(xs[1], ys[1]);
	LNUM t2 = sub(t1, AC);
	LNUM t3 = sub(t2, BD);
	LNUM t4 = add(t3, BD, c);
	LNUM res = add(AC, t4, 2 * c);
	deleteLNUM(t1);
	deleteLNUM(t2);
	deleteLNUM(t3);
	deleteLNUM(t4);
	deleteLNUM(AC);
	deleteLNUM(BD);
	return res;
}

int compare(LNUM x, LNUM y)
{
	if (x->len > y->len)
		return 1;
	if (x->len < y->len)
		return -1;
	for (int i = 0; i < x->len; i++)
	{
		if (x->num[i] > y->num[i])
			return 1;
		if (x->num[i] < y->num[i])
			return -1;
	}
	return 0;
}

double getUsedTimeMs(LONGLONG start, LONGLONG end, double dfFreq)
{
	return (double)(end - start) * 1000 / dfFreq;
}

void test(int n, int tag)
{
	LNUM x, y, res;
	x = getLargeNum(n);
	y = getLargeNum(n);

	LARGE_INTEGER litmp;
	LONGLONG start, end;
	double dfFreq;
	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;

	//计时开始
	QueryPerformanceCounter(&litmp);
	start = litmp.QuadPart;
	//执行程序
	switch (tag)
	{
	case 0:
		res = mul(x, y);
		break;
	case 1:
		res = DACMul(x, y);
		break;
	case 2:
		break;
	}
	//计时结束
	QueryPerformanceCounter(&litmp);
	end = litmp.QuadPart;
	cout << "n=" << n << ": " << getUsedTimeMs(start, end, dfFreq) << "ms" << endl;
}

int main()
{
	int maxlen = 1000000;
	cout << "Vanilla:" << endl;
	for (int i = 10; i <= maxlen; i *= 10)
		test(i, 0);
	cout << "DAC:" << endl;
	for (int i = 10; i <= maxlen; i *= 10)
		test(i, 1);
	return 0;
}