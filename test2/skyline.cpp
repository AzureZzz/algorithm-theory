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
#include <sys/time.h> //Linux System
using namespace std;

#define MAX_QNUM 5                  //查询keyword最大数量
static int allnum = 8091179;        //顶点数目：8091179
static vector<int> *key;            //keyword信息
static vector<int> *graph_T;        //反向邻接表
static vector<int> *graph;          //正向邻接表
static vector<int> ps;              //地点序号存储
static map<int, int> pmap;          //地点坐标检索使用
static map<int, vector<int>> kvmap; //关键字哈希表
static int qnum = 0;                //查询keyword数目
static int pnum = 0;                //site数目
static int **gap;                   //语义距离矩阵：P点到各个keyword的距离
static int maxlayer = 0x3f3f3f3f;   //记录支配情况下的最高层
static int *visited;                //记录点是否被访问 0:未访问，1：已访问
static vector<int> sps;             //skyline points result


#pragma region read and create
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
            if (kvmap.find(temp) != kvmap.end())
            {
                kvmap[temp].push_back(index);
            }
            else
            {
                vector<int> v;
                v.push_back(index);
                kvmap.insert(pair<int, vector<int>>(temp, v));
            }
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
            graph_T[v].push_back(u);
            graph[u].push_back(v);
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
    fscanf(file, "%d#\n", &pnum);
    int u;
    int no = 0;
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
        pmap.insert(pair<int, int>(u, no++));
        ps.push_back(u);
    }
    pnum = ps.size();
    fclose(file);
}

void initRead(string keyword_file, string graph_file, string site_file)
{
    key = new vector<int>[allnum];
    graph_T = new vector<int>[allnum];
    graph = new vector<int>[allnum];
    visited = new int[allnum];
    readKeyword(keyword_file);
    readGraph(graph_file);
    readSite(site_file);
}
#pragma endregion

#pragma region algorithm
void initGap()
{
    gap = new int *[pnum];
    for (int i = 0; i < pnum; i++)
    {
        gap[i] = new int[MAX_QNUM];
        for (int j = 0; j < MAX_QNUM; j++)
        {
            gap[i][j] = -1;
        }
    }
}

void clearGap()
{
    for (int i = 0; i < pnum; i++)
    {
        for (int j = 0; j < MAX_QNUM; j++)
        {
            gap[i][j] = -1;
        }
    }
}

int govern(int *p, int *q)
{
    int pg = 0, qg = 0;
    for (int i = 0; i < qnum; i++)
    {
        if (p[i] < q[i])
            pg++;
        else if (q[i] < p[i])
            qg++;
    }
    if (pg && qg)
        return 0;
    else if (qg)
        return 1;
    else if (pg)
        return -1;
    return 0;
}

bool assessible(int *p)
{
    for (int i = 0; i < qnum; i++)
        if (p[i] == -1)
            return false;
    return true;
}

void BFS(int start, int d)
{
    int u, v;
    queue<int> qu;
    int layer = 0, nodeNum = 1, nextnodeNum = 0;
    qu.push(start);
    visited[start] = 1;
    while (!qu.empty())
    {
        u = qu.front();
        qu.pop();
        if (pmap.find(u) != pmap.end())
        {
            int g = pmap[u];
            if (gap[g][d] == -1 || gap[g][d] > layer)
                gap[g][d] = layer;
        }
        for (int j = 0; j < (int)graph[u].size(); j++)
        {
            v = graph[u][j];
            if (!visited[v])
            {
                qu.push(v);
                visited[v] = 1;
                nextnodeNum++;
            }
        }
        nodeNum--;
        if (nodeNum == 0)
        {
            layer++;
            nodeNum = nextnodeNum;
            nextnodeNum = 0;
        }
    }
}

void search()
{
    sps.clear();
	for(int i =0;i<pnum;i++)
    {
        if(!assessible(gap[i]))
            continue;
        bool isSP = true;
        for (vector<int>::iterator it = sps.begin(); it != sps.end();)
		{
			int flag = govern(gap[i], gap[*it]);
			if (flag == -1)
				it = sps.erase(it);
			else if (flag == 1)
			{
				isSP = false;
				break;
			}
			else
				it++;
		}
		if (isSP)
			sps.push_back(i);
    }
}
#pragma endregion

#pragma region use and test
double getUsedTimeMs(struct timeval start, struct timeval end)
{
    return ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;
}

vector<int> inputKeywords()
{
    vector<int> keywords;
    char tempstr[2048];
    cin.getline(tempstr, 2048);
    char seps[] = " ,";
    char *token;
    char *next_token;
    token = strtok(tempstr, seps);
    while (token != NULL)
    {
        int temp = atoi(token);
        keywords.push_back(temp);
        token = strtok(NULL, seps);
    }
    qnum = keywords.size();
    return keywords;
}

double query(vector<int> keywords)
{
    struct timeval start, end;
    vector<int> starts;
    int count=0;
    gettimeofday(&start, NULL);
    clearGap();
    for(int i=0;i<qnum;i++)
    {
        starts = kvmap[keywords[i]];
        for(int j=0;j<starts.size();j++)
        {
            count++;
            BFS(starts[j],i);
        }
    }
    search();
    gettimeofday(&end, NULL);
    cout<<"count:"<<count<<endl;
    return getUsedTimeMs(start,end);
}

void printResult()
{
    cout<<"skyline point number:"<<sps.size()<<endl;
    if(sps.size()<=10)
        for(int i=0;i<sps.size();i++)
            cout<<sps[i]<<" ";
    cout<<endl;
}
#pragma region use

int main()
{
    int flag;
    double time;
    struct timeval start, end;
    vector<int> keywords,sps;

    string keyword_file = "../data/Yago/node_keywords.txt";
    string graph_file = "../data/Yago/edge.txt";
    string site_file = "../data/placeid2coordYagoVB.txt";

    cout << "Reading and creating..." << endl;
    gettimeofday(&start, NULL);
    initRead(keyword_file, graph_file, site_file);
    gettimeofday(&end, NULL);
    double d1 = getUsedTimeMs(start, end);
    cout << "Read and create time：" << d1 << "ms" << endl;

    initGap();
    while(true)
    {
        cout << "continue? quit:0 query:1" << endl;
		cin >> flag;
		getchar();
		if (!flag)
			break;
		cout << "Input keywords：" << endl;
        keywords = inputKeywords();
        time = query(keywords);
        printResult();
        cout<<"query time:"<<time<<endl;
    }
    //test: 8787951,10321429,11375631
    return 0;
}