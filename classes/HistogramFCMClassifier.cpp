#include "HistogramFCMClassifier.h"

using namespace std;

HistogramFCMClassifier::HistogramFCMClassifier(Real fuzzifier)
:FCMClassifier(fuzzifier)
{
}


// Function to insert a new FeatureVector into HistoX, if it is necessary (no doublon)
// Returns the position of insertion into the vector
// It assumes that the vector is sorted
inline unsigned HistogramFCMClassifier::insert(const HistoPixelFeature& xj)
{
	unsigned bsup = HistoX.size();
	unsigned binf = 0;
	unsigned pos = 0;
	while(binf < bsup)
	{
		pos = unsigned((bsup+binf)/2);
		switch(compare(HistoX[pos], xj))
		{
			case -1 :
				binf = pos + 1;
				break;
			case 1 :
				bsup = pos;
				break;
			default :
				return pos;

		}
	}
	if (bsup == HistoX.size())
	{
		HistoX.push_back(xj);
	}
	else
	{

		vector<HistoPixelFeature>::iterator there = HistoX.begin();
		there += bsup;
		HistoX.insert(there,xj);
	}
	return bsup;

}


void HistogramFCMClassifier::addImages(const std::vector<SunImage*>& images, const RealFeature& binSize)
{

	#if defined(DEBUG) && DEBUG >= 1
	for (unsigned p = 0; p <  NUMBERWAVELENGTH; ++p)
	{
		if( binSize.v[p] == 0 )
		{
			cerr<<"binSize cannot be 0."<<endl;
			exit(EXIT_FAILURE);
		}
	}
	#endif

	//I will need the images in the end to show the classification anyway
	Classifier::addImages(images);
	PixelFeature xj;
	for (unsigned j = 0; j < numberValidPixels; ++j)
	{
		for (unsigned p = 0; p <  NUMBERWAVELENGTH; ++p)
		{
			xj.v[p] = (int(X[j].v[p]/binSize.v[p]) * binSize.v[p]) + ( binSize.v[p] / 2 );
		}

		unsigned pos = insert(xj);
		++HistoX[pos].c;
	}

	//Be carefull here
	numberValidPixels = HistoX.size();
	#if defined(DEBUG) && DEBUG >= 2
	string filename = outputFileName + "histogram.txt";
	saveHistogram(filename, binSize);
	#endif

}

void HistogramFCMClassifier::initHistogram(const std::string& histogramFilename, RealFeature& binSize, bool reset)
{
	ifstream histoFile(histogramFilename.c_str());
	stringstream histoStream;
	vector<char> buffer;
	//We put the file into a stringstream for rapidity
	if (histoFile.good())
	{
		// We get the size of the file   
		histoFile.seekg(0,ios::end);
		streampos length = histoFile.tellg();
		histoFile.seekg(0,ios::beg);
		buffer.resize(length);

		//We read the whole file into the buffer.
		histoFile.read(&buffer[0],length);

		// We create the string stream.
		histoStream.rdbuf()->pubsetbuf(&buffer[0],length);

	}
	else
	{
		cerr<<"Error : file "<<histogramFilename<<" not found."<<endl;
		return;
	}
	histoFile.close();
	
	//We initialise the histogram
	unsigned numberBins = 0;
	for (unsigned p = 0; p < NUMBERWAVELENGTH; ++p)
		histoStream>>binSize.v[p];
		
	histoStream>>numberBins;
	
	HistoX.resize(numberBins);

	if (!reset)
	{
		for (unsigned j = 0; j < numberBins && histoStream.good(); ++j)
		{
			for (unsigned p = 0; p < NUMBERWAVELENGTH; ++p)
				histoStream>>HistoX[j].v[p];
			histoStream>>HistoX[j].c;
		}
	}
	else
	{
		unsigned garbage;
		for (unsigned j = 0; j < numberBins && histoStream.good(); ++j)
		{
			for (unsigned p = 0; p < NUMBERWAVELENGTH; ++p)
				histoStream>>HistoX[j].v[p];
			histoStream>>garbage;
		}

	}
	
}

void HistogramFCMClassifier::saveHistogram(const std::string& histogramFilename, const RealFeature& binSize)
{
	ofstream histoFile(histogramFilename.c_str());
	if (histoFile)
	{
		//We save the binSize and the number of bins
		for (unsigned p = 0; p < NUMBERWAVELENGTH; ++p)
				histoFile<<binSize.v[p]<<" ";
		histoFile<<HistoX.size()<<endl;
		
		//We save the Histogram
		for (unsigned j = 0; j < numberValidPixels && histoFile.good(); ++j)
		{
			for (unsigned p = 0; p < NUMBERWAVELENGTH; ++p)
				histoFile<<HistoX[j].v[p]<<" ";
			histoFile<<HistoX[j].c<<endl;
		}

	}
	else
	{
		cerr<<"Error : Could not open file "<<histogramFilename<<" for writing."<<endl;

	}
	

	histoFile.close();
}

//Because the numberValidPixels of X is not the same as numberValidPixels of HistoX
void HistogramFCMClassifier::saveResults(SunImage* outImage)
{
	numberValidPixels = X.size();
	FCMClassifier::computeU();
	Classifier::saveResults(outImage);
	numberValidPixels = HistoX.size();
}


void HistogramFCMClassifier::saveARmap(SunImage* outImage)
{
	numberValidPixels = X.size();
	FCMClassifier::computeU();
	Classifier::saveARmap(outImage);
	numberValidPixels = HistoX.size();
}


void HistogramFCMClassifier::computeB()
{

	Real sum, uij_m;

	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{
		B[i] = 0.;
		sum = 0;
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{
			if (fuzzifier == 2)
				uij_m = U[i*numberValidPixels+j] * U[i*numberValidPixels+j] * HistoX[j].c;
			else
				uij_m = pow(U[i*numberValidPixels+j],fuzzifier) * HistoX[j].c;

			B[i] += HistoX[j] * uij_m ;
			sum += uij_m;

		}

		B[i] /= sum;

	}
}


void HistogramFCMClassifier::computeU()
{

	Real sum;
	vector<Real> d2XjB(numberClasses);
	unsigned i;
	U.resize(numberValidPixels * numberClasses);
	for (unsigned j = 0 ; j < numberValidPixels ; ++j)
	{
		for (i = 0 ; i < numberClasses ; ++i)
		{
			d2XjB[i] = d2(HistoX[j],B[i]);
			if (d2XjB[i] < precision)
				break;
		}
		if(i < numberClasses)					  // The pixel is very close to B[i]
		{
			for (unsigned ii = 0 ; ii < numberClasses ; ++ii)
			{
				U[ii*numberValidPixels+j] = 0.;
			}
			U[i*numberValidPixels+j] = 1.;
		}
		else
		{
			for (i = 0 ; i < numberClasses ; ++i)
			{
				sum = 0;
				for (unsigned ii = 0 ; ii < numberClasses ; ++ii)
				{
					if (fuzzifier == 2)
						sum += (d2XjB[i]/d2XjB[ii]);
					else
						sum += pow(d2XjB[i]/d2XjB[ii],1./(fuzzifier-1.));

				}
				U[i*numberValidPixels+j] = 1./sum;
			}

		}

	}

}


Real HistogramFCMClassifier::computeJ() const
{
	Real result = 0;

	for (unsigned i = 0 ; i < numberClasses ; ++i)
	{

		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{

			if (fuzzifier == 2)
				result +=  U[i*numberValidPixels+j] * HistoX[j].c * U[i*numberValidPixels+j] * d2(HistoX[j],B[i]) ;
			else
				result +=  pow(U[i*numberValidPixels+j], fuzzifier) * HistoX[j].c * d2(HistoX[j],B[i]) ;

		}
	}
	return result;

}


void HistogramFCMClassifier::classification(Real precision, unsigned maxNumberIteration)
{
	#if defined(DEBUG) && DEBUG >= 1
	if(HistoX.size() == 0 || B.size() == 0)
	{
		cerr<<"Error : The Classifier must be initialized before doing classification."<<endl;
		exit(EXIT_FAILURE);

	}
	#endif

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--HistogramFCMClassifier::classification--START--"<<endl;
	#endif

	//Initialisation of precision & U

	this->precision = precision;

	Real precisionReached = numeric_limits<Real>::max();
	vector<RealFeature> oldB = B;
	for (unsigned iteration = 0; iteration < maxNumberIteration && precisionReached > precision ; ++iteration)
	{
		HistogramFCMClassifier::computeU();
		HistogramFCMClassifier::computeB();

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
		cout<<"\tJFCMH :"<<computeJ();
		cout<<"\tB :"<<B<<endl;
		#endif

	}

	#if defined(DEBUG) && DEBUG >= 2
	string filename = outputFileName + "segmented." + itos(numberClasses) + "classes.fits" ;
	Image<unsigned> * segmentedMap = crispSegmentedMap();
	segmentedMap->writeFitsImage(filename);
	delete segmentedMap;
	#endif
	#if defined(DEBUG) && DEBUG >= 3
	cout<<"--HistogramFCMClassifier::classification--END--"<<endl;
	#endif

}


Real HistogramFCMClassifier::assess(vector<Real>& V)
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
		for (unsigned j = 0 ; j < numberValidPixels ; ++j)
		{
			if (fuzzifier == 2)
				V[i] += d2(HistoX[j],B[i]) * U[i*numberValidPixels+j] * U[i*numberValidPixels+j] * HistoX[j].c;
			else
				V[i] += d2(HistoX[j],B[i]) * pow(U[i*numberValidPixels+j],fuzzifier) * HistoX[j].c;

		}

		score += V[i];
		if(minDist[i] < minDistBiBii)
			minDistBiBii = minDist[i];

		V[i] /= (minDist[i] * numberValidPixels);

	}

	score /= (minDistBiBii * numberValidPixels);

	return score;

}


#if MERGE==MERGEMAX
//We merge according to Benjamin's method
void HistogramFCMClassifier::merge(unsigned i1, unsigned i2)
{

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"Merging centers :"<<B[i1]<<"\t"<<B[i2];
	#endif

	Real max_uij, uij_m, sum = 0;
	unsigned max_i;
	B[i1] = 0;
	for (unsigned j = 0 ; j < numberValidPixels ; ++j)
	{
		max_uij = 0;
		max_i = 0;
		for (unsigned i = 0 ; i < numberClasses ; ++i)
			if (U[i*numberValidPixels+j] > max_uij)
		{
			max_uij = U[i*numberValidPixels+j];
			max_i = i;
		}
		if(max_i == i1 || max_i == i2)
		{
			if (fuzzifier == 2)
				uij_m = max_uij * max_uij * HistoX[j].c;
			else
				uij_m = pow(max_uij,fuzzifier) * HistoX[j].c;

			B[i1] += HistoX[j] * uij_m;
			sum += uij_m;

		}

	}

	B[i1] /= sum;

	#if defined(DEBUG) && DEBUG >= 3
	cout<<" into new center :"<<B[i1]<<endl;
	#endif

	B.erase(B.begin()+i2);
	--numberClasses;

	HistogramFCMClassifier::computeU();
}


#elif MERGE==MERGECIS
//We merge according to Cis's method
void HistogramFCMClassifier::merge(unsigned i1, unsigned i2)
{

	#if defined(DEBUG) && DEBUG >= 3
	cout<<"Merging centers :"<<B[i1]<<"\t"<<B[i2];
	#endif

	Real uij_m, sum = 0;
	B[i1] = 0;
	for (unsigned j = 0 ; j < numberValidPixels ; ++j)
	{
		if(U[i1*numberValidPixels+j] < U[i2*numberValidPixels+j])
			U[i1*numberValidPixels+j] = U[i2*numberValidPixels+j];

		if (fuzzifier == 2)
			uij_m = U[i1*numberValidPixels+j] * U[i1*numberValidPixels+j] * HistoX[j].c;
		else
			uij_m = pow(U[i1*numberValidPixels+j],fuzzifier) * HistoX[j].c;

		B[i1] += HistoX[j] * uij_m;
		sum += uij_m;

	}

	B[i1] /= sum;

	#if defined(DEBUG) && DEBUG >= 3
	cout<<" into new center :"<<B[i1]<<endl;
	#endif

	B.erase(B.begin()+i2);
	--numberClasses;
	U.erase(U.begin() + i2 * numberValidPixels, U.begin() + (i2 + 1)  * numberValidPixels);
}
#endif



