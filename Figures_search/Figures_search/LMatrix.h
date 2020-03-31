#pragma once


#include <vector>


class LMatrix {
public:
	//���������� �����
	int n;
	//���������� ��������
	int m;
	//������ ������ �������
	std::vector<std::vector<double>> data;

	LMatrix();
	LMatrix(int, int);

	//�������������
	void init(int, int);

	LMatrix operator=(LMatrix);
	LMatrix operator+(LMatrix);
	LMatrix operator-(LMatrix);
	LMatrix operator*(LMatrix);
	LMatrix operator*(double);

	//���������������� 
	LMatrix T();

	//������ �������
	void read();

	//����� �������
	void write();



};