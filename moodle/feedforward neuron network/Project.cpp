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
	- scaling the initial weights and adding biases
	- decreasing the learning rate during training
	- randomizing the order of samples in the training set
	
*/

/*
	This example trains the network over the "all.txt" data set.
	It learns to correctly classify the symbols 0-9, a-z, A-Z.

	Test data: 
*/
	


//#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <map>
#include <string>

#define MAXOUTPUT 100
#define MAXLAYERS 5
#define MAXNEURONS 5000
using namespace std;

double w[MAXLAYERS - 1][MAXNEURONS][MAXNEURONS]; //weights
double b[MAXLAYERS - 1][MAXNEURONS]; // biases
double l[MAXLAYERS][MAXNEURONS]; // neurons values
double e[MAXLAYERS - 1][MAXNEURONS]; // errors
int neurons[MAXLAYERS]; // neurons counts

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
	neurons[1] = neurons[2] = 500;*/
	layersCount = 3;
	neurons[1] = 1000;
	epochs = 50;
	//instancesCount = 120;
	learningRate = 0.02;
	
	// task specific values
	inputMatrixSize = 29;
	neurons[0] = inputMatrixSize * inputMatrixSize; // 841
	neurons[layersCount - 1] = 62;
	fontsCount = 12;
	instancesCount = fontsCount * neurons[layersCount - 1]; 	

	sprintf(weightsFileName, "./output/weights_%d_%d_inst%d.txt", layersCount - 2, neurons[1], instancesCount);
	sprintf(resultFileName, "./output/results_%d_%d_e%d_m%lf_inst%d.txt", layersCount - 2, neurons[1], epochs, learningRate, instancesCount);
	resultFile = fopen(resultFileName, "w");
	char debugFileName[100];
	sprintf(debugFileName, "./output/debug_%d_%d_e%d_m%lf_inst%d.txt", layersCount - 2, neurons[1], epochs, learningRate, instancesCount);
	debug = fopen(debugFileName, "w");

	fillSymbolsToProbabilitiesMap();
}

// values in range (-x; x)
double randInRange(double x)
{
	double r = double (rand() % 10000) / 10000;  // [0; 1]
	r = r * 2 * x - x;
	return r;
	// return (double (double (rand() % 100000) / 100000) * 2 * x) - x;
}

// values in range (-range; range)
void initializeWeights(double range)
{
	for (int layer = 0; layer < layersCount - 1; layer++)
	{
		for (int i = 0; i < neurons[layer]; i++)
		{
			for (int j = 0; j < neurons[layer + 1]; j++)
			{
				w[layer][i][j] = randInRange(range);
			}
		}
	}
}

void initializeScaledWeights()
{
	initializeWeights(1);

	// rescale weights  
	double magnitude = 0.7 * pow((double) neurons[1], (double) 1 / neurons[0]);
	double oldMagn, rescaleFactor;

	for (int i = 0; i < neurons[1]; i++)
	{
		oldMagn = 0;
		for (int j = 0; j < neurons[0]; j++)
		{
			oldMagn += w[0][j][i] * w[0][j][i];
		}
		oldMagn = sqrt(oldMagn);
		rescaleFactor = magnitude / oldMagn;

		for (int j = 0; j < neurons[0]; j++)
		{
			w[0][j][i] *= rescaleFactor; 
		}
	}

	// biases in hidden layer 1 in range [-magnitude; magnitude] 
	for (int i = 0; i < neurons[1]; i++)
	{
		b[0][i] = randInRange(magnitude);
	}

	for (int layer = 1; layer < layersCount - 1; layer++)
	{
		for (int i = 0; i < neurons[layer]; i++)
		{	
			b[layer][i] = 0;
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

	for (int i = 0; i < neurons[1]; i++)
	{
		fprintf(file, "%lf ", b[0][i]);
	}
}

void printOutputLayer()
{
	
}

// example: scaleInput("./input/all.txt");

void scaleInput(char* fileName)
{
	FILE* inputFile = fopen(fileName, "r");
	char scaledInputFileName[100];
	char* lastSlash = strrchr(fileName, '/');
	sprintf(scaledInputFileName, "./input/scaled_%s", lastSlash + 1);
	FILE* scaledInputFile = fopen(scaledInputFileName, "w");
	char symbol;
	double inputValue;
	int n;

	fscanf(inputFile, "%d", &n);
	fprintf(scaledInputFile, "%d\n", n);

	for (int k = 0; k < n; k++)
	{
	    for (int i = 0; i < inputMatrixSize; i++)
	    {
		    for(int j = 0; j < inputMatrixSize; j++)
		    {
			    fscanf(inputFile, "%lf", &inputValue);
			    inputValue /= 127.5;
			    inputValue -= 1;
			    fprintf(scaledInputFile, "%lf ", inputValue);
		    }
		    fprintf(scaledInputFile, "\n"); 
	    }

	    do 
	    {
		    fscanf(inputFile, "%c", &symbol);
	    }
	    while (symbols.count(symbol) == 0);
	    fprintf(scaledInputFile, "%c\n", symbol);
	}

	fclose(inputFile);
	fclose(scaledInputFile);
}

// task specific function
void readInputAndAnswer(FILE* inputFile)
{	
	char symbol;
	for (int i = 0; i < inputMatrixSize; i++)
	{
		for(int j = 0; j < inputMatrixSize; j++)
		{
			fscanf(inputFile, "%lf%c", &l[0][i * inputMatrixSize + j], &symbol);
		}
	}

	do 
	{
		fscanf(inputFile, "%c", &symbol);
	}
	while (symbols.count(symbol) == 0);
	answerSymbol = symbol;
	output o = symbols.find(answerSymbol)->second;
	for (int i = 0; i < neurons[layersCount - 1]; i++)
	{
		answer[i] = o.probabilities[i];
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
			// sum = b[k][i]; 
			for (int j = 0; j < neurons[k]; j++)
			{
				sum += w[k][j][i] * l[k][j];
			}

			l[k + 1][i] = sigmoid(sum);
			// fprintf(debug, "sum: %lf, f: %lf\n", sum, l[k + 1][i]);
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

	/*for (int i = 0; i < neurons[1]; i++)
	{
		b[0][i] += learningRate * e[0][i] * 1;
	}
	*/
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
	FILE* file = fopen("./input/scaled_all.txt", "r");
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

		/*if (i != 0 && i % 20 == 0)
		{
			testClassifying("./input/scaled_all.txt", false, instancesCount);
		}*/
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

	for (int i = 0; i < neurons[1]; i++)
	{
		fscanf(file, "%lf ", &b[0][i]);
	}

	for (int layer = 1; layer < layersCount - 1; layer++)
	{
		for (int i = 0; i < neurons[layer]; i++)
		{	
			b[layer][i] = 0;
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

	if (l[layersCount - 1][greatestProbabilityIdx] > 0.5 &&
			greatestProbabilityIdx == symbols.find(answerSymbol)->second.index)
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

//int main()
//{
//	srand(time(NULL));
//	if (inDebug)
//	{
//		debug = fopen("debug.txt", "w");
//	}
//	
//	initialize();
//	//initializeWeights(0.05);
//	//initializeScaledWeights();
//	loadWeights();
//	train(epochs);
//	return 0;
//}

//int main()
//{
//	initialize();
//	//scaleInput("./input/all.txt");
//	/*scaleInput("./input/arial.png.txt");
//	scaleInput("./input/comic-sans-ms.png.txt");
//	scaleInput("./input/courier-new.png.txt");
//	scaleInput("./input/georgia.png.txt");*/
//	scaleInput("./input/impact.png.txt");
//	scaleInput("./input/lucida-console.png.txt");
//	//scaleInput("./input/helvetica.txt");
//}

int main()
{
	initialize();
	loadWeights();

	resultFile = fopen("./output/comic-sans-ms.png.txt", "w");
	testClassifying("./input/scaled_comic-sans-ms.png.txt", false);
	fclose(resultFile);

	resultFile = fopen("./output/courier-new.png.txt", "w");
	testClassifying("./input/scaled_courier-new.png.txt", false);
	fclose(resultFile);

	resultFile = fopen("./output/georgia.png.txt", "w");
	testClassifying("./input/scaled_georgia.png.txt", false);
	fclose(resultFile);

	resultFile = fopen("./output/impact.png.txt", "w");
	testClassifying("./input/scaled_impact.png.txt", false);
	fclose(resultFile);

	resultFile = fopen("./output/lucida-console.png.txt", "w");
	testClassifying("./input/scaled_lucida-console.png.txt", false);
	fclose(resultFile);
	return 0;
}
