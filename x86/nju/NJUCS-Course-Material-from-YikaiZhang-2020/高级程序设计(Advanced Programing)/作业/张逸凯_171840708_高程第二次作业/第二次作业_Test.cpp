#include <iostream>
using namespace std;

class Test
{
private:
	//1. const ��Ա����
	const int a;
public:
	//2. ���캯����ʼ��
	Test() : a(10) { }
	Test(int tmpa) : a(tmpa) { }

	int getConst()
	{
		//3. ���� const ��Ա
		return a;
	}
};


int main()
{
	// 4. ʵ���� Test����
	Test t;
	Test tmp(10);
	// 5. ��� const ��Ա
	cout << t.getConst() << endl;
	cout << tmp.getConst() << endl;
	return 0;
}