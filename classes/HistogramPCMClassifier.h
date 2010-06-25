
#ifndef HistogramPCMClassifier_H
#define HistogramPCMClassifier_H

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <limits>

#include "SunImage.h"
#include "FeatureVector.h"
#include "HistogramFCMClassifier.h"
#include "PCMClassifier.h"

class HistogramPCMClassifier : public virtual HistogramFCMClassifier, public virtual PCMClassifier
{
	protected :

		void computeB(){HistogramFCMClassifier::computeB();}
		void computeU();
		Real computeJ() const;
		virtual void computeEta();
		virtual void computeEta(Real alpha);

		//Asses & Merge functions for the sursegmentation
		Real assess(std::vector<Real>& V);
		void merge(unsigned i1, unsigned i2)
			{HistogramFCMClassifier::merge(i1, i2);}

	public :
		//Constructors & Destructors
		HistogramPCMClassifier(Real fuzzifier = 1.5);
		void addImages(const std::vector<SunImage*>& images, RealFeature binSize)
			{HistogramFCMClassifier::addImages(images, binSize);}

		//Classification functions
		void classification(Real precision = 1., unsigned maxNumberIteration = 100);
		void fixCentersClassification();

		//Function to initialise the centers
		void init(const std::vector<RealFeature>& initB, const std::vector<Real>& initEta);
		void init(const std::vector<RealFeature>& initB, Real precision = std::numeric_limits<Real>::max(), unsigned maxNumberIteration = 0);
		void randomInit(unsigned C, Real precision = 1., unsigned maxNumberIteration = 100);

		//Utilities functions for outputing results
		void saveResults(SunImage* outImage);
		void saveARmap(SunImage* outImage);

};
#endif
