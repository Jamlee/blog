#include "string_operator.h"


///////////////// �Լ�д�Ĳ���:
// ���캯��
CustomString::CustomString() : p(NULL), len(0) { }
CustomString::CustomString(const char* str) : len(strlen(str) + 1) {
	p = new char[len];
	strcpy(p, str);
}

// ע�ⲻҪ©�˿������캯��:
CustomString::CustomString(const CustomString& a) : len(a.len) {
	p = new char[a.len];
	strcpy(p, a.p);
}



char& CustomString::operator [](int idx) {
	if (idx >= len) {
		printf("index ERROR!");
		exit(0);
	}
	return p[idx];
}

// ���ؼӺ�, ���غʹ���ȥ����const A&, ���صĶ��Ǻ����ڴ�������ʱ����
CustomString CustomString::operator +(const CustomString& a) {
	char* arr = new char[this->len + a.len];
	int cnt = 0;
	for (int i = 0; p[i] != '\0'; ++i) {
		arr[cnt++] = p[i];
	}
	for (int i = 0; a.p[i] != '\0'; ++i) {
		arr[cnt++] = a.p[i];
	}
	arr[cnt++] = '\0';

	CustomString tmp(arr);

	delete[]arr;
	return tmp;
}
// ����+=, ���غʹ���ȥ������+һ��, ������ֱ���޸ı���(���ǵ�һ������), ����*this
CustomString CustomString::operator +=(const CustomString& a) {
	char *arr = new char[this->len + a.len];
	int cnt = 0;
	for (int i = 0; p[i] != '\0'; ++i) {
		arr[cnt++] = p[i];
	}
	for (int i = 0; a.p[i] != '\0'; ++i) {
		arr[cnt++] = a.p[i];
	}
	arr[cnt++] = '\0';
	this->len = cnt;

	delete[]this->p;
	p = new char[this->len];
	strcpy(p, arr);

	return *this;
}

bool CustomString::operator ==(const CustomString& a) {
	if (strcmp(this->p, a.p) == 0) {
		return true;
	}

	return false;
}

bool CustomString::operator !=(const CustomString& a) {
	return !(*this == a);
}
// ���� << ����������Ӧ��ostream&��Ϊ���һ����������������const�����������Ϊ�ڶ��������������ض�ostream�βε�����, ��������Ϊ��Ԫ.
ostream& operator <<(ostream& out, const CustomString& a) {
	out << a.p;
	return out;
}

// ע����������Ͳ���Ҫ const A& ��.
istream& operator >>(istream& in, CustomString& a) {
	delete[]a.p;
	char arr[100010] = "";
	in >> arr;
	a.len = strlen(arr) + 1;

	a.p = new char[a.len];
	strcpy(a.p, arr);
	return in;
}

///////////////// �Լ�д�Ĳ���

// ��������
CustomString::~CustomString()
{
	delete[] p;
	p = NULL;
}

int main() {
	CustomString mystr("this is e CustomString class for testing!");
	cout << mystr[8] << endl;
	mystr[8] = 'a';
	cout << mystr << endl;
	CustomString mystr2 = mystr;			// ע�������ǵ��ÿ������캯��, ���Ǹ�ֵ����������!
	cout << mystr2 << endl;
	CustomString mystr3;
	mystr3.operator=(mystr + mystr2);
	cout << mystr3 << endl;
	cout << mystr + mystr2 << endl;
	mystr3 += mystr;
	cout << mystr3 << endl;
	cout << (mystr == mystr2) << endl;
	cout << (mystr != mystr3) << endl;
	CustomString mystr4;
	cout << "Input any string to test the overloaded input operator >>: " << endl;
	cin >> mystr4;
	cout << mystr4 << endl;
	cout << "Congratulations! testing passed!" << endl;
	
	return 0;
}