
#ifndef PCMClassifier_H
#define PCMClassifier_H

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "SunImage.h"
#include "FeatureVector.h"
#include "FCMClassifier.h"

class PCMClassifier : public virtual FCMClassifier
{
	protected :
		std::vector<Real> eta;

		void computeB(){FCMClassifier::computeB();}
		void computeU();
		Real computeJ() const;
		virtual void computeEta();
		virtual void computeEta(Real alpha);

		Real assess(std::vector<Real>& V);

	public :
		//Constructors & Destructors
		PCMClassifier(Real fuzzifier = 1.5);

		//Classification functions
		void classification(Real precision = 1., unsigned maxNumberIteration = 100);
		void fixCentersClassification();

		//Function to initialise the centers
		void init(const std::vector<RealFeature>& initB, const std::vector<Real>& initEta);
		void init(const std::vector<RealFeature>& initB);
		void randomInit(unsigned C, Real precision = 1., unsigned maxNumberIteration = 100);

		std::vector<Real> getEta();

};
#endif
