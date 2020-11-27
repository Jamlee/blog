#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>

using namespace std;

const int CITY_NUMBER = 100;
const int CITY_WORDS = 30;
const int FILE_WORDS = 30;

class File;
class CCityList;
class CRoute;
class CVipRoute;

struct CityType
{
	double price;
	string name;
};

struct CityNode
{
	CityType city;
	CityNode *next;
};




class File
{
public:
	File(){}
	void SetSpacing(char *arr, int num);
	void InputList(char *filename, CityType *cities);
	bool InputRoute(char *filename, CRoute *&comroute, CRoute *&viproute, CCityList &cities);
	void OutputRoute(char *filename, CRoute *route);

private:
	char *file_name_;
	char *out_file_name_;
};


class CCityList
{
public:
	CCityList()
	{
		city_num_ = 0;
	}
	void BuildCityList(char filename[]);
	void PrintPriceList();
	char PrintMenu();
	CityType *get_cities_(int i)
	{
		return &cities[i];
	}
	void set_cities(string cha_name, double cha_price, int i_location)
	{
		cities[i_location].name = cha_name;
		cities[i_location].price = cha_price;
	}
	void CCityList::ChangeCityPrice(CRoute &cityroute);

private:
	CityType cities[CITY_NUMBER];
	int city_num_;
	File f_list_;
};

class CRoute
{
public:
	CRoute()
	{
		route_head_ = NULL;
		city_count_ = 0;
	}
	CityNode *&get_route_head_ ()
	{
		return route_head_;
	}
	int &get_city_count_ ()
	{
		return city_count_;
	}
	File &get_f_route_ ()
	{
		return f_route_;
	}
	void CreateRoute(CCityList &curcitylist);
	double SearchCityPrice(string SearchCity, CCityList &curcitylist);
	void PrintRoute(bool isbuild);
	virtual void PrintSumPrice();
	void ChangeRoute(CCityList &curcitylist);
	void InsertCity(CCityList &curcitylist);
	void DeleteCity(CCityList &curcitylist);
	void ChangeRoutePrice(string changename, double changeprice);


private:
	CityNode *route_head_;
	int city_count_;
	File f_route_;
};

class CVipRoute :public CRoute
{
public:
	virtual void PrintSumPrice();

private:
};



void File::SetSpacing(char *arr, int num)
{
	for (int i = 0; i < num; ++i)
	{
		if (arr[i] == ' ' || arr[i] == '\t' || arr[i] == '\n')
			arr[i] = '\0';
	}
}


// read from the file into the cities[]
// how to set to num_city in class CCityList?
// judge whether vip by who call it
void File::InputList(char *filename, CityType *cities)
{
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, filename, "r");
	if (err != NULL)
	{
		cerr << "Initial file can't open !" << endl;
		exit(-1);
	}


	char filecity[CITY_WORDS];
	fgets(filecity, CITY_WORDS, pf);   // throw away the flag of file
	fgets(filecity, CITY_WORDS, pf);
	int i_cities = 0;

	while (!feof(pf))
	{
		SetSpacing(filecity, CITY_WORDS);

		bool iscity = true;
		for (int i = 1; i < CITY_WORDS; ++i)
		{
			if (iscity && filecity[i] != '\0')
			{
				iscity = false;
				char *p = &filecity[i];
				string temp = p;
				cities[i_cities].name = temp;
			}
			else if (!iscity && filecity[i] <= '9' && filecity[i] >= '0')
			{
				double sum_price = 0;
				while (filecity[i] <= '9' && filecity[i] >= '0')
				{
					sum_price = sum_price * 10 + double(filecity[i] - '0');
					++i;
				}
				cities[i_cities].price = sum_price;
				break;
			}
		}
		++i_cities;

		fgets(filecity, CITY_WORDS, pf);
	}

	cities[i_cities].price = -1;
	fclose(pf);
}

