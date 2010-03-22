
#ifndef HistogramPCM2Classifier_H
#define HistogramPCM2Classifier_H

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "SunImage.h"
#include "FeatureVector.h"
#include "HistogramPCMClassifier.h"
#include "PCM2Classifier.h"

class HistogramPCM2Classifier : public virtual HistogramPCMClassifier, public virtual PCM2Classifier
{
	protected :

		void computeB()
			{HistogramPCMClassifier::computeB();}
		void computeU();
		void computeEta();
		//We don't know how to compute J for PCM2H
		Real computeJ() const
			{return HistogramPCMClassifier::computeJ();}

		//Asses & Merge functions for the sursegmentation
		Real assess(std::vector<Real>& V)
			{return HistogramPCMClassifier::assess(V);}
		void merge(unsigned i1, unsigned i2)
			{HistogramPCMClassifier::merge(i1, i2);}

	public :
		//Constructors & Destructors
		HistogramPCM2Classifier(Real fuzzifier = 2);
		void addImages(const std::vector<SunImage*>& images, RealFeature binSize)
			{HistogramFCMClassifier::addImages(images, binSize);}

		//Classification functions
		void classification(Real precision = 1., unsigned maxNumberIteration = 100)
			{HistogramPCMClassifier::classification(precision, maxNumberIteration);}
		void fixCentersClassification()
			{HistogramPCMClassifier::fixCentersClassification();}

		//Function to initialise the centers
		void init(const std::vector<RealFeature>& initB, const std::vector<Real>& initEta)
			{HistogramPCMClassifier::init(initB, initEta);}
		void init(const std::vector<RealFeature>& initB)
			{HistogramPCMClassifier::init(initB);}
		void randomInit(unsigned C, Real precision = 1., unsigned maxNumberIteration = 100)
			{HistogramPCMClassifier::randomInit(C, precision, maxNumberIteration);}

		//Utilities functions for outputing results
		void saveResults(SunImage* outImage);
		void saveARmap(SunImage* outImage);

};
#endif
