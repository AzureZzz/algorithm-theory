#include <iostream>
#include <cstdio>
#include <malloc.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <map>
#include <sstream>
#include <time.h>
#include "BTree.h"
#include <sys/time.h> //Linux System
using namespace std;

#define MAX_DNUM 5
static int pnum = 8091179;		  //顶点数目：8091179
static vector<int> *key;		  //关键词信息存储
static vector<int> *graph;		  //反向邻接表存储
static vector<int> *origin_graph; //原始邻接表存储
static vector<int> site;		  //地点序号存储
static map<int, int> pmap;		  //地点坐标检索使用
static int dnum = 0;			  //查询关键词数目
static int sitenum = 0;
static int **gap;				  //记录距离
static int maxlayer = 0x3f3f3f3f; //记录支配情况下的最高层
static BTNode *Bt = NULL;
static int *visited; //

#pragma region read data

void readInit()
{
	key = new vector<int>[pnum];
	graph = new vector<int>[pnum];
	origin_graph = new vector<int>[pnum];
	visited = new int[pnum];
}

void readKeyword(string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		cout << "Open key file failed!" << endl;
		exit(-1);
	}
	while (true)
	{
		char temp_str[10000] = {'\0'};
		if (fgets(temp_str, 10000, file) == NULL)
		{
			break;
		}
		char seps[] = ": ,\n";
		char *token;
		char *next_token;
		token = strtok(temp_str, seps);
		if (token == NULL)
		{
			continue;
		}
		int index = atoi(token), temp;
		token = strtok(NULL, seps);
		while (token != NULL)
		{
			temp = atoi(token);
			key[index].push_back(temp);
			token = strtok(NULL, seps);
		}
	}
	fclose(file);
}

void readGraph(string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		cout << "Open graph file failed!" << endl;
		exit(-1);
	}
	int u, v;
	while (true)
	{
		char temp_str[10000] = {'\0'};
		if (fgets(temp_str, 10000, file) == NULL)
		{
			break;
		}
		char seps[] = ": ,\n";
		char *token;
		char *next_token;
		token = strtok(temp_str, seps);
		if (token == NULL)
		{
			continue;
		}
		u = atoi(token);
		token = strtok(NULL, seps);
		while (token != NULL)
		{
			v = atoi(token);
			graph[v].push_back(u);
			origin_graph[u].push_back(v);
			token = strtok(NULL, seps);
		}
	}
	fclose(file);
}

void readSite(string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		cout << "Open site file failed!" << endl;
		exit(-1);
	}
	fscanf(file, "%d#\n", &sitenum);
	int u;
	int no = 0;
	while (true)
	{
		char temp_str[10000] = {'\0'};
		if (fgets(temp_str, 10000, file) == NULL)
		{
			break;
		}
		//cout << temp_str << endl;
		char seps[] = ": ,\n";
		char *token;
		char *next_token;
		token = strtok(temp_str, seps);
		if (token == NULL)
		{
			continue;
		}
		//cout << token << ":" << endl;
		u = atoi(token);
		pmap.insert(pair<int, int>(u, no++));
		site.push_back(u);
	}
	sitenum = site.size();
	fclose(file);
}

void show()
{
	int num_edge = 0;
	for (int i = 0; i < pnum; i++)
	{
		if ((int)graph[i].size() > 0)
		{
			// cout << i << ": ";
			for (int j = 0; j < (int)graph[i].size(); j++)
			{
				// cout << graph[i][j] << ",";
				num_edge++;
			}
			// cout << endl;
		}
	}
	cout << endl;
	cout << "Edges:" << num_edge << endl;
}

#pragma endregion

void CreateBTree()
{
	Findres r;
	cout << "创建一颗" << m << "阶B树" << endl;
	for (int i = 0; i < pnum; i++)
	{
		int len = key[i].size();
		for (int j = 0; j < len; j++)
		{

			pkey k(key[i][j]);
			k.insert_no(i);
			//cout<<"第"<<j+1<<"步，"<<"插入元素"<<key[i][j]<<endl;
			r = SearchBTree(Bt, k);
			if (r.flag == 0)
			{
				InsertBTree(Bt, r.i, k, r.pt);
			}
			else
			{
				r.pt->key[r.i].insert_no(i);
			}
		}
	}
	//PrintBTree(Bt);
}

