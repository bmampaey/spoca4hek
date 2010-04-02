#pragma once
#ifndef SunImage_H
#define SunImage_H

#include <iostream>
#include <limits>
#include <typeinfo>
#include <vector>
#include <cmath>
#include <string>
#include <ctime>

#include "fitsio.h"
#include "longnam.h"
#include "Image.h"
#include "Coordinate.h"

class SunImage : public Image<PixelType>
{

	double radius;
	double wavelength;
	time_t observationTime;
	Coordinate suncenter;
	double cdelt[2];
	PixelType median, datap01, datap95;
	std::vector<char*> header;
	char date_obs[80];

	public :
		// Preprocessing types declaration
		enum {None = 0, ALC = 1, TakeLog = 2, DivMedian = 3, TakeSqrt = 4, DivMode = 5};
		
		//Constructors and destructors
		SunImage(const std::string& filename);
		SunImage(const long xAxes = 0, const long yAxes = 0, const double radius = 0., const double wavelength = 0.);
		SunImage(const SunImage& i);
		SunImage(const SunImage* i);
		~SunImage();
		
		//Accessors
		double Wavelength() const;
		double Median() const;
		Coordinate SunCenter() const;
		double SunRadius() const;
		time_t ObservationTime() const;
		std::string ObservationDate() const;
		double PixelArea() const;

		//Various routines to work on SunImages
		unsigned numberValidPixelsEstimate() const;
		void recenter(const Coordinate& newCenter);
		void nullifyAboveRadius(const Real radiusRatio = 1.0);
		Real percentCorrection(const Real r) const;
		void annulusLimbCorrection(const Real radiusRatio = 1.0, const Real minLimbRadius = 0.90);
		void ALCDivMedian(const Real radiusRatio = 1.0, const Real minLimbRadius = 0.90);
		void ALCDivMode(const Real radiusRatio = 1.0, const Real minLimbRadius = 0.90);
		void preprocessing(const int type = 0, Real maxLimbRadius = 1.0, Real minLimbRadius = 0.90);
		void copyKeywords(const SunImage* i);
		SunImage* writeFitsImage (const std::string& filename) ;
		
		// Future routines to derotate an image (in progress) 
		Real angularSpeed(Real latitude);
		unsigned newPos(Real x, Real y, const Real t);
		SunImage* rotate(const unsigned t);

};
#endif
