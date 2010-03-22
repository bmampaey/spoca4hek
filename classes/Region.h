#pragma once
#ifndef Region_H
#define Region_H

#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>

#include "constants.h"
#include "Coordinate.h"
#include "SunImage.h"

class Region
{
	protected :
		unsigned  id;
		time_t date_obs;
		unsigned long color;
		Coordinate first, boxmin, boxmax, center;
		unsigned numberPixels;
		
	protected :
		//Update routines
		void add(const unsigned& x, const unsigned& y);
		void add(const Coordinate& pixelCoordinate);

	public :
		//Constructors
		Region();
		Region(const time_t& date_obs);
		Region(const time_t& date_obs, const unsigned id, const unsigned long color = 0);

		//accessor and operators
		bool operator==(const Region& r)const;
		unsigned  Id() const;
		void setId(const unsigned& id);
		unsigned long Color() const;
		void setColor(const unsigned long& color);
		Coordinate Boxmin() const;
		Coordinate Boxmax() const;
		Coordinate Center() const;
		Coordinate FirstPixel() const;
		unsigned NumberPixels() const;
		time_t ObsDate() const;
		std::string Label() const;
		
		// The date as the number of seconds since 1 Jan 1979 00:00:00 (for IDL)
		int DS79() const;

	public :

		static const std::string header;
		friend std::ostream& operator<<(std::ostream& out, const Region& r);
		friend std::vector<Region*> getRegions(const SunImage* colorizedComponentsMap);

};

std::vector<Region*> getRegions(const SunImage* colorizedComponentsMap);

#endif
