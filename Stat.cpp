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

double AvrgResoures(std::vector<double> &res, std::vector<double> &time)
{
	double sum = 0.0;
	double prev_time = 0.0;
	for (size_t i = 0; i < time.size(); i++) {
		sum += res[i] * (time[i] - prev_time);
		prev_time = time[i];
	}
	return sum / time.back();
}

void Stat(const char *filename1, const char *filename2, std::ifstream &stream)
{
	std::vector<double> data_time;
	std::vector<double> data_mem;
	std::vector<double> data_core;
	std::vector<double> data_res_time;
	
	stream.open(filename1);
	double value = 0.0;
	double time, mem, core;
	while (!stream.eof()){
		stream >> value;
		data_time.push_back(value);
	}
	std::cout << MeanValue(data_time) << " ";
	stream.close();
	
	stream.open(filename2);
	while (stream >> time >> mem >> core) {
		data_res_time.push_back(time);
		data_mem.push_back(mem);
		data_core.push_back(core);
	}
	stream.close();
	std::cout << AvrgResoures(data_mem, data_res_time) << " "
			  << AvrgResoures(data_core, data_res_time) << "\n";
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
