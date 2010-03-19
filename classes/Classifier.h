#pragma once
#ifndef Classifier_H
#define Classifier_H

#include <iostream>
#include <fstream>
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <algorithm>

#include "tools.h"
#include "constants.h"
#include "Image.h"
#include "SunImage.h"
#include "FeatureVector.h"
#include "RegionStats.h"
#include "Coordinate.h"

class Classifier
{
	protected :
		unsigned    numberClasses;
		unsigned    numberValidPixels;
		unsigned    Xaxes, Yaxes;

		//vector of membership
		std::vector<Real> U;

		//vector of centers
		std::vector<RealFeature> B;

		//vector of feature vectors (pixel intensities for example)
		std::vector<PixelFeature> X;

		//The coordinates of the feature vectors (needed to output the results)
		std::vector<Coordinate> coordinates;

		//Computation of centers and membership
		virtual void computeB() = 0;
		virtual void computeU() = 0;

		//Asses & Merge functions for the sursegmentation
		virtual Real assess(std::vector<Real>& V) = 0;
		virtual void merge(unsigned i1, unsigned i2);

	public :
		//Constructors & Destructors
		Classifier();
		virtual ~Classifier(){};

		//Functions to add and check images
		void checkImages(const std::vector<SunImage*>& images);
		virtual void addImages (const std::vector<SunImage*>& images);

		//Classification functions
		virtual void classification(Real precision = 1., unsigned maxNumberIteration = 100) = 0;
		virtual void fixCentersClassification() = 0;

		//Sursegmentation functions
		unsigned sursegmentation(std::vector<RealFeature>& initB, unsigned C = 0);
		unsigned sursegmentation(unsigned C = 0);
		unsigned fixSursegmentation(std::vector<RealFeature>& initB, std::vector<RealFeature>& maxLimits);

		//Utilities functions for outputing results
		virtual Image<unsigned>* crispSegmentedMap();
		virtual Image<Real>* fuzzyMap(const unsigned i);
		virtual Image<Real>* normalizedFuzzyMap(const unsigned i);
		void saveResults(SunImage* outImage);
		void saveARmap(SunImage* outImage);
		std::vector<PixelFeature> percentiles(std::vector<Real>);
		std::vector<RealFeature> getB();
		SunImage* getImage(unsigned p);

		//Function to initialise the centers
		virtual void init(const std::vector<RealFeature>& initB);
		virtual void randomInit(unsigned C);

};

extern std::string outputFileName;
#endif
