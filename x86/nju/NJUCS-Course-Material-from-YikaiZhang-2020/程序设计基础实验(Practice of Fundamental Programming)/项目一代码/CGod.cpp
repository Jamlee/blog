#include "Shopping.h"

CGod::CGod()
{
	for (int i = 0; i < MAX_USERS_NUM; ++i)
		pRun[i] = NULL;
	for (int i = 0; i < FILE_WORDS; ++i)
	{
		for (int j = 0; j < FILE_WORDS; ++j)
		{
			comm_arr_[i][j] = '\0';
			sell_arr[i][j] = '\0';
		}
	}
	for (int i = 0; i < MAX_COMM_LINE; ++i)
		arr_coupon_[i] = true;
	num_user_ = 0;
	line_comm_arr_ = 0;
	pComm_ = NULL;
	pComm_tail_ = NULL;
	pSell_ = NULL;
	pSell_tail_ = NULL;
	cur_who_ = -1;
	line_sell_arr = 0;

}

void CGod::GodSetCommCity()
{
	string newCity;
	char modiCity[NAME_WORDS] = "";
	int modiID = 0;
	Commodity *pModi = NULL;
	cout << "��������Ҫ�޸ķ���������Ʒ��ID(���� * �˳�):";
	cin >> modiCity;
	if (modiCity[0] == '*')
		return;
	for (int i = 1; modiCity[i] != '\0'; ++i)
		modiID = modiID * 10 + modiCity[i] - '0';
	pModi = pRun[get_cur_who_()]->UserFindCommdity(this->get_pComm_(), modiID);
	if (pModi->commCity.length() != 0)
		cout << "ԭ���ķ���������:" << pModi->commCity << endl;
	cout << "������ĺ�ķ�����ַ:";
	cin >> newCity;
	pModi->commCity = newCity;
	cout << "�޸ĳɹ� !" << endl;
}

void CGod::InitCommCity(string cityDeliver)
{
	for (Commodity *initCity = pComm_; initCity != NULL; initCity = initCity->next)
		initCity->commCity = cityDeliver;
}

int CGod::GodEnroll(string &loginname)
{
	char fileName[NAME_WORDS] = "";
	char name[NAME_WORDS] = "";
	bool isUser = true;
	cout << "��ע��ʲô�˻�: ��ͨ�˻��밴 1 , ����Ա�˻��밴 0 :";
	(cin >> isUser).get();
	if (isUser == 0)
	{
		char adminEnroll[6] = "66666", inputAdEn[NAME_WORDS] = "";
		cout << "������ע����:";
		(cin >> inputAdEn).get();
		int i = 0;
		for (; adminEnroll[i] != '\0'; ++i)
		{
			if (adminEnroll[i] != inputAdEn[i])
			{
				cout << "ע������� !" << endl;
				return -1;
			}
		}
		if (inputAdEn[i] != '\0')
		{
			cout << "ע������� !" << endl;
			return -1;
		}

		strcpy_s(fileName, "D:\\������Ա.txt");
	}
	else if (isUser == 1)
		strcpy_s(fileName, "D:\\�û�.txt");
	else
	{
		cout << "�벻Ҫ������ !" << endl;
		return -1;
	}

	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, fileName, "r+");
	if (err != NULL)
	{
		cerr << "Can't open:" << fileName << endl;
		exit(-1);
	}

	while (true)
	{
		cout << "�������û���:";
		(cin >> name).get();

		while (fgetc(pf) != '\n');  // ������һ��
		while (!feof(pf))
		{
			char realNamPas[NAME_WORDS] = "";
			fgets(realNamPas, NAME_WORDS, pf);
			f_.SetSpacing(realNamPas, NAME_WORDS);
			if (strcmp(realNamPas, name) == 0)
				break;
		}
		if (!feof(pf))
		{
			cout << "�û����Ѿ����� !" << endl;
			fseek(pf, 0, SEEK_SET);
			continue;
		}
		else
		{
			while (true)
			{
				bool isCon = true;
				char password[NAME_WORDS] = "", confirmPass[NAME_WORDS] = "";
				cout << "����������:";
				gets_s(password);
				cout << "��ȷ������:";
				gets_s(confirmPass);
				for (int i = 0; password[i] != '\0' && confirmPass[i] != '\0'; ++i)
				{
					if (password[i] != confirmPass[i])
					{
						cout << "ȷ���������� !" << endl;
						isCon = false;
						break;
					}
				}
				if (isCon == false)
					continue;
				fputc('\n', pf);
				fprintf(pf, "%s\t%s", name, password);
				break;
			}
		}

		break;
	}
	loginname = name;
	++get_num_user();
	fclose(pf);


	if (isUser == 0)
		pRun[get_num_user()] = new CAdmin;
	else if (isUser == 1)
	{
		pRun[get_num_user()] = new CUser;

		// �������ﳵ�ļ�
		char cartFileName[NAME_WORDS] = "";
		int curNewPeo = get_num_user();
		FILE *pCart = NULL;
		errno_t e;
		intTostrFileName(cartFileName, curNewPeo);
		e = fopen_s(&pCart, cartFileName, "wt+");
		if (err != NULL)
		{
			cerr << "Can't open:" << cartFileName << endl;
			exit(-1);
		}
		fclose(pCart);
	}

	get_cur_who_() = get_num_user();

	cout << "ע��ɹ� !" << endl;

	return isUser + 1;
}


