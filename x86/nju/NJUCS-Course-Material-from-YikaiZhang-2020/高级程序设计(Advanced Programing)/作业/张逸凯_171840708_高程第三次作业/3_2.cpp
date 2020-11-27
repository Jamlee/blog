#include <iostream>
using namespace std;

class Shooting
{
	float FTPercentage; // ����������
	float FGPercentage; // Ͷ��������
	float TPPercentage; // ����������
public:
	Shooting()
	{
		FTPercentage = 0.2;
		FGPercentage = 0.2;
		TPPercentage = 0.2;
	}
	Shooting(float ftp, float fgp, float tpp)
	{
		FTPercentage = ftp;
		FGPercentage = fgp;
		TPPercentage = tpp;
	}
};

class NBAPlayer
{
	Shooting shoot; // ʵ���� Shooting ���� shoot
	string name;
public:
	//��ȫ1. ���� Shooting ��Ĭ�Ϲ��캯���� shoot ��ʼ��;
	NBAPlayer(string n) : name(n) { };
	//��ȫ2. ���� Shooting(float ftp, float fgp, float tpp) ���캯���� shoot ��ʼ��;
	NBAPlayer(string n, float ftpp, float fgpp, float tppp) : name(n), shoot(ftpp, fgpp, tppp) { }
};

int main()
{
	//��ȫ3. p1.name ��ʼ��Ϊ Curry��p1.shoot�ĸ��������ʲ���Ĭ�ϳ�ʼ����
	NBAPlayer p1("Curry");
	//��ȫ4. p2.name ��ʼ��Ϊ Curry��p2.shoot�� FTPercentage��ʼ��Ϊ 0.9, FGPercentage ��ʼ��Ϊ 0.71��TPPercentage ��ʼ��Ϊ0.44
	NBAPlayer p2("", 0.9, 0.71, 0.44);

	return 0;
}