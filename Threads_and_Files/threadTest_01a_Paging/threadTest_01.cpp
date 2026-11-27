#include <iostream>
#include <stdio.h>
#include <string>
#include <iomanip>
#include <Windows.h>

int main(int argc, char* argv[])
{
//	int myArray[4000000];	// STACK (1MBtyte)
	// 16,000,000 

	int* pMyArray = new int[1'000'000'000];		// HEAP
	int* pMyArray2 = new int[1'000'000'000];		// HEAP
	int* pMyArray3 = new int[1'000'000'000];		// HEAP
	int* pMyArray4 = new int[1'000'000'000];		// HEAP
	// 16,000,000

	int a = 6;
	int* pA = &a;

	unsigned long long startOfArray = (unsigned int)pMyArray;

//	std::cout << std::dec << &a << std::endl;
//	std::cout << std::dec << pA << std::endl;
	std::cout << startOfArray << std::endl;
	std::cout << startOfArray + (1'000'000'000 * 4) << std::endl;


	pMyArray[35] = 83323;

	printf("Array allocated.\n");


	MessageBox(NULL, L"Hi", L"Testing", MB_OK);

	delete[] pMyArray;

	return 0;
}