void CGod::SetCouponComm()
{
	bool isOK = false;
	int ID = 0;
	while (isOK != true)
	{
		ID = 0;
		char couID[NAME_WORDS] = "";
		cout << "��������Ҫ���ô�������Ʒ���(���� * �˳�):";
		cin >> couID;
		if (couID[0] == '*')
			return;

		for (int i = 1; couID[i] != '\0'; ++i)
			ID = ID * 10 + couID[i] - '0';

		Commodity *comm = pRun[0]->UserFindCommdity(get_pComm_(), ID);
		cout.fill('0');
		cout << right << 'F' << setw(5) << comm->id << "    ";
		cout.fill(' ');
		cout << left << setw(10) << comm->name << setw(10) << comm->brand << setw(10) << comm->price << setw(10) << comm->number << endl;
		cout << "������� ? ȷ���밴 1, ���������밴 0 :";
		cin >> isOK;
	}

	arr_coupon_[ID] = true;
	cout << "���óɹ� ! " << endl;
}

char CGod::PrintMenu()
{
	char input;
	cout << "==========================================================" << endl;
	cout << "0. �û������Ա��¼   1. �û�ע��   2. �����   *.�˳�" << endl;
	cout << "==========================================================" << endl;
	cout << "���������:";

	cin >> input;

	return input;
}

char CGod::PrintAdminMenu()
{
	char input;
	cout << endl;
	cout << "=================================================================================================================" << endl;
	cout << "��ǰ��¼:" << get_cur_who_() << "�Ź���Ա 0.ע�� 1.����Ʒ 2.����Ʒ 3.ɾ��Ʒ 4.����Ʒ�۸� 5.����Ʒ�� 6.����Ʒ 7.��ӡ�ۻ��� 8.�ķ�����ַ" << endl;
	cout << "=================================================================================================================" << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�� @ �鿴�û�����<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

	cout << "���������:";

	cin >> input;
	cout << endl;

	return input;
}

char CGod::PrintUserMenu()
{
	char input;
	cout << endl;
	cout << "======================================================================================================================" << endl;
	cout << " ��ǰ��¼:" << get_cur_who_() << "���û� 0.ע����¼ 1.�鿴��Ʒ 2.���� 3.����Ʒ�����ﳵ 4.ɾ���ﳵ 5.���ﳵ 6.���� 7.��ֵ 8.��� 9.�齱" << endl;
	cout << "======================================================================================================================" << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�� & ���� % ����<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>�� @ ���̼�����<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

	cout << "���������:";

	cin >> input;
	cout << endl;

	return input;
}

char CGod::PrintTouristMenu()
{
	char input;
	cout << endl;
	cout << "=======================================" << endl;
	cout << " 0.�˳�   1.�鿴��Ʒ    2.��Ʒ���� " << endl;
	cout << "=======================================" << endl;
	cout << "���������:";

	cin >> input;
	cout << endl;

	return input;
}

