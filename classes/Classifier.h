#pragma once
#ifndef Classifier_H
#define Classifier_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <algorithm>

#include "tools.h"
#include "constants.h"
#include "Image.h"
#include "SunImage.h"
#include "FeatureVector.h"
#include "Region.h"
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
		virtual void attribution() = 0;

		//Function to initialise the centers
		virtual void init(const std::vector<RealFeature>& initB);
		virtual void randomInit(unsigned C);
		
		//Segmentation functions
		virtual Image<unsigned>* segmentedMap_maxUij();
		virtual Image<unsigned>* segmentedMap_closestCenter();
		virtual Image<unsigned>* segmentedMap_classTreshold(unsigned i, Real lowerIntensity_minMembership, Real higherIntensity_minMembership);
		virtual Image<unsigned>* segmentedMap_limits(std::vector<RealFeature>& limits);
		virtual Image<Real>* fuzzyMap(const unsigned i);
		virtual Image<Real>* normalizedFuzzyMap(const unsigned i);
		
		//Sursegmentation functions
		unsigned sursegmentation(std::vector<RealFeature>& initB, unsigned C = 0);
		unsigned sursegmentation(unsigned C = 0);

		//Utilities function for outputing results
		virtual void saveAllResults(SunImage* outImage);
		virtual void saveARmap(SunImage* outImage);	
		void saveB(const std::string& filename, const RealFeature& wavelengths);
		
		//Accessors
		SunImage* getImage(unsigned p);
		std::vector<RealFeature> getB();
		std::vector<PixelFeature> percentiles(std::vector<Real> percentileValues);

};

extern std::string outputFileName;
#endif
