#pragma once
#ifndef Region_H
#define Region_H

#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <string>

#include "Coordinate.h"

class Region
{
	protected :
		unsigned  id;
		time_t date_obs;
		unsigned long color;
		Coordinate first, boxmin, boxmax, center;
		unsigned numberPixels;

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
		unsigned size() const;
		time_t ObsDate() const;
		
		//various routines
		void add(const unsigned& x, const unsigned& y);
		void add(const Coordinate& c);
		std::string Label() const;

	public :

		static const char * header();
		friend std::ostream& operator<<(std::ostream& out, const Region& r);
		friend std::istream& operator>>(std::istream& in, Region& r);

};
#endif
