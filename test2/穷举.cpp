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
	string name;			//������ƴ��
	vector<string> key;		//�ؼ��ʴ��
	Node(string n=""):name(n){}
};
static vector<Node> node;			//������Ϣ�洢
static int pnum = 8091179;//8091179,24;			//�����Ŀ
static vector<int> *key;			//�ؼ�����Ϣ�洢
static vector<int> *graph;			//�ڽӱ�洢
static vector<int> site;			//�ص���Ŵ洢
static int dnum = 0;				//��ѯ�ؼ�����Ŀ
static int sitenum = 0;
static int **gap;
static int *cengshu;				//��¼ÿ����Ĳ��� 
static int maxlayer = 0x3f3f3f3f;	//��¼֧������µ���߲� 
#pragma region ��ȡ����
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
		cout << "key�ļ���ʧ��" << endl;
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
		cout << "graph�ļ���ʧ��" << endl;
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
		cout << "site�ļ���ʧ��" << endl;
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

#pragma region ��ȡ��̾�����Ϣ��������gapfile�ļ���
//������̾��루BFS����������
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
		//��֦ 
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
			if (!visited[v])	//δ���ʹ�
			{
				que.push(v);
				//cout << "v = " << v << endl;
				visited[v] = 1;
				nextnodeNum++;
			}
		}
		nodeNum--;	//ÿ��ѭ�����һ���㣬��������ǰ������-1
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
	//�����Ż���֦˼�� 
	for (int j = 0; j < sitenum/2; j++)
	{
		BFS_CalculateMinDistance(j, site[j], des);
		
		//��֦�Ż� 
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

#pragma region skyline��ѯ�㷨һ��BNL�㷨
//����ֵ˵����1��ʾa<b��-1��ʾb<a��0��ʾ����û��֧���ϵ
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
		//������ڵ��봰�ڽڵ�֮��ĶԱ�
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
	string keyfile = "B:\\�㷨����ҵ\\Yago\\node_keywords.txt";
	string graphfile = "B:\\�㷨����ҵ\\Yago\\edge.txt";
	string sitefile = "B:\\�㷨����ҵ\\����\\placeid2coordYagoVB.txt";
	*/
	
	string keyfile = "B:\\�㷨����ҵ\\Yago_small\\node_keywords.txt";
	string graphfile = "B:\\�㷨����ҵ\\Yago_small\\edge.txt";
	string sitefile = "B:\\�㷨����ҵ\\����\\placeid2coordYagoVB.txt";
	
	/*
	string keyfile = "B:\\�㷨����ҵ\\question1\\node_kewords.txt";
	string graphfile = "B:\\�㷨����ҵ\\question1\\edge.txt";
	string sitefile = "B:\\�㷨����ҵ\\question1\\position.txt";
	*/
	double starttime, endtime, deltatime;
	PreSet();
	ReadKey(keyfile);
	ReadGraph(graphfile);
	ReadSite(sitefile);
	vector<int> des;
	cout <<"������Ŀ�ǣ�"<< pnum << endl;
	//Show();
	cout << "�������ļ���Ŀ����(�ÿո�������س���ʾ��ʼ���м���)��" << endl;
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
	cout << "****************�ؼ��ʾ��������ϣ�******************" << endl;
	cout << "��������ʱ�䣺" << endtime - starttime << endl;
	BNL();
	cout << "****************BNL��ѯ���������ʾ******************" << endl;
	for (int i = 0; i < (int)BNLresult.size(); i++)
	{
		int sp = site[BNLresult[i]];
		cout << "��" << i + 1 << "��SP����" << sp << endl;
		cout << "�ؼ���" << endl;
		for (int j = 0; j < (int)key[sp].size(); j++)
		{
			cout << key[sp][j] << " ";
		}
		cout << endl;
		cout << "����" << endl;
		for (int j = 0; j < dnum; j++)
		{
			cout << gap[BNLresult[i]][j] << " ";
		}
		cout << endl;
	}
	endtime = clock();
	deltatime = endtime - starttime;
	cout << "ƽ���ܲ�ѯʱ�䣺" << deltatime << endl;
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