void CGod::GodAllInit()
{
	char initFileName[NAME_WORDS] = "D:\\user1.txt";
	int allNum = 1;
	while ((_access(initFileName, 0)) != -1)
	{
		++allNum;
		intTostrFileName(initFileName, allNum);
	}
	get_num_user() = allNum - 1;

	/*char fileName[NAME_WORDS] = "D:\\�����嵥.txt";
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, fileName, "w");
	if (err != NULL)
	{
	cerr << "Can't open:" << fileName << endl;
	exit(-1);
	}*/

	line_sell_arr = f_.ReadCommodity(sell_arr, "D:\\�����嵥.txt");

	for (int i = 0; i < line_sell_arr; ++i)
		f_.SetSpacing(sell_arr[i], FILE_WORDS);

	tran_arr_pComm_(get_pSell_(), false);
	SumListComm(get_pSell_());


	InitCommCity("����");
}

void CGod::GodReadCommodity(int who)
{
	line_comm_arr_ = f_.ReadCommodity(comm_arr_, "D:\\���.txt");

	for (int i = 0; i < line_comm_arr_; ++i)
		f_.SetSpacing(comm_arr_[i], FILE_WORDS);
}

void CGod::UpdataCommFile(char *fileName)
{
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, fileName, "w");
	if (err != NULL)
	{
		cerr << "Can't open:" << fileName << endl;
		exit(-1);
	}

	Commodity *pFileAdd = get_pComm_();
	while (pFileAdd != NULL)
	{
		fprintf_s(pf, "F%04d\t%s\t%s\t%.1f\t%d", pFileAdd->id, pFileAdd->name.c_str(), pFileAdd->brand.c_str(), pFileAdd->price, pFileAdd->number);
		if (pFileAdd->next != NULL)
			fprintf_s(pf, "\n");
		pFileAdd = pFileAdd->next;
	}

	fclose(pf);
}

void CGod::tran_arr_pComm_(Commodity *&pTo, bool flag)
{
	Commodity *tail = NULL;
	bool isNULL = true;

	if (pTo != NULL)
	{
		isNULL = false;
		/*	bool isUpdate = false;
		cout << "�Ƿ����п�����(����0:������, ����1:����):";
		cin >> isUpdate;
		if (isUpdate == 0)
		return;
		else*/
		{
			tail = pTo;
			for (; tail->next != NULL; tail = tail->next);
		}
	}

	int line_arr = 0;
	if (flag == true)
		line_arr = line_comm_arr_;
	else
		line_arr = line_sell_arr;
	for (int i = 0; i < line_arr; ++i)
	{
		int t_id = 0;
		string t_name;
		string t_brand;
		double t_price = 0;
		int t_number = 0;
		char *read = NULL, *end = NULL;
		if (flag == true)
			read = &comm_arr_[i][0], end = &comm_arr_[i][FILE_WORDS];
		else
			read = &sell_arr[i][0], end = &sell_arr[i][FILE_WORDS];

		while (*read != '\0')
		{
			if (*read <= '9' && *read >= '0')
				t_id = t_id * 10 + *read - '0';
			++read;
		}

		int isName = 0;
		while (read != end && isName != 2)
		{
			if (*read != '\0' && *(read - 1) == '\0')
			{
				if (isName == 0)
				{
					t_name = read;
					++isName;
				}
				else if (isName == 1)
				{
					t_brand = read;
					++isName;
				}
			}
			++read;
		}

		int flagSitua = 0;
		int point = 1;
		while (read != end)
		{
			// ���۸�
			if (flagSitua == 0 && *read <= '9' && *read >= '0')
				t_price = t_price * 10 + *read - '0';
			else if (*read == '.')
				++flagSitua;
			else if (flagSitua == 1 && *read <= '9' && *read >= '0')
			{
				t_price += (*read - '0') / pow(10, point);
				++point;
			}

			else if (flagSitua == 1 && *read == '\0')
				++flagSitua;

			// ������
			else if (flagSitua == 2 && *read <= '9' && *read >= '0')
				t_number = t_number * 10 + *read - '0';
			else if (flagSitua == 2 && *read == '\0')
				break;

			++read;
		}

		if (isNULL == true || this->get_puser(0)->UserFindCommdity(pTo, t_id) == NULL)
		{
			Commodity *p = new Commodity(t_id, t_name, t_brand, t_price, t_number);
			if (pTo == NULL)
			{
				pTo = p;
				tail = pTo;
			}
			else
			{
				tail->next = p;
				p->pre = tail;
				tail = p;
			}
			if (flag == true)
				pComm_tail_ = tail;
			else
				pSell_tail_ = tail;
		}

		else
		{
			Commodity *p = this->get_puser(0)->UserFindCommdity(pTo, t_id);
			if (p->name != t_name)
				p->name = t_name;
			if (p->brand != t_brand)
				p->brand = t_brand;
			if (p->price != t_price)
				p->price = t_price;
			if (p->number != t_number)
				p->number = t_number;
		}
	}


}

