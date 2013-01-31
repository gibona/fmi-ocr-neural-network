#ifndef __LeNET__
#define __LeNET__

#include <cmath>
#define INITIAL_LEARN_RATE 0.005
//#define dFdx(x) ((1.0-x*x) * 1.71 * 0.66)
//#define dFdx(x) (2.0*x*(1.0-x))
#define dFdx(x) (1.0-(x)*(x))

class LeNet
{
public:
	int primFeatures, secFeatures, fullNodes, finalNodes;
	double *conv0, *conv1, *full, *final;
	double learn_rate;

	LeNet() : learn_rate(INITIAL_LEARN_RATE) {}

	void LoadFromTextFile(const char* path);
	void SaveToTextFile(const char* path);
	void LoadFromBinFile(const char* path);
	void SaveToBinFile(const char* path);

	int Classify(double* data);
	void Learn(double* target);

private:
	void allocate();
	//internals
	double *conv0_val, *conv1_val, *full_val, *final_val;
	double *conv0_err, *conv1_err, *full_err, *final_err;
	double *conv0_in, *conv1_in, *full_in, *final_in;
	double *input_data;

	inline double F(double x)
	{ return tanh(x); }
	/*inline double F(double x)
	{ double t = exp(x); return 2.0 * t / (t+1) - 1.0; }*/

	/*inline double dFdx(double x)
	{ double y = tanh(x); return (1-y*y) * 1.71 * 0.66; }*/
	
	/*
	inline double F(double x)
	{ double t = exp(x); return t / (t+1); }
	inline double dFdx(double x)
	{ double t = exp(x); return t / (t*t+2*t+1); }
	*/
};

#endif