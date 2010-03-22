#include "PCMClassifier.h"

using namespace std;

PCMClassifier::PCMClassifier(Real fuzzifier)
:FCMClassifier(fuzzifier)
{}

void PCMClassifier::computeU()
{
	U.resize(numberValidPixels * numberClasses);
	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{
			if(fuzzifier == 2)
				U[i*numberValidPixels+j] = 1. / (1. + d2(X[j],B[i]) / eta[i] ) ;
			else if(fuzzifier == 1.5)
				U[i*numberValidPixels+j] = 1. / (1. + ( d2(X[j],B[i]) / eta[i] ) *  ( d2(X[j],B[i]) / eta[i] )) ;
			else
				U[i*numberValidPixels+j] = 1. / (1. + pow( d2(X[j],B[i]) / eta[i] , 1./(fuzzifier-1.) ) );

		}

	}

}


void PCMClassifier::computeEta()
{

	eta = vector<Real>(numberClasses,0.);

	Real sum, uij_m;
	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		sum = 0;
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			if(fuzzifier == 2)
				uij_m = U[i*numberValidPixels+j] * U[i*numberValidPixels+j];
			else
				uij_m = pow(U[i*numberValidPixels+j],fuzzifier);

			eta[i] += uij_m*d2(X[j],B[i]);
			sum += uij_m;
		}
		eta[i] /= sum;
	}
	#if defined(DEBUG) && DEBUG >= 3
	cout<<"eta:\t"<<eta<<endl;
	#endif

}


void PCMClassifier::computeEta(Real alpha)
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
				eta[i] += d2(X[j],B[i]);
				++sum;
			}

		}

		eta[i] /= sum;

	}

}


Real PCMClassifier::computeJ() const
{
	Real result = 0;

	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		Real sum1 = 0, sum2 = 0;

		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			if(fuzzifier == 2)
				sum1 +=  U[i*numberValidPixels+j] * U[i*numberValidPixels+j] * d2(X[j],B[i]);
			else
				sum1 +=  pow(U[i*numberValidPixels+j], fuzzifier) * d2(X[j],B[i]);

			if(fuzzifier == 2)
				sum2 += (1 - U[i*numberValidPixels+j]) * (1 - U[i*numberValidPixels+j]);
			else
				sum2 +=  pow(1 - U[i*numberValidPixels+j], fuzzifier);

		}
		result += sum1 + (eta[i] * sum2);
	}
	return result;

}


void PCMClassifier::classification(Real precision, unsigned maxNumberIteration)
{

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


void PCMClassifier::fixCentersClassification()
{

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--PCMClassifier::fixCentersClassification--START--"<<endl;
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
	cout<<"--PCMClassifier::fixCentersClassification--END--"<<endl;
	#endif

}


Real PCMClassifier::assess(vector<Real>& V)
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
				sum1 +=  U[i*numberValidPixels+j] * U[i*numberValidPixels+j] * d2(X[j],B[i]);
			else
				sum1 +=  pow(U[i*numberValidPixels+j], fuzzifier) * d2(X[j],B[i]);

			if(fuzzifier == 2)
				sum2 += (1 - U[i*numberValidPixels+j]) * (1 - U[i*numberValidPixels+j]);
			else
				sum2 +=  pow(1 - U[i*numberValidPixels+j], fuzzifier);

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


vector<Real> PCMClassifier::getEta()
{
	return eta;
}


void PCMClassifier::init(const vector<RealFeature>& initB, const vector<Real>& initEta)
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


void PCMClassifier::init(const vector<RealFeature>& initB)
{

	#if defined(DEBUG) && DEBUG >= 1
	if(X.size() == 0)
	{
		cerr<<"Error : The vector of FeatureVector must be initialized before doing a centers only init."<<endl;
		exit(EXIT_FAILURE);

	}
	#endif
	B = initB;
	//We like our centers to be sorted
	sort(B.begin(), B.end());
	numberClasses = B.size();
	//To initialise eta we need to compute the U with FCM
	Real temp = fuzzifier;
	fuzzifier = 2.;

	FCMClassifier::computeU();
	fuzzifier = temp;
	computeEta();
}


void PCMClassifier::randomInit(unsigned C, Real precision, unsigned maxNumberIteration)
{
	numberClasses = C;
	//We initialise the centers by setting each one randomly to one of the actual pixel, then we do a FCM classification!
	Classifier::randomInit(C);
	Real temp = fuzzifier;
	fuzzifier = 2.;
	FCMClassifier::classification(precision, maxNumberIteration);
	//We like our centers to be sorted 
	sort(B.begin(), B.end());
	FCMClassifier::computeU();
	fuzzifier = temp;
	//We initialise eta
	computeEta();
}
