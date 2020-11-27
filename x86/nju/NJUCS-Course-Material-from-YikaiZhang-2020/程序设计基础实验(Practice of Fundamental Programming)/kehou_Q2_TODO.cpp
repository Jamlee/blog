#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//�����С 
#define MAX_SIZE 1000

int cmp(int a, int b)
{
	// TODO
	return a > b ? 1 : 0;
}

void sort(int *data, int n, int (*cmp)(int, int))
{
	// TODO: ����Ҫ������ʵ�ִ���
	//ѡ������
	for (int i = 0; i < n - 1; ++i)
	{
		int min = i; 
		for (int k = min + 1; k < n; ++k)
		{ 
			if (cmp(data[k], data[min]) == 0)
				min = k; 
		}
		int temp = data[min]; 
		data[min] = data[i]; 
		data[i] = temp; 
	}
}


int main()
{
	//NOTE:�������������в��� 

	int data[MAX_SIZE], n;
	scanf_s("%d", &n);
	for (int i = 0; i < n; i++)
	{
		int m;
		puts("*****************");
		scanf_s("%d", &m);
		for (int j = 0; j < m; j++)
			scanf_s("%d", data + j);
		sort(data, m, cmp);
		puts("after sorting:");
		for (int j = 0; j < m; j++)
		{
			printf("%d ", data[j]);
		}
		puts("\n*****************");
	}

	return 0;	
}

// NOTE: ����ע��Ƭ���ǲ��Դ��룬�����޸ģ����޸ĺ����̲��Գ��ִ��� ����Ը� 
/*

int main()
{
	int data[MAX_SIZE] , n;
	scanf("%d",&n);
	for(int i = 0 ; i < n ; i++)
	{
		int m;
		puts("*****************");
		scanf("%d",&m);
		for(int j = 0 ; j < m ; j++)
			scanf("%d",data+j); 
		sort(data, m, cmp);
		puts("after sorting:");
		for(int j = 0 ; j < m ; j++)
		{
			printf("%d ",data[j]);
		}
		puts("\n*****************");
	}
	
	return 0;	
}
*/

