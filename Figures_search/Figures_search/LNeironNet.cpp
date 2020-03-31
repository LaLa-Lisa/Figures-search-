#include <iostream>
#include "LMatrix.h"
#include <fstream>
#include "LNeironNet.h"


double sigmoid(double x) {
	return 1 / (1 + exp(-x));
}

LNeironNet::LNeironNet(){}

LNeironNet::LNeironNet(int layer_num0, int* layer_size0) {
	layer_num = layer_num0;
	layer_size = layer_size0;

	Weits.resize(layer_num - 1);
	Rez.resize(layer_num);
	Err.resize(layer_num);

	for (int i = 0; i < layer_num; i++) {
		Rez[i].init(1, layer_size[i]);
		Err[i].init(1, layer_size[i]);
	}

	for (int i = 0; i < layer_num - 1; i++) {
		Weits[i].init(layer_size[i], layer_size[i + 1]);
		for (int j = 0; j < Weits[i].n; j++)
			for (int k = 0; k < Weits[i].m; k++)
			{
				//Weits[i].data[j][k] = 0.0001 * (rand() % 20) + 0.0001; // рандомим коэффициенты
				Weits[i].data[j][k] = 0.0001 * (rand() % (int)1000/(sqrt(layer_size[i]))) + 0.0001;
			}
	}
}

LMatrix LNeironNet::work(LMatrix inputs) {
	if (inputs.n != 1 || inputs.m != layer_size[0]) std::cout << "(work) ti durak?\n";
	
	Rez[0] = inputs;

	for (int i = 0; i < layer_num - 1; i++)
	{
		//пропускаем первый элемент, когда делаем сигмойду
		if (i != 0)
			for (int j = 0; j < Rez[i].m; j++)
			{
				Rez[i].data[0][j] = sigmoid(Rez[i].data[0][j]);			//подумать над написанием этого в классе матриц
			}

		Rez[i + 1] = Rez[i] * Weits[i];

	}

	//применяем сигмойду к последнему слою, т.к. в цикле она не применилась
	for (int j = 0; j < Rez[layer_num - 1].m; j++)
	{
		Rez[layer_num - 1].data[0][j] = sigmoid(Rez[layer_num - 1].data[0][j]); 
	}

	return Rez[layer_num - 1];
}

void LNeironNet::backtracking(LMatrix inputs, LMatrix answers) {
	if (inputs.m != layer_size[0]) std::cout << "(backtracking)";
	if (answers.m != layer_size[layer_num - 1]) std::cout << "(backtracking)";

	LMatrix result = work(inputs);
	Err[layer_num - 1] = answers - result;

	for(int i = layer_num - 1; i > 0; i--)
	{
		Err[i - 1] = Err[i] * Weits[i - 1].T();
	}

	std::vector<LMatrix> dWeits = Weits;
	LMatrix E;

	for (int i = 0; i < layer_num - 1; i++) {
		E = Rez[i + 1];
		for (int j = 0; j < layer_size[i + 1]; j++) 
		{
			E.data[0][j] = (Err[i + 1].data[0][j]) * Rez[i + 1].data[0][j] * (1 - Rez[i + 1].data[0][j]);
		}

		dWeits[i] = (Rez[i].T() * E);
		Weits[i] = Weits[i] + dWeits[i] * alpha;
	}
}

void LNeironNet::trainFromFile(std::string fileName, int mum, double alpha0) {
	alpha = alpha0;
	std::ifstream fin(fileName);
	int train_num;
	fin >> train_num;
	std::vector<LMatrix> train_data(train_num);
	std::vector<LMatrix> ans_data(train_num);

	for (int i = 0; i < train_num; i++)
	{
		train_data[i].init(1, layer_size[0]);

		for (int j = 0; j < layer_size[0]; j++)
			fin >> train_data[i].data[0][j];
	
		ans_data[i].init(1, layer_size[layer_num - 1]);
		for (int j = 0; j < layer_size[layer_num - 1]; j++)
			fin >> ans_data[i].data[0][j];

	}

	for (int i = 0; i < mum; i++) 
		for (int j = 0; j < train_num; j++)
		{	
			backtracking(train_data[j], ans_data[j]);
		}
		
}

void LNeironNet::print_coeff() {
	for (int k = 0; k < layer_num - 1; k++) {
		for (int i = 0; i < layer_size[k]; i++) {
			for (int j = 0; j < layer_size[k + 1]; j++) {
				std::cout << Weits[k].data[i][j] << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}

void LNeironNet::write_coeff(std::string fileName) {
	std::ofstream fout(fileName);
	for (int i = 0; i < layer_num - 1; i++) {
		for (int j = 0; j < Weits[i].n; j++) {
			for (int k = 0; k < Weits[i].m; k++)
			{
				fout << Weits[i].data[j][k] << " ";
			}
			fout << "\n";
		}
		fout << "\n";
	}
}

void LNeironNet::read_coeff(std::string fileName) {
	std::ifstream fin(fileName);
	for (int i = 0; i < layer_num - 1; i++)
		for (int j = 0; j < Weits[i].n; j++)
			for (int k = 0; k < Weits[i].m; k++)
			{
				fin >> Weits[i].data[j][k];
			}
}