void CGod::UpdataSellFile(char *fileName)
{
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, fileName, "w");
	if (err != NULL)
	{
		cerr << "Can't open:" << fileName << endl;
		exit(-1);
	}

	Commodity *pFileSell = get_pSell_();
	while (pFileSell != NULL)
	{
		fprintf_s(pf, "F%04d\t%s\t%s\t%.1f\t%d", pFileSell->id, pFileSell->name.c_str(), pFileSell->brand.c_str(), pFileSell->price, pFileSell->number);
		if (pFileSell->next != NULL)
			fprintf_s(pf, "\n");
		pFileSell = pFileSell->next;
	}
	fclose(pf);
}

void CGod::GodPrintCommodity(int who, Commodity *ppri)
{
	Commodity *pri = ppri;
	if (pri == NULL)
	{
		cout << "����ļ��ǿյ�Ŷ~" << endl;
		return;
	}

	cout.setf(std::ios::left);
	cout << setw(10) << "ID" << setw(10) << "����" << setw(10) << "Ʒ��" << setw(10) << "�۸�" << setw(10) << "����" << setw(10) << "�� !" << endl;
	while (pri != NULL)
	{
		if (who == 1 && pri->number <= 0)
		{
			pri = pri->next;
			continue;
		}
		cout.fill('0');
		cout << right << 'F' << setw(5) << pri->id << "    ";
		cout.fill(' ');
		cout << left << setw(10) << pri->name << setw(10) << pri->brand << setw(10) << pri->price << setw(10) << pri->number << setw(10) << pri->likes << endl;
		pri = pri->next;
	}
}

bool CGod::Cmp_ID(Commodity *a, Commodity *b)
{
	return (a->id >= b->id);
}

// b < a �ͷ��� true
bool CGod::p_Cmp(Commodity *a, Commodity *b)
{
	if (a == NULL || b == NULL)
		return false;
	while (a != NULL)
	{
		if (a == b)
			return false;
		a = a->next;
	}

	return true;
}

void CGod::QuickSort(Commodity *beg, Commodity *end, bool(*Cmp)(Commodity *, Commodity*), bool flag)
{
	Commodity *left = beg, *right = end;
	Commodity stan_temp;

	// �ж��ǲ���������Χ
	int isIn = 0;
	Commodity *i = NULL;
	if (flag == false)
		i = get_pSell_();
	else
		i = get_pComm_();

	for (; i != NULL; i = i->next)
	{
		if (i == left)
			++isIn;
		else if (i == right)
			++isIn;
	}
	if (isIn != 2)
		return;

	// ��������
	if (!p_Cmp(left, right))
	{
		stan_temp = *left;
		while (left != right)
		{
			while (right != NULL && left != right && Cmp(right, &stan_temp))   // ע��ע������ left < right û�е��ں�
				right = right->pre;
			*left = *right;

			while (left != NULL && left != right && Cmp(&stan_temp, left))
				left = left->next;
			*right = *left;
		}
		*right = stan_temp;

		QuickSort(beg, left->pre, Cmp, flag);

		QuickSort(right->next, end, Cmp, flag);
	}

	return;
}

bool CGod::isPartSame(string a, string b)
{
	static string same;
	if (a == b)
		return true;
	for (unsigned int i = 0; i < a.length(); i += 2)
	{
		for (unsigned int k = 0; k < b.length(); k += 2)
		{
			if (a.substr(i, 2) == b.substr(k, 2))
			{
				if (same.length() == 0 || b != same)
				{
					same = b;
					cout << "��Ҫ�ҵ���" << b << "��?" << endl;
				}
				return true;
			}
		}
	}

	return false;
}

