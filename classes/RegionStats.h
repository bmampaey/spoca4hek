#pragma once
#ifndef RegionStats_H
#define RegionStats_H

#include <limits>
#include <math.h>
#include <iostream>
#include "Region.h"
#include "FeatureVector.h"
#include "Coordinate.h"

class RegionStats : public Region
{

	private :
		RealFeature m1, m2, m3, m4, minPixel, maxPixel;

	public :
		//Constructors
		RegionStats();
		RegionStats(const time_t& date_obs);
		RegionStats(const time_t& date_obs, const unsigned id, const unsigned long color = 0);

		//various routines
		void add(const PixelFeature& fv, const Coordinate& c);
		void update(const PixelFeature& fv);

		RealFeature mean() const;
		RealFeature skewness() const;
		RealFeature kurtosis() const;

	public :
		static const char * header();
		friend std::ostream& operator<<(std::ostream& out, const RegionStats& r);

};
#endif
