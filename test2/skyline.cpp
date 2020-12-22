#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h> //Linux System
using namespace std;

static int DATA = 2;                //0:example 1:Yago_small 2:Yago
#define MAX_QNUM 5                  //查询keyword最大数量
static int allnum = 8091179;        //顶点数目：8091179 or 24
static vector<int> *key;            //keyword信息
static vector<int> *graph_T;        //反向邻接表
static vector<int> *graph;          //正向邻接表
static vector<int> ps;              //地点序号存储
static map<int, int> pmap;          //地点坐标检索使用
static map<int, vector<int>> kvmap; //关键字哈希表
static int qnum = 0;                //查询keyword数目
static int pnum = 0;                //site数目
static int **gap;                   //语义距离矩阵：P点到各个keyword的距离
static int *visited;                //记录点是否被访问 0:未访问，1：已访问
static vector<int> sps;             //skyline points result

static vector<double> times; //记录多次测试时间
vector<int> keywords;        //查询关键字集合

typedef struct node
{
    int v;
    struct node *parent;
} * Node;

#pragma region common
double getUsedTimeMs(struct timeval start, struct timeval end)
{
    return ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;
}

void printVectorInt(vector<int> v)
{
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i] << ' ';
    }
    cout << endl;
}
#pragma endregion

#pragma region read and create
string *getFilePaths(int tag)
{
    string *file_paths = new string[3];
    if (tag == 0)
    {
        file_paths[0] = "../data/example/node_keywords.txt";
        file_paths[1] = "../data/example/edge.txt";
        file_paths[2] = "../data/example/placeid.txt";
        allnum = 24;
    }
    else if (tag == 1)
    {
        file_paths[0] = "../data/Yago_small/node_keywords.txt";
        file_paths[1] = "../data/Yago_small/edge.txt";
        file_paths[2] = "../data/placeid2coordYagoVB.txt";
        allnum = 8091179;
    }
    else
    {
        file_paths[0] = "../data/Yago/node_keywords.txt";
        file_paths[1] = "../data/Yago/edge.txt";
        file_paths[2] = "../data/placeid2coordYagoVB.txt";
        allnum = 8091179;
    }
    return file_paths;
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
            break;
        char seps[] = ": ,\n";
        char *token, *next_token;
        token = strtok(temp_str, seps);
        if (token == NULL)
            continue;
        int index = atoi(token), temp;
        token = strtok(NULL, seps);
        while (token != NULL)
        {
            temp = atoi(token);
            key[index].push_back(temp);
            if (kvmap.find(temp) != kvmap.end())
                kvmap[temp].push_back(index);
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

int readGraph(string filename)
{

    int edge_number = 0;
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
            break;
        char seps[] = ": ,\n";
        char *token, *next_token;
        token = strtok(temp_str, seps);
        if (token == NULL)
            continue;
        u = atoi(token);
        token = strtok(NULL, seps);
        while (token != NULL)
        {
            v = atoi(token);
            graph_T[v].push_back(u);
            graph[u].push_back(v);
            token = strtok(NULL, seps);
            edge_number++;
        }
    }
    fclose(file);
    return edge_number;
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
    int u, no = 0;
    while (true)
    {
        char temp_str[10000] = {'\0'};
        if (fgets(temp_str, 10000, file) == NULL)
            break;
        char seps[] = ": ,\n";
        char *token, *next_token;
        token = strtok(temp_str, seps);
        if (token == NULL)
            continue;
        u = atoi(token);
        pmap.insert(pair<int, int>(u, no++));
        ps.push_back(u);
    }
    pnum = ps.size();
    fclose(file);
}

double initRead(string keyword_file, string graph_file, string site_file, bool show_log = true)
{
    struct timeval start, end;
    if (show_log)
        cout << "Reading and creating..." << endl;

    gettimeofday(&start, NULL);

    key = new vector<int>[allnum];
    graph_T = new vector<int>[allnum];
    graph = new vector<int>[allnum];
    visited = new int[allnum];
    readKeyword(keyword_file);
    int edge_number = readGraph(graph_file);
    readSite(site_file);

    gettimeofday(&end, NULL);
    if (show_log)
    {
        cout << "Point number:" << allnum << endl;
        cout << "Site number:" << pnum << endl;
        cout << "Edge number:" << edge_number << endl;
        cout << "Keyword number:" << kvmap.size() << endl;
    }
    return getUsedTimeMs(start, end);
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
            gap[i][j] = -1;
    }
}