void CGod::FindCommodity(int who)
{
	string find;
	cout << "��������Ҫ���ҵ���Ʒ:";
	cin >> find;

	Commodity arrFind[FILE_WORDS];
	Commodity *pFind = get_pComm_(), *pHead = NULL, *pTail = NULL;
	while (pFind != NULL)
	{
		bool isSame = false;
		isSame = isPartSame(find, pFind->name);
		if (isSame == false)
			isSame = isPartSame(find, pFind->brand);

		if (isSame == true)
		{
			Commodity *p = new Commodity();
			*p = *pFind;
			if (pHead == NULL)
			{
				pHead = p;
				pTail = pHead;
			}
			else
			{
				pTail->next = p;
				p->pre = pTail;
				pTail = p;
			}
		}

		pFind = pFind->next;
	}

	if (pHead == NULL)
	{
		cout << "��!�����˻�ԭ!û���ҵ�!" << endl;
		return;
	}

	QuickSort(pHead, pTail, Cmp_ID, true);
	GodPrintCommodity(who, pHead);
}

// ���� LoginAdminOrUser() �Ƿ��� int ���� ��������ʲô ע��ú����������
// ��������������� return -1  ע������� return 0 �ɹ���½����Ա���û� return 1
int CGod::GodLogin()
{
	int situa = pRun[0]->LoginAdminOrUser(this->get_cur_who_(), login_user_name_);   // ר��������¼��
	//cout << "situa:" << situa << endl;

	if (situa == 0)   // ��������������� (�ܱ�����???)
		return -1;

	else if (situa == -1)   // û������û��� ��Ҫע��
	{
		int isEnroll = GodEnroll(login_user_name_);
		if (isEnroll == -1)          // ע�����������
			return 0;
		return isEnroll;
	}

	else if (situa == 1)
		pRun[get_cur_who_()] = new CAdmin;

	else if (situa == 2)
		pRun[get_cur_who_()] = new CUser;

	return situa;
}

void CGod::SumListComm(Commodity *&phead)
{
	if (phead == NULL)
	{
		cout << "error:void CGod::SumList(Commodity *&phead)ָ��Ϊ�� !" << endl;
		return;
	}
	Hash hash[FILE_WORDS];
	Commodity *del = phead, *newTail = NULL;
	int maxID = phead->number;
	while (phead != NULL)
	{
		if (maxID < phead->id)
			maxID = phead->id;

		Commodity *&pDo = hash[phead->id].get_commo_(phead->price);
		if (pDo == NULL)
			pDo = phead;
		else
			*pDo = *pDo + *phead;
		phead = phead->next;
	}

	for (int i = 0; i <= maxID; ++i)
	{
		for (int k = 0; hash[i].getRealComm(k) != NULL; ++k)
		{
			Commodity *p = new Commodity;
			*p = *hash[i].getRealComm(k);
			if (phead == NULL)
			{
				phead = p;
				newTail = phead;
			}
			else
			{
				newTail->next = p;
				p->pre = newTail;
				newTail = p;
			}
		}
	}

	get_pSell_tail_() = newTail;
}

// ������û�����ֲ�ͬ�۸�Ĺ�ϣ
void CGod::SumList(Commodity *&phead)
{
	if (phead == NULL)
	{
		cout << "error:void CGod::SumList(Commodity *&phead)ָ��Ϊ�� !" << endl;
		return;
	}
	Commodity hash[FILE_WORDS], *newTail = NULL;
	Commodity *del = phead;
	int maxID = phead->number;
	while (phead != NULL)
	{
		if (maxID < phead->id)
			maxID = phead->id;
		hash[phead->id] = hash[phead->id] + *phead;
		phead = phead->next;
	}
	DeleteList(del);

	for (int i = 0; i <= maxID; ++i)
	{
		if (hash[i].id == -1)
			continue;
		Commodity *p = new Commodity;
		*p = hash[i];
		if (phead == NULL)
		{
			phead = p;
			newTail = phead;
		}
		else
		{
			newTail->next = p;
			p->pre = newTail;
			newTail = p;
		}
	}

	get_pSell_tail_() = newTail;
}

void intTostrFileName(char str[], int a)
{
	int t_a = a, c_a = 0;
	char t_str[NAME_WORDS];
	strcpy_s(t_str, "D:\\user");

	while (t_a != 0)
	{
		++c_a;
		t_a /= 10;
	}

	for (int i = 0; i < c_a; ++i)
		t_str[7 + c_a - i - 1] = (int)(a / pow(10, i)) % 10 + '0';

	t_str[7 + c_a] = '.';
	t_str[7 + c_a + 1] = 't';
	t_str[7 + c_a + 2] = 'x';
	t_str[7 + c_a + 3] = 't';
	t_str[7 + c_a + 4] = '\0';

	for (int i = 0; t_str[i] != '\0'; ++i)
		str[i] = t_str[i];
}

void DeleteList(Commodity *del)
{
	while (del != NULL)
	{
		Commodity *d = del;
		del = del->next;
		delete d;
	}
}

void CGod::UserLike()
{
	GodPrintCommodity(USER, get_pComm_());
	
	int ID = 0;
	char couID[NAME_WORDS] = "";
	cout << "����������Ҫ���޵���Ʒ���(���� * �˳�):";
	cin >> couID;
	if (couID[0] == '*')
		return;

	for (int i = 1; couID[i] != '\0'; ++i)
		ID = ID * 10 + couID[i] - '0';

	Commodity *comm = pRun[0]->UserFindCommdity(get_pComm_(), ID);
	++comm->likes;

	cout.setf(std::ios::left);
	cout << setw(10) << "ID" << setw(10) << "����" << setw(10) << "Ʒ��" << setw(10) << "�۸�" << setw(10) << "����" << setw(10) << "�� !" << endl;
	cout.fill('0');
	cout << right << 'F' << setw(5) << comm->id << "    ";
	cout.fill(' ');
	cout << left << setw(10) << comm->name << setw(10) << comm->brand << setw(10) << comm->price << setw(10) << comm->number << comm->likes - 1 << " + 1" << endl;
	cout << "��л���ĵ���, ���ǻ����ø��� !" << endl;	
}

void CGod::UserComment()
{
	GodPrintCommodity(USER, get_pComm_());
	for (int i_priCom = 0; i_priCom < MAX_COMM_LINE; ++i_priCom)
	{
		if (arr_comment_[i_priCom].num_com != 0)
			cout << "��������:" << endl;
		for (int i = 0; i < arr_comment_[i_priCom].num_com; ++i)
			cout << arr_comment_[i_priCom].name[i] << ": " << arr_comment_[i_priCom].comment[i] << endl;
	}

	bool isComment = false;
	cout << "ȷ��Ҫ������ ? д�������� 1 , �˳����� 0 :";
	cin >> isComment;
	if (isComment == 0)
		return;

	string tempCom;
	bool isOK = false;
	int ID = 0;
	while (isOK != true)
	{
		ID = 0;
		char couID[NAME_WORDS] = "";
		cout << "����������Ҫ���۵���Ʒ���(���� * �˳�):";
		cin >> couID;
		if (couID[0] == '*')
			return;

		for (int i = 1; couID[i] != '\0'; ++i)
			ID = ID * 10 + couID[i] - '0';

		Commodity *comm = pRun[0]->UserFindCommdity(get_pComm_(), ID);
		cout.fill('0');
		cout << right << 'F' << setw(5) << comm->id << "    ";
		cout.fill(' ');
		cout << left << setw(10) << comm->name << setw(10) << comm->brand << setw(10) << comm->price << setw(10) << comm->number << endl;
		cout << "������� ? ȷ���밴 1, ���������밴 0 :";
		cin >> isOK;
	}

	cout << "��������������:";
	cin >> tempCom;
	arr_comment_[ID].name[arr_comment_[ID].num_com] = login_user_name_;
	arr_comment_[ID].comment[arr_comment_[ID].num_com] = tempCom;
	++arr_comment_[ID].num_com;
	cout << "������ӳɹ� !" << endl;
}
