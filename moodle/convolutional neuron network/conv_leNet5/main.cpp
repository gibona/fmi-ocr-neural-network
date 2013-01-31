#include <cstdio>
#include <ctime>
#include <cstdlib>
//#include <cctype>
#include "leNet.h"

#define LEARN_THRESHOLD 0.001
#define LEARN_MULT 0.3

void CreateRandomNN_Text(const char* name, int primFeatures, int secFeatures, int fullNodes, int finalNodes)
{
	srand(time(0));
	double x;
	FILE* f = fopen(name, "w");
	fprintf(f, "%d %d %d %d\n", primFeatures, secFeatures, fullNodes, finalNodes);
	int i, l = 26*primFeatures + (25*primFeatures+1)*secFeatures + (25*secFeatures+1)*fullNodes + finalNodes*(fullNodes+1);
	for (i=0;i<l;++i)
	{
		x = (double)(rand() % 10000) / 9999.0f;
		fprintf(f, "%lf ", (2.0 * x - 1.0)*0.05);
	}
	fclose(f);
}

void CreateRandomNN_Bin(const char* path, int primFeatures, int secFeatures, int fullNodes, int finalNodes)
{
	srand(time(0));
	double x;
	FILE* f = fopen(path, "wb");
	fwrite(&primFeatures, sizeof(int), 1, f);
	fwrite(&secFeatures, sizeof(int), 1, f);
	fwrite(&fullNodes, sizeof(int), 1, f);
	fwrite(&finalNodes, sizeof(int), 1, f);
	int i, l = 26*primFeatures + (25*primFeatures+1)*secFeatures + (25*secFeatures+1)*fullNodes + finalNodes*(fullNodes+1);
	for (i=0;i<l;++i)
	{
		x = (double)(rand() % 10000) / 9999.0f;
		x =  (2.0 * x - 1.0)*0.05;
		fwrite(&x, sizeof(double), 1, f);
	}
	fclose(f);
}

LeNet NN;
double input[1024][1024];
double output[1024][128];
int ans[1024];
char c;
int N,i,j,a,ok=0;
double last, cur;

int MapCharToInt(char i)
{
	if (i <= '9') return i-'0';
	if (i <= 'Z') return i-'A'+10;
	if (i <= 'z') return i-'a'+36;
	return 0;
}

char MapIntToChar(int i)
{
	if (i < 10) return '0'+i;
	i -= 10;
	if (i < 26) return 'A'+i;
	i -= 26;
	if (i < 26) return 'a'+i;
	return 0;
}

int main()
{
	//CreateRandomNN_Bin("test.db", 20, 80, 200, 62);

	NN.LoadFromBinFile("test.db");
	//freopen("train_letters.txt", "r", stdin);
	freopen("verdana.png.txt", "r", stdin);
	
	scanf("%d",&N);
	printf("N=%d\n", N);
	for (i=0;i<N;++i)
	{
		for (j=0;j<841;++j)
		{
			scanf("%d", &a);
			input[i][j] = ((double)a/255.0) * 2.0 - 1.0;
		}
		scanf("\n%c\n", &c);
		ans[i] = MapCharToInt(c);
		//printf("char %c -> code %d\n", c, ans[i]);
		for (j=0;j<NN.finalNodes;++j)
			output[i][j] = -1.0f;
		output[i][ans[i]] = 1.0f;
	}

	int epo;
	for (epo=0;epo<1024;++epo)
	{
		ok = 0;
		for (i=0;i<N;++i)
		{
			j = NN.Classify(input[i]);
			if (j == ans[i])
				++ok;
			//NN.Learn(output[i]);
		}

		last = cur;
		cur = (double)ok/N;
		printf("epoch %2d : %.4lf\n", epo, cur);
		/*if (cur > 0.85)
		{
			NN.learn_rate = 0.002;
			printf("Learning rate set to %lf\n", NN.learn_rate);
		}*/
		/*if (fabs(cur-last) < LEARN_THRESHOLD)
		{
			//NN.learn_rate *= LEARN_MULT;
			printf("Learning rate set to %lf\n", NN.learn_rate);
		}*/
	}
	
	NN.SaveToBinFile("test.db");

	return 0;
}

