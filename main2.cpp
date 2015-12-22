#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <malloc.h>
#include <string.h>
#define MAXVALUE  10000
#define N 128 //本程序只对ascii码表中出现的字符编码。
#define MAXBIT 128
#define MAXLEAF 128
#define MAXNODE MAXLEAF*2 -1
using namespace std;

typedef struct node1
{
	int weight;          //本节点的权重，出现的频率。
	int parent;			//父亲节点的权重
	int lch;			//左儿子的权重
	int rch;			//右儿子的权重
	int value;			//这个节点代表的字符的ascii码
}*HuffmanTree, HTNode;

typedef struct node2
{
	char bit[128];
}*HuffmanCode, HCNode;

void strRev(char *s)//用于字符串反转
{
	char temp, *endchar;
	endchar = s + strlen(s) - 1;
	while (endchar > s)
	{
		temp = *s;
		*s = *endchar;
		*endchar = temp;
		--endchar;
		++s;
	}
}

int count_not_zero(int *data, int n)//用于求数组中不为零的个数
{
	int i, j=0;
	for (i = 0;i < n;i++)
	{
		if (data[i])
		{
			j++;
		}
	}
	return j;
}

void Select(HuffmanTree HT, int n, int *x1, int *x2)	//在HT[k](1 <= k <= m)中选择两个双亲为0且权值最小的结点，记下标为s1，s2，n=N
{
	int i, j;
	int m1, m2;
	for (i = 0; i < n; i++)
	{
		m1 = m2 = MAXVALUE;     /* m1、m2中存放两个无父结点且结点权值最小的两个结点 */
								/* 找出所有结点中权值最小、无父结点的两个结点，并合并之为一颗二叉树 */
		for (j = 0; j < n + i; j++)
		{
			if (HT[j].weight < m1 && HT[j].parent == 0)
			{
				m2 = m1;
				x2 = x1;
				m1 = HT[j].weight;
				*x1 = j;
			}
			else if (HT[j].weight < m2 && HT[j].parent == 0)
			{
				m2 = HT[j].weight;
				*x2 = j;
			}
		}
	}
}

/*统计一个文本文件的各个字符出现的频次 */
void count(int *data)//data是一个数组，每个元素的下标代表字符的ascii码
{
	int i;
	char w;
	ifstream fin;
	for (i = 0;i < N;i++)                             //初始化频率为0
		data[i] = 0;							//i代表字符的ascii码。
	fin.open("source.txt", ios::in);
	while (!fin.eof())                         //循环读入直至文件末尾                           
	{
		fin.get(w);
		data[(int)w]++;      //增加频率              
	}
	fin.close();
}
/*2.依此文本出现的字符和出现的频次，建立该文本字符集的哈夫曼编码表，以文件形式存储并显示。 */
//（1）我们先建立一个哈弗曼树，我们在这里运用排序的方法找到两个权值最小的点，然后生成新的点并且删除之前的两个点，然后再对新的点集进行排序。
void CreatHuffmanTree(HuffmanTree HT, int n, int *data)
{
	int i;
	int j = 0;
	int s1, s2;
	for (i = 0; i<2 * n - 1; i++)            //初始化所有结点的权值，双亲，孩子结点都为0     
	{
		HT[i].parent = 0;
		HT[i].lch = 0;
		HT[i].rch = 0;
		//HT[j].weight = MAXVALUE;
	}
	for (i = 0;i < n;i++)
	{
		if (data[i])
		{
			HT[j].weight = data[i];          //为前n个单元中的叶子节点赋权值
			HT[j].value = i;
			j++;							//这一步完事之后一共有j个节点里面有权值。
		}
	}

	for (i = j; i < 2*j-1; i++)             //通过j-1次的选择、删除、合并来创建霍夫曼树
	{
		Select(HT, i, &s1, &s2);			// 在HT[k](1<=k<=m)中选择两个双亲为0且权值最小的结点，记下标为s1，s2
		HT[s1].parent = i;					// 得到新结点i,它的左右结点为s1,s2,权值为它们之和
		HT[s2].parent = i;
		HT[i].lch = s1;
		HT[i].rch = s2;
		HT[i].weight = HT[s1].weight + HT[s2].weight;
	}
	//return j;//返回有效的真正代表字符的哈夫曼树节点个数
}

//（2）在建立完哈弗曼树之后我们就可以生成哈弗曼编码了，过程是已建立的哈夫曼树中，从叶结点开始，沿结点的双亲链域回退到根结点，每回退一步，就走过了哈夫曼树的一个分支，从而得到一位哈夫曼码值，由于一个字符的哈夫曼编码是从根结点到相应叶结点所经过的路径上各分支所组成的0，1序列，因此先得到的分支代码为所求编码的低位码，后得到的分支代码为所求编码的高位码：

void CreatHuffmanCode(HuffmanTree HT, HuffmanCode HC, int n)//这里的n代表哈夫曼码的位数，应该和HC.bit的位数一样多。128个
{
	//char cd[n];
	int start, c, i;
	int f;
	char *cd;
	cd = (char*)malloc(n*sizeof(char));
	cd[n - 1] = '\0';
	for (i = 0;i < n; i++)
	{
		start = n - 1;                 //start开始时指向最后，即编码结束的位置
		c = i;
		f = HT[i].parent;                //f指向c的双亲结点
		while (f != 0)                     //从叶子开始向上回溯，直到根结点
		{
			--start;                     //回溯一次start指向前一个位置
			if (HT[f].lch == c)
				cd[start] = '0';             //c是f的左孩子，则生成代码‘0’
			else
				cd[start] = '1';                 //c是f的右孩子，则生成代码‘1‘
			c = f;                                 //  继续向上回溯
			f = HT[f].parent;
		}
		//HC[i].bit = new char[n - start];               //为第i个字符编码分配空间
		strcpy_s(HC[i].bit, 128, &cd[start]);                 //将求得的编码从临时空间cd复制到HC的当前行中
	}
	delete cd;
	ofstream fout;
	fout.open("HTree.txt");
	for (i = 0;i < n;i++)
	{
		char ch = i;
		fout << ch << "\t" << HC[i].bit << endl;//以文本文件的形式显示
	}
	fout.close();
}

/*3.给出该字符集的任意文本文件的哈夫曼编码文件。
在上一步已经实现。*/
//4.对哈夫曼编码文件进行译码。 
void TranslateHuffmanCode(HuffmanTree HT, HCNode HC, int n)
{
	int f;
	char c;
	ifstream fin;
	fin.open("CodeFile.txt");
	ofstream fout;
	fout.open("RecieveFile.txt");
	f = 2 * n - 1;
	while (!fin.eof())  //读到文件未
	{
		fin >> c;

		if (HT[f].lch == 0 && HT[f].rch == 0)         //到达叶子结点时输出字符
		{
			char ch = f + 96;
			fout << ch;
			cout << c;                         //显示结果
			f = 2 * n - 1;                              //从根结点开始
		}
		if (c == '0')                              //当c为0是代表c为f的左孩子
			f = HT[f].lch;
		else
			f = HT[f].rch;
	}
}

int main()
{
	int *data, j, k;
	HuffmanTree HT;

	data = (int*)malloc(N*sizeof(int));
	count(data);
	k = count_not_zero(data, N);
	j = 2 * k - 1;
	HT = (HuffmanTree)malloc(j*sizeof(HTNode));
	CreatHuffmanTree(HT, j, data);//有k个字符,哈夫曼树有j个节点。
	HuffmanCode HC;
	HC = (HuffmanCode)malloc(k*sizeof(HTNode));
	CreatHuffmanCode(HT, HC, j);
}
