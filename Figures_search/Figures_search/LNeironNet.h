#pragma once

#include "LMatrix.h"
#include <vector>

class LNeironNet {
public:

	LNeironNet();
	LNeironNet(int, int*);

	LMatrix work(LMatrix);
	void backtracking(LMatrix, LMatrix);
	void trainFromFile(std::string, int, double);
	void print_coeff();
	void write_coeff(std::string);
	void read_coeff(std::string);

private:
	std::vector<LMatrix> Weits;
	std::vector<LMatrix> Rez;
	std::vector<LMatrix> Err;
	int* layer_size;
	int layer_num;
	double alpha = 0.01;
};