void clearGap()
{
    for (int i = 0; i < pnum; i++)
        for (int j = 0; j < MAX_QNUM; j++)
            gap[i][j] = -1;
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
    int layer = 0, node_num = 1, nextnode_num = 0;
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
        for (int j = 0; j < graph_T[u].size(); j++)
        {
            v = graph_T[u][j];
            if (!visited[v])
            {
                qu.push(v);
                visited[v] = 1;
                nextnode_num++;
            }
        }
        node_num--;
        if (node_num == 0)
        {
            layer++;
            node_num = nextnode_num;
            nextnode_num = 0;
        }
    }
}

void BNL()
{
    sps.clear();
    for (int i = 0; i < pnum; i++)
    {
        if (!assessible(gap[i]))
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
//手动输入keyword
void inputKeywords()
{
    cout << "Input keywords：" << endl;
    keywords.clear();
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
}

//随机获得n个keyword
void randomKeywords(int n)
{
    keywords.clear();
    srand(time(NULL));
    for (int i = 0; i < n;)
    {
        int t1 = rand() % allnum;
        if (key[t1].size() > 0)
        {
            int t2 = rand() % key[t1].size();
            keywords.push_back(key[t1][t2]);
            i++;
        }
    }
    qnum = keywords.size();
}

//skyline查询
double query()
{
    struct timeval start, end;
    vector<int> starts;
    int count = 0;
    gettimeofday(&start, NULL);
    clearGap();
    for (int i = 0; i < qnum; i++)
    {
        memset(visited, 0, sizeof(int) * allnum);
        starts = kvmap[keywords[i]];
        for (int j = 0; j < starts.size(); j++)
        {
            count++;
            BFS(starts[j], i);
        }
    }
    BNL();
    gettimeofday(&end, NULL);
    // cout << "count:" << count << endl;
    return getUsedTimeMs(start, end);
}

//获得sp点的语义树结点
vector<int> getTp(int sp)
{
    memset(visited, 0, sizeof(int) * allnum);
    int c, v;
    int layer = 0, node_num = 1, nextnode_num = 0;
    vector<int> c_index;
    for (int i = 0; i < keywords.size(); i++)
        if (gap[sp][i] == 1)
            c_index.push_back(i);
    vector<int> Tp;
    Tp.push_back(sp);
    queue<int> qu;
    qu.push(sp);
    while (!qu.empty())
    {
        c = qu.front();
        qu.pop();
        for (int i = 0; i < graph[c].size(); i++)
        {
            v = graph[c][i];
            for (int j = 0; j < c_index.size(); j++)
                //遍历该层所含的kewords,如果v点中包含该关键字则v加入Tp中
                if (find(key[v].begin(), key[v].end(), keywords[c_index[j]]) != key[v].end())
                    if (find(Tp.begin(), Tp.end(), v) == Tp.end())
                        Tp.push_back(v);
            if (!visited[v])
            {
                qu.push(v);
                visited[v] = 1;
                nextnode_num++;
            }
        }
        node_num--;
        if (node_num == 0)
        {
            layer++;
            c_index.clear();
            for (int i = 0; i < keywords.size(); i++)
                if (gap[sp][i] == layer + 1) //如果距离某个关键字的距离等于当前层,则该层存在包含该关键字的点
                    c_index.push_back(i);    //记录该层所含关键字在keywords中的索引
            node_num = nextnode_num;
            nextnode_num = 0;
        }
    }
    return Tp;
}

//输出Tp
void printTp(vector<int> Tp)
{
    cout << "Tp<" << Tp[0] << ">:" << endl;
    cout << "<P_" << Tp[0] << ",(";
    for (int i = 1; i < Tp.size() - 1; i++)
    {
        if (find(ps.begin(), ps.end(), Tp[i]) == ps.end())
            cout << "V_" << Tp[i] << ",";
        else
            cout << "P_" << Tp[i] << ",";
    }
    if (Tp.size() - 1 >= 0)
        if (find(ps.begin(), ps.end(), Tp[Tp.size() - 1]) == ps.end())
            cout << "V_" << Tp[Tp.size() - 1] << ")>" << endl;
        else
            cout << "P_" << Tp[Tp.size() - 1] << ")>" << endl;
    else
        cout << ")>" << endl;

    for (int i = 0; i < Tp.size(); i++)
    {
        if (find(ps.begin(), ps.end(), Tp[i]) == ps.end())
            cout << " V_" << Tp[i] << ":";
        else
            cout << " P_" << Tp[i] << ":";
        for (int j = 0; j < key[Tp[i]].size(); j++)
            cout << key[Tp[i]][j] << " ";
        cout << endl;
    }
    cout << endl;
}

//输出SP点
void printResultSimple()
{
    cout << "sps:";
    for (int i = 0; i < sps.size(); i++)
    {
        cout << sps[i] << " ";
    }
    cout << endl;
}

//输出详细结果
void printResult()
{
    cout << endl;
    cout << "#====< Query Result >====#" << endl;
    cout << "SP number:" << sps.size() << endl;
    printVectorInt(sps);
    if (sps.size() <= 10000)
    {
        vector<int> Tp;
        for (int i = 0; i < sps.size(); i++)
        {
            Tp = getTp(sps[i]);
            printTp(Tp);
        }
        cout << endl;
    }
    cout << endl;
}

//测试:随机产生n个keyword,循环loop次
void test(int n, int loop, string logfile)
{
    times.clear();
    FILE *file;
    file = fopen(logfile.c_str(), "a");
    if (!file)
    {
        cout << "logfile open filed!" << endl;
        exit(-1);
    }
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S", localtime(&t));
    fprintf(file, "=================%s:(keyword number=%d, loop=%d, data=%d)=================\n", tmp, n, loop, DATA);

    double time;
    for (int i = 0; i < loop;)
    {
        // inputKeywords();
        randomKeywords(n);

        time = query();

        if (sps.size() > 1)
        {
            i++;
            cout << "keywords:";
            printVectorInt(keywords);
            fprintf(file, "Keys:");
            for (int j = 0; j < keywords.size(); j++)
                fprintf(file, "%d ", keywords[j]);

            fprintf(file, "\nSPs:");
            for (int j = 0; j < sps.size(); j++)
                fprintf(file, "%d ", sps[j]);
            fprintf(file, "\ntime:%f\n", time);

            printResultSimple();
            times.push_back(time);
            cout << "query time:" << time << endl;
        }
        // printResult();
    }
    fprintf(file, "times:");
    for (int j = 0; j < times.size(); j++)
        fprintf(file, "%.4f ", times[j]);
    fprintf(file, "\n\n");
    fclose(file);
}

#pragma endregion

int main()
{
    double time;
    string logfiles[2] = {"./Yago_small_logs.txt", "./Yago_logs.txt"};
    for (int i = 0; i < 2; i++)
    {
        DATA = i + 1;
        string *file_paths = getFilePaths(DATA);
        time = initRead(file_paths[0], file_paths[1], file_paths[2]);
        cout << "Successfully! Read and create time：" << time << "ms" << endl;
        int ns[5] = {1, 2, 3, 4, 5};
        int loops[2] = {100, 50};
        initGap();
        for (int j = 0; j < 5; j++)
        {
            test(ns[j], loops[i], logfiles[i]);
        }
    }

    return 0;

    int flag;
    int n, loop;

    string *file_paths = getFilePaths(DATA);

    time = initRead(file_paths[0], file_paths[1], file_paths[2]);
    cout << "Successfully! Read and create time：" << time << "ms" << endl;

    initGap();
    while (true)
    {
        cout << "continue? quit:0 autotest:1 inputtest:2" << endl;
        cin >> flag;
        getchar();
        if (!flag)
            break;
        if (flag == 1)
        {
            cout << "input keywords number:";
            cin >> n;
            getchar();
            cout << "input loop number:";
            cin >> loop;
            test(n, loop, "./result.txt");
        }
        else
        {
            inputKeywords();
            time = query();
            cout << "query time:" << time << endl;
        }
    }
    //test: 8787951,10321429,11375631
    //test: example:0302,1903
    return 0;
}