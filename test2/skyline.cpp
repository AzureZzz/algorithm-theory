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
#include <sys/time.h> //Linux System
using namespace std;

#define MAX_QNUM 5                  //查询keyword最大数量
int allnum = 24;                    //顶点数目：8091179 or 24
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

typedef struct node
{
    int v;
    struct node *parent;
} * Node;

double getUsedTimeMs(struct timeval start, struct timeval end)
{
    return ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;
}

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
        cout << "point number" << allnum << endl;
        cout << "site number:" << pnum << endl;
        cout << "edge number:" << edge_number << endl;
        cout << "keyword number:" << kvmap.size() << endl;
    }
    cout << "" << endl;
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

void search()
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
    int count = 0;
    gettimeofday(&start, NULL);
    clearGap();
    for (int i = 0; i < qnum; i++)
    {
        starts = kvmap[keywords[i]];
        for (int j = 0; j < starts.size(); j++)
        {
            count++;
            BFS(starts[j], i);
        }
    }
    search();
    gettimeofday(&end, NULL);
    cout << "count:" << count << endl;
    return getUsedTimeMs(start, end);
}

void printResult()
{
    cout << "skyline point number:" << sps.size() << endl;
    if (sps.size() <= 10)
        for (int i = 0; i < sps.size(); i++)
            cout << sps[i] << " ";
    cout << endl;
}

void printTree(int sp, vector<int> keywords)
{
    Node u;
    int v;
    queue<Node> qu;
    int maxlayer = 0;
    for (int i = 0; i < MAX_QNUM; i++)
        if (gap[sp][i] > maxlayer)
            maxlayer = gap[sp][i];
    vector<Node> *layers = new vector<Node>[maxlayer];
    memset(visited, 0, sizeof(int) * pnum);

    //构造一棵最大层为最远语义距离的正向BFS树
    int layer = 0, node_num = 1, nextnode_num = 0;

    Node root = new struct node;
    root->parent = NULL;
    root->v = sp;
    qu.push(root);
    visited[sp] = 1;
    while (!qu.empty() && layer <= maxlayer)
    {
        u = qu.front();
        qu.pop();
        for (int j = 0; j < graph[u->v].size(); j++)
        {
            v = graph[u->v][j];
            if (!visited[v])
            {
                Node n = new struct node;
                n->v = v;
                n->parent = u;
                layers[layer].push_back(n);
                qu.push(n);
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

    vector<Node> *tree = new vector<Node>[maxlayer];
    //从最大层开始剪去无用结点
    for (int i = maxlayer - 1; i > 0; i--)
    {
        for (int j = 0; j < MAX_QNUM; j++)
        {
            if ((i + 1) == gap[sp][j])
            {
                for (int k = 0; k < layers[i].size(); k++)
                {
                    vector<int> keys = key[layers[i][k]->v];
                    if (find(keys.begin(), keys.end(), keywords[j]) != keys.begin())
                    {
                        tree[i].push_back(layers[i][k]);
                        tree[i - 1].push_back(layers[i][k]->parent);
                    }
                }
            }
            for (int k = 0; k < tree[i].size(); k++)
            {
                tree[i - 1].push_back(tree[i][k]->parent);
            }
        }
    }
    for (int j = 0; j < MAX_QNUM; j++)
    {
        if (gap[sp][j] == 1)
        {
            for (int k = 0; k < layers[0].size(); k++)
            {
                vector<int> keys = key[layers[0][j]->v];
                if (find(keys.begin(), keys.end(), keywords[j]) != keys.begin())
                {
                    tree[0].push_back(layers[0][j]);
                }
            }
        }
    }
}
#pragma endregion

int main()
{
    int flag;
    double time;
    vector<int> keywords;
    string *file_paths = getFilePaths(0);

    time = initRead(file_paths[0], file_paths[1], file_paths[2]);
    cout << "Read and create time：" << time << "ms" << endl;

    initGap();
    while (true)
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
        cout << "query time:" << time << endl;
    }
    //test: 8787951,10321429,11375631
    return 0;
}