#include <iostream>
#include <fstream>
#include "LMatrix.h"


std::ifstream fin("in.txt");
std::ofstream fout("out.txt");

LMatrix::LMatrix(){}

LMatrix::LMatrix(int n0, int m0) {
	if (n0 == 0 || m0 == 0) std::cout << "Matix size is 0\n";	//тут ретурн написать

	n = n0;
	m = m0;
	data.resize(n);
	for (int i = 0; i < n; i++)
		data[i].resize(m);
}

//инициализация
void LMatrix::init(int n0, int m0) {
	if (n0 == 0 || m0 == 0) std::cout << "Matix size is 0\n";	

	n = n0;
	m = m0;
	data.resize(n);
	for (int i = 0; i < n; i++)
		data[i].resize(m);
}

LMatrix LMatrix::operator=(LMatrix right) {
	//проверка на самоприсваивание
	if (this == &right) {
		return *this;
	}

	n = right.n;
	m = right.m;
	data = right.data;
	return *this;
}

LMatrix LMatrix::operator*(LMatrix right) {
	if (m != right.n) {
		std::cout << "(matrix_mult) are you stupid?\n";
		return *this;
	}

	LMatrix temp(n, right.m);
	//std::cout << right.data[0][0] << "\n";
	for (int i = 0; i < n; i++)
		for (int j = 0; j < right.m; j++) {
			temp.data[i][j] = 0;
			for (int k = 0; k < m; k++)
			{
				temp.data[i][j] += data[i][k] * right.data[k][j];
			}
		}
	return temp;
}

LMatrix LMatrix::operator*(double right) {

	LMatrix temp(n, m);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++) 
		{
			temp.data[i][j] = data[i][j] * right;
		}
	return temp;
}

LMatrix LMatrix::operator+(LMatrix right) {
	if (n != right.n || m != right.m) std::cout << "Matrix sizes are different\n";

	LMatrix temp(n, m);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
		{
			temp.data[i][j] = data[i][j] + right.data[i][j];
		}
	//this->Delete();
	return temp;
}

LMatrix LMatrix::operator-(LMatrix right) {
	if (n != right.n || m != right.m) std::cout << "Matrix sizes are different\n";

	LMatrix temp(n, m);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
		{
			temp.data[i][j] = data[i][j] - right.data[i][j];
		}
	//this->Delete();
	return temp;
}


//транспонирование 
LMatrix LMatrix::T() {
	LMatrix temp(m, n);

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
		{
			temp.data[i][j] = data[j][i];
		}
	return temp;
}

//чтение матрицы
void LMatrix::read() {
	for (int i = 0; i < n; i++) 
		for (int j = 0; j < m; j++)
		{
			fin >> data[i][j];
		}
}

//вывод матрицы
void LMatrix::write() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
		{
			fout << data[i][j] << " ";
		}
		fout << "\n";
	}
	fout << "\n";
}


