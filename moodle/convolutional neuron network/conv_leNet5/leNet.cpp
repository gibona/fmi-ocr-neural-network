#include "leNet.h"

int LeNet::Classify(double* data)
{
	int i,j,a,b,k,s,pos;
	input_data = data;

	// conv0
	for (k=0;k<primFeatures;++k)
		for (i=0;i<13;++i)
			for (j=0;j<13;++j)
			{
				pos = k*13*13 + i*13 + j;
				conv0_in[pos] = conv0[(k+1)*26-1];
				for (a=0;a<5;++a)
					for (b=0;b<5;++b)
						conv0_in[pos] += conv0[k*26 + a*5 + b] * data[(2*i+a)*29 + (2*j+b)];
				conv0_val[pos] = F(conv0_in[pos]);
			}

	// conv1
	for (k=0;k<secFeatures;++k)
		for (i=0;i<5;++i)
			for (j=0;j<5;++j)
			{
				pos = k*5*5 + i*5 + j;
				conv1_in[pos] = conv1[(k+1)*(25*primFeatures+1)-1];
				for (s=0;s<primFeatures;++s)
					for (a=0;a<5;++a)
						for (b=0;b<5;++b)
							conv1_in[pos] += conv1[k*(25*primFeatures+1) + s*25 + a*5 + b] * conv0_val[s*13*13 + (2*i+a)*13 + (2*j+b)];
				conv1_val[pos] = F(conv1_in[pos]);
			}
	
	// full
	for (k=0;k<fullNodes;++k)
	{
		full_in[k] = full[(k+1)*(25*secFeatures+1)-1];
		for (s=0;s<secFeatures;++s)
			for (a=0;a<5;++a)
				for (b=0;b<5;++b)
					full_in[k] += full[k*(25*secFeatures+1) + s*25 + 5*a + b] * conv1_val[s*25 + a*5 + b];
		full_val[k] = F(full_in[k]);
	}

	// final
	for (k=0;k<finalNodes;++k)
	{
		final_in[k] = final[(k+1)*(fullNodes+1)-1];
		for (s=0;s<fullNodes;++s)
			final_in[k] += final[k*(fullNodes+1) + s] * full_val[s];
		final_val[k] = F(final_in[k]);
	}

	k = -1;
	double mm = -10.0;
	for (i=0;i<finalNodes;++i)
		if (final_val[i] > mm)
		{
			mm = final_val[i];
			k = i;
		}
	return k;
}

void LeNet::Learn(double* target)
{
	int i,j,a,b,k,s,pos;
	// propagate the error backwards
	// final
	for (k=0;k<finalNodes;++k)
		final_err[k] = target[k] - final_val[k];
	// full
	for (k=0;k<fullNodes;++k)
	{
		full_err[k] = 0;
		for (s=0;s<finalNodes;++s)
			full_err[k] += final_err[s] * final[s*(fullNodes+1) + k];
	}
	// conv1
	for (k=0;k<secFeatures;++k)
		for (i=0;i<5;++i)
			for (j=0;j<5;++j)
			{
				pos = k*25 + i*5 + j;
				conv1_err[pos] = 0;
				for (s=0;s<fullNodes;++s)
					conv1_err[pos] += full_err[s] * full[s*(secFeatures*25+1) + k*25 + 5*i + j];
			}
	//conv0 -- trick
	for (k=0;k<primFeatures;++k)
		for (i=0;i<13;++i)
			for (j=0;j<13;++j)
				conv0_err[k*13*13 + i*13 + j] = 0; // memset for doubles?
	// feed the error updates "backwards"
	for (k=0;k<secFeatures;++k)
		for (i=0;i<5;++i)
			for (j=0;j<5;++j)
			{
				pos = k*5*5 + i*5 + j;
				for (s=0;s<primFeatures;++s)
					for (a=0;a<5;++a)
						for (b=0;b<5;++b)
							conv0_err[s*13*13 + (2*i+a)*13 + (2*j+b)] += conv1[k*(25*primFeatures+1) + s*25 + a*5 + b] * conv1_err[pos];
			}
	//update weights
	double diff;
	// conv0
	for (k=0;k<primFeatures;++k)
	{
		for (i=0;i<5;++i)
			for (j=0;j<5;++j)
			{
				diff = 0;
				for (a=0;a<13;++a)
					for (b=0;b<13;++b)
					{
						pos = k*13*13 + a*13 + b;
						diff += conv0_err[pos] * dFdx(conv0_val[pos]) * input_data[(2*a+i)*29 + (2*b+j)];
					}
				conv0[k*26 + i*5 + j] += learn_rate * diff;
			}
		// bias
		diff = 0;
		for (a=0;a<13;++a)
			for (b=0;b<13;++b)
			{
				pos = k*13*13 + a*13 + b;
				diff += conv0_err[pos] * dFdx(conv0_val[pos]);
			}
		conv0[(k+1)*26-1] += learn_rate * diff;
	}
	// conv1
	for (k=0;k<secFeatures;++k)
	{
		for (s=0;s<primFeatures;++s)
			for (i=0;i<5;++i)
				for (j=0;j<5;++j)
				{
					diff = 0;
					for (a=0;a<5;++a)
						for (b=0;b<5;++b)
						{
							pos = k*25 + a*5 + b;
							diff += conv1_err[pos] * dFdx(conv1_val[pos]) * conv0_val[s*13*13 + (2*a+i)*13 + (2*b+j)];
						}
					conv1[k*(25*primFeatures+1) + s*25 + i*5 + j] += learn_rate * diff;
				}
		// bias
		diff = 0;
		for (a=0;a<5;++a)
			for (b=0;b<5;++b)
			{
				pos = k*25 + a*5 + b;
				diff += conv1_err[pos] * dFdx(conv1_val[pos]);
			}
		conv1[(k+1)*(25*primFeatures+1) - 1] += learn_rate * diff;
	}
	// full
	for (k=0;k<fullNodes;++k)
	{
		for (s=0;s<secFeatures;++s)
			for (i=0;i<5;++i)
				for (j=0;j<5;++j)
					full[k*(25*secFeatures+1) + s*25 + 5*i + j] += learn_rate * full_err[k] * dFdx(full_val[k]) * conv1_val[s*5*5 + i*5 + j];
		//bias
		full[(k+1)*(25*secFeatures+1) - 1] += learn_rate * full_err[k] * dFdx(full_val[k]);
	}
	// final
	for (k=0;k<finalNodes;++k)
	{
		for (s=0;s<fullNodes;++s)
			final[k*(fullNodes+1) + s] += learn_rate * final_err[k] * dFdx(final_val[k]) * full_val[s];
		//bias
		final[(k+1)*(fullNodes+1) - 1] += learn_rate * final_err[k] * dFdx(final_val[k]);
	}
}
