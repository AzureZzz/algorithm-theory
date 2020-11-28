#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<cstring>
#include<queue>
#include<sstream>
#include<ctime>
using namespace std;
struct Node {
	string name;			//结点名称存放
	vector<string> key;		//关键词存放
	Node(string n=""):name(n){}
};
static vector<Node> node;			//点类信息存储
static int pnum = 8091179;//8091179,24;			//点的数目
static vector<int> *key;			//关键词信息存储
static vector<int> *graph;			//邻接表存储
static vector<int> site;			//地点序号存储
static int dnum = 0;				//查询关键词数目
static int sitenum = 0;
static int **gap;
static int *cengshu;				//记录每个点的层数 
static int maxlayer = 0x3f3f3f3f;	//记录支配情况下的最高层 
#pragma region 读取数据
void PreSet()
{
	key = new vector<int>[pnum];
	graph = new vector<int>[pnum];
	cengshu = new int[pnum];
	memset(cengshu, 0, sizeof(int)*pnum);
}
void ReadKey(string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		cout << "key文件打开失败" << endl;
		exit(-1);
	}
	while (1)
	{
		char tempstr[10000] = { '\0' };
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
void ReadGraph(string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		cout << "graph文件打开失败" << endl;
		exit(-1);
	}
	int u, v;
	while (1)
	{
		char tempstr[10000] = { '\0' };
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
			graph[u].push_back(v);
			token = strtok(NULL, seps);
		}
	}
	fclose(file);
}
void ReadSite(string filename)
{
	FILE *file;
	file = fopen(filename.c_str(), "r");
	if (!file)
	{
		cout << "site文件打开失败" << endl;
		exit(-1);
	}
	fscanf(file, "%d#\n", &sitenum);
	//cout << sitenum << endl;
	int u;
	while (1)
	{
		char tempstr[10000] = { '\0' };
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
		site.push_back(u);
	}
	sitenum = site.size();
	fclose(file);
}
void Show()
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

#pragma region 获取最短距离信息并保留在gapfile文件中
//计算最短距离（BFS遍历层数）
static int *visited;
void BFS_CalculateMinDistance(int x, int start, vector<int> des)
{
	// int count = 0;
	// memset(visited, 0, sizeof(int)*pnum);
	int u, v, sum = 0;
	queue<int> que;
	int layer = 0, nodeNum = 1, nextnodeNum = 0;
	que.push(start);
	visited[start] = 1;
	while (!que.empty())
	{
		/* 
		//剪枝 
		if(layer>maxlayer)
		{
			return;
		}
		*/ 
		u = que.front();
		//cout << "u = " << u << endl;
		que.pop();
		for (int i = 0; i < (int)key[u].size(); i++)
		{
			for (int k = 0; k < (int)des.size(); k++)
			{
				if (key[u][i] == des[k] && gap[x][k] == 0x3f3f3f3f)
				{
					gap[x][k] = layer;
					sum++;
				}
			}
		}
		if (sum == des.size())
		{
			return;
		}
		for (int j = 0; j < (int)graph[u].size(); j++)
		{
			v = graph[u][j];
			if (!visited[v])	//未访问过
			{
				que.push(v);
				//cout << "v = " << v << endl;
				visited[v] = 1;
				nextnodeNum++;
			}
		}
		nodeNum--;	//每次循环输出一个点，即遍历当前层结点数-1
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
void GetMinDistanceFile(vector<int> des, int times = 1000)
{
	visited = new int[pnum];
	gap = new int*[sitenum];
	for (int i = 0; i < sitenum; i++)
	{
		gap[i] = new int[dnum];
		for (int j = 0; j < dnum; j++)
		{
			gap[i][j] = 0x3f3f3f3f;
		}
		//cout << gap[i][0] << endl;
	}
	int low;
	int high = 0;
	for(int i = 0; i < times; i++)
	{
		memset(visited, 0, sizeof(int)*pnum);
		// cout << i << endl; 
		low = high;
		high = (double)(i+1)*sitenum/(double)times;
		//cout<< low << " " << high <<endl;
		for (int j = low; j < high; j++)
		{
			if(!visited[j])
			{
				//cout<< "start = " << site[j] << endl;
				BFS_CalculateMinDistance(j, site[j], des);	
			}
		}
	}
	/*
	//具有优化剪枝思想 
	for (int j = 0; j < sitenum/2; j++)
	{
		BFS_CalculateMinDistance(j, site[j], des);
		
		//剪枝优化 
		int temp = gap[j][0];
		for(int k = 1; k < dnum; k++)
		{
			if(temp < gap[j][k])
			{
				temp = gap[j][k];
			}
		}
		if(maxlayer > temp)
		{
			maxlayer = temp;
		}
		
		cout << j << endl;
		for (int i = 0; i < dnum; i++)
		{
			cout << gap[j][i] << " ";
		}
		cout << endl;
		
	}
	*/
	delete[]visited;
}

#pragma endregion

#pragma region skyline查询算法一：BNL算法
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
		else if(b[i] < a[i])
		{
			bg++;
		}
	}
	if (ag&&bg)
	{
		return 0;
	}
	else if(bg)
	{
		return 1;
	}
	else
	{
		return -1;
	}
	return 0;
}
bool isAssessible(int *temp)
{
	for(int i = 0; i < dnum; i++)
	{
		if(temp[i] == 0x3f3f3f3f)
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
	for(int i = 0; i<sitenum; i++)
	{
		if(isAssessible(gap[i]))
		{
			BNLresult.push_back(i);
			break;
		}
	}
	//cout << sitenum << endl;
	for (int i = 0; i < sitenum; i++)
	{
		bool isSP = true;
		//新引入节点与窗口节点之间的对比
		if(!isAssessible(gap[i]))
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
			else if(flag == 1)
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


#pragma endregion


int main()
{
	/*
	string keyfile = "B:\\算法大作业\\Yago\\node_keywords.txt";
	string graphfile = "B:\\算法大作业\\Yago\\edge.txt";
	string sitefile = "B:\\算法大作业\\数据\\placeid2coordYagoVB.txt";
	*/
	
	string keyfile = "B:\\算法大作业\\Yago_small\\node_keywords.txt";
	string graphfile = "B:\\算法大作业\\Yago_small\\edge.txt";
	string sitefile = "B:\\算法大作业\\数据\\placeid2coordYagoVB.txt";
	
	/*
	string keyfile = "B:\\算法大作业\\question1\\node_kewords.txt";
	string graphfile = "B:\\算法大作业\\question1\\edge.txt";
	string sitefile = "B:\\算法大作业\\question1\\position.txt";
	*/
	double starttime, endtime, deltatime;
	PreSet();
	ReadKey(keyfile);
	ReadGraph(graphfile);
	ReadSite(sitefile);
	vector<int> des;
	cout <<"顶点数目是："<< pnum << endl;
	//Show();
	cout << "请告诉你的检索目标是(用空格隔开，回车表示开始进行检索)：" << endl;
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

	starttime = clock();
	GetMinDistanceFile(des);
	/*
	for (int i = 0; i < sitenum; i++)
	{
		for (int j = 0; j < dnum; j++)
		{
			cout << gap[i][j] << " ";
		}
		cout << endl;
	}
	*/
	endtime = clock();
	cout << "****************关键词距离计算完毕！******************" << endl;
	cout << "距离计算的时间：" << endtime - starttime << endl;
	BNL();
	cout << "****************BNL查询结果如下所示******************" << endl;
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
		cout << endl;
	}
	endtime = clock();
	deltatime = endtime - starttime;
	cout << "平均总查询时间：" << deltatime << endl;
	//Show();
	/*
	for (int i = 0; i < (int)site.size(); i++)
	{
		cout << site[i] << endl;
	}
	
	for (int i = 0; i < (int)des.size(); i++)
	{
		cout << des[i] << ",";
	}
	cout << endl;
	*/
	
	return 0;
}