bool File::InputRoute(char *filename, CRoute *&comroute, CRoute *&viproute, CCityList &cities)
{
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, filename, "r");
	if (err != NULL)
	{
		cerr << "InputRouteFile can't open !" << endl;
		exit(-1);
	}

	CRoute *route = NULL;
	char isVip, rouArr[CITY_NUMBER] = "";
	isVip = fgetc(pf);
	if (isVip == 't')
		route = viproute;
	else
		route = comroute;
	fgets(rouArr, CITY_NUMBER, pf);
		
	CityNode *tail = route->get_route_head_();
	this->SetSpacing(rouArr, CITY_NUMBER);
	bool isACity = true, isHead = true;
	for (int i = 1; i < CITY_WORDS; ++i)
	{
		if (i != 1 && rouArr[i] != '\0' && rouArr[i - 1] == '\0')
			isACity = true;
		if (isACity && rouArr[i] != '\0')
		{
			isACity = false;
			char *p = &rouArr[i];
			string temp = p;
			if (isHead)
			{
				isHead = false;
				CityNode *p= new CityNode;
				p->next = NULL;
				p->city.name = temp;
				p->city.price = route->SearchCityPrice(temp, cities);
				route->get_route_head_() = p;
				tail = p;
			}
			else
			{
				CityNode *p = new CityNode;
				p->next = NULL;
				p->city.name = temp;
				p->city.price = route->SearchCityPrice(temp, cities);
				tail->next = p;
				tail = p;
			}
		}
	}
	fclose(pf);

	if (isVip == 't')
		return true;
	else
		return false;
}

void File::OutputRoute(char *filename, CRoute *route)
{
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, filename, "wb");
	if (err != NULL)
	{
		cerr << "InputRouteFile can't open !" << endl;
		exit(-1);
	}

	CityNode *outRoute = route->get_route_head_();

	while (outRoute != NULL)
	{
		fwrite(outRoute, sizeof(struct CityNode), 1, pf);
		outRoute = outRoute->next;
	}

	fclose(pf);
}




















void CCityList::BuildCityList(char filename[])
{
	f_list_.InputList(filename, cities);
}

void CCityList::PrintPriceList()
{
	for (int num = 0; (*this->get_cities_(num)).price != -1; ++num)
		cout << num << "    " << (*this->get_cities_(num)).name << "   " << (*this->get_cities_(num)).price << endl;
}

char CCityList::PrintMenu()
{
	puts("****************************************************************");
	cout << setw(33) << "�˵�" << endl;
	cout << "           " << "1. ��ӡ���γ��м۸��" << endl;
	cout << "           " << "2. ��������·��" << endl;
	cout << "           " << "3. ������·���е�ĳ�������滻Ϊ�����ĳ���" << endl;
	cout << "           " << "4. ������·���е���������֮��, �����µĳ���" << endl;
	cout << "           " << "5. ɾ������·���е�ĳ������" << endl;
	cout << "           " << "6. �������γ��м۸����ĳ�����еļ۸�" << endl;
	cout << "           " << "7. ��������·����������ļ���" << endl;
	cout << "           " << "8. ���ļ��ж�ȡ������·����" << endl;
	cout << "           " << "*. �˳�����" << endl;
	puts("****************************************************************\n");

	char input;
	cout << "���������Ĳ���:";
	(cin >> input).get();

	return input;
}

void CCityList::ChangeCityPrice(CRoute &cityroute)
{
	string changecity;
	CityType *pcity = this->get_cities_(0);
	double changeprice = 0;

	cout << "��������Ҫ�����۸�ĳ���:";
	cin >> changecity;
	int i_pcity = 0;
	for (; pcity[i_pcity].price != -1 && pcity[i_pcity].name != changecity; ++i_pcity);
	if (pcity[i_pcity].price == -1)
	{
		cout << "�����б���û��������� !" << endl;
		return;
	}

	cout << changecity << "ԭ���ļ۸�Ϊ" << pcity[i_pcity].price << ", �����������ļ۸�:";
	cin >> changeprice;

	pcity[i_pcity].price = changeprice;
	cityroute.ChangeRoutePrice(changecity, changeprice);
}




















void CRoute::PrintRoute(bool isbuild)
{
	if (this->get_route_head_() == NULL)
	{
		cout << "���·���ǿյ�Ŷ~" << endl;
		return;
	}
	CityNode *pri = this->get_route_head_();
	if (isbuild)
		cout << "������·Ϊ:";
	else
		cout << "������·����Ϊ:";

	cout << pri->city.name;
	pri = pri->next;

	while (pri != NULL)
	{
		cout << "->" << pri->city.name;
		pri = pri->next;
	}

	cout << endl;
}

void CRoute::PrintSumPrice()
{
	if (this->get_route_head_() == NULL)
		cout << "���·���ǿյ�Ŷ~" << endl;

	double sum_price = 0;
	CityNode *sum = this->get_route_head_();
	while (sum != NULL)
	{
		sum_price += sum->city.price;
		sum = sum->next;
	}

	cout << "�ܼ۸�:" << setiosflags(ios::fixed) << setprecision(1) << sum_price << endl;
}

void CRoute::CreateRoute(CCityList &curcitylist)
{
	// input
	char city_array[CITY_NUMBER * 3];
	cout << "���������, �Կո�ָ�, ���н���:";
	gets_s(city_array);

	// trun spacing into \0 for copying to 'string' type
	int cityarrnum = 0;
	for (; city_array[cityarrnum] != '\0'; ++cityarrnum);
	this->get_f_route_().SetSpacing(city_array, cityarrnum);

	// read and build
	CityNode *tail = NULL;
	char *strhead = city_array;
	bool isfirst = true;
	for (int i_city_array = 0; i_city_array < cityarrnum; ++i_city_array)
	{
		if (i_city_array == 0 || (i_city_array != cityarrnum - 1 && city_array[i_city_array] == '\0'))
		{
			if (i_city_array == 0)
				strhead = &city_array[i_city_array];
			else
				strhead = &city_array[i_city_array + 1];
			string inputcity = strhead;

			if (SearchCityPrice(inputcity, curcitylist) == -1)
			{
				cout << "��Ǹ���������к��г����б���û�еĳ���, ������ !" << endl;
				continue;
			}

			if (isfirst)
			{
				isfirst = false;
				this->get_route_head_()= new CityNode;
				this->get_route_head_()->next = NULL;
				this->get_route_head_()->city.name = inputcity;
				this->get_route_head_()->city.price = SearchCityPrice(inputcity, curcitylist);
				tail = this->get_route_head_();
			}
			else
			{
				CityNode *p = new CityNode;
				p->next = NULL;
				p->city.price = SearchCityPrice(inputcity, curcitylist);
				p->city.name = inputcity;
				tail->next = p;
				tail = p;
			}
			++city_count_;
		}
	}

	cout << "�����ɹ�!" << endl;
}

double CRoute::SearchCityPrice(string SearchCity, CCityList &curcitylist)
{
	CityType *getcity = curcitylist.get_cities_(0);
	for (; getcity->price != -1; ++getcity)
	{
		if (getcity->name.compare(SearchCity) == 0)
			return getcity->price;
	}
	return -1;
}

void CRoute::ChangeRoute(CCityList &curcitylist)
{
	string change_city, input_city;
	cout << "�����뱻�滻�ĳ���:";
	cin >> change_city;
	cout << "�������滻����:";
	cin >> input_city;

	if (SearchCityPrice(input_city, curcitylist) == -1)
	{
		cout << "��Ǹ�滻���в��ڳ����б���, ���������� !" << endl;
		return;
	}

	CityNode *change = this->get_route_head_();
	while (change->city.price != -1 && change->city.name != change_city)
		change = change->next;
	if (change->city.price == -1)
	{
		cout << "���滻�ĳ��в���·����, ���������� !" << endl;
		return;
	}

	change->city.name = input_city;
	change->city.price = SearchCityPrice(input_city, curcitylist);

}

void CRoute::InsertCity(CCityList &curcitylist)
{
	string city1, city2, insertcity;
	cout << "������������·���������ĳ���:";
	cin >> city1 >> city2;
	cout << "��������Ҫ����ĳ���:";
	cin >> insertcity;

	// find the location to insert
	CityNode *ins = this->get_route_head_();
	for (; ins != NULL && ins->city.name != city1; ins = ins->next);
	if (ins == NULL)
	{
		cout << "�Ҳ�������λ��, ������������Ƿ���·���� !" << endl;
		return;
	}

	if (SearchCityPrice(insertcity, curcitylist) == -1)
	{
		cout << "��Ҫ����ĳ��в��ڳ����б���, ���������� !" << endl;
		return;
	}
	CityNode *p = new CityNode;
	p->city.price = SearchCityPrice(insertcity, curcitylist);
	p->city.name = insertcity;

	if (ins->next != NULL)
		p->next = ins->next;
	else
		p->next = NULL;

	ins->next = p;

}

