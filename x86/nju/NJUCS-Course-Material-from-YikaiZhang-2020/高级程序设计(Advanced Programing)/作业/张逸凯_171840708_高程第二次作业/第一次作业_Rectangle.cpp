#include "Rectangle.h"

// ���ó����ο�͸ߵ� setWH() ����
void Rectangle::setWH(int w, int h) {
	width = w;
	height = h;
}

// ���㳤�����ܳ��� CRectangle() ����
int Rectangle::CRectangle() {
	return width * 2 + height * 2;
}

int main() {
	Rectangle rectangle(3, 4); // ʵ���� rectangle ����
	cout << "The perimeter of the rectangle is: " << rectangle.CRectangle() << endl;
	rectangle.setWH(5, 6); // �޸Ŀ�͸ߵ�ֵ
	cout << "The perimeter of the rectangle is: " << rectangle.CRectangle() << endl;


	return 0;
}


