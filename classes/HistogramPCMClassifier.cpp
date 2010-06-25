#include "HistogramPCMClassifier.h"

using namespace std;

HistogramPCMClassifier::HistogramPCMClassifier(Real fuzzifier)
:HistogramFCMClassifier(fuzzifier),PCMClassifier()
{}

//Because the numberValidPixels of X is not the same as numberValidPixels of HistoX
void HistogramPCMClassifier::saveResults(SunImage* outImage)
{
	numberValidPixels = X.size();
	PCMClassifier::computeU();
	Classifier::saveResults(outImage);
	numberValidPixels = HistoX.size();
}


void HistogramPCMClassifier::saveARmap(SunImage* outImage)
{
	numberValidPixels = X.size();
	PCMClassifier::computeU();
	Classifier::saveARmap(outImage);
	numberValidPixels = HistoX.size();
}


void HistogramPCMClassifier::computeU()
{
	U.resize(numberValidPixels * numberClasses);
	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			U[i*numberValidPixels+j] = d2(HistoX[j],B[i]) / eta[i] ;
			if(fuzzifier == 1.5)
				U[i*numberValidPixels+j] *=  U[i*numberValidPixels+j];
			else if(fuzzifier != 2)
				U[i*numberValidPixels+j] = pow( U[i*numberValidPixels+j] , 1./(fuzzifier-1.) );

			U[i*numberValidPixels+j] = 1. / (1. + U[i*numberValidPixels+j]);

		}

	}

}


void HistogramPCMClassifier::computeEta()
{

	eta = vector<Real>(numberClasses,0.);

	Real sum, uij_m;

	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		sum = 0;

		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			if(fuzzifier == 2)
				uij_m = U[i*numberValidPixels+j] * HistoX[j].c * U[i*numberValidPixels+j] ;
			else
				uij_m = pow(U[i*numberValidPixels+j],fuzzifier) * HistoX[j].c;

			eta[i] += uij_m*d2(HistoX[j],B[i]);
			sum += uij_m;

		}
		eta[i] /= sum;
	}

}


void HistogramPCMClassifier::computeEta(Real alpha)
{
	//This is the other method to calculate eta, descibed by Krishnapuram and Keller. It is a little faster by the way
	eta = vector<Real>(numberClasses,0.);

	Real sum;
	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		sum = 0;
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{
			if (U[i*numberValidPixels+j]>alpha)
			{
				eta[i] += d2(HistoX[j],B[i]) * HistoX[j].c;
				sum +=  HistoX[j].c;
			}

		}

		eta[i] /= sum;

	}
}


Real HistogramPCMClassifier::computeJ() const
{
	Real result = 0;

	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		Real sum1 = 0, sum2 = 0;

		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			if(fuzzifier == 2)
				sum1 +=  U[i*numberValidPixels+j] * U[i*numberValidPixels+j] * d2(HistoX[j],B[i]) * HistoX[j].c;
			else
				sum1 +=  pow(U[i*numberValidPixels+j], fuzzifier) * d2(HistoX[j],B[i]) * HistoX[j].c;

			if(fuzzifier == 2)
				sum2 += (1 - U[i*numberValidPixels+j]) * (1 - U[i*numberValidPixels+j]) * HistoX[j].c;
			else
				sum2 +=  pow(1 - U[i*numberValidPixels+j], fuzzifier) * HistoX[j].c;

		}
		result += sum1 + (eta[i] * sum2);
	}
	return result;

}

#ifdef ETA_CIS

// VERSION WITH LIMITED VARIATION OF ETA W.R.T. ITS INITIAL VALUE

void HistogramPCMClassifier::classification(Real precision, unsigned maxNumberIteration)
{	
	const Real maxFactor = 100.;



	#if defined(DEBUG) && DEBUG >= 1
	if(X.size() == 0 || B.size() == 0|| B.size() != eta.size())
	{
		cerr<<"Error : The Classifier must be initialized before doing classification."<<endl;
		exit(EXIT_FAILURE);

	}
	#endif

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--PCMClassifier::classification--START--"<<endl;
	#endif

	//Initialisation of precision & U
	this->precision = precision;

	Real precisionReached = numeric_limits<Real>::max();
	vector<RealFeature> oldB = B;
	vector<Real> old_eta;
	vector<Real> start_eta = eta;
	bool stopComputationEta = false;
	for (unsigned iteration = 0; iteration < maxNumberIteration && precisionReached > precision ; ++iteration)
	{

		if ( (!FIXETA) && (!stopComputationEta) )	//eta is to be recalculated each iteration.
		{
			old_eta = eta;
			computeEta();
	
			for (unsigned i = 0 ; i < numberClasses && !stopComputationEta ; ++i)
			{
				if ( (start_eta[i] / eta[i] > maxFactor) || (start_eta[i] / eta[i] < 1. / maxFactor) )
				{
					stopComputationEta = true;
				}
			}
		}

		computeU();
		computeB();

		for (unsigned i = 0 ; i < numberClasses ; ++i)
		{
			precisionReached = d2(oldB[i],B[i]);
			if (precisionReached > precision)
				break;
		}

		oldB = B;

		#if defined(DEBUG) && DEBUG >= 3
		cout<<"iteration :"<<iteration;
		cout<<"\tprecisionReached :"<<precisionReached;
		cout<<"\tJPCM :"<<computeJ();
		cout<<"\tB :"<<B;
		cout<<endl;
		#endif


	}

	#if defined(DEBUG) && DEBUG >= 2
	string filename = outputFileName + "segmented." + itos(numberClasses) + "classes.fits" ;
	Image<unsigned> * segmentedMap = crispSegmentedMap();
	segmentedMap->writeFitsImage(filename);
	delete segmentedMap;
	#endif
	
	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--PCMClassifier::classification--END--"<<endl;
	#endif


}


#endif

#ifndef ETA_CIS

void HistogramPCMClassifier::classification(Real precision, unsigned maxNumberIteration)
{

	#if defined(DEBUG) && DEBUG >= 1
	if(HistoX.size() == 0 || B.size() == 0 || B.size() != eta.size())
	{
		cerr<<"Error : The Classifier must be initialized before doing classification."<<endl;
		exit(EXIT_FAILURE);

	}
	#endif

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--HistogramPCMClassifier::classification--START--"<<endl;
	#endif

	//Initialisation of precision & U

	this->precision = precision;

	Real precisionReached = numeric_limits<Real>::max();
	vector<RealFeature> oldB = B;
	for (unsigned iteration = 0; iteration < maxNumberIteration && precisionReached > precision ; ++iteration)
	{
		if(! FIXETA)							  //eta is to be recalculated each iteration.
			computeEta();

		computeU();
		computeB();

		for (unsigned i = 0 ; i < numberClasses ; ++i)
		{
			precisionReached = d2(oldB[i],B[i]);
			if (precisionReached > precision)
				break;

		}

		oldB = B;

		#if defined(DEBUG) && DEBUG >= 3
		cout<<"iteration :"<<iteration;
		cout<<"\tprecisionReached :"<<precisionReached;
		cout<<"\tJPCM :"<<computeJ();
		cout<<"\tB :"<<B<<" eta :"<<eta<<endl;
		#endif

	}

	#if defined(DEBUG) && DEBUG >= 2
	string filename = outputFileName + "segmented." + itos(numberClasses) + "classes.fits" ;
	Image<unsigned> * segmentedMap = crispSegmentedMap();
	segmentedMap->writeFitsImage(filename);
	delete segmentedMap;
	#endif
	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--HistogramPCMClassifier::classification--END--"<<endl;
	#endif
}

#endif

void HistogramPCMClassifier::fixCentersClassification()
{

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--HistogramPCMClassifier::fixCentersClassification--START--"<<endl;
	#endif

	//Initialisation of U

	computeU();

	#if defined(DEBUG) && DEBUG >= 2
	string filename = outputFileName + "segmented." + itos(numberClasses) + "classes.fits" ;
	Image<unsigned> * segmentedMap = crispSegmentedMap();
	segmentedMap->writeFitsImage(filename);
	delete segmentedMap;
	#endif
	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--HistogramPCMClassifier::fixCentersClassification--END--"<<endl;
	#endif

}


Real HistogramPCMClassifier::assess(vector<Real>& V)
{
	V = vector<Real>(numberClasses, 0.);
	Real score = 0;

	//This is the vector of the min distances between the centers Bi and all the others centers Bii with ii!=i
	vector<Real> minDist(numberClasses, numeric_limits<Real>::max());
	//The min distance between all centers
	Real minDistBiBii = numeric_limits<Real>::max() ;

	Real distBiBii;
	for (unsigned i = 0 ; i < numberClasses ; ++i)
		for (unsigned ii = i + 1 ; ii < numberClasses ; ++ii)
	{
		distBiBii = d2(B[i],B[ii]);
		if(distBiBii < minDist[i])
			minDist[i] = distBiBii;
		if(distBiBii < minDist[ii])
			minDist[ii] = distBiBii;
	}

	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		Real sum1 = 0, sum2 = 0;

		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			if(fuzzifier == 2)
				sum1 +=  U[i*numberValidPixels+j] * U[i*numberValidPixels+j] * d2(HistoX[j],B[i]) * HistoX[j].c;
			else
				sum1 +=  pow(U[i*numberValidPixels+j], fuzzifier) * d2(HistoX[j],B[i]) * HistoX[j].c;

			if(fuzzifier == 2)
				sum2 += (1 - U[i*numberValidPixels+j]) * (1 - U[i*numberValidPixels+j]) * HistoX[j].c;
			else
				sum2 +=  pow(1 - U[i*numberValidPixels+j], fuzzifier) * HistoX[j].c;

		}

		V[i] = sum1 + (eta[i] * sum2);
		score += V[i];
		if(minDist[i] < minDistBiBii)
			minDistBiBii = minDist[i];

		V[i] /= (minDist[i] * numberValidPixels);

	}

	score /= (minDistBiBii * numberValidPixels);
	return score;

}


void HistogramPCMClassifier::init(const vector<RealFeature>& initB, const vector<Real>& initEta)
{
	#if defined(DEBUG) && DEBUG >= 1
	if(initB.size() != initEta.size())
	{
		cerr<<"Error : The size of initB is different than the size of initEta"<<endl;
		exit(EXIT_FAILURE);

	}
	#endif

	B = initB;
	eta = initEta;
	numberClasses = B.size();
}


void HistogramPCMClassifier::init(const vector<RealFeature>& initB, Real precision, unsigned maxNumberIteration)
{

	#if defined(DEBUG) && DEBUG >= 1
	if(HistoX.size() == 0)
	{
		cerr<<"Error : The vector of FeatureVector must be initialized before doing a centers only init."<<endl;
		exit(EXIT_FAILURE);

	}
	#endif

	B = initB;
	numberClasses = B.size();
	Real temp = fuzzifier;
	fuzzifier = 2.;
	if ( maxNumberIteration != 0 )
	{
		HistogramFCMClassifier::classification(precision, maxNumberIteration);
	}
	
	//We like our centers to be sorted 
	sort(B.begin(), B.end());
	HistogramFCMClassifier::computeU();
	fuzzifier = temp;
	//We initialise eta
	computeEta();

	#ifdef ETA_BEN
	//This is just a test
	vector<Real> oldEta = eta;
	Real precisionReached = numeric_limits<Real>::max();
	for (unsigned iteration = 0; iteration < maxNumberIteration && precisionReached > precision ; ++iteration)
	{		
		computeU();
		computeEta();

		
		for (unsigned i = 0 ; i < numberClasses ; ++i)
		{
			precisionReached = abs(oldEta[i] - eta[i]);
			if (precisionReached > precision)
				break;
		}
		cout<<"eta :"<<eta<<endl;
		oldEta = eta;
	}
	#endif
}


void HistogramPCMClassifier::randomInit(unsigned C, Real precision, unsigned maxNumberIteration)
{

	#if defined(DEBUG) && DEBUG >= 1
	if(HistoX.size() == 0)
	{
		cerr<<"Error : The vector of FeatureVector must be initialized before doing a centers only init."<<endl;
		exit(EXIT_FAILURE);

	}
	#endif
	
	numberClasses = C;
	//We initialise the centers by setting each one randomly to one of the actual pixel, then we do a FCM classification!
	Classifier::randomInit(C);
	Real temp = fuzzifier;
	fuzzifier = 2.;
	HistogramFCMClassifier::classification(precision, maxNumberIteration);
	//We like our centers to be sorted 
	sort(B.begin(), B.end());
	HistogramFCMClassifier::computeU();
	fuzzifier = temp;
	//We initialise eta
	computeEta();
	
	#ifdef ETA_BEN
	//This is just a test
	vector<Real> oldEta = eta;
	Real precisionReached = numeric_limits<Real>::max();
	for (unsigned iteration = 0; iteration < maxNumberIteration && precisionReached > precision ; ++iteration)
	{		
		computeU();
		computeEta();

		
		for (unsigned i = 0 ; i < numberClasses ; ++i)
		{
			precisionReached = abs(oldEta[i] - eta[i]);
			if (precisionReached > precision)
				break;
		}
		cout<<"eta :"<<eta<<endl;
		oldEta = eta;
	}
	#endif
	

}

