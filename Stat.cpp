#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>

double MeanValue(std::vector<double> &dvec)
{
	double sum = 0.0;
	for (auto item : dvec)
		sum += item;
	return sum / static_cast<int>(dvec.size());
}

void Stat(const char *filename1, const char *filename2, std::ifstream &stream)
{
	std::vector<double> data_time;
	std::vector<double> data_mem;
	std::vector<double> data_core;
	
	stream.open(filename1);
	double value = 0.0;
	while (!stream.eof()){
		stream >> value;
		data_time.push_back(value);
	}
	std::cout << MeanValue(data_time) << " ";
	stream.close();
	
	stream.open(filename2);
	while(!stream.eof()){
		stream >> value;
		data_mem.push_back(value);
		stream >> value;
		data_core.push_back(value);
	}
	stream.close();
	std::cout << MeanValue(data_mem) << " " << MeanValue(data_core);
}

int main(int argc, char *argv[])
{
	std::ifstream filein;
	if (argc < 3)
		return -1;
	std::cout << "mean_value, mem, core\n";
	Stat(argv[1], argv[2], filein);
	std::cout << "\n";
	return 0;
}
