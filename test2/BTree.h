#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <cstdlib>
#include <vector>
using namespace std; 
struct noPoint{
	int no;
	noPoint *next;
	noPoint(int _no,noPoint *n=NULL):no(_no),next(n){
	}
};
struct pkey{
	int key;
	int sum;
	noPoint *nolist;
	noPoint *tail;
	pkey(int k = 0):key(k){
		nolist = NULL;
		tail = NULL;
		sum = 0;
	}
	void insert_no(int no)
	{
		if(nolist==NULL){
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
//�ص���-�ؼ�����
//#ifndef _DTREE_H
#define _BTREE_H
#define MAX_M 10			//B���������� 
const int m = 4;			//�趨B���Ľ���
const int Max = m - 1;		//�������ؼ������� 
const int Min = (m-1)/2;	//������С�ؼ������� 
typedef pkey keytype;		//KeyTypeΪ�ؼ������� 

//B����B���������
typedef struct node{
	int keynum;					 //���ؼ��ʵĸ���
	keytype key[MAX_M];		//�ؼ������飬����key[0]ʹ��
	struct node *parent;		//˫�׽��ָ��
	struct node *ptr[MAX_M];	//���ӽ��ָ��
}BTNode, *BTree;

//B�����ҽ������ 
struct Findres{
	node *pt;		//ָ���ҵ��Ľ�� 
	int i;			//�ڽ���еĹؼ���λ�� 
	int flag;		//���ҳɹ����ı�־ 
};
//������ 
typedef struct LNode{
	BTree data;				//������
	struct LNode *next;		//ָ���� 
}LNode, *LinkList;

typedef enum status{
	TRUE,
	FALSE,
	OK,
	ERROR,
	OVERFLOW,
	EMPTY
}Status;
#pragma region B���������� 
//	��ʼ��B�� 
Status InitBTree(BTree &t)
{
	t=NULL;
	return OK;
}

//	�ڽ��p�в��ҹؼ���k�Ĳ���λ��i
int SearchBTNode(BTNode *p, keytype k)
{
	int i = 0;
	for(i = 0; i < p->keynum&&p->key[i+1].key <= k.key;i++);
	return i;
}

//	����t�ֲ��ҹؼ���k�����ز��ҽ������
Findres SearchBTree(BTree t, keytype k)
{
	BTNode *p = t, *q = NULL;		//��ʼ�����p�ͽ��q��pָ������� 
	int found_flag = 0;				//�趨���ҳɹ����ı�־
	int i = 0;
	Findres r;
	while(p!=NULL&&!found_flag)
	{
		i = SearchBTNode(p,k);		//�ڽ���в��ҹؼ���k
		if(i>0&&p->key[i].key==k.key)		//���ҳɹ�
		{
			found_flag = 1;
		}
		else						//����ʧ��
		{
			q = p;
			p = p->ptr[i];
		}
	}
	if(found_flag == 1)				//���ҳɹ�
	{
		r.pt = p;
		r.i = i;
		r.flag = 1;
	}
	else							//����ʧ�� 
	{
		r.pt = q;
		r.i = i;
		r.flag = 0;
	}
	return r;						//���عؼ���k��λ�ã����߲���λ�ã� 
} 

//	���ؼ���k�ͽ��q�ֱ���뵽p->key[i+1]��p->ptr[i+1]�� 
void InsertBTNode(BTNode *&p, int i, keytype k, BTNode *q) 
{
	int j;
	for(j=p->keynum;j>i;j--)
	{
		p->key[j+1] = p->key[j];
		p->ptr[j+1] = p->ptr[j];
	}
	p->key[i+1] = k;
	p->ptr[i+1] = q;
	if(q != NULL)
	{
		q->parent = p;
	}
	p->keynum++;
}

//	�����p���ѳ�2����㣬ǰһ�뱣������һ��������q
void SplitBTNode(BTNode *&p, BTNode *&q)
{
	int i;
	int s = (m+1)/2;
	q = (BTNode *)malloc(sizeof(BTNode));	//�����q����ռ�
	
	q->ptr[0] = p->ptr[s];
	for(i=s+1;i<=m;i++)
	{
		q->key[i-s] = p->key[i];
		q->ptr[i-s] = p->ptr[i];
	}
	q->keynum = p->keynum - s;
	q->parent = p->parent;
	for(i=0;i<=p->keynum-s;i++)
	{
		if(q->ptr[i] != NULL)
		{
			q->ptr[i]->parent = q;
		}
	}
	p->keynum = s - 1;
}

//	�����µĸ����t��ԭp��qΪ����ָ�� 
void NewRoot(BTNode *&t,keytype k,BTNode *p,BTNode *q)
{
	t = (BTNode *)malloc(sizeof(BTNode));		//����ռ�
	t->keynum = 1;
	t->ptr[0] = p;
	t->ptr[1] = q;
	t->key[1] = k;
	if(p!=NULL)
	{
		p->parent = t;
	} 
	if(q!=NULL)
	{
		q->parent = t;
	}
	t->parent = NULL;
}

//����t�в���ؼ���k�����ز�����
void InsertBTree(BTree &t,int i,keytype k, BTNode *p)
{
	BTNode *q;
	int finish_flag, newroot_flag,s;
	keytype x;
	if(!t)								//t�ǿ���% 
	{
		NewRoot(t,k,NULL,NULL);
	}
	else
	{
		x = k;
		q = NULL;
		finish_flag = 0;
		newroot_flag = 0;
		while(finish_flag == 0 && newroot_flag == 0)
		{
			InsertBTNode(p,i,x,q);		//���ؼ���x��q�ֱ���뵽p->key[i+1]��p->ptr[i+1]
			if(p->keynum <= Max)
			{
				finish_flag = 1;		//�������
			} 
			else
			{
				s = (m+1)/2;
				SplitBTNode(p,q);		//���ѽ�� 
				x = p->key[s];
				if(p->parent)			//����x�Ĳ���λ�� 
				{
					p = p->parent;
					i = SearchBTNode(p,x);
				}
				else					//û�ҵ�x����Ҫ�½�� 
				{
					newroot_flag = 1;
				}
			}
		} 
		if(newroot_flag == 1)			//������ѷ���Ϊ���p��q 
		{
			NewRoot(t,x,p,q);			//�����¸����t,p��qΪ����ָ�� 
		}
	}
}

//	��p���ɾ��key[i]�����ĺ���ָ��ptr[i] 
void Remove(BTNode *p,int i)
{
	int j;
	for(j = i+1; j<=p->keynum; j++)
	{
		p->key[j-1] = p->key[j];
		p->ptr[j-1] = p->ptr[j];
	}
	p->keynum--;
}

//	Ѱ�����ֵ���������е���С�ؼ��ʣ�% 
void Substitution(BTNode *p,int i)
{
	BTNode *q;
	for(q = p->ptr[i]; q->ptr[0]!=NULL;q=q->ptr[0]);
	p->key[i] = q->key[1];		//���ƹؼ��� 
}


//��˫�׽��p�е����һ���ؼ��������ҽ��q�� 
//��������ap�е����һ���ؼ�������˫�׽��p��
void MoveRight(BTNode *p,int i)
{
	int j;
	BTNode *q = p->ptr[i];
	BTNode *aq = p->ptr[i-1];
	
	for(j=q->keynum;j>0;j--)	//�����ֵ�q�е����йؼ��ʺ���һλ 
	{
		q->key[j+1] = q->key[j];
		q->ptr[j+1] = q->ptr[j];
	}
	
	q->ptr[1] = q->ptr[0];		//��˫�׽��p�ƶ��ؼ��ʵ����ֵ�q�� 
	q->key[1] = p->key[i];
	q->keynum++;
	
	p->key[i] = aq->key[aq->keynum];	//�����ֵ�aq�����һ���ؼ����ƶ���˫�׽���� 
	p->ptr[i]->ptr[0] = aq->ptr[aq->keynum];
	aq->keynum--; 
} 

//��˫�׽��p�еĵ�һ���ؼ�����������aq��
//�����ҽ��q�еĵ�һ���ؼ�������˫�׽��p�� 
void MoveLeft(BTNode *p,int i)
{
	int j;
	BTNode *aq = p->ptr[i-1];
	BTNode *q = p->ptr[i];
	
	aq->keynum++;				//��˫�׽��p�еĹؼ����ƶ������ֵ�aq��
	aq->key[aq->keynum] = p->key[i];
	aq->ptr[aq->keynum] = p->ptr[i]->ptr[0];
	
	p->key[i] = q->key[1];		//�����ֵ�q�еĹؼ����ƶ���˫�׽��p�� 
	q->ptr[0] = q->ptr[1];
	q->keynum--;
	for(j=1;j<=q->keynum;j++)		//%�����ֵ�q�����йؼ�����ǰ�ƶ�һλ 
	{
		q->key[j] = q->key[j+1];
		q->ptr[j] = q->ptr[j+1];
	} 
	 
}

//˫�׽��p���ҽ��q�ϲ�������aq��
//������˫�׽��p��ʣ��ؼ��ֵ�λ�� 
void Combine(BTNode *p,int i)
{
	int j;
	BTNode *q = p->ptr[i];
	BTNode *aq = p->ptr[i-1];
	//��˫�׽��Ĺؼ���p->key[i]���뵽����aq 
	aq->keynum++;
	aq->key[aq->keynum] = p->key[i];
	aq->ptr[aq->keynum] = q->ptr[0];
	//���ҽ��q�����еĹؼ��ʶ����뵽����aq 
	for(j = 1;j<=q->keynum;j++)
	{
		aq->keynum++;
		aq->key[aq->keynum] = q->key[j];
		aq->ptr[aq->keynum] = q->ptr[j];
	}
	//��˫�׽��p�е�p->key[i]������йؼ�����ǰ�ƶ�һλ 
	for(j = i; j<p->keynum; j++)
	{
		p->key[j] = p->key[j+1];
		p->ptr[j] = p->ptr[j+1];
	}
	//�޸�˫�׽��Ĺؼ�����Ŀ
	p->keynum--;
	//�ͷſ��ҽ��q�Ŀռ�	 
	free(q);
} 

//ɾ�����p�еĵ�i���ؼ��ʺ󣬵���B�� 
void AdjustBTree(BTNode *p,int i)
{
	if(i==0)	//ɾ����������ߵĹؼ��� 
	{
		if(p->ptr[1]->keynum>Min)//�ҽ��ɽ� 
		{
			MoveLeft(p,1);
		}
		else					//�ҽ�㲻���� 
		{
			Combine(p,1);
		}
	} 
	//ɾ�����ұߵĹؼ���
	else if(i == p->keynum)		 
	{
		//����ɽ�
		if(p->ptr[i - 1]->keynum > Min)
		{
			MoveRight(p,i);
		}
		//���㲻�ɽ�
		else
		{
			Combine(p,i);
		} 
	} 
	//ɾ���ؼ������в������㹻�� 
	else if(p->ptr[i-1]->keynum > Min)
	{
		MoveRight(p,i);
	}
	//ɾ���ؼ������в����ҽ�㹻�� 
	else if(p->ptr[i+1]->keynum > Min)
	{
		MoveRight(p,i+1);
	}
	//ɾ���ؼ������в������ҽ�㶼������ 
	else
	{
		Combine(p,i);
	}
}
//��ѯ���p���Ƿ��ҵ��ؼ���k
int FindBTNode(BTNode *p,keytype k, int &i)
{
	if(k.key < p->key[1].key)
	{
		//���p�в��ҹؼ���ʧ�� 
		i = 0;
		return 0;
	} 
	else
	{
		//��p���в��� 
		i = p->keynum;
		while(k.key<p->key[i].key&&i>1)
		{
			i--;
		}
		if(k.key==p->key[i].key)
		{
			return 1;
		}
	}
	return 0;
}

//�ڽ��p�в��Ҳ�ɾ���ؼ���k
int BTNodeDelete(BTNode *p,keytype k)
{
	int i;
	//���ұ�־ 
	int found_flag;
	if(!p)
	{
		return 0;
	}
	else
	{
		//���ز��ҽ�� 
		found_flag = FindBTNode(p,k,i);
		if(found_flag==1)
		{
			//ɾ���Ĳ���Ҷ�ӽ�� 
			if(p->ptr[i-1]!=NULL)
			{
				//Ѱ�����ڹؼ��ʣ�Ҳ��������������С�Ĺؼ��ʣ� 
				Substitution(p,i);
				//ɾ������:һֱ��ǰ�ƶ���ֱ��Ҷ�ӽ��ֹͣ�� 
				BTNodeDelete(p->ptr[i],p->key[i]);
			}
			else
			{
				//���p��λ��i��ɾ���ؼ���
				Remove(p,i); 
			}
		} 
		else
		{
			//�غ��ӽ��ݹ���Ҳ�ɾ���ؼ���k 
			found_flag = BTNodeDelete(p->ptr[i],k);
		} 
		if(p->ptr[i]!=NULL)
		{
			if(p->ptr[i]->keynum < Min)
			{
				AdjustBTree(p,i);
			}
		}
		return found_flag;
	}
} 

//����ɾ����ܣ�ִ��ɾ������
void BTreeDelete(BTree &t,keytype k) 
{
	BTNode *p;
	//ɾ���ؼ���k 
	int flag = BTNodeDelete(t,k);
	//����ʧ�� 
	if(flag==0)
	{
		cout<<"�ؼ���"<<k.key<<"����B����"<<endl; 
	}
	else if(t->keynum==0)
	{
		p = t;
		t = t->ptr[0];
		free(p); 
	}
}

//�ݹ��ͷ�B��
void DestroyBTree(BTree &t)
{
	int i;
	BTNode *p=t;
	//B����Ϊ�� 
	if(p!=NULL)
	{
		for(i=0;i<=p->keynum;i++)
		{
			//�ͷ�ÿ����� 
			DestroyBTree(*&p->ptr[i]);//% 
		}
		free(p); 
	}
	t = NULL;
} 

//	��ʼ������
Status InitQueue(LinkList &L)
{
	//������ռ� 
	L = (LNode*)malloc(sizeof(LNode));
	if(L == NULL)
	{
		return OVERFLOW;
	}
	L->next = NULL;
	return OK;
}

//�½�һ�����
LNode* CreateNode(BTNode *p)
{
	LNode *q;
	q = (LNode*)malloc(sizeof(LNode));
	if(q!=NULL)
	{
		q->data = p;
		q->next = NULL;
	}
	return q;
} 

//Ԫ��q�����
Status Enqueue(LNode *p, BTNode *q)
{
	if(p==NULL)
	{
		return ERROR;
	}
	//����������� 
	while(p->next!=NULL)
	{
		p=p->next;
	}
	//���ɽ����q������� 
	p->next = CreateNode(q);
	return OK;
} 

//�����У�����q����ֵ 
Status Dequeue(LNode *p, BTNode *&q)
{
	LNode *aq;
	//ɾ��λ�ò����� 
	if(p==NULL||p->next==NULL)
	{
		return ERROR;
	}
	//�޸ı�ɾ�����ǰ��nextֵ 
	aq = p->next;
	p->next = aq->next;
	q = aq->data;
	//�ͷ��׸��ڵ� 
	free(aq);
	return OK;
} 

//������������ 
void DestroyQueue(LinkList L)
{
	LinkList p;
	if(L != NULL)
	{
		p = L;
		L = L->next;
		free(p);		//��һ�ͷ� 
		DestroyQueue(L);
	}
}
//�ö��б������B��
Status Traverse(BTree t, LinkList L, int newline, int sum)
{
	int i;
	BTree p;
	if(t!=NULL)
	{
		cout<<"[";
		Enqueue(L, t->ptr[0]);		//��� 
		for(i = 1; i<=t->keynum; i++)
		{
			cout<<" "<<t->key[i].key;
			Enqueue(L,t->ptr[i]);	//�ӽ����� 
		}
		sum += t->keynum + 1;
		cout<<"]"; 
		if(newline == 0)
		{
			cout<<endl;
			newline = sum - 1;
			sum = 0;
		}
		else
		{
			newline--;
		}
	}
	//L��Ϊ�� 
	if(L != NULL&&L->next != NULL)
	{
		Dequeue(L,p);				//���ӣ���p���� 
		Traverse(p,L,newline,sum);	//�������ӽ�� 
	}
	return OK; 
} 

//���B��
Status PrintBTree(BTree t)
{
	LinkList L;
	if(t==NULL)
	{
		cout<<"��B���ǿ���";
		return OK;
	}
	InitQueue(L);			//��ʼ������ 
	Traverse(t,L,0,0);		//���ö������ 
	DestroyQueue(L);		//���ٶ��� 
	return OK;
} 

void Test1()
{
	BTNode *t = NULL;
	Findres r;
	int j,n=15;
	int k;
	int a[] = {1,2,3,4,5,1,7,8,9,10,11,12,13,14,15};
	cout<<"����һ��"<<m<<"��B��"<<endl;
	for(j = 0; j < n; j++) 
	{
		r = SearchBTree(t,pkey(a[j]));
		if(r.flag==0)
		{
			//cout<<"��"<<j+1<<"����"<<"����Ԫ��"<<a[j]<<endl;
			InsertBTree(t,r.i,pkey(a[j]),r.pt);
		}
		
		PrintBTree(t);
	}
	
	cout<<endl;
	cout<<"ɾ������"<<endl;
	k=9;
	BTreeDelete(t,pkey(k));
	cout<<"ɾ��"<<k<<endl;
	cout<<"ɾ�����B��"<<endl;
	PrintBTree(t);
	cout<<endl;
	
	k=1;
	BTreeDelete(t,pkey(k));
	cout<<"ɾ��"<<k<<endl;
	cout<<"ɾ�����B��"<<endl; 
	PrintBTree(t);
	cout<<endl;
	
	cout<<"�ݹ��ͷ�B��"<<endl; 
	DestroyBTree(t);
	PrintBTree(t);
	
}


void Test2()
{
	int i,k;
	BTree t=NULL;
	Findres r;
	while(1)
	{
		cout<<"��ʱ��B����"<<endl;
		PrintBTree(t);
		cout<<endl;
		cout<<"*****************�����б�����****************"<<endl;
		cout<<"1����ʼ����2�����룻3��ɾ����4�����٣�5���˳�"<<endl;
		cout<<"���������������"<<endl;
		cin>>i;
		switch(i)
		{
			case 1:{
				InitBTree(t);
				cout<<"��ʼ��B���ɹ�"<<endl;
				PrintBTree(t);
				cout<<endl; 
				break;
			}
			case 2:{
				cout<<"����Ҫ����B��������"<<endl;
				cin >> k;
				r = SearchBTree(t,pkey(k));
				InsertBTree(t,r.i,k,r.pt); 
				PrintBTree(t);
				cout<<endl;
				cout<<"����ɹ�"<<endl;
				break;
			}
			case 3:{
				cout<<"����Ҫɾ��B���е�����"<<endl;
				cin >> k;
				BTreeDelete(t,pkey(k));
				cout<<endl;
				PrintBTree(t);
				cout<<endl;
				cout<<"ɾ���ɹ�"<<endl;
				break;
			}
			case 4:{
				DestroyBTree(t);
				cout<<"���ٳɹ�"<<endl; 
				break;
			}
			case 5:{
				exit(-1);
				break;
			}
		}	
	}
}

#pragma endregion 
