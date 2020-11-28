#include <iostream>
#include <cstdio>
#include <malloc.h>
#include <cstdlib>
#include<fstream>
#include<vector>
#include<string>
#include<cstring>
#include<queue>
#include<map>
#include<sstream>
#include<time.h>
#include "BTree.h"
#define MAX_DNUM 5
using namespace std; 
static int pnum = 8091179;//8091179,24;			//�����Ŀ
static vector<int> *key;			//�ؼ�����Ϣ�洢
static vector<int> *graph;			//�ڽӱ�洢
static vector<int> site;			//�ص���Ŵ洢
static map<int,int> pmap;			//�ص��������ʹ�� 
static int dnum = 0;				//��ѯ�ؼ�����Ŀ
static int sitenum = 0;
static int **gap;					//��¼���� 
static int maxlayer = 0x3f3f3f3f;	//��¼֧������µ���߲� 
static BTNode *Bt = NULL;
static int *visited;
#pragma region ��ȡ����
void PreSet()
{
	key = new vector<int>[pnum];
	graph = new vector<int>[pnum];
	visited = new int[pnum];
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
			graph[v].push_back(u);
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
	int no = 0;
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
		pmap.insert(pair<int,int>(u,no++));
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
void CreateBTree()
{
	Findres r;
	cout<<"����һ��"<<m<<"��B��"<<endl;
	for(int i = 0; i < pnum; i++)
	{
		int len = key[i].size();
		for(int j = 0; j < len; j++) 
		{
			
			pkey k(key[i][j]);
			k.insert_no(i);
			//cout<<"��"<<j+1<<"����"<<"����Ԫ��"<<key[i][j]<<endl;
			r = SearchBTree(Bt,k);
			if(r.flag == 0)
			{
				InsertBTree(Bt,r.i,k,r.pt);
			}
			else
			{
				r.pt->key[r.i].insert_no(i);
			}
		}
	}
	//PrintBTree(Bt);
}
//***************************************************************
void BFS_CalculateMinDistance(int start,int d)
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
		if(pmap.find(u)!=pmap.end())
		{
			int g = pmap[u];
			if(gap[g][d]==-1||gap[g][d]>layer)
			{
				gap[g][d] = layer;
			}
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
		nodeNum--;				//ÿ��ѭ�����һ���㣬��������ǰ������-1
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
	else if(ag)
	{
		return -1;
	}
	return 0;
}
bool isAssessible(int *temp)
{
	for(int i = 0; i < dnum; i++)
	{
		if(temp[i] == -1)
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
	if(BNLresult.size()==0)
	{
		return;
	}
	//cout << sitenum << endl;
	for (int i = BNLresult[0] + 1; i < sitenum; i++)
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
//***************************************************************
void ShowList(noPoint *head)
{
	noPoint* p = head;
	while(p!=NULL)
	{
		cout << " " << p->no;
		p=p->next;
	}
	cout << endl;
}
void SearchInBTree(int key, Findres &r)
{
	r = SearchBTree(Bt,key);
	cout << "Flag = " << r.flag << endl;
	cout << "sum of point = " << r.pt->key[r.i].sum << endl;
	//cout << "no��" << endl;
	//ShowList(r.pt->key[r.i].nolist);
}
void ShowBNLResult()
{
	//���������� 
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
}
void GetRunTime(FILE *file, int knum)
{
	double starttime, endtime, deltatime;
	vector<int> des;
	dnum = knum;
	int times = 10;
	while(times--)
	{
		cout << "����Ŀ����(�ÿո�������س���ʾ��ʼ���м���)��" << endl;
		for(int i = 0; i < knum; i++)
		{
			int temp1 = rand()%pnum;
			int temp2 = rand()%key[temp1].size();
			des.push_back(key[temp1][temp2]);
			cout << des[i] << ",";
		}
		cout << endl;
		Findres r;
		starttime = clock();
		cout << "****************���ڼ���****************" << endl;
		for(int i = 0; i < dnum; i++)
		{
			SearchInBTree(des[i], r);
			noPoint *p = r.pt->key[r.i].nolist;
			while(p!=NULL)
			{
				memset(visited, 0, sizeof(int)*pnum);
				BFS_CalculateMinDistance(p->no,i);
				//cout<<p->no<<endl;
				p=p->next;
			}
		}
		//ֻ��һ�������ʣ��ҳ�������p��ļ�Ϊskyline������p����Ҫ������� 
		if(dnum == 1)
		{
			SearchInBTree(des[0], r);
			noPoint *p = r.pt->key[r.i].nolist;
			int no;
			pkey res(r.pt->key[r.i].key);
			while(p!=NULL)
			{
				no = p->no;
				if(pmap.find(no) != pmap.end())
				{
					res.insert_no(no);
				}
				p=p->next;
			}
			cout << "sum of P in res = " << res.sum << endl;
			if(res.sum == 0)
			{
				p = r.pt->key[r.i].nolist;
				while(p!=NULL)
				{
					BFS_CalculateMinDistance(p->no,0);
					//cout<<p->no<<endl;
					p=p->next;
				}
				BNL();
				cout << "BNL result's size = " << BNLresult.size() << endl;
				//ShowBNLResult();
			}
		}
		if(dnum>1)
		{
			BNL();
			cout << "BNL result's size = " << BNLresult.size() << endl;
			//ShowBNLResult();
		}
		endtime = clock();
		deltatime = endtime - starttime;
		fprintf(file,"%f\n",deltatime);
		cout << "*********��������ʱ�䣺" << deltatime << endl;
		BNLresult.clear();
		des.clear();
	}
	fclose(file);
}

int main(){
	string timefile = ".\\rumtime.txt";
	FILE *file;
	file = fopen(timefile.c_str(), "a+");
	if (!file)
	{
		cout << "timefile�ļ���ʧ��" << endl;
		exit(-1);
	}
    
	string keyfile = "D:\\AppWorkSpace\\vscode_workspace\\algorithm\\data\\Yago_small\\node_keywords.txt";
	string graphfile = "D:\\AppWorkSpace\\vscode_workspace\\algorithm\\data\\Yago_small\\edge.txt";
	string sitefile = "D:\\AppWorkSpace\\vscode_workspace\\algorithm\\data\\placeid2coordYagoVB.txt";
	 
	/*
	string keyfile = "B:\\�㷨����ҵ\\Yago_small\\node_keywords.txt";
	string graphfile = "B:\\�㷨����ҵ\\Yago_small\\edge.txt";
	string sitefile = "B:\\�㷨����ҵ\\����\\placeid2coordYagoVB.txt";
	*/
	/*
	string keyfile = "B:\\�㷨����ҵ\\question1\\node_kewords.txt";
	string graphfile = "B:\\�㷨����ҵ\\question1\\edge.txt";
	string sitefile = "B:\\�㷨����ҵ\\question1\\position.txt";
	*/
	PreSet();
	ReadKey(keyfile);
	ReadGraph(graphfile);
	ReadSite(sitefile);
	gap = new int*[sitenum];
	for (int i = 0; i < sitenum; i++)
	{
		gap[i] = new int[MAX_DNUM];
		for (int j = 0; j < MAX_DNUM; j++)
		{
			gap[i][j] = -1;
		}
		//cout << gap[i][0] << endl;
	}
	cout <<"������Ŀ�ǣ�"<< pnum << endl;
	cout << "**********************************" << endl;
	double s1 = clock();
	CreateBTree();
	double s2 = clock();
	double d1 = s2 - s1;
	cout << "*********����B������ʱ�䣺" << d1 << endl;
	GetRunTime(file, 3);
	/*
	double starttime, endtime, deltatime;
	PreSet();
	ReadKey(keyfile);
	ReadGraph(graphfile);
	ReadSite(sitefile);
	gap = new int*[sitenum];
	for (int i = 0; i < sitenum; i++)
	{
		gap[i] = new int[MAX_DNUM];
		for (int j = 0; j < MAX_DNUM; j++)
		{
			gap[i][j] = -1;
		}
		//cout << gap[i][0] << endl;
	}
	vector<int> des;
	cout <<"������Ŀ�ǣ�"<< pnum << endl;
	cout << "**********************************" << endl;
	starttime = clock();
	CreateBTree();
	endtime = clock();
	deltatime = endtime - starttime;
	cout << "*********����B������ʱ�䣺" << deltatime << endl;
	//BFS_CreateBTree();
	//Show();
	while(1)
	{
		cout << "�Ƿ������������������1����������0" <<endl;
		int flag;
		cin>>flag;
		getchar();
		if(!flag)
		{
			break;
		}
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
		//cout<<des[0]<<endl;
		Findres r;
		starttime = clock();
		cout << "****************���ڼ���****************" << endl;
		for(int i = 0; i < dnum; i++)
		{
			SearchInBTree(des[i], r);
			noPoint *p = r.pt->key[r.i].nolist;
			while(p!=NULL)
			{
				//�˲������Ż�������������ص� 
				memset(visited, 0, sizeof(int)*pnum);
				BFS_CalculateMinDistance(p->no,i);
				//cout<<p->no<<endl;
				p=p->next;
			}
		}
		//ֻ��һ�������ʣ��ҳ�������p��ļ�Ϊskyline������p����Ҫ������� 
		if(dnum == 1)
		{
			SearchInBTree(des[0], r);
			noPoint *p = r.pt->key[r.i].nolist;
			int no;
			pkey res(r.pt->key[r.i].key);
			while(p!=NULL)
			{
				no = p->no;
				if(pmap.find(no) != pmap.end())
				{
					res.insert_no(no);
				}
				p=p->next;
			}
			cout << "sum of P in res = " << res.sum << endl;
			if(res.sum == 0)
			{
				p = r.pt->key[r.i].nolist;
				while(p!=NULL)
				{
					BFS_CalculateMinDistance(p->no,0);
					//cout<<p->no<<endl;
					p=p->next;
				}
				BNL();
				cout << "BNL result's size = " << BNLresult.size() << endl;
				ShowBNLResult();
			}
		}
		if(dnum>1)
		{
			BNL();
			cout << "BNL result's size = " << BNLresult.size() << endl;
			ShowBNLResult();
		}
		endtime = clock();
		deltatime = endtime - starttime;
		fprintf(file,"%f",deltatime);
		cout << "*********��������ʱ�䣺" << deltatime << endl;
		BNLresult.clear();
		des.clear();
	}
	fclose(file);
	*/
	//Test1();
    return 0;
}
