#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#define Population 500
#define Iteration 100
#define BitLength 20
#define CrossvoerRate 0.8
#define MutationRate 0.01
using namespace std;

vector<string> chromosome(Population);
vector<double> maxData;
vector<double> averageData;

string _makeChromosome() {
	string bitStream("");
	for (int i = 0; i < BitLength; i++) {
		bitStream += ((rand() % 2) + '0');
	}

	return bitStream;
}

double _getValue(string chro) {
	double x(0.0), flag(0.5);

	if (chro[1] == '1') x += 3.0;
	if (chro[2] == '1') x += 2.0;
	if (chro[3] == '1') x += 1.0;

	for (int i = 4; i < BitLength; i++) {
		if (chro[i] == '1') x += flag;

		if (i < BitLength - 2) flag /= 2.0;
	}

	if (chro[0] == '1') x *= -1.0;

	return x;
}

double _getFitness(string chro) {
	double x, fitness;
	x = _getValue(chro);

	fitness = pow(x, 5.0) - 4.0 * pow(x, 4.0) - 12.0 * pow(x, 3.0) + 34.0 * pow(x, 2.0) + 11.0 * x - 30.0;

	return fitness;
}

double _getAverageFitness(vector<string> chromosome) {
	double sum_(0.0), average_;

	for (int i = 0; i < Population; i++) {
		sum_ += _getFitness(chromosome[i]);
	}

	average_ = sum_ / (double)Population;

	return average_;
}

vector<string> _sort(vector<string> before) {
	vector<pair<double, string>> after_tmp(Population);
	vector<string> after(Population);

	for (int i = 0; i < Population; i++) {
		after_tmp[i].first = _getFitness(before[i]);
		after_tmp[i].second = before[i];
	}

	sort(after_tmp.begin(), after_tmp.end());

	for (int i = 0; i < Population; i++) {
		after[i] = after_tmp[i].second;
	}

	return after;
}

vector<double> _makeRoulette(vector<string> chromosome) {
	vector<double> roulette(Population);
	double minFitness = (double)INT_MAX;

	for (int i = 0; i < Population; i++) {
		roulette[i] = _getFitness(chromosome[i]);
		if (roulette[i] < minFitness) minFitness = roulette[i];
	}

	if (minFitness >= 0.0) return roulette;

	for (int i = 0; i < Population; i++) {
		roulette[i] -= minFitness;
	}

	return roulette;
}

int _selection(vector<double> roulette) {
	double fitnessSum(0.0), flag(0.0);
	int index(Population - 1);

	for (int i = 0; i < Population; i++) {
		fitnessSum += roulette[i];
	}

	for (int i = 0; i < Population; i++) {
		roulette[i] = (roulette[i] / fitnessSum) * 100.0;
	}

	int randVal = rand() % 99 + 1;

	for (int i = 0; i < Population; i++) {
		flag += roulette[i];
		if (flag >= (double)randVal) {
			index = i;
			break;
		}
	}

	return index;
}

string _mutation(string chro) {
	string mutateChro = chro;

	for (int i = 0; i < BitLength; i++) {
		float randVal = rand() / (float)RAND_MAX * 1.0f;
		if (randVal <= MutationRate) {
			if (mutateChro[i] == '0') mutateChro[i] = '1';
			else mutateChro[i] = '0';
		}
	}

	return mutateChro;
}

pair<string, string> _crossover(string newChro1, string newChro2) {
	pair<string, string> newChromo;
	string newChro_tmp1, newChro_tmp2;

	newChro_tmp1 = newChro1.substr(0, 7) + newChro2.substr(7, 7) + newChro1.substr(14, 6);
	newChro_tmp2 = newChro2.substr(0, 7) + newChro1.substr(7, 7) + newChro2.substr(14, 6);

	newChro_tmp1 = _mutation(newChro_tmp1);
	newChro_tmp2 = _mutation(newChro_tmp2);
	newChromo.first = newChro_tmp1;
	newChromo.second = newChro_tmp2;

	return newChromo;
}

vector<string> _nextGeneration(vector<string> nowGeneration) {
	vector<double> roulette = _makeRoulette(nowGeneration);
	vector<string> nextGeneration(Population);
	int cnt;

	for (int i = 0; i < Population * (1 - CrossvoerRate);) {
		int index1 = _selection(roulette);
		int index2 = _selection(roulette);
		if (index1 == index2) continue;

		pair<string, string> newChro = _crossover(nowGeneration[index1], nowGeneration[index2]);
		nextGeneration[i++] = newChro.first;
		nextGeneration[i++] = newChro.second;
		cnt = i;
	}

	for (int i = cnt; i < Population; i++) {
		nextGeneration[i] = nowGeneration[Population - (i - cnt + 1)];
	}

	nextGeneration = _sort(nextGeneration);

	return nextGeneration;
}

void _print(int generation, vector<string> chromosome) {
	cout << "Generation : " << generation << '\n';
	cout << "Max Value : " << _getValue(chromosome[Population - 1]) << '\n';
	cout << "Best Fiteness : " << _getFitness(chromosome[Population - 1]) << '\n';
	cout << "Average Fiteness : " << _getAverageFitness(chromosome) << '\n' << '\n';
	maxData.push_back(_getFitness(chromosome[Population - 1]));
	averageData.push_back(_getAverageFitness(chromosome));
}

void _makeFile() {
	string filePath = "test.txt";
	ofstream writeFile(filePath.data());

	if (writeFile.is_open()) {
		for (int i = 0; i <= Iteration; i++) {
			writeFile << i << " " << maxData[i] << " " << averageData[i] << '\n';
		}
		writeFile.close();
	}
}

int main() {
	srand((unsigned int)time(NULL));

	for (int i = 0; i < Population; i++) {
		chromosome[i] = _makeChromosome();
	}

	chromosome = _sort(chromosome);
	_print(0, chromosome);

	for (int Generation = 1; Generation <= Iteration; Generation++) {
		chromosome = _nextGeneration(chromosome);
		_print(Generation, chromosome);
	}

	_makeFile();

	return 0;
}
