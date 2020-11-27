#pragma once
#include <iostream>
#include <string.h>

using namespace std;

class CustomString
{
public:
	//���캯��
	CustomString();
	CustomString(const char* str);
	CustomString(const CustomString& a);		// ��Ҫ���˿������캯��.
	CustomString operator +(const CustomString& a);
	CustomString operator +=(const CustomString& a);
	bool operator ==(const CustomString& a);
	bool operator !=(const CustomString& a);
	friend ostream& operator <<(ostream& os, const CustomString& a);
	friend istream& operator >>(istream& in, CustomString& a);

	CustomString& operator=(const CustomString& a) {
		delete[]this->p;

		p = new char[a.len];
		strcpy(p, a.p);
		len = a.len;

		return *this;
	}


	//��������
	~CustomString();


	// �Լ�д�Ĳ���:
	char& operator [](int idx);
private:
	char* p; // �ַ�������ʼ��ַ
	int len; // �ַ����ĳ���
};
