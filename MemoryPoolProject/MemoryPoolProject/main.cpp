#include "pch.h"

using namespace std;

int main()
{
	cout << "Memory Pool Test Start" << endl;

	auto Start = chrono::high_resolution_clock::now();

	// Todo : new/delete ��İ� �޸�Ǯ ��� �� ����



	auto End = chrono::high_resolution_clock::now();
	auto Duration = chrono::duration_cast<chrono::milliseconds>(End - Start);
	cout << "Elapsed: " << Duration.count() << " ms" << endl;

	return 0;
}