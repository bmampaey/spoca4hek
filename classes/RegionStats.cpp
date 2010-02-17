#include "RegionStats.h"

using namespace std;

RegionStats::RegionStats()
:Region(), m1(0), m2(0), m3(0), m4(0), minPixel(0), maxPixel(0)
{}

RegionStats::RegionStats(const time_t& date_obs)
:Region(date_obs), m1(0), m2(0), m3(0), m4(0), minPixel(0), maxPixel(0)
{}

RegionStats::RegionStats(const time_t& date_obs, const unsigned id, const unsigned long color)
:Region(date_obs, id, color), m1(0), m2(0), m3(0), m4(0), minPixel(0), maxPixel(0)
{}

void RegionStats::add(const PixelFeature& fv, const Coordinate& c)
{
	Region::add(c);
	m1 += fv;
	if( maxPixel < fv )
		minPixel = maxPixel = fv;
}


void RegionStats::update(const PixelFeature& fv)
{
	RealFeature delta = fv - (m1 / numberPixels);
	RealFeature delta2 = delta * delta;
	m2 += delta2;
	m4 += delta2 * delta2;
	m3 += delta2 * delta;
	if( fv < minPixel)
		minPixel = fv;
}


RealFeature RegionStats::mean() const
{
	if (numberPixels > 0)
		return m1/numberPixels;
	else
		return 0;
}


RealFeature RegionStats::skewness() const
{
	if (numberPixels > 0)
		return m3 / sqrt(m2 * m2 * m2);
	else
		return 0;
}


RealFeature RegionStats::kurtosis() const
{
	if (numberPixels > 0)
		return ( m4 / (m2 * m2) ) - 3;
	else
		return 0;
}


const char * RegionStats::header()
{return "id\tcolor\tdate_obs\tnumberPixels\t(1stPix.x,\t1stPix.y)\t(boxmin.x,\tboxmin.y)\t(boxmax.x,\tboxmax.y)\t(center.x,\tcenter.y)""\tminPixel\tmaxPixel\tmean";}

ostream& operator<<(ostream& out, const RegionStats& r)
{
	out<<Region(r)<<"\t"<<r.minPixel<<"\t"<<r.maxPixel<<"\t"<<r.mean();
	return out;
}