void CRoute::DeleteCity(CCityList &curcitylist)
{
	string delcity;
	CityNode *pfind = this->get_route_head_();
	if (pfind == NULL)
	{
		cout << "����·�߿յ��� !" << endl;
		return;
	}
	cout << "��������Ҫɾ���ĳ���:";
	cin >> delcity;

	if (pfind->city.name == delcity)
	{
		if (pfind->next == NULL)
		{
			delete this->get_route_head_();
			this->get_route_head_() = NULL;
		}
		else
		{
			this->get_route_head_() = pfind->next;
			delete pfind;
		}
		return;
	}

	// find the location to delete
	for (; pfind->next != NULL && pfind->next->city.name != delcity; pfind = pfind->next);

	if (pfind->next == NULL)
	{
		cout << "���в���������·��!" << endl;
		return;
	}

	CityNode *tdel = pfind->next;

	if (pfind->next->next != NULL)
		pfind->next = pfind->next->next;
	else
		pfind->next = NULL;

	delete tdel;
}

void CRoute::ChangeRoutePrice(string changename, double changeprice)
{
	CityNode *pchangename = this->get_route_head_();
	for (; pchangename != NULL; pchangename = pchangename->next)
	{
		if (pchangename->city.name == changename)
			break;
	}
	if (pchangename != NULL)
	{
		cout << "error: ·����û��������� !" << endl;
		pchangename->city.price = changeprice;
	}
}


void CVipRoute::PrintSumPrice()
{
	double sum_price = 0;
	CityNode *sum = this->get_route_head_();
	while (sum != NULL)
	{
		sum_price += sum->city.price;
		sum = sum->next;
	}

	cout << "�ܼ۸�:" << setiosflags(ios::fixed) << setprecision(1) << sum_price * 0.8 << endl;
}





















int main()
{
	File f;
	CRoute *pcomroute = NULL, *pviproute = NULL;
	CVipRoute vip_city_route;
	CRoute city_route;
	pviproute = &vip_city_route;
	pcomroute = &city_route;
	CCityList city_list;
	city_list.BuildCityList("D:\\pricelist.txt");
	bool isinitial = false;
	CRoute *proute = NULL;

	char input;
	bool isOk = true;
	while ((input = city_list.PrintMenu()) != '*')
	{
		if (isinitial == false && input != '2' && input != '1' && input != '8')
			continue;
		if (isOk == false)
			break;
		switch (input)
		{
		case '1':
			city_list.PrintPriceList();
			break;
			
		case '2':
			isinitial = true;
			int isviprou;
			cout << "����vip·������0, ������ͨ·������1:";
			(cin >> isviprou).get();
			if (isviprou != 0 && isviprou != 1)
			{
				input = '*';
				cin.clear();
				cout << "������������ !" << endl;
				break;
			}
			if (isviprou == 0)
				proute = pviproute;
			else
				proute = pcomroute;

			proute->CreateRoute(city_list);
			proute->PrintRoute(true);
			proute->PrintSumPrice();
			break;

		case '3':
			proute->ChangeRoute(city_list);
			proute->PrintRoute(false);
			proute->PrintSumPrice();
			break;

		case '4':
			proute->InsertCity(city_list);
			proute->PrintRoute(false);
			proute->PrintSumPrice();
			break;

		case '5':
			proute->DeleteCity(city_list);
			proute->PrintRoute(false);
			proute->PrintSumPrice();
			break;

		case '6':
			city_list.ChangeCityPrice(*proute);
			city_list.PrintPriceList();
			proute->PrintRoute(true);
			proute->PrintSumPrice();
			break;

		case '7':
			char filename[FILE_WORDS];
			cout << "�������ļ���:";
			cin >> filename;
			proute->get_f_route_().OutputRoute(filename, proute);
			cout << "�����ɹ� !" << endl;
			break;

		case '8':
			isinitial = true;
			char infilename[FILE_WORDS];
			bool isVip;
			cout << "�������ļ���:";
			cin >> infilename;
			/*"D:\\route.txt"*/
			isVip = f.InputRoute(infilename, pcomroute, pviproute, city_list);
			if (isVip == true)
				proute = pviproute;
			else
				proute = pcomroute;
			cout << "����·�߳ɹ� !" << endl;
			proute->PrintRoute(true);
			proute->PrintSumPrice();
			break;


		default:
			isOk = false;
			break;
		}
	}

	return 0;
}
