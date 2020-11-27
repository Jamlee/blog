#include <iostream>
#include <cstring>
using namespace std;
class Matrix {
private:
	int dim;
	double* m_data;
public:
	Matrix(int d);
	Matrix(const Matrix& a);	// �������
	~Matrix();
};
Matrix::Matrix(int d) {
	dim = d;
	m_data = new double[dim * dim];
	cout << "Matrix" << endl;
}

// ����Ŀ������캯��:
Matrix::Matrix(const Matrix& a) {
	this->dim = a.dim;
	this->m_data = new double[dim * dim];	// ������Դ.
	// �����ݸ��Ƶ��µĶ���ָ����ڴ�ռ���.
	for (int i = 0; i < dim * dim; ++i) {
		m_data[i] = a.m_data[i];
	}
}

Matrix::~Matrix() {
	cout << "~Matrix " << (int)m_data << endl;
	delete[] m_data;
	m_data = NULL;
}

int main()
{
	{
		Matrix m1(5);
		Matrix m2(m1);
	}


	system("pause");
	return 0;
}