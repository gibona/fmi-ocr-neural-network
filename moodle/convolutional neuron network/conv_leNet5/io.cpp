#include "leNet.h"
#include <cstdio>

void LeNet::allocate()
{
	conv0 = new double[26*primFeatures];
	conv1 = new double[(25*primFeatures+1)*secFeatures];
	full = new double[(25*secFeatures+1)*fullNodes];
	final = new double[finalNodes*(fullNodes+1)];

	conv0_val = new double[13*13*primFeatures];
	conv0_err = new double[13*13*primFeatures];
	conv0_in = new double[13*13*primFeatures];
	conv1_val = new double[5*5*secFeatures];
	conv1_err = new double[5*5*secFeatures];
	conv1_in = new double[5*5*secFeatures];
	full_val = new double[fullNodes];
	full_err = new double[fullNodes];
	full_in = new double[fullNodes];
	final_val = new double[finalNodes];
	final_err = new double[finalNodes];
	final_in = new double[finalNodes];
}

void LeNet::LoadFromTextFile(const char* path)
{
	FILE* f = fopen(path, "r");
	fscanf(f, "%d%d%d%d", &primFeatures, &secFeatures, &fullNodes, &finalNodes);
	allocate();
	int i;
	for (i=0;i<26*primFeatures;++i)
		fscanf(f, "%lf", &conv0[i]);
	for (i=0;i<(25*primFeatures+1)*secFeatures;++i)
		fscanf(f, "%lf", &conv1[i]);
	for (i=0;i<(25*secFeatures+1)*fullNodes;++i)
		fscanf(f, "%lf", &full[i]);
	for (i=0;i<finalNodes*(fullNodes+1);++i)
		fscanf(f, "%lf", &final[i]);

	fclose(f);
}

void LeNet::SaveToTextFile(const char* path)
{
	FILE* f = fopen(path, "w");
	fprintf(f, "%d %d %d %d\n", primFeatures, secFeatures, fullNodes, finalNodes);
	int i;
	for (i=0;i<26*primFeatures;++i)
		fprintf(f, "%lf ", conv0[i]);
	for (i=0;i<(25*primFeatures+1)*secFeatures;++i)
		fprintf(f, "%lf ", conv1[i]);
	for (i=0;i<(25*secFeatures+1)*fullNodes;++i)
		fprintf(f, "%lf ", full[i]);
	for (i=0;i<finalNodes*(fullNodes+1);++i)
		fprintf(f, "%lf ", final[i]);
	fclose(f);
}

void LeNet::LoadFromBinFile(const char* path)
{
	FILE* f = fopen(path, "rb");
	fread(&primFeatures, sizeof(int), 1, f);
	fread(&secFeatures, sizeof(int), 1, f);
	fread(&fullNodes, sizeof(int), 1, f);
	fread(&finalNodes, sizeof(int), 1, f);
	allocate();
	fread(conv0, sizeof(double), 26*primFeatures, f);
	fread(conv1, sizeof(double), (25*primFeatures+1)*secFeatures, f);
	fread(full, sizeof(double), (25*secFeatures+1)*fullNodes, f);
	fread(final, sizeof(double), finalNodes*(fullNodes+1), f);
	fclose(f);
}

void LeNet::SaveToBinFile(const char* path)
{
	FILE* f = fopen(path, "wb");
	fwrite(&primFeatures, sizeof(int), 1, f);
	fwrite(&secFeatures, sizeof(int), 1, f);
	fwrite(&fullNodes, sizeof(int), 1, f);
	fwrite(&finalNodes, sizeof(int), 1, f);
	fwrite(conv0, sizeof(double), 26*primFeatures, f);
	fwrite(conv1, sizeof(double), (25*primFeatures+1)*secFeatures, f);
	fwrite(full, sizeof(double), (25*secFeatures+1)*fullNodes, f);
	fwrite(final, sizeof(double), finalNodes*(fullNodes+1), f);
	fclose(f);
}