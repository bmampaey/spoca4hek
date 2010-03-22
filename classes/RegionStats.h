#pragma once
#ifndef RegionStats_H
#define RegionStats_H

#include <limits>
#include <cmath>
#include <iostream>
#include "constants.h"
#include "Region.h"
#include "Coordinate.h"
#include "SunImage.h"

class RegionStats : public Region
{

	private :
		Real m1, m2, m3, m4, minIntensity, maxIntensity, totalIntensity, area_Raw, area_RawUncert, area_AtDiskCenter, area_AtDiskCenterUncert, numberContourPixels;
		
	private :
		//Update routines
		void add(const Coordinate& pixelCoordinate, const PixelType& pixelIntensity, const Coordinate relativePixelCoordinate, const bool atBorder, const double R);
		void update(const PixelType& pixelIntensity);


	public :
		//Constructors
		RegionStats();
		RegionStats(const time_t& date_obs);
		RegionStats(const time_t& date_obs, const unsigned id, const unsigned long color = 0);


		Real Mean() const;
		Real Variance() const;
		Real Skewness() const;
		Real Kurtosis() const;
		Real MinIntensity() const;
		Real MaxIntensity() const;
		Real TotalIntensity() const;
		Real Area_Raw() const;
		Real Area_RawUncert() const;
		Real Area_AtDiskCenter() const;
		Real Area_AtDiskCenterUncert() const;

	public :
		static const std::string header;
		friend std::ostream& operator<<(std::ostream& out, const RegionStats& r);
		friend std::vector<RegionStats*> getRegions(const SunImage* colorizedComponentsMap, const SunImage* image);

};

std::vector<RegionStats*> getRegions(const SunImage* colorizedComponentsMap, const SunImage* image);

#endif
