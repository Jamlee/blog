#include <iostream>
#include <stdlib.h>
using namespace std;
const int MAX = 101;

int main()
{
	char rea[MAX][MAX];
	char ins[MAX];
	int toPos;
	int n_ins = 0;
	cin >> toPos;
	cin >> ins;
	for (; ins[n_ins] != '\0'; ++n_ins);      // ͳ���������ݵĸ���
	

	//���ļ�
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, "D:\\sample.txt", "r+");
	if (err != NULL)
	{
		cerr << "can't open" << endl;
		exit(-1);
	}

	//��ȡ�ļ�ֱ��Ҫ����ĵط�
	int i_line = 0, i_row = 0;
	int sum_pos = 0;
	while (!feof(pf))
	{
		i_row = 0;
		fgets(rea[i_line], toPos - sum_pos, pf);
		for (; rea[i_line][i_row] != '\0' && rea[i_line][i_row] != '\n'; ++i_row);
		sum_pos += i_row;
		if (sum_pos + 1 == toPos)
			break;
		++i_line;
	}

	// ����Ҫ���������
	for (int ti = 0; ins[ti] != '\0'; ++i_row, ++ti)
		rea[i_line][i_row] = ins[ti];

	// �ƶ�ָ�����������ǵ�
	fseek(pf, n_ins, SEEK_CUR);

	//������ȡ
	fgets(&rea[i_line][i_row], sizeof(rea[i_line]), pf);
	while (!feof(pf))
	{
		++i_line;
		fgets(rea[i_line], sizeof(rea[i_line]), pf);
	}

	// ���
	fseek(pf, 0, SEEK_SET);
	for (int k = 0; k < i_line; ++k)
		fputs(rea[k], pf);

	fclose(pf);

	return 0;

	//char a[MAX];
	//int b;
	//cin >> b >> a;
	//fseek(pf, b - 1, SEEK_SET);
	//fputs(a, pf);

	//fgets(&in[3], 1, pf);
	// fscanf_s(pf, "%s", in);  Ϊʲô���һֱ�ᱬ��?
}