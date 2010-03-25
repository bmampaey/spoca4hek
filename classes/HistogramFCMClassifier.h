
#ifndef HistogramFCMClassifier_H
#define HistogramFCMClassifier_H

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <fstream>

#include "Image.h"
#include "SunImage.h"
#include "HistogramFeatureVector.h"
#include "FeatureVector.h"
#include "FCMClassifier.h"

class HistogramFCMClassifier : public virtual FCMClassifier
{
	protected :
		std::vector<HistoPixelFeature> HistoX;

	protected :
		void computeB();
		void computeU();
		Real computeJ() const;

		unsigned insert(const HistoPixelFeature& xj);

		//Asses & Merge functions for the sursegmentation
		Real assess(std::vector<Real>& V);
		void merge(unsigned i1, unsigned i2);

	public :
		//Constructors & Destructors
		HistogramFCMClassifier(Real fuzzifier = 2.);
		void addImages(const std::vector<SunImage*>& images, const RealFeature& binSize);

		//Classification functions
		void classification(Real precision = 1., unsigned maxNumberIteration = 100);

		//Utilities functions for outputing results
		void saveResults(SunImage* outImage);
		void saveARmap(SunImage* outImage);
		
		//Function to initialise/save the Histogram
		void initHistogram(const std::string& histogramFilename, RealFeature& binSize, bool reset = true);
		void saveHistogram(const std::string& histogramFilename, const RealFeature& binSize);
};
#endif
