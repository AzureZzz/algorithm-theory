#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <cstdlib>
#include <vector>
using namespace std;
struct noPoint
{
	int no;
	noPoint *next;
	noPoint(int _no, noPoint *n = NULL) : no(_no), next(n)
	{
	}
};
struct pkey
{
	int key;
	int sum;
	noPoint *nolist;
	noPoint *tail;
	pkey(int k = 0) : key(k)
	{
		nolist = NULL;
		tail = NULL;
		sum = 0;
	}
	void insert_no(int no)
	{
		if (nolist == NULL)
		{
			nolist = new noPoint(no);
			tail = nolist;
		}
		else
		{
			noPoint *p = new noPoint(no);
			tail->next = p;
			tail = p;
		}
		sum++;
	}
};
//地点编号-关键词类
//#ifndef _DTREE_H
#define _BTREE_H
#define MAX_M 10			 //B树的最大阶数
const int m = 4;			 //设定B树的阶数
const int Max = m - 1;		 //结点的最大关键词数量
const int Min = (m - 1) / 2; //结点的最小关键词数量
typedef pkey keytype;		 //KeyType为关键词类型

//B树和B树结点类型
typedef struct node
{
	int keynum;				 //结点关键词的个数
	keytype key[MAX_M];		 //关键词数组，其中key[0]使用
	struct node *parent;	 //双亲结点指针
	struct node *ptr[MAX_M]; //孩子结点指针
} BTNode, *BTree;

//B树查找结果类型
struct Findres
{
	node *pt; //指向找到的结点
	int i;	  //在结点中的关键字位置
	int flag; //查找成功与否的标志
};
//链表结点
typedef struct LNode
{
	BTree data;			//数据域
	struct LNode *next; //指针域
} LNode, *LinkList;

typedef enum status
{
	TRUE,
	FALSE,
	OK,
	ERROR,
	OVERFLOW,
	EMPTY
} Status;
#pragma region B树方法声明
//	初始化B树
Status InitBTree(BTree &t)
{
	t = NULL;
	return OK;
}

//	在结点p中查找关键词k的插入位置i
int SearchBTNode(BTNode *p, keytype k)
{
	int i = 0;
	for (i = 0; i < p->keynum && p->key[i + 1].key <= k.key; i++)
		;
	return i;
}

//	在树t种查找关键词k，返回查找结果类型
Findres SearchBTree(BTree t, keytype k)
{
	BTNode *p = t, *q = NULL; //初始化结点p和结点q，p指向待查结点
	int found_flag = 0;		  //设定查找成功与否的标志
	int i = 0;
	Findres r;
	while (p != NULL && !found_flag)
	{
		i = SearchBTNode(p, k);				 //在结点中查找关键词k
		if (i > 0 && p->key[i].key == k.key) //查找成功
		{
			found_flag = 1;
		}
		else //查找失败
		{
			q = p;
			p = p->ptr[i];
		}
	}
	if (found_flag == 1) //查找成功
	{
		r.pt = p;
		r.i = i;
		r.flag = 1;
	}
	else //查找失败
	{
		r.pt = q;
		r.i = i;
		r.flag = 0;
	}
	return r; //返回关键词k的位置（或者插入位置）
}

//	将关键词k和结点q分别插入到p->key[i+1]和p->ptr[i+1]中
void InsertBTNode(BTNode *&p, int i, keytype k, BTNode *q)
{
	int j;
	for (j = p->keynum; j > i; j--)
	{
		p->key[j + 1] = p->key[j];
		p->ptr[j + 1] = p->ptr[j];
	}
	p->key[i + 1] = k;
	p->ptr[i + 1] = q;
	if (q != NULL)
	{
		q->parent = p;
	}
	p->keynum++;
}

//	将结点p分裂成2个结点，前一半保留，后一半移入结点q
void SplitBTNode(BTNode *&p, BTNode *&q)
{
	int i;
	int s = (m + 1) / 2;
	q = (BTNode *)malloc(sizeof(BTNode)); //给结点q分配空间

	q->ptr[0] = p->ptr[s];
	for (i = s + 1; i <= m; i++)
	{
		q->key[i - s] = p->key[i];
		q->ptr[i - s] = p->ptr[i];
	}
	q->keynum = p->keynum - s;
	q->parent = p->parent;
	for (i = 0; i <= p->keynum - s; i++)
	{
		if (q->ptr[i] != NULL)
		{
			q->ptr[i]->parent = q;
		}
	}
	p->keynum = s - 1;
}

//	生成新的根结点t，原p和q为子树指针
void NewRoot(BTNode *&t, keytype k, BTNode *p, BTNode *q)
{
	t = (BTNode *)malloc(sizeof(BTNode)); //分配空间
	t->keynum = 1;
	t->ptr[0] = p;
	t->ptr[1] = q;
	t->key[1] = k;
	if (p != NULL)
	{
		p->parent = t;
	}
	if (q != NULL)
	{
		q->parent = t;
	}
	t->parent = NULL;
}

//在树t中插入关键词k，返回插入结果
void InsertBTree(BTree &t, int i, keytype k, BTNode *p)
{
	BTNode *q;
	int finish_flag, newroot_flag, s;
	keytype x;
	if (!t) //t是空树%
	{
		NewRoot(t, k, NULL, NULL);
	}
	else
	{
		x = k;
		q = NULL;
		finish_flag = 0;
		newroot_flag = 0;
		while (finish_flag == 0 && newroot_flag == 0)
		{
			InsertBTNode(p, i, x, q); //将关键词x和q分别插入到p->key[i+1]和p->ptr[i+1]
			if (p->keynum <= Max)
			{
				finish_flag = 1; //插入完成
			}
			else
			{
				s = (m + 1) / 2;
				SplitBTNode(p, q); //分裂结点
				x = p->key[s];
				if (p->parent) //查找x的插入位置
				{
					p = p->parent;
					i = SearchBTNode(p, x);
				}
				else //没找到x，需要新结点
				{
					newroot_flag = 1;
				}
			}
		}
		if (newroot_flag == 1) //根结点已分裂为结点p和q
		{
			NewRoot(t, x, p, q); //生成新根结点t,p和q为子树指针
		}
	}
}

//	从p结点删除key[i]和它的孩子指针ptr[i]
void Remove(BTNode *p, int i)
{
	int j;
	for (j = i + 1; j <= p->keynum; j++)
	{
		p->key[j - 1] = p->key[j];
		p->ptr[j - 1] = p->ptr[j];
	}
	p->keynum--;
}

//	寻找替代值（右子树中的最小关键词）%
void Substitution(BTNode *p, int i)
{
	BTNode *q;
	for (q = p->ptr[i]; q->ptr[0] != NULL; q = q->ptr[0])
		;
	p->key[i] = q->key[1]; //复制关键词
}

//将双亲结点p中的最后一个关键词移入右结点q中
//并将左结点ap中的最后一个关键词移入双亲结点p中
void MoveRight(BTNode *p, int i)
{
	int j;
	BTNode *q = p->ptr[i];
	BTNode *aq = p->ptr[i - 1];

	for (j = q->keynum; j > 0; j--) //将右兄弟q中的所有关键词后移一位
	{
		q->key[j + 1] = q->key[j];
		q->ptr[j + 1] = q->ptr[j];
	}

	q->ptr[1] = q->ptr[0]; //从双亲结点p移动关键词到右兄弟q中
	q->key[1] = p->key[i];
	q->keynum++;

	p->key[i] = aq->key[aq->keynum]; //将左兄弟aq中最后一个关键词移动到双亲结点中
	p->ptr[i]->ptr[0] = aq->ptr[aq->keynum];
	aq->keynum--;
}

//将双亲结点p中的第一个关键词移入左结点aq中
//并将右结点q中的第一个关键词移入双亲结点p中
void MoveLeft(BTNode *p, int i)
{
	int j;
	BTNode *aq = p->ptr[i - 1];
	BTNode *q = p->ptr[i];

	aq->keynum++; //把双亲结点p中的关键字移动到左兄弟aq中
	aq->key[aq->keynum] = p->key[i];
	aq->ptr[aq->keynum] = p->ptr[i]->ptr[0];

	p->key[i] = q->key[1]; //将右兄弟q中的关键词移动到双亲结点p中
	q->ptr[0] = q->ptr[1];
	q->keynum--;
	for (j = 1; j <= q->keynum; j++) //%将右兄弟q中所有关键词向前移动一位
	{
		q->key[j] = q->key[j + 1];
		q->ptr[j] = q->ptr[j + 1];
	}
}

//双亲结点p，右结点q合并入左结点aq，
//并调整双亲结点p中剩余关键字的位置
void Combine(BTNode *p, int i)
{
	int j;
	BTNode *q = p->ptr[i];
	BTNode *aq = p->ptr[i - 1];
	//将双亲结点的关键词p->key[i]插入到左结点aq
	aq->keynum++;
	aq->key[aq->keynum] = p->key[i];
	aq->ptr[aq->keynum] = q->ptr[0];
	//将右结点q中所有的关键词都插入到左结点aq
	for (j = 1; j <= q->keynum; j++)
	{
		aq->keynum++;
		aq->key[aq->keynum] = q->key[j];
		aq->ptr[aq->keynum] = q->ptr[j];
	}
	//将双亲结点p中的p->key[i]后的所有关键词向前移动一位
	for (j = i; j < p->keynum; j++)
	{
		p->key[j] = p->key[j + 1];
		p->ptr[j] = p->ptr[j + 1];
	}
	//修改双亲结点的关键词数目
	p->keynum--;
	//释放空右结点q的空间
	free(q);
}

//删除结点p中的第i个关键词后，调整B树
void AdjustBTree(BTNode *p, int i)
{
	if (i == 0) //删除的是最左边的关键词
	{
		if (p->ptr[1]->keynum > Min) //右结点可借
		{
			MoveLeft(p, 1);
		}
		else //右结点不够借
		{
			Combine(p, 1);
		}
	}
	//删除最右边的关键词
	else if (i == p->keynum)
	{
		//左结点可借
		if (p->ptr[i - 1]->keynum > Min)
		{
			MoveRight(p, i);
		}
		//左结点不可借
		else
		{
			Combine(p, i);
		}
	}
	//删除关键词在中部且左结点够借
	else if (p->ptr[i - 1]->keynum > Min)
	{
		MoveRight(p, i);
	}
	//删除关键词在中部且右结点够借
	else if (p->ptr[i + 1]->keynum > Min)
	{
		MoveRight(p, i + 1);
	}
	//删除关键字在中部且左右结点都不够借
	else
	{
		Combine(p, i);
	}
}
//查询结点p中是否找到关键词k
int FindBTNode(BTNode *p, keytype k, int &i)
{
	if (k.key < p->key[1].key)
	{
		//结点p中查找关键词失败
		i = 0;
		return 0;
	}
	else
	{
		//在p点中查找
		i = p->keynum;
		while (k.key < p->key[i].key && i > 1)
		{
			i--;
		}
		if (k.key == p->key[i].key)
		{
			return 1;
		}
	}
	return 0;
}

//在结点p中查找并删除关键字k
int BTNodeDelete(BTNode *p, keytype k)
{
	int i;
	//查找标志
	int found_flag;
	if (!p)
	{
		return 0;
	}
	else
	{
		//返回查找结果
		found_flag = FindBTNode(p, k, i);
		if (found_flag == 1)
		{
			//删除的不是叶子结点
			if (p->ptr[i - 1] != NULL)
			{
				//寻找相邻关键词（也就是右子树中最小的关键词）
				Substitution(p, i);
				//删除操作:一直往前移动，直到叶子结点停止。
				BTNodeDelete(p->ptr[i], p->key[i]);
			}
			else
			{
				//结点p中位置i处删除关键词
				Remove(p, i);
			}
		}
		else
		{
			//沿孩子结点递归查找并删除关键词k
			found_flag = BTNodeDelete(p->ptr[i], k);
		}
		if (p->ptr[i] != NULL)
		{
			if (p->ptr[i]->keynum < Min)
			{
				AdjustBTree(p, i);
			}
		}
		return found_flag;
	}
}

//构建删除框架，执行删除操作
void BTreeDelete(BTree &t, keytype k)
{
	BTNode *p;
	//删除关键词k
	int flag = BTNodeDelete(t, k);
	//查找失败
	if (flag == 0)
	{
		cout << "关键词" << k.key << "不在B树中" << endl;
	}
	else if (t->keynum == 0)
	{
		p = t;
		t = t->ptr[0];
		free(p);
	}
}

//递归释放B树
void DestroyBTree(BTree &t)
{
	int i;
	BTNode *p = t;
	//B树不为空
	if (p != NULL)
	{
		for (i = 0; i <= p->keynum; i++)
		{
			//释放每个结点
			DestroyBTree(*&p->ptr[i]); //%
		}
		free(p);
	}
	t = NULL;
}

//	初始化队列
Status InitQueue(LinkList &L)
{
	//分配结点空间
	L = (LNode *)malloc(sizeof(LNode));
	if (L == NULL)
	{
		return OVERFLOW;
	}
	L->next = NULL;
	return OK;
}

//新建一个结点
LNode *CreateNode(BTNode *p)
{
	LNode *q;
	q = (LNode *)malloc(sizeof(LNode));
	if (q != NULL)
	{
		q->data = p;
		q->next = NULL;
	}
	return q;
}

//元素q入队列
Status Enqueue(LNode *p, BTNode *q)
{
	if (p == NULL)
	{
		return ERROR;
	}
	//调至队列最后
	while (p->next != NULL)
	{
		p = p->next;
	}
	//生成结点让q进入队列
	p->next = CreateNode(q);
	return OK;
}

//出队列，并以q返回值
Status Dequeue(LNode *p, BTNode *&q)
{
	LNode *aq;
	//删除位置不合理
	if (p == NULL || p->next == NULL)
	{
		return ERROR;
	}
	//修改被删除结点前的next值
	aq = p->next;
	p->next = aq->next;
	q = aq->data;
	//释放首个节点
	free(aq);
	return OK;
}

//销毁整个队列
void DestroyQueue(LinkList L)
{
	LinkList p;
	if (L != NULL)
	{
		p = L;
		L = L->next;
		free(p); //逐一释放
		DestroyQueue(L);
	}
}
//用队列遍历输出B树
Status Traverse(BTree t, LinkList L, int newline, int sum)
{
	int i;
	BTree p;
	if (t != NULL)
	{
		cout << "[";
		Enqueue(L, t->ptr[0]); //入队
		for (i = 1; i <= t->keynum; i++)
		{
			cout << " " << t->key[i].key;
			Enqueue(L, t->ptr[i]); //子结点入队
		}
		sum += t->keynum + 1;
		cout << "]";
		if (newline == 0)
		{
			cout << endl;
			newline = sum - 1;
			sum = 0;
		}
		else
		{
			newline--;
		}
	}
	//L不为空
	if (L != NULL && L->next != NULL)
	{
		Dequeue(L, p);				  //出队，以p返回
		Traverse(p, L, newline, sum); //遍历出队结点
	}
	return OK;
}

//输出B树
Status PrintBTree(BTree t)
{
	LinkList L;
	if (t == NULL)
	{
		cout << "该B树是空树";
		return OK;
	}
	InitQueue(L);		  //初始化队列
	Traverse(t, L, 0, 0); //利用队列输出
	DestroyQueue(L);	  //销毁队列
	return OK;
}

void Test1()
{
	BTNode *t = NULL;
	Findres r;
	int j, n = 15;
	int k;
	int a[] = {1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	cout << "创建一颗" << m << "阶B树" << endl;
	for (j = 0; j < n; j++)
	{
		r = SearchBTree(t, pkey(a[j]));
		if (r.flag == 0)
		{
			//cout<<"第"<<j+1<<"步，"<<"插入元素"<<a[j]<<endl;
			InsertBTree(t, r.i, pkey(a[j]), r.pt);
		}

		PrintBTree(t);
	}

	cout << endl;
	cout << "删除操作" << endl;
	k = 9;
	BTreeDelete(t, pkey(k));
	cout << "删除" << k << endl;
	cout << "删除后的B树" << endl;
	PrintBTree(t);
	cout << endl;

	k = 1;
	BTreeDelete(t, pkey(k));
	cout << "删除" << k << endl;
	cout << "删除后的B树" << endl;
	PrintBTree(t);
	cout << endl;

	cout << "递归释放B树" << endl;
	DestroyBTree(t);
	PrintBTree(t);
}

void Test2()
{
	int i, k;
	BTree t = NULL;
	Findres r;
	while (1)
	{
		cout << "此时的B树：" << endl;
		PrintBTree(t);
		cout << endl;
		cout << "*****************操作列表如下****************" << endl;
		cout << "1、初始化；2、插入；3、删除；4、销毁；5、退出" << endl;
		cout << "请输入操作符整数" << endl;
		cin >> i;
		switch (i)
		{
		case 1:
		{
			InitBTree(t);
			cout << "初始化B树成功" << endl;
			PrintBTree(t);
			cout << endl;
			break;
		}
		case 2:
		{
			cout << "输入要插入B树的数字" << endl;
			cin >> k;
			r = SearchBTree(t, pkey(k));
			InsertBTree(t, r.i, k, r.pt);
			PrintBTree(t);
			cout << endl;
			cout << "插入成功" << endl;
			break;
		}
		case 3:
		{
			cout << "输入要删除B树中的数字" << endl;
			cin >> k;
			BTreeDelete(t, pkey(k));
			cout << endl;
			PrintBTree(t);
			cout << endl;
			cout << "删除成功" << endl;
			break;
		}
		case 4:
		{
			DestroyBTree(t);
			cout << "销毁成功" << endl;
			break;
		}
		case 5:
		{
			exit(-1);
			break;
		}
		}
	}
}

#pragma endregion
