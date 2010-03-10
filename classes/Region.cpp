#include "Region.h"

using namespace std;

Region::Region()
:id(0),date_obs(0),color(0), first(C::Max), boxmin(C::Max), boxmax(0), center(0), numberPixels(0)
{}

Region::Region(const time_t& date_obs)
:id(0),date_obs(date_obs),color(0),first(C::Max), boxmin(C::Max), boxmax(0), center(0), numberPixels(0)
{}

Region::Region(const time_t& date_obs, const unsigned id, const unsigned long color)
:id(id),date_obs(date_obs),color(color),first(C::Max), boxmin(C::Max), boxmax(0), center(0), numberPixels(0)
{}

bool Region::operator==(const Region& r)const
{return date_obs == r.date_obs && id == r.id;}

unsigned  Region::Id() const
{return id;}

void Region::setId(const unsigned& id)
{this->id = id;}

unsigned long Region::Color() const
{
	return color;
}


void Region::setColor(const unsigned long& color)
{
	this->color = color;
}


Coordinate Region::Boxmin() const
{return boxmin;}

Coordinate Region::Boxmax() const
{return boxmax;}

Coordinate Region::Center() const
{
	if (numberPixels > 0)
		return Coordinate(center.x/numberPixels, center.y/numberPixels);
	else
		return C::Max;
}


Coordinate Region::FirstPixel() const
{
	return first;
}


unsigned Region::size() const
{
	return numberPixels;
}

time_t Region::DateObs() const
{
	return date_obs;
}

void Region::add(const unsigned& x, const unsigned& y)
{
	if( y < first.y || (y == first.y && x < first.x))
	{
		first.y = y;
		first.x = x;
	}
	boxmin.x = x < boxmin.x ? x : boxmin.x;
	boxmin.y = y < boxmin.y ? y : boxmin.y;
	boxmax.x = x > boxmax.x ? x : boxmax.x;
	boxmax.y = y > boxmax.y ? y : boxmax.y;
	center.x += x;
	center.y += y;
	++ numberPixels;
}


void Region::add(const Coordinate& c)
{
	this->add(c.x, c.y);
}


string Region::Label() const
{
	tm * ptm;
	ptm = gmtime(&date_obs);
	ostringstream ss;
	ss<<setfill('0')<<setw(4)<<ptm->tm_year<<setw(2)<<ptm->tm_mon + 1<<setw(2)<<ptm->tm_mday<<"T"<<setw(2)<<ptm->tm_hour + 1<<setw(2)<<ptm->tm_min<<setw(2)<<ptm->tm_sec<<"_"<<id;
	return ss.str();
}


const char * Region::header ()
{return "id\tcolor\tdate_obs\tnumberPixels\t(1stPix.x,\t1stPix.y)\t(boxmin.x,\tboxmin.y)\t(boxmax.x,\tboxmax.y)\t(center.x,\tcenter.y)";}

ostream& operator<<(ostream& out, const Region& r)
{
	out<<r.id<<"\t"<<r.color<<"\t"<<r.date_obs<<"\t"<<r.numberPixels<<"\t"
		<<r.first<<"\t"<<r.boxmin<<"\t"<<r.boxmax<<"\t"<<r.center;
	return out;
}


istream& operator>>(istream& in, Region& r)
{
	char separator;
	in>>r.id>>separator>>r.color>>separator>>r.date_obs>>separator>>r.numberPixels>>separator
		>>r.first>>separator>>r.boxmin>>separator>>r.boxmax>>separator>>r.center;
	return in;
}