void BFS_CreateBTree()
{
	int sumofkey = 0;
	int u, v, len;
	queue<int> que;
	Findres r;
	memset(visited, 0, sizeof(int) * pnum);
	for (int i = 0; i < pnum; i++)
	{
		if (visited[i] == 0)
		{
			que.push(i);
			visited[i] = 1;
			while (!que.empty())
			{
				u = que.front();
				//cout << "u = " << u << endl;
				que.pop();
				len = key[u].size();
				for (int j = 0; j < len; j++)
				{
					//查询关键词
					pkey k(key[u][j]);
					//将含有该关键词的结点列表初始化为u
					k.insert_no(u);
					//cout<<"第"<<j+1<<"步，"<<"插入元素"<<key[i][j]<<endl;
					r = SearchBTree(Bt, k);
					if (r.flag == 0) //查询失败
					{
						//把这个关键词类插入到B树中
						InsertBTree(Bt, r.i, k, r.pt);
						sumofkey++;
					}
					else //查找成功
					{
						//在B树查找到的关键词类的nolist中添加结点u
						r.pt->key[r.i].insert_no(u);
					}
				}
				for (int j = 0; j < (int)origin_graph[u].size(); j++)
				{
					v = origin_graph[u][j];
					if (!visited[v]) //未访问过
					{
						que.push(v);
						//cout << "v = " << v << endl;
						visited[v] = 1;
					}
				}
			}
		}
	}
	cout << "关键词的总数量是 = " << sumofkey << endl;
}

//***************************************************************
void BFS_CalculateMinDistance(int start, int d)
{
	// int count = 0;
	int u, v;
	queue<int> que;
	int layer = 0, nodeNum = 1, nextnodeNum = 0;
	que.push(start);
	visited[start] = 1;
	while (!que.empty())
	{
		u = que.front();
		//cout << "u = " << u << endl;
		que.pop();
		if (pmap.find(u) != pmap.end())
		{
			int g = pmap[u];
			if (gap[g][d] == -1 || gap[g][d] > layer)
			{
				gap[g][d] = layer;
			}
		}
		for (int j = 0; j < (int)graph[u].size(); j++)
		{
			v = graph[u][j];
			if (!visited[v]) //未访问过
			{
				que.push(v);
				//cout << "v = " << v << endl;
				visited[v] = 1;
				nextnodeNum++;
			}
		}
		nodeNum--; //每次循环输出一个点，即遍历当前层结点数-1
		if (nodeNum == 0)
		{
			layer++;
			nodeNum = nextnodeNum;
			nextnodeNum = 0;
		}
	}
	queue<int> empty;
	swap(empty, que);
}

#pragma region my improve
void ImprovedBFSDistanceFirst(int start)
{
	int u, v;
	queue<int> que;
	int layer = 0, nodeNum = 1, nextnodeNum = 0;
	que.push(start);
	visited[start] = 1;
	while (!que.empty())
	{
		u = que.front();
		//cout << "u = " << u << endl;
		que.pop();
		if (pmap.find(u) != pmap.end())
		{
			int g = pmap[u];
			if (gap[g][0] == -1 || gap[g][0] > layer)
			{
				gap[g][0] = layer;
			}
		}
		for (int j = 0; j < (int)graph[u].size(); j++)
		{
			v = graph[u][j];
			if (!visited[v]) //未访问过
			{
				que.push(v);
				//cout << "v = " << v << endl;
				visited[v] = 1;
				nextnodeNum++;
			}
		}
		nodeNum--; //每次循环输出一个点，即遍历当前层结点数-1
		if (nodeNum == 0)
		{
			layer++;
			nodeNum = nextnodeNum;
			nextnodeNum = 0;
		}
	}
	queue<int> empty;
	swap(empty, que);
}

void ImprovedBFSDistanceNext(int start, int d)
{
	int u, v;
	queue<int> que;
	int layer = 0, nodeNum = 1, nextnodeNum = 0;
	que.push(start);
	visited[start] = 1;
	while (!que.empty())
	{
		u = que.front();
		//cout << "u = " << u << endl;
		que.pop();
		if (pmap.find(u) != pmap.end())
		{
			int g = pmap[u];
			if (gap[g][d - 1] == -1) //该地点不包含上一个keyword，剪去
			{
				continue;
			}
			if (gap[g][d] == -1 || gap[g][d] > layer)
			{
				gap[g][d] = layer;
			}
		}
		for (int j = 0; j < (int)graph[u].size(); j++)
		{
			v = graph[u][j];
			if (!visited[v]) //未访问过
			{
				que.push(v);
				//cout << "v = " << v << endl;
				visited[v] = 1;
				nextnodeNum++;
			}
		}
		nodeNum--; //每次循环输出一个点，即遍历当前层结点数-1
		if (nodeNum == 0)
		{
			layer++;
			nodeNum = nextnodeNum;
			nextnodeNum = 0;
		}
	}
	queue<int> empty;
	swap(empty, que);
}
#pragma endregion

//返回值说明：1表示a<b，-1表示b<a，0表示两者没有支配关系
int isGovern(int *a, int *b)
{
	int ag = 0, bg = 0;
	for (int i = 0; i < dnum; i++)
	{
		if (a[i] < b[i])
		{
			ag++;
		}
		else if (b[i] < a[i])
		{
			bg++;
		}
	}
	if (ag && bg)
	{
		return 0;
	}
	else if (bg)
	{
		return 1;
	}
	else if (ag)
	{
		return -1;
	}
	return 0;
}

bool isAssessible(int *temp)
{
	for (int i = 0; i < dnum; i++)
	{
		if (temp[i] == -1)
		{
			return false;
		}
	}
	return true;
}

static vector<int> BNLresult;
void BNL()
{
	BNLresult.clear();
	for (int i = 0; i < sitenum; i++)
	{
		if (isAssessible(gap[i]))
		{
			BNLresult.push_back(i);
			break;
		}
	}
	//cout << site[BNLresult[0]] <<endl;
	//cout << sitenum << endl;
	if (BNLresult.size() == 0)
	{
		return;
	}
	for (int i = BNLresult[0] + 1; i < sitenum; i++)
	{
		bool isSP = true;
		//新引入节点与窗口节点之间的对比
		if (!isAssessible(gap[i]))
		{
			continue;
		}
		for (vector<int>::iterator it = BNLresult.begin(); it != BNLresult.end();)
		{
			int flag = isGovern(gap[i], gap[*it]);
			// cout << flag << endl;
			if (flag == -1)
			{
				// cout << *it << endl;
				it = BNLresult.erase(it);
			}
			else if (flag == 1)
			{
				isSP = false;
				break;
			}
			else
			{
				it++;
			}
		}
		if (isSP)
		{
			BNLresult.push_back(i);
		}
	}
}

//***************************************************************
void ShowList(noPoint *head)
{
	noPoint *p = head;
	while (p != NULL)
	{
		cout << " " << p->no;
		p = p->next;
	}
	cout << endl;
}

void SearchInBTree(int key, Findres &r)
{
	r = SearchBTree(Bt, key);
	// cout << "Flag = " << r.flag << endl;
	// cout << "sum of point = " << r.pt->key[r.i].sum << endl;
	//cout << "no：" << endl;
	//ShowList(r.pt->key[r.i].nolist);
}

void BFSPrint(int start)
{
	// int count = 0;
	memset(visited, 0, sizeof(int) * pnum);
	int u, v;
	queue<int> que;
	int layer = 0, nodeNum = 1, nextnodeNum = 0;
	que.push(start);
	visited[start] = 1;
	cout << "<";
	while (!que.empty())
	{
		u = que.front();
		cout << u << ", ";
		que.pop();
		for (int j = 0; j < (int)origin_graph[u].size(); j++)
		{
			v = origin_graph[u][j];
			if (!visited[v]) //未访问过
			{
				que.push(v);
				visited[v] = 1;
				nextnodeNum++;
			}
		}
		nodeNum--; //每次循环输出一个点，即遍历当前层结点数-1
		if (nodeNum == 0)
		{
			layer++;
			nodeNum = nextnodeNum;
			nextnodeNum = 0;
			cout << ">" << endl;
			cout << "<";
		}
	}
}

void ShowBNLResult()
{
	//输出检索结果
	for (int i = 0; i < (int)BNLresult.size(); i++)
	{
		int sp = site[BNLresult[i]];
		cout << "第" << i + 1 << "个SP点是" << sp << endl;
		cout << "关键词" << endl;
		for (int j = 0; j < (int)key[sp].size(); j++)
		{
			cout << key[sp][j] << " ";
		}
		cout << endl;
		cout << "距离" << endl;
		for (int j = 0; j < dnum; j++)
		{
			cout << gap[BNLresult[i]][j] << " ";
		}
		cout << "输出树结构" << endl;
		BFSPrint(sp);
		cout << endl;
	}
}

double getUsedTimeMs(struct timeval start, struct timeval end)
{
	return ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;
}

void GetRunTime(int total,int knum)
{
	struct timeval start, end;
	double deltatime;
	vector<int> des;
	vector<double> time1;
	vector<double> time2;
	dnum = knum;
	int times = total;
	while (times--)
	{
		srand(time(NULL)); 
		cout << "n="<<(times+1) <<" keywords：" << endl;
		for (int i = 0; i < knum;)
		{
			int temp1 = rand() % pnum;
			if (key[temp1].size() == 0)
				continue;
			int temp2 = rand() % key[temp1].size();
			des.push_back(key[temp1][temp2]);
			cout << des[i] << ",";
			i++;
		}
		cout << endl;

		Findres r;
		gettimeofday(&start, NULL);
		
		for (int i = 0; i < dnum; i++)
		{
			memset(visited, 0, sizeof(int) * pnum);
			SearchInBTree(des[i], r);
			noPoint *p = r.pt->key[r.i].nolist;
			while (p != NULL)
			{
				BFS_CalculateMinDistance(p->no, i);
				p = p->next;
			}
		}
		if (dnum > 1)
		{
			BNL();
			cout << "BNL result's size = " << BNLresult.size() << endl;
			//ShowBNLResult();
		}
		
		gettimeofday(&end, NULL);
		deltatime = getUsedTimeMs(start, end);
		cout << "Query time：" << deltatime << "ms" << endl;
		time1.push_back(deltatime);
		BNLresult.clear();

		gettimeofday(&start, NULL);

		memset(visited, 0, sizeof(int) * pnum);
		SearchInBTree(des[0], r);
		noPoint *p = r.pt->key[r.i].nolist;
		while (p != NULL)
		{
			ImprovedBFSDistanceFirst(p->no);
			p = p->next;
		}
		for (int i = 1; i < dnum; i++)
		{
			memset(visited, 0, sizeof(int) * pnum);
			SearchInBTree(des[i], r);
			noPoint *p = r.pt->key[r.i].nolist;
			while (p != NULL)
			{
				ImprovedBFSDistanceNext(p->no, i);
				p = p->next;
			}
		}

		if (dnum > 1)
		{
			BNL();
			cout << "BNL result's size = " << BNLresult.size() << endl;
			//ShowBNLResult();
		}
		
		gettimeofday(&end, NULL);
		deltatime = getUsedTimeMs(start, end);
		cout << "Improved query time：" << deltatime << "ms" << endl;
		time2.push_back(deltatime);
		BNLresult.clear();
		des.clear();
	}
	cout << "origin:" << endl;
	double sum = 0;
	for (int i = 0; i < time1.size(); i++)
	{
		cout << time1[i] << ",";
		sum += time1[i];
	}
	cout << endl;
	cout << sum / time1.size() << endl;
	sum = 0;
	cout << "Improved:" << endl;
	for (int i = 0; i < time2.size(); i++)
	{
		cout << time2[i] << ",";
		sum += time2[i];
	}
	cout << endl;
	cout << sum / time2.size() << endl;
}

int main()
{
	struct timeval start, end;

	string timefile = "rumtime.txt";
	FILE *file;
	file = fopen(timefile.c_str(), "a+");
	if (!file)
	{
		cout << "timefile文件打开失败" << endl;
		exit(-1);
	}

	string path_keyword = "../data/Yago/node_keywords.txt";
	string path_graph = "../data/Yago/edge.txt";
	string path_site = "../data/placeid2coordYagoVB.txt";
	readInit();
	readKeyword(path_keyword);
	readGraph(path_graph);
	readSite(path_site);
	gap = new int *[sitenum];
	for (int i = 0; i < sitenum; i++)
	{
		gap[i] = new int[MAX_DNUM];
		for (int j = 0; j < MAX_DNUM; j++)
		{
			gap[i][j] = -1;
		}
		//cout << gap[i][0] << endl;
	}
	cout << "Site number：" << sitenum << endl;

	gettimeofday(&start, NULL);
	BFS_CreateBTree();
	gettimeofday(&end, NULL);
	double d1 = getUsedTimeMs(start, end);
	cout << "Building B-tree time：" << d1 << "ms" << endl;
	//开启随机查询
	GetRunTime(10,4);
	return 0;

	double deltatime;
	vector<int> des;
	while (1)
	{
		cout << "continue? quit:0 improve:1 origin:2" << endl;
		int flag;
		cin >> flag;
		getchar();
		if (!flag)
		{
			break;
		}
		cout << "Input keywords：" << endl;
		char tempstr[2048];
		cin.getline(tempstr, 2048);
		char seps[] = " ,";
		char *token;
		char *next_token;
		token = strtok(tempstr, seps);
		while (token != NULL)
		{
			int temp = atoi(token);
			des.push_back(temp);
			token = strtok(NULL, seps);
		}
		dnum = des.size();
		Findres r;
		gettimeofday(&start, NULL);
		cout << "Querying..." << endl;
		if (flag == 1)
		{
			memset(visited, 0, sizeof(int) * pnum);
			SearchInBTree(des[0], r);
			noPoint *p = r.pt->key[r.i].nolist;
			while (p != NULL)
			{
				ImprovedBFSDistanceFirst(p->no);
				p = p->next;
			}
			for (int i = 1; i < dnum; i++)
			{
				memset(visited, 0, sizeof(int) * pnum);
				SearchInBTree(des[i], r);
				noPoint *p = r.pt->key[r.i].nolist;
				while (p != NULL)
				{
					ImprovedBFSDistanceNext(p->no, i);
					p = p->next;
				}
			}
		}
		else
		{
			for (int i = 0; i < dnum; i++)
			{
				memset(visited, 0, sizeof(int) * pnum);
				SearchInBTree(des[i], r);
				noPoint *p = r.pt->key[r.i].nolist;
				while (p != NULL)
				{
					BFS_CalculateMinDistance(p->no, i);
					p = p->next;
				}
			}
		}
		//只有一个搜索词，找出其中是p点的即为skyline，若无p点则要反向遍历
		if (dnum == 1)
		{
			SearchInBTree(des[0], r);
			noPoint *p = r.pt->key[r.i].nolist;
			int no;
			pkey res(r.pt->key[r.i].key);
			while (p != NULL)
			{
				no = p->no;
				if (pmap.find(no) != pmap.end())
				{
					res.insert_no(no);
				}
				p = p->next;
			}
			cout << "sum of P in res = " << res.sum << endl;
			if (res.sum == 0)
			{
				p = r.pt->key[r.i].nolist;
				while (p != NULL)
				{
					BFS_CalculateMinDistance(p->no, 0);
					p = p->next;
				}
				BNL();
				cout << "BNL result's size = " << BNLresult.size() << endl;
			}
		}
		if (dnum > 1)
		{
			BNL();
			cout << "BNL result's size = " << BNLresult.size() << endl;
		}
		// deltatime = endtime - starttime;
		gettimeofday(&end, NULL);
		deltatime = getUsedTimeMs(start, end);
		fprintf(file, "%f", deltatime);
		cout << "Query used time：" << deltatime << "ms" << endl;
		// ShowBNLResult();
		BNLresult.clear();
		des.clear();
	}
	fclose(file);

	return 0;
}