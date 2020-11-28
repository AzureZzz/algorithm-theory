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
using namespace std;

#define MAX_DNUM 5
static int pnum = 8091179;		 //8091179,24;			点的数目
static vector<int> *key;		 //关键词信息存储
static vector<int> *graph;		 //反向邻接表存储
static vector<int> *origingraph; //原始邻接表存储
static vector<int> site;		 //地点序号存储
static map<int, int> pmap;		 //地点坐标检索使用
static int dnum = 0;			 //查询关键词数目
static int sitenum = 0;
static int **gap;				  //记录距离
static int maxlayer = 0x3f3f3f3f; //记录支配情况下的最高层
static BTNode *Bt = NULL;
static int *visited;

#pragma region read_data
void preset()
{
	key = new vector<int>[pnum];
	graph = new vector<int>[pnum];
	origingraph = new vector<int>[pnum];
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
	while (1)
	{
		char tempstr[10000] = {'\0'};
		if (fgets(tempstr, 10000, file) == NULL)
		{
			break;
		}
		char seps[] = ": ,\n";
		char *token;
		char *next_token;
		token = strtok(tempstr, seps);
		if (token == NULL)
		{
			continue;
		}
		//cout << token << ":" << endl;
		int index = atoi(token), temp;
		token = strtok(NULL, seps);
		while (token != NULL)
		{
			//cout << token << ",";
			temp = atoi(token);
			key[index].push_back(temp);
			token = strtok(NULL, seps);
		}
		//cout << endl;
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
	while (1)
	{
		char tempstr[10000] = {'\0'};
		if (fgets(tempstr, 10000, file) == NULL)
		{
			break;
		}
		char seps[] = ": ,\n";
		char *token;
		char *next_token;
		token = strtok(tempstr, seps);
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
			origingraph[u].push_back(v);
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
	//cout << sitenum << endl;
	int u;
	int no = 0;
	while (1)
	{
		char tempstr[10000] = {'\0'};
		if (fgets(tempstr, 10000, file) == NULL)
		{
			break;
		}
		//cout << tempstr << endl;
		char seps[] = ": ,\n";
		char *token;
		char *next_token;
		token = strtok(tempstr, seps);
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
	for (int i = 0; i < pnum; i++)
	{
		cout << i << ": ";
		for (int j = 0; j < (int)graph[i].size(); j++)
		{
			cout << graph[i][j] << ",";
		}
		cout << endl;
	}
}
#pragma endregion

int main()
{
    string path_keyword = "../data/Yago_small/node_keywords.txt";
	string path_graph = "../data/Yago_small/edge.txt";
	string path_site = "../data/placeid2coordYagoVB.txt";
	readGraph(path_graph);
	// show();
    return 0;
}