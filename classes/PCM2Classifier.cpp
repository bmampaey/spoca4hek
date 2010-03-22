#include "PCM2Classifier.h"

using namespace std;

PCM2Classifier::PCM2Classifier(Real fuzzifier)
:PCMClassifier(fuzzifier)
{}

void PCM2Classifier::computeU()
{
	U.resize(numberValidPixels * numberClasses);
	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{
			U[i*numberValidPixels+j] = d2(X[j],B[i]) / eta[i];
			if(fuzzifier == 1.5)
			{
				U[i*numberValidPixels+j] *= U[i*numberValidPixels+j];
				U[i*numberValidPixels+j] *= U[i*numberValidPixels+j];
				U[i*numberValidPixels+j] = 1. / (1. + U[i*numberValidPixels+j] ) ;
			}
			else if(fuzzifier == 2)
			{
				U[i*numberValidPixels+j] *= U[i*numberValidPixels+j];
				U[i*numberValidPixels+j] = 1. / (1. + U[i*numberValidPixels+j] ) ;
			}
			else
				U[i*numberValidPixels+j] = 1. / (1. + pow( U[i*numberValidPixels+j] , 2./(fuzzifier-1.) ) );

		}

	}

}


void PCM2Classifier::computeEta()
{
	PCMClassifier::computeEta();
	const Real reduction = 0.95;

	for (unsigned i = 1; i < numberClasses; ++i)
	{
		Real min = numeric_limits<Real>::max();
		for(unsigned ii = 0; ii < i; ++ii)
		{
			if (eta[ii] < eta[i])
			{
				Real min_Bi_above_Bii = B[i].v[0]/B[ii].v[0];
				for(unsigned p = 1; p < NUMBERWAVELENGTH; ++p)
					if (B[i].v[p]/B[ii].v[p] < min_Bi_above_Bii )
						min_Bi_above_Bii = B[i].v[p]/B[ii].v[p];

				if( eta[ii]*min_Bi_above_Bii < min)
					min = eta[ii]*min_Bi_above_Bii;
			}
		}

		if(eta[i] > reduction * min)
		{
			eta[i] = reduction * min;

		}

	}
	
	#if defined(DEBUG) && DEBUG >= 3
	cout<<"eta2:\t"<<eta<<endl;
	#endif

}
