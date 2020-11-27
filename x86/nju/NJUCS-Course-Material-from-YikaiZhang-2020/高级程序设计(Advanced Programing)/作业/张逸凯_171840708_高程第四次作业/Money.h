#pragma once
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

class Money
{
public:
	Money() {
		cout << "������Ԫ���� �֣�" << endl;
		cin >> yuan >> jiao >> fen;
	}
	Money(int y, int j, int f) : yuan(y), jiao(j), fen(f) { }

	// ǰ��
	friend const Money operator +(const Money& a, const Money& b);
	friend const Money operator -(const Money& a, const Money& b);

	void display() {
		if (yuan < 0) {
			cout << "-";
			cout << -yuan << "Ԫ" << -jiao << "��" << -fen << "��" << endl;
		}
		else {
			cout << yuan << "Ԫ" << jiao << "��" << fen << "��" << endl;
		}
	}
private:
	int yuan, jiao, fen;
};