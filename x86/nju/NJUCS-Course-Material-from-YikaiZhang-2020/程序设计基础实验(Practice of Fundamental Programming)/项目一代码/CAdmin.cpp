#include "Shopping.h"
void CAdmin::AddtoCartOrComm(Commodity *pAdd, int who)
{
	Commodity *padd = 0;
	char addID[NAME_WORDS] = "";
	int addid = 0;
	string addName, addBrand;
	double addPrice = 0;
	int addNum = 0;

	cout << "��������Ҫ�������Ʒ���(���� * �˳�):";
	cin >> addID;
	if (addID[0] == '*')
		return;


	for (int i = 1; addID[i] != '\0'; ++i)
		addid = addid * 10 + addID[i] - '0';

	padd = CInterface::UserFindCommdity(pAdd, addid);

	if (padd == NULL)
	{
		cout << "�����Ʒ����������Ʒ��, ��������Ʒ����, Ʒ��, �۸�, ����(�Կո����):";
		cin >> addName >> addBrand >> addPrice >> addNum;
		Commodity *p = new Commodity(addid, addName, addBrand, addPrice, addNum);
		if (pAdd == NULL)
			pAdd = p;
		else
		{
			Commodity *tail = pAdd;
			for (; tail->next != NULL; tail = tail->next);
			tail->next = p;
		}
		cout << "��ӳɹ� !" << endl;
	}
	else if (padd->number >= 0)
	{
		cout << "��Ʒ�Ѿ�����, ������Ҫ��ӵ�����:";
		cin >> addNum;
		padd->number += addNum;
		cout << "��ӳɹ� !" << endl;
	}
	else if (padd->number == -1)
	{
		bool isRestart = false;
		cout << "��Ʒ�������¼ܹ�, ����Ҫ�ָ�����, �ָ��밴 1 , �����밴 0 :";
		cin >> isRestart;
		if (isRestart == 0)
			return;
		else if (isRestart == 1)
		{
			cout << "������Ҫ���������:";
			cin >> addNum;
			padd->number = addNum;
			cout << "��ӳɹ� !" << endl;
		}
	}
}

void CAdmin::ChangeCommNum(Commodity *pCha)
{
	Commodity *pcha = NULL;
	char chaID[NAME_WORDS] = "";
	int chaid = 0;
	int chaPrice = 0;

	cout << "��������Ҫ�޸���������Ʒ���:";
	cin >> chaID;

	for (int i = 1; chaID[i] != '\0'; ++i)
		chaid = chaid * 10 + chaID[i] - '0';

	pcha = CInterface::UserFindCommdity(pCha, chaid);

	if (pcha == NULL)
	{
		cout << "�����Ʒ����������Ʒ�� !" << endl;
		return;
	}

	cout << "��������Ϊ:" << pcha->number << endl << "�������޸ĺ������:";
	cin >> chaPrice;

	while (chaPrice <= 0)
	{
		cout << "�Ƿ����� !" << endl;
		cin >> chaPrice;
	}

	pcha->number = chaPrice;
	cout << "�޸ĳɹ� !" << endl;
}

void CAdmin::ChangeCommPrice(Commodity *pCha)
{
	Commodity *pcha = NULL;
	char chaID[NAME_WORDS] = "";
	int chaid = 0;
	double chaPrice = 0;

	cout << "��������Ҫ�޸ļ۸����Ʒ���:";
	cin >> chaID;

	for (int i = 1; chaID[i] != '\0'; ++i)
		chaid = chaid * 10 + chaID[i] - '0';

	pcha = CInterface::UserFindCommdity(pCha, chaid);

	if (pcha == NULL)
	{
		cout << "�����Ʒ����������Ʒ�� !" << endl;
		return;
	}

	cout << "���м۸�Ϊ:" << pcha->price << endl << "�������޸ĺ�ļ۸�:";
	cin >> chaPrice;

	while (chaPrice <= 0)
	{
		cout << "�Ƿ����� !" << endl;
		cin >> chaPrice;
	}

	pcha->price = chaPrice;
	cout << "�޸ĳɹ� !" << endl;
}

void CAdmin::DeleteCartOrComm(Commodity *pDel)
{
	int delSitua = 0, delid = 0;
	bool isMany = true;
	do
	{
		delid = 0;
		cout << "��������Ҫɾ������ƷID:";
		char delID[NAME_WORDS] = "";
		cin >> delID;

		for (int i = 1; delID[i] != '\0'; ++i)
			delid = delid * 10 + delID[i] - '0';

		Commodity *ppDel = pDel;
		for (; ppDel != NULL && ppDel->id != delid; ppDel = ppDel->next);

		if (ppDel == NULL || ppDel->number < 0)
		{
			bool isConti = false;
			cout << "��Ҫɾ������Ʒ��ͱ��¼���~\n����ɾ���밴 1 , �˳��밴 0 :";
			cin >> isConti;
			if (isConti == 0)
				return;
			else if (isConti == 1)
				continue;

			return;
		}

		if (ppDel->number != 0)
		{
			bool isAllDel;
			cout << "��Ҫ�¼ܵ���Ʒ��:" << ppDel->brand << "��" << ppDel->name << "  �۸�:" << ppDel->price << "\nȷ���¼��밴 1 , ���������밴 0 :";
			cin >> isAllDel;
			if (isAllDel == 0)
				continue;
		}

		ppDel->number = -1;
		cout << "����Ʒ���ɹ��¼� !" << endl;
		return;
	} while (true);

	return;
}
