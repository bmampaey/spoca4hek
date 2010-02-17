#pragma once
#ifndef FCMClassifier_H
#define FCMClassifier_H

#include <iostream>
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "Image.h"
#include "SunImage.h"
#include "FeatureVector.h"
#include "Classifier.h"

class FCMClassifier : public virtual Classifier
{
	protected :
		Real precision;
		Real fuzzifier;

		void computeB();
		void computeU();
		Real computeJ();

		//Asses & Merge functions for the sursegmentation
		Real assess(std::vector<Real>& V);
		void merge(unsigned i1, unsigned i2);

	public :
		//Constructors & Destructors
		FCMClassifier(Real fuzzifier = 2.);

		//Classification functions
		void classification(Real precision = 1., unsigned maxNumberIteration = 100);
		void fixCentersClassification();
};
#endif
