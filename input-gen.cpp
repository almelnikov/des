#include <iostream>
#include <fstream>
#include "generators.hpp"

/*
	Формат файла для генерации:
	Число входящих событий
	N
	V
	lambda - время следующего события
	mu - Expon(mu) + rho - время выполнения события
	rho
*/

/*
	Формат выходного файла:
	N V
	множество строк вида
	"время до прихода следующего события" "время выполнения" n v
*/

int main(int argc, char *argv[])
{
	std::ifstream filein;
	std::ofstream fileout;
	int num, N, V;
	double lambda, mu, rho;

	srand(time(NULL));
	
	if (argc < 3) {
		std::cerr << "Uncorrect command line arguments";
		return -1;
	}
	filein.open(argv[1]);
	fileout.open(argv[2]);
	filein >> num >> N >> V;
	filein >> lambda >> mu >> rho;
	fileout << N << " " << V << "\n";
	for (int i = 0; i < num; i++) {
		fileout << Expon(lambda) << " ";
		fileout << rho + Expon(mu) << " ";
		fileout << Uniform(1, N / 2) << " ";
		fileout << Uniform(V / 3) << "\n";
	}
	return 0;
}
