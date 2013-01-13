// Nikolina Eftimova 80403
/*  Neural Network trained by backpropagation.
	!!! The params of the network (number of neurons in the hidden layer
	and k = number of trainings over the data set) need to be set to appropriate values.
*/

/*
	This example trains the network over the iris.arff data set.
	It learns to correctly classify iris species.
	For this example:
	hiddenNeurons = 100;
	k = 10000;
*/
	


// #include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <string.h>

#define MAXINPUT 100
#define MAXOUTPUT 100
#define MAXHIDDEN 100
using namespace std;

double w1[MAXINPUT][MAXHIDDEN];
double w2[MAXHIDDEN][MAXOUTPUT];
double x[MAXINPUT]; // input
double y[MAXHIDDEN];
double z[MAXOUTPUT]; // network output
double answer[MAXOUTPUT]; // the true answer for the current example
double ey[MAXHIDDEN]; // errors for hidden layer
double ez[MAXOUTPUT]; // errors for output layer
int inputNeurons;
int hiddenNeurons;
int outputNeurons;
int k; // the number of trainings
const double m = 0.1;
int instancesCount; // the number of instances in the training data set
FILE* debug;
const bool inDebug = false;

void initialize()
{
	// task specific values
	inputNeurons = 4;
	outputNeurons = 3;
	instancesCount = 150;

	// params
	hiddenNeurons = 100;
	k = 10000;
}

// values in range (-0.05; 0.05)
void initializeWeights()
{
	for (int i = 0; i < inputNeurons; i++)
	{
		for (int j = 0; j < hiddenNeurons; j++)
		{
			w1[i][j] = (double (double (rand() % 10000) / 10000) / 10) - 0.05;
		}
	}

	for (int i = 0; i < hiddenNeurons; i++)
	{
		for (int j = 0; j < outputNeurons; j++)
		{
			w2[i][j] = (double (double (rand() % 10000) / 10000) / 10) - 0.05;
		}
	}
}

void printWeights(int instanceNumber)
{
	fprintf(debug, "instance N: %d\n", instanceNumber);
	fprintf(debug, "weights 1:\n");
	for (int i = 0; i < inputNeurons; i++)
	{
		for (int j = 0; j < hiddenNeurons; j++)
		{
			fprintf(debug, "%lf ", w1[i][j]);
		}
		fprintf(debug, "\n");
	}

	fprintf(debug, "weights 2:\n");
	for (int i = 0; i < hiddenNeurons; i++)
	{
		for (int j = 0; j < outputNeurons; j++)
		{
			fprintf(debug, "%lf ", w2[i][j]);
		}
		fprintf(debug, "\n");
	}
}

void printOutputLayer()
{
	fprintf(debug, "output layer:\n");
	for (int i = 0; i < outputNeurons; i++)
	{
		fprintf(debug, "%lf ", z[i]);
	}
	fprintf(debug, "\n");

	fprintf(debug, "true answer:\n");
	for (int i = 0; i < outputNeurons; i++)
	{
		fprintf(debug, "%lf ", answer[i]);
	}
	fprintf(debug, "\n");
}

// task specific function
void readInputAndAnswer(FILE* inputFile)
{	
	char comma;
	char className[50];

	for(int i = 0; i < inputNeurons; i++)
	{
		fscanf(inputFile, "%lf%c", &x[i], &comma);
	}
	
	fscanf(inputFile, "%s", className);
	if (strcmp(className, "Iris-setosa") == 0)
	{
		answer[0] = 1;
		answer[1] = 0;
		answer[2] = 0;
	}
	else if (strcmp(className, "Iris-versicolor") == 0)
	{
		answer[0] = 0;
		answer[1] = 1;
		answer[2] = 0;
	}
	else if (strcmp(className, "Iris-virginica") == 0)
	{
		answer[0] = 0;
		answer[1] = 0;
		answer[2] = 1;
	}
	else
	{
		printf("error parsing\n");
	}
}

void calculateActivation()
{
	double sum;
	for (int i = 0; i < hiddenNeurons; i++)
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
	}
}

void calculateError()
{
	double sum;
	for (int i = 0; i < outputNeurons; i++)
	{
		ez[i] = z[i] * (1 - z[i]) * (answer[i] - z[i]);
	}

	for (int i = 0; i < hiddenNeurons; i++)
	{
		sum = 0;
		for (int j = 0; j < outputNeurons; j++)
		{
			sum += w2[i][j] * ez[j];
		}
		ey[i] = y[i] * (1 - y[i]) * sum;
	}
}

void backpropagateError()
{
	for (int i = 0; i < inputNeurons; i++)
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
	}
}

void train(int k)
{
	FILE* file = fopen("iris.arff", "r");
	for (int i = 0; i < k; i++) // k trainings over the same data set
	{
		rewind(file);
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
	}
	fclose(file);
	if (inDebug)
	{
		fprintf(debug, "Final weights:\n");
		printWeights(0);
	}
}

void classify(FILE* resultFile, int* correctAnswers)
{
	fprintf(resultFile, "%lf %lf %lf\n", z[0], z[1], z[2]);
	int classNumber;
	if (answer[0] == 1)
		classNumber = 0;
	else if (answer[1] == 1)
		classNumber = 1;
	else 
		classNumber = 2;
	if (z[classNumber] > 0.9)
	{
		(*correctAnswers)++;
	}
}

/*
	Test the network over the same data set but mixed.
*/
void testClassifying()
{
	FILE* file = fopen("iris_mixed.arff", "r");
	FILE* resultFile = fopen("results_mixed.txt", "w");
	int correctAnswers = 0;
	for (int i = 0; i < instancesCount; i++)  
	{
		readInputAndAnswer(file);
		calculateActivation();
		if (i == 50 || i == 100)
		{
			fprintf(resultFile, "%s\n", "------------------------");
		}
		classify(resultFile, &correctAnswers);
	}
	fclose(file);
	fclose(resultFile);
	printf("Test: all instances = %d, correctly classified: %d\n", instancesCount, correctAnswers);
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
	train(k);
	testClassifying();
	return 0;
}

