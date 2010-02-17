#pragma once
#ifndef SunImage_H
#define SunImage_H

#include <iostream>
#include <limits>
#include <typeinfo>
#include <vector>
#include <math.h>
#include <string>
#include <time.h>

#include "fitsio.h"
#include "longnam.h"
#include "Image.h"
#include "Coordinate.h"

class SunImage : public Image<PixelType>
{

	double radius;
	double wavelength;
	time_t date_obs;
	Coordinate suncenter;
	double cdelt[2];
	PixelType median, datap01, datap95;

	char date_obs_string[80];

	public :

		static const unsigned ALC = 1;
		static const unsigned TakeLog = 2;
		static const unsigned DivMedian = 3;
		static const unsigned TakeSqrt = 4;
		SunImage(const std::string& fitsFileName);
		SunImage(const long xAxes = 0, const long yAxes = 0, const double radius = 0., const double wavelength = 0.);
		SunImage(const SunImage& i);
		SunImage(const SunImage* i);

		double Wavelength() const{return wavelength;}
		double Median() const{return median;}
		Coordinate SunCenter() const{return suncenter;}
		double SunRadius() const{return radius;}
		time_t ObsDate() const{return date_obs;}
		double PixelArea() const{return cdelt[0] * cdelt[1];}

		unsigned numberValidPixelsEstimate() const;
		void recenter(const Coordinate& newCenter);
		void nullifyAboveRadius(const Real radiusRatio = 1.0);
		Real percentCorrection(const Real r) const;
		void annulusLimbCorrection(const Real radiusRatio = 1.0, const Real minLimbRadius = 0.90);
		void preprocessing(const unsigned type = 0, Real maxLimbRadius = 1.0, Real minLimbRadius = 0.90);
		void copyKeywords(const SunImage* i);
		SunImage* writeFitsImage (const std::string& filename) ;

		Real angularSpeed(Real latitude);
		unsigned newPos(Real x, Real y, const Real t);
		SunImage* rotate(const unsigned t);

};
#endif
