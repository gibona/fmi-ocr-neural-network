/**
 * @author Damyana Ivanova 80404

 The params of the network are set in the initializeNeuralNetwork method:
 - number of input and output neurons
 - number of hidden layers
 - number of neurons in the hidden layers
 - error tolerance (Quit if error drops lower that this value)
 The learning rate is defined in the method backPropagating.

 */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <string>


#define MAXINPUT 1000
#define MAXOUTPUT 100
#define MAXHIDDEN 10
#define MAXNEURONS 1000
using namespace std;

int N;
int hits = 0;
int numberInput;
int numberLayers; // number of layers + input and output layer
int numberOutput;
int numberLNeurons; // numer neurons in the hidden layers

double input[MAXINPUT];
double output[MAXOUTPUT];
double hidden[MAXHIDDEN][MAXNEURONS];
double result[MAXOUTPUT];
double error[MAXHIDDEN][MAXNEURONS];
char alphabet[MAXOUTPUT];

double weigths[MAXHIDDEN][MAXNEURONS][MAXNEURONS]; // weigths between layers
double errorTolerant; // Quit if error drops this low

void initializeNeuralNetwork(int layers, int input, int output, int neurons, double tolerant){
    numberLayers = layers;
    numberInput = input * input;
    numberOutput = output;
    numberLNeurons = neurons;
    errorTolerant = tolerant;

    int j = 0;
    for(int i = '0'; i <= '9'; i++){
        alphabet[j++] = i;
    }
    for(char i = 'a'; i <= 'z'; i++){
        alphabet[j++] = i;
    }
    for(char i = 'A'; i <= 'Z'; i++){
        alphabet[j++] = i;
    }
}

 double activationFunction( double x){
    //double f = ( double) exp(-x);
    //f = ( double) (1.0 - f) / ( double) (1.0 + f);
    return double (1 / (1 + exp((double) (-1) * x)));
   }

 double derivative( double x){
    return ( double) x * (1 - x);
}

 double calculateError(){
     double sum = 0.0;
    for(int i = 0; i < numberOutput; i++){
        sum += (result[i] - output[i]) * (result[i] - output[i]);
    }
    return sum * 0.5;
}

void calculateErrOutput(){
    for(int j = 0; j < numberOutput; j++){
       error[numberLayers - 2][j] = (result[j] - output[j]) * derivative(output[j]);
       //cout<<error[numberLayers - 2][j]<<"  \n";
    }
   }

void calculateErrHidden(){
    double sumErr;

    for(int i = numberLayers - 2; i > 0; i--){
        for(int j = 0; j < numberLNeurons; j++){
            sumErr = 0.0;
            for(int k = 0; k < numberLNeurons; k++){
                sumErr += error[i][k] * weigths[i - 1][j][k];
            }
            error[i - 1][j] = sumErr * derivative(hidden[i - 1][j]);
            //cout<<error[i - 1][j]<<"  ";
        }
        //cout<<"\n";
    }
}

 double randomWeight(double x) {
        //return (( double) (( double) (rand() % 1000) / 1000) / 10) ;
    double random = double (rand() % 1000) / 1000;  // [0; 1]
	random = random * 2 * x - x;
	return random;
   }

void assignWeigths(){
    for(int i = 0; i < numberLayers - 1; i++) {
        for(int j = 0; j < numberLNeurons; j++) {
            for(int k = 0; k < numberLNeurons; k++) {
                weigths[i][j][k] = randomWeight(0.05);
                //cout<< weigths[i][j][k]<<" ";
            }
            //cout<<"\n";
        }
    }
}

void printWeigths(const char* str){
    FILE* print = fopen(str, "w");
    for(int i = 0; i < numberLayers - 1; i++) {
        for(int j = 0; j < numberLNeurons; j++) {
            for(int k = 0; k < numberLNeurons; k++) {
               fprintf(print, "%lf  ", weigths[i][j][k]);
            }
            fprintf(print, "\n");
        }
        fprintf(print, "\n\n\n");
    }
}

void toPercents()
{
	double sum = 0.0;
	for (int i = 0; i < numberOutput; i++)
	{
		sum += output[i];
	}
	for (int i = 0; i < numberOutput; i++)
	{
		output[i] = output[i]/sum;
	}
}

void feedForward(){
	 double sum;

	for(int i = 0; i < numberLNeurons; i++){
        sum = 0.0;
        for(int j = 0; j < numberInput; j++){
            sum += weigths[0][j][i] * input[j];
            //cout<<"Sum hid : "<< sum<<"\n";
        }
        hidden[0][i] = activationFunction(sum);
    }

	for (int i = 1; i < numberLayers - 2; i++){
		for (int j = 0; j < numberLNeurons; j++){
			sum = 0.0;
            for (int k = 0; k < numberLNeurons; k++){
				sum += weigths[i][k][j] * hidden[i - 1][k];
			}
			hidden[i][j] = activationFunction(sum);
			// fprintf(debug, "sum: %lf, f: %lf\n", sum, l[k + 1][i]);
		}
	}

	for(int i = 0; i < numberOutput; i++){
        sum = 0.0;
        for(int j = 0; j < numberLNeurons; j++){
            sum += weigths[numberLayers - 2][j][i] * hidden[numberLayers - 3][j];
            //cout<<"Sum hid : "<< sum<<"\n";
        }
        output[i] = activationFunction(sum);
    }
}

void backPropagating(){
    calculateErrOutput();
    calculateErrHidden();
    double alpha = 0.3;

    for(int j = 0; j < numberLNeurons; j++){
        for(int k = 0; k < numberInput; k++){
            weigths[0][k][j] += alpha * error[0][j] * input[k];
            }
        }

    for(int i = 1; i < numberLayers - 1; i++){
        for(int j = 0; j < numberLNeurons; j++){
            for(int k = 0; k < numberLNeurons; k++){
                weigths[i][j][k] += alpha * error[i][k] * hidden[i - 1][j];
                //cout<<"e "<<error[i][k]<<"  err  "<<err<<"  h  "<<  hidden[i - 1][j]<<" alpha  "<<alpha;
                //cout<<"e "<<error[i][k]<< " h  "<<  hidden[i - 1][j];

                //cout<<"\n"<<" alpha  "<<alpha<<"\n";
            }
        }
    }
}

void readInput(FILE* file)
{
    int number;
	char res;
	int in = sqrt(numberInput);
	for (int i = 0; i < in; i++){
		for(int j = 0; j < in; j++){
			fscanf(file, "%lf%c", &input[i * in + j], &res);
			//cout<<input[i * in + j]<<"   ";
		}
	}

        if(fscanf(file, "%d", &number) != 1)
            fscanf(file, "%c", &res);
        else{
            itoa(number, &res, 10);
            //cout<<res<<"= res \n";
        }
        for(int i = 0; i < numberOutput; i++){
            if(res == alphabet[i]){
                result[i] = 1;
            }
            else{
                result[i] = 0;
            }
        }
}

double rescale(double x){
    return ((double) x - 127.5) / 127.5;
}

void trainNetwork(int trainings){
    FILE* file = fopen("train_digits.txt", "r");
    for(int t = 0; t < trainings; t++){
        rewind(file);
        int number;
        char res;
        long double error;
        fscanf(file, "%d", &N);
        cout<<"\n N = "<<N<<"\n";
            while(N > 0){
            int k = 0;

            for(int l = 0; l < 29; l++){
                for(int m = 0; m < 29; m++){
                    fscanf(file, "%d", &number);
                    input[k++] = rescale(number);
                    /*if (number == 0)
                        cout<<number<<"   ";
                    else
                        cout<<number<<" ";*/
                    }
                //cout<<"\n";
            }

            if(fscanf(file, "%d", &number) != 1)
                fscanf(file, "%c", &res);
            else{
                itoa(number, &res, 10);
                //cout<<res<<"= res \n";
            }
            //cout<<"\n";
           // cout<<res<<"= res \n";
            for(int i = 0; i < numberOutput; i++){
                if(res == alphabet[i]){
                    result[i] = 1;
                }
                else{
                    result[i] = -1;
                }
            }
            /*cout<<"\n";
            for(int i = 0; i < numberOutput; i++){
                cout<<result[i]<<" ";
            }
            cout<<"\n";*/
                feedForward();
                /*
                cout<<"\n\n Output: ";
                for(int i = 0; i < numberOutput; i++){
                    cout<<output[i]<<" ";
                }*/
                //cout<<"\n\n";
                backPropagating();

                error = calculateError();
                //cout<<"Error : "<<error<<"\n";
                if(error <= errorTolerant){
                cout<<"Trained";
                    break;
                }
                else{
                    N--;
                }
            }
    }
    fclose(file);
}

bool findResult()
{
    toPercents();
	int hit = 0;
	for (int i = 1; i < numberOutput; i++)
	{
		if (output[i] > output[hit])
		{
			hit = i;
		}
	}

	if (output[hit] > 0.5 && result[hit] == 1)
        return true;
    else
        return false;
}

void guess(){
    FILE * file = fopen("guess.txt", "r");
    int number;
    fscanf(file, "%d", &N);
    while(N > 0){
        int k = 0;
        for(int l = 0; l < 29; l++){
            for(int m = 0; m < 29; m++){
                fscanf(file, "%d", &number);
                input[k++] = number;
            }
        }
        feedForward();
        for(int i = 0; i < numberOutput; i++){
            cout<<output[i]<<" ";
        }
        cout<<"\n\n";
        if(findResult())
            hits++;
        //printweigths();
        N--;
    }
    fclose(file);
}



int main(){
    srand(time(NULL));
    int trainings = 50;
    //FILE* file = fopen("train_0.txt", "r");
    //initializeNeuralNetwork(int layers, int input, int output, int neurons, double alpha, double tolerant)
    initializeNeuralNetwork(4, 29, 10, 1000, 0.001);
    assignWeigths();
    //printweigths();
    trainNetwork(trainings);
    guess();
    cout<<"\n"<<hits<<"\n";
    return 0;
   }
