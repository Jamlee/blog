// ��� fwrite + ������ ����Ǽ�Ԧ���˰� �������ֻ������Ϊ fprintf �Ͷ�����.....

// ע�� ������Խ��Ĵ��� ������ �ڴ治����MAX ̫�� �� ̫С ���ݷŲ���
// wb ��ʽ \n д����ȥ???
// �Ƿ�һ��Ҫ�����ַ��������ʽ��д ��ת����int double֮�����������?
#include <iostream>
#include <stdlib.h>
using namespace std;
const int MAX = 100;
const double eps = 1e-6;

void read_dat(FILE *pf);

int main()
{
	FILE *pf, *out0, *out1;
	errno_t err;
	err = fopen_s(&pf, "D:\\wine.txt", "r");
	if (err != NULL)
	{
		cerr << "can't open" << endl;
		exit(-1);
	}
	err = fopen_s(&out0, "D:\\wine_cpl.dat", "wb");
	if (err != NULL)
	{
		cerr << "can't open" << endl;
		exit(-1);
	}
	err = fopen_s(&out1, "D:\\wine_norm.csv", "w");
	if (err != NULL)
	{
		cerr << "can't open" << endl;
		exit(-1);
	}

	char oriWine[MAX * 2][MAX];   // ԭ����ȡ���ַ�����(�տ�ʼû�뵽ֱ�Ӷ�ȡdouble����)
	//int unclear[MAX] = { 0 };
	int i_re = 0;

	double wine[MAX * 2][30] = { 0 };  // ת����double����

	while (!feof(pf))
	{
		fgets(oriWine[i_re++], sizeof(oriWine[0]), pf);
	}

	// ��ȡ����
	for (int k = 0; k < i_re; ++k)
	{
		int i_wine = 0;
		bool isInte = true;
		double temp_sum = 0, temp_poi = 0.1;
		for (int i = 0;; ++i)      // ����������?
		{
			if (oriWine[k][i] == '\n' || oriWine[k][i] == '\0')
			{
				wine[k][i_wine++] = temp_sum;
				wine[k][i_wine] = -1;
				break;
			}

			//if (oriWine[k][i] == '#')   // ��¼û�����Ե�λ��
			//{
			//	cout << k << " " << i_wine << endl;
			//	cout << "######################" << endl;
			//	unclear[k] = i_wine + 1;
			//}

			else if (oriWine[k][i] == ',')  // ����
			{
				wine[k][i_wine++] = temp_sum;
				isInte = true;
				temp_sum = 0;
				temp_poi = 0.1;
			}

			else if (oriWine[k][i] == '.')  // С����
				isInte = false;

			else if (oriWine[k][i] <= '9' && oriWine[k][i] >= '0')
			{
				if (isInte == true)
					temp_sum = temp_sum * 10 + oriWine[k][i] - '0';
				else
				{
					temp_sum += temp_poi * (oriWine[k][i] - '0');
					temp_poi *= 0.1;
				}
			}
		}
	}

	// ����û�����Ե� (����ʱ�����)
	for (int k = 0; k < i_re; ++k)
	{
		for (int i = 0; wine[k][i] != -1; ++i)
		{
			if (wine[k][i] == 0)
			{
				double temp_sum = 0;
				for (int n = 0; n < i_re; ++n)
					temp_sum += wine[n][i];

				wine[k][i] = temp_sum / (i_re - 1);
			}
			
			char a[MAX];
			char dou = ',', huan = '\n';
			if (wine[k][i + 1] != -1)
			{
				fwrite(&wine[k][i], sizeof(wine[k][i]), 1, out0);
				fwrite(&dou, sizeof(dou), 1, out0);
			}
			else
			{
				fwrite(&wine[k][i], sizeof(wine[k][i]), 1, out0);
				fwrite(&huan, sizeof(huan), 1, out0);
			}
		}
	}

	// max min ��һ��
	for (int i = 1, k = 0; wine[k][i] != -1; ++i, k = 0)    // ������񲻺�
	{
		double max = wine[k][i], min = wine[k][i];
		//double* const t = &wine[k][i];

		for (; k < i_re; ++k)
		{
			if (wine[k][i] > max)
				max = wine[k][i];
			if (wine[k][i] < min)
				min = wine[k][i];
		}
		for (k = 0; k < i_re; ++k)
		{
			if (max - min < eps)
				wine[k][i] = 1.0;
			else
				wine[k][i] = (wine[k][i] - min) / (max - min);
		}
	}

	// �������һ���ļ�
	for (int k = 0; k < i_re; ++k)
	{
		for (int i = 0; wine[k][i] != -1; ++i)
		{
			if (wine[k][i + 1] != -1)
				fprintf(out1, "%.1f,", wine[k][i]);
			else
				fprintf(out1, "%.1f\n", wine[k][i]);
		}
	}

	
			
			
	/*for (int i = 0; i < i_re; ++i)
	{
		double line_sum = 0;
		int t = unclear[i];
		if (t == 0)
			continue;
		for (int k = 0; k < i_re; ++k)
		{
			cout << t - 1 << endl;
			line_sum += wine[k][t - 1];
		}
		wine[i][t - 1] = line_sum / i_re;
	}*/
	


	fclose(pf);
	fclose(out0);
	fclose(out1);
	return 0;
}

void read_dat(FILE *pf)
{
	while (!feof(pf))
	{
		char ch = fgetc(pf);
		cout << ch;
	}
}