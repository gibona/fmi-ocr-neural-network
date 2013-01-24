/*  Feedforward Neural Network trained by backpropagation.

	!!! The params of the network need to be set to appropriate values:

	- number of hidden layers;
	- number of neurons in each hidden layer;
	- epochs = number of trainings over the data set;

	
	Other values and functions to be specified:
	- the activation funcion (sigmoid, tanh)
	- weights ranges
	- the learning rate - in updating the weights 
	- criteria for correct classification

	Improving network performance:
	- re-scaling the input (for example: in [-1, 1])
	- decreasing the learning rate during training
	- randomizing the order of samples in the training set
	
*/

/*
	This example trains the network over the "all.txt" data set.
	It learns to correctly classify the symbols 0-9, a-z, A-Z.

	Test data: 
*/
	


#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <map>
#include <string>

#define MAXOUTPUT 100
#define MAXLAYERS 10
#define MAXNEURONS 5000
using namespace std;

double w[MAXLAYERS - 1][MAXNEURONS][MAXNEURONS];
double l[MAXLAYERS][MAXNEURONS];
double e[MAXLAYERS - 1][MAXNEURONS];
int neurons[MAXLAYERS];

double answer[MAXOUTPUT]; // the true answer for the current example
char answerSymbol;
int inputMatrixSize;
int layersCount;
const int OUTPUT = 62;
int fontsCount;
int epochs; // the number of trainings
double learningRate;
int instancesCount; // the number of instances in the training data set

char weightsFileName[100];
char resultFileName[100];
FILE* resultFile;
FILE* debug;
const bool inDebug = false;

struct output
{
	double probabilities[OUTPUT];
	int index;
	output() {}
	output(int i)
	{
		index = i;
		for (int i = 0; i < OUTPUT; i++)
		{
			probabilities[i] = 0;
		}
		probabilities[index] = 1;
	}
};
typedef struct output output;
map<char, output> symbols;

void testClassifying(char* fileName, bool print, int);

void fillSymbolsToProbabilitiesMap()
{	
	// insert 0-9, a-z, A-Z
	char asciiCode = 48;
	int i = 0;
	while (asciiCode < 58)
	{
		symbols.insert(pair<char, output>(asciiCode, output(i)));
		i++;
		asciiCode++;
	}

	asciiCode = 97;
	while (asciiCode < 123)
	{
		symbols.insert(pair<char, output>(asciiCode, output(i)));
		i++;
		asciiCode++;
	}

	asciiCode = 65;
	while (asciiCode < 91)
	{
		symbols.insert(pair<char, output>(asciiCode, output(i)));
		i++;
		asciiCode++;
	}
}

void initialize()
{
	// params
	/*layersCount = 5;
	neurons[1] = neurons[2] = neurons[3] = 1000;*/
	/*layersCount = 4;
	neurons[1] = neurons[2] = 100;*/
	layersCount = 3;
	neurons[1] = 1;
	epochs = 1;
	instancesCount = 120;
	learningRate = 0.1;
	
	// task specific values
	inputMatrixSize = 29;
	neurons[0] = inputMatrixSize * inputMatrixSize; // 841
	neurons[layersCount - 1] = 62;
	fontsCount = 12;
	//instancesCount = fontsCount * outputNeurons; 	

	sprintf(weightsFileName, "./output/weights_%d_%d.txt", layersCount - 2, neurons[1]);
	sprintf(resultFileName, "./output/results_%d_%d_e%d_m%lf.txt", layersCount - 2, neurons[1], epochs, learningRate);
	resultFile = fopen(resultFileName, "w");

	fillSymbolsToProbabilitiesMap();
}

// values in range (-0.05; 0.05)
void initializeWeights()
{
	for (int layer = 0; layer < layersCount - 1; layer++)
	{
		for (int i = 0; i < neurons[layer]; i++)
		{
			for (int j = 0; j < neurons[layer + 1]; j++)
			{
				w[layer][i][j] = (double (double (rand() % 10000) / 10000) / 10) - 0.05;
			}
		}
	}
}
void printWeights(int i) {}

void saveWeights()
{
	FILE* file = fopen(weightsFileName, "w");
	for (int layer = 0; layer < layersCount - 1; layer++)
	{
		for (int i = 0; i < neurons[layer]; i++)
		{
			for (int j = 0; j < neurons[layer + 1]; j++)
			{
				fprintf(file, "%lf ", w[layer][i][j]);
			}
			fprintf(file, "\n");
		}
	}
}

void printOutputLayer()
{
	
}

// task specific function
void readInputAndAnswer(FILE* inputFile)
{	
	char space, symbol;
	for (int j = 0; j < inputMatrixSize; j++)
	{
		for(int j = 0; j < inputMatrixSize; j++)
		{
			fscanf(inputFile, "%lf%c", &l[0][j], &space);
		}
	}

	do 
	{
		fscanf(inputFile, "%c", &symbol);
	}
	while (symbols.count(symbol) == 0);
	answerSymbol = symbol;
	output o = symbols.find(answerSymbol)->second;
	for (int j = 0; j < neurons[layersCount - 1]; j++)
	{
		answer[j] = o.probabilities[j];
	}
}

// activation functions

double sigmoid(double x)
{
	return double (1 / (1 + exp((double) (-1) * x)));
}

// tanh(x) from <math.h>


void calculateActivation()
{
	double sum;
	for (int k = 0; k < layersCount - 1; k++)
	{
		for (int i = 0; i < neurons[k + 1]; i++)
		{
			sum = 0;
			for (int j = 0; j < neurons[k]; j++)
			{
				sum += w[k][j][i] * l[k][j];
			}

			l[k + 1][i] = sigmoid(sum);
		}
	}
	
	/*for (int i = 0; i < hiddenNeurons; i++)
	{
		sum = 0;
		for (int j = 0; j < inputNeurons; j++)
		{
			sum += w1[j][i] * x[j];
		}

		y[i] = double (1 / (1 + exp((double) (-1) * sum)));
	}

	for (int i = 0; i < outputNeurons; i++)
	{
		sum = 0;
		for (int j = 0; j < hiddenNeurons; j++)
		{
			sum += w2[j][i] * y[j];
		}

		z[i] = double (1 / (1 + exp((double) (-1) * sum)));
	}*/
}

void calculateError()
{
	double sum;
	for (int i = 0; i < neurons[layersCount - 1]; i++)
	{
		e[layersCount - 2][i] = l[layersCount - 1][i] * (1 - l[layersCount - 1][i]) * (answer[i] - l[layersCount - 1][i]);
	}

	for (int k = layersCount - 2; k > 0; k--)
	{
		for (int i = 0; i < neurons[k]; i++)
		{
			sum = 0;
			for (int j = 0; j < neurons[k + 1]; j++)
			{
				sum += w[k][i][j] * e[k][j];
			}
			e[k - 1][i] = l[k][i] * (1 - l[k][i]) * sum;
		}
	}
	/*for (int i = 0; i < hiddenNeurons; i++)
	{
		sum = 0;
		for (int j = 0; j < outputNeurons; j++)
		{
			sum += w2[i][j] * ez[j];
		}
		ey[i] = y[i] * (1 - y[i]) * sum;
	}*/
}

void backpropagateError()
{
	for (int k = 0; k < layersCount - 1; k++)
	{
		for (int i = 0; i < neurons[k]; i++)
		{
			for (int j = 0; j < neurons[k + 1]; j++)
			{
				w[k][i][j] += learningRate * e[k][j] * l[k][i];
			}
		}
	}
	
	/*for (int i = 0; i < inputNeurons; i++)
	{
		for (int j = 0; j < hiddenNeurons; j++)
		{
			w1[i][j] += m * ey[j] * x[i];
		}
	}

	for (int i = 0; i < hiddenNeurons; i++)
	{
		for (int j = 0; j < outputNeurons; j++)
		{
			w2[i][j] += m * ez[j] * y[i];
		}
	}*/
}

void decreaseLearningRate()
{
	learningRate -= 0.01;
}

void train(int epochs)
{
	FILE* file = fopen("./input/all.txt", "r");
	for (int i = 0; i < epochs; i++) // do trainings over the same data set
	{
		if (i % 50 == 0)
		{
			printf("Training: epoch %d\n", i); 
		}
		rewind(file);
		int n;
		fscanf(file, "%d", &n);
		for (int j = 0; j < instancesCount; j++)  // processing of each instance in the data set
		{
			if (inDebug)
			{
				printWeights(j);
			}
			readInputAndAnswer(file);
			calculateActivation();
			if (inDebug)
			{
				printOutputLayer();
			}
			calculateError();
			backpropagateError();
		}

		if (i % 20 == 0)
		{
			testClassifying("./input/all.txt", false, instancesCount);
		}
	}
	fclose(file);
	saveWeights();
}


void loadWeights()
{
	FILE* file = fopen(weightsFileName, "r");
	for (int layer = 0; layer < layersCount - 1; layer++)
	{
		for (int i = 0; i < neurons[layer]; i++)
		{
			for (int j = 0; j < neurons[layer + 1]; j++)
			{
				fscanf(file, "%lf ", &w[layer][i][j]);
			}
		}
	}
}

void normalizeOutput()
{
	double sum = 0;
	for (int i = 0; i < neurons[layersCount - 1]; i++)
	{
		sum += l[layersCount - 1][i];
	}
	for (int i = 0; i < neurons[layersCount - 1]; i++)
	{
		l[layersCount - 1][i] /= sum; 
	}
}

bool satisfiesCriteria()
{
	int greatestProbabilityIdx = 0;
	for (int i = 0; i < neurons[layersCount - 1]; i++)
	{
		if (l[layersCount - 1][i] > l[layersCount - 1][greatestProbabilityIdx])
		{
			greatestProbabilityIdx = i;
		}
	}

	/*if (z[greatestProbabilityIdx] > 0.5 && 
			greatestProbabilityIdx == symbols.find(answerSymbol)->second.index)
			return true;*/
	if (greatestProbabilityIdx == symbols.find(answerSymbol)->second.index)
			return true;

	return false;
}

void classify(FILE* resultFile, int* correctAnswers, bool printProbabilities)
{
	normalizeOutput();
	if (printProbabilities) 
	{
		for (int i = 0; i < neurons[layersCount - 1]; i++)
		{
			fprintf(resultFile, "%lf ", l[layersCount - 1][i]);
		}
		fprintf(resultFile, "\n");
	}	
	
	if (satisfiesCriteria())
	{
		(*correctAnswers)++;
	}
}

/*
	Test the network over some data set.
*/
void testClassifying(char* fileName, bool printProbabilities, int testInstancesCount = 0)
{
	//loadWeights();
	FILE* file = fopen(fileName, "r");
	
	int correctAnswers = 0;
	int n;
	fscanf(file, "%d", &n);

	if (testInstancesCount == 0)
	{
		testInstancesCount = n;
	}
	
	for (int i = 0; i < testInstancesCount; i++)  
	{
		readInputAndAnswer(file);
		calculateActivation();
		if (printProbabilities && i % fontsCount == 0)
		{
			fprintf(resultFile, "%s%c\n", "------------------------", answerSymbol);
		}
		classify(resultFile, &correctAnswers, printProbabilities);
	}
	fclose(file);
	fprintf(resultFile, "Test: all instances = %d, correctly classified: %d\n", testInstancesCount, correctAnswers);
	fprintf(resultFile, "Hit rate: %lf %\n", (correctAnswers / (double) testInstancesCount) * 100);
}

int main()
{
	srand(time(NULL));
	if (inDebug)
	{
		debug = fopen("debug.txt", "w");
	}
	initialize();
	initializeWeights();
	//loadWeights();
	train(epochs);
	testClassifying("./input/all.txt", true, instancesCount);
	return 0;
}

//int main()
//{
//	initialize();
//	testClassifying();
//	return 0;
//}
