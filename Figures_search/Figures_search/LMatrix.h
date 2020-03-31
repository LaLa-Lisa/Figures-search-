#pragma once


#include <vector>


class LMatrix {
public:
	//количество строк
	int n;
	//количсетво столбцов
	int m;
	//массив данных матрицы
	std::vector<std::vector<double>> data;

	LMatrix();
	LMatrix(int, int);

	//инициализация
	void init(int, int);

	LMatrix operator=(LMatrix);
	LMatrix operator+(LMatrix);
	LMatrix operator-(LMatrix);
	LMatrix operator*(LMatrix);
	LMatrix operator*(double);

	//транспонирование 
	LMatrix T();

	//чтение матрицы
	void read();

	//вывод матрицы
	void write();



};