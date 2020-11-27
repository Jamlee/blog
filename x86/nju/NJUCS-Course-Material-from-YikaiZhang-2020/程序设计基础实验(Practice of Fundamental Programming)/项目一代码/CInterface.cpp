#include "Shopping.h"


Commodity *CInterface::UserFindCommdity(Commodity *pComm, int findID)
{
	Commodity *pfin = pComm;
	if (pfin == NULL)
		cout << "Ҫ���ҵ���Ʒ�б�Ϊ�� !" << endl;
	while (pfin != NULL)
	{
		if (pfin->id == findID)
		{
			/*if (pfin->number <= 0)
			return NULL;*/
			return pfin;
		}
		pfin = pfin->next;
	}
	return NULL;
}

// ���� 0 û������û� 1 admin 2 user
int CInterface::ThisIsWho(char userName[], int &cur)
{
	int happen = 0;   // 0 û�г��� 1 ���û��� 2 �ڹ���Ա�� 3 ������
	int curUserwho = 0, curAdminwho = 0;
	char fileName[NAME_WORDS] = "D:\\�û�.txt";

	curUserwho = f_Interface_.FindName(userName, fileName);
	if (curUserwho != -1)
		++happen;

	strcpy_s(fileName, "D:\\������Ա.txt");
	curAdminwho = f_Interface_.FindName(userName, fileName);
	if (curAdminwho != -1)
		happen += 2;

	if (happen == 3)
	{
		bool who = false;
		cout << "������ֿ����ǹ���ԱҲ�������û�, ��¼����Ա�밴 0 , ��¼�û��밴 1 :";
		(cin >> who).get();
		if (who == 0)
		{
			cur = curAdminwho - 1;
			return 1;
		}
		else if (who == 1)
		{
			cur = curUserwho - 1;
			return 2;
		}
	}
	else if (happen == 2)
	{
		cur = curAdminwho - 1;
		return 1;
	}
	else if (happen == 1)
	{
		cur = curUserwho - 1;
		return 2;
	}
	else if (happen == 0)
		return 0;
}

int CInterface::LoginAdminOrUser(int &cur, string &loginname)
{
	int isUser = 1, isLog = 2, tryCount = 2;
	bool isHave = false;
	char fileName[NAME_WORDS] = "";

	char userName[NAME_WORDS] = "";
	while (true)
	{
		isLog = 2;
		cout << "�������û���:";
		(cin >> userName).get();

		int wwho = this->ThisIsWho(userName, cur);
		if (wwho == 1)
			isLog = 1;

		if (wwho == 0)
		{
			bool isAgain = false;
			cout << "û������û�Ŷ\n�ٴγ����밴 1 , ע���밴 0 :";
			cin >> isAgain;

			if (isAgain == 1)
				continue;
			else if (isAgain == 0)  // ���ߵ�¼������
				return -1;
		}
		else if (wwho == 1)
			strcpy_s(fileName, "D:\\������Ա.txt");
		else if (wwho == 2)
			strcpy_s(fileName, "D:\\�û�.txt");

		break;
	}


	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, fileName, "r");
	if (err != NULL)
	{
		cerr << "Can't open:" << fileName << endl;
		exit(-1);
	}

	while (fgetc(pf) != '\n');  // ������һ��
	while (!feof(pf))
	{
		char realNamPas[NAME_WORDS];
		fgets(realNamPas, NAME_WORDS, pf);
		f_Interface_.SetSpacing(realNamPas, NAME_WORDS);
		if (strcmp(realNamPas, userName) == 0)
		{
			int i_password = 0, inputCount = 0;
			char passWord[NAME_WORDS] = "";

			// check password
			do
			{
				++inputCount;

				i_password = 0;
				for (int i = 0; i < NAME_WORDS; ++i)
					passWord[i] = '\0';
				cout << "����������:";
				gets_s(passWord);

				for (; i_password < NAME_WORDS; ++i_password)
				{
					if (realNamPas[i_password - 1] == '\0' && realNamPas[i_password] != '\0')
						break;
				}

				if (strcmp(&realNamPas[i_password], passWord) != 0)
				{
					if (inputCount == 3)
						isLog = 0;
					cout << "������� ! ����" << NUM_TRY_INPUT_PASSWORD - inputCount << "�λ��� !" << endl;
				}
				else
				{
					loginname = userName;
					cout << "��¼�ɹ� !" << endl;
					break;
				}
			} while (inputCount < NUM_TRY_INPUT_PASSWORD);

			if (isLog != 0)
			{
				fclose(pf);
				return isLog;
			}
		}

		if (isHave == true)
			break;
	}

	fclose(pf);

	if (isLog == 0)
		cout << "��Ǹ����ʧȥ���λ���~" << endl;

	return isLog;
}

