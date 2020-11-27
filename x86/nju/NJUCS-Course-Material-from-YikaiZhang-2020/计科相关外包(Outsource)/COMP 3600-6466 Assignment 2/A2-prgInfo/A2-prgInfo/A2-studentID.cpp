#include <iostream>
#include <fstream>

using namespace std;

#define in(x) x=read()

inline int read() {
	int X = 0, w = 1;
	char ch = getchar();
	while (ch<'0' || ch>'9') { if (ch == '-') w = -1; ch = getchar(); }
	while (ch >= '0' && ch <= '9') X = (X << 3) + (X << 1) + ch - '0', ch = getchar();
	return X * w;
}

int MyMin(int a, int b) {
	return a > b ? b : a;
}

void Swap(int arr[], int i, int j) {
	if (i == j)
		return;

	arr[i] = arr[i] ^ arr[j];
	arr[j] = arr[i] ^ arr[j];
	arr[i] = arr[i] ^ arr[j];
}

// ���ÿ���, ÿ�ζ��ǰѻ�׼Ԫ�ع鵽��ȷ��λ��, ��������ǻ�ȡ��һ��Ԫ��λ��, arr[]: 1 ~ n.
// �������������һ�ֶ���.
int GetRank(int arr[], int n) {
	int pivot = arr[1], l = 1, r = n;

	while (l < r) {
		while (l < r && arr[r] >= pivot)		// �ŵ���һ��С��pivot��
			--r;
		arr[l] = arr[r];

		while (l < r && arr[l] <= pivot)		// �ŵ���һ������pivot��
			++l;
		arr[r] = arr[l];
	}
	arr[l] = pivot;					// ��Ҫ���˷Ż�ȥ

	return l;
}

int rand_approx_median(int arr[], int n, int pos) {
	int tmp = GetRank(arr, n);		// ��һ�����ڵڼ���, �������ŵ���ȷλ��

	if (tmp == pos)
		return arr[pos];
	else if (tmp < pos)				// �ҵ��ĸ�С, ˵����pos���ҵ����Ǹ�����
		return rand_approx_median(arr + tmp, n - tmp, pos - tmp);
	// ע������ tmp - 1, ��Ϊ�� tmp ��ǰ��, ����ֻ�� tmp - 1 ������ ( ������ 1 ��ͷ! )
	else
		return rand_approx_median(arr, tmp - 1, pos);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Please supply the name of the input file\n";
    } 
    else {
		// Read input
		ifstream inFile(argv[1]);
		// ifstream inFile("./testCases/test-2.txt");
		int n, x, y;
		inFile >> n;
		int* xVec = new int[n + 1], * yVec = new int[n + 1];
		for (int i = 1; i <= n; i++) {
			inFile >> x >> y;
			// Store x and y data
			xVec[i] = x;
			yVec[i] = y;
		}

		int xh, yh;
		// Place your algorithm here
		// If you need to create a function, place the function above the main function
		// The results of your algorithm should be placed in variable xh and yh
		int mid = (n + 1) / 2;
		xh = rand_approx_median(xVec, n, mid), yh = rand_approx_median(yVec, n, mid);
		// Print output
        cout << xh << " " << yh << "\n";
    }

    return 0;
}