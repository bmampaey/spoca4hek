
#ifndef PCM2Classifier_H
#define PCM2Classifier_H

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "SunImage.h"
#include "FeatureVector.h"
#include "PCMClassifier.h"

class PCM2Classifier : public virtual PCMClassifier
{
	protected :

		void computeB()
			{PCMClassifier::computeB();}
		void computeU();
		void computeEta();
		//We don't know how to compute J for PCM2
		Real computeJ() const
			{return PCMClassifier::computeJ();}

	public :
		//Constructors & Destructors
		PCM2Classifier(Real fuzzifier = 2);

		//Classification functions
		void classification(Real precision = 1., unsigned maxNumberIteration = 100)
			{PCMClassifier::classification(precision, maxNumberIteration);}
		void fixCentersClassification()
			{PCMClassifier::fixCentersClassification();}

};
#endif
