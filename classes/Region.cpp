#include "Region.h"

using namespace std;

Region::Region()
:id(0),date_obs(0),color(0), first(Coordinate::Max), boxmin(Coordinate::Max), boxmax(0), center(0), numberPixels(0)
{}

Region::Region(const time_t& date_obs)
:id(0),date_obs(date_obs),color(0),first(Coordinate::Max), boxmin(Coordinate::Max), boxmax(0), center(0), numberPixels(0)
{}

Region::Region(const time_t& date_obs, const unsigned id, const unsigned long color)
:id(id),date_obs(date_obs),color(color),first(Coordinate::Max), boxmin(Coordinate::Max), boxmax(0), center(0), numberPixels(0)
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
{
	return boxmin;
}

Coordinate Region::Boxmax() const
{
	return boxmax;
}

Coordinate Region::Center() const
{
	if (numberPixels > 0)
		return Coordinate(center.x/numberPixels, center.y/numberPixels);
	else
		return Coordinate::Max;
}


Coordinate Region::FirstPixel() const
{
	return first;
}


unsigned Region::NumberPixels() const
{
	return numberPixels;
}

time_t Region::ObsDate() const
{
	return date_obs;
}

int Region::DS79() const
{
	// The times in IDL are specified as the number of seconds since 1 Jan 1979 00:00:00
	tm time;
	time.tm_year = 1979;
	time.tm_mon = 0; //Because stupid c++ standard lib has the month going from 0-11
	time.tm_mday = 1;
	time.tm_hour = time.tm_min = time.tm_sec = 0;
	time_t time0 = mktime(&time);
	
	return int(difftime(date_obs, time0));
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


void Region::add(const Coordinate& pixelCoordinate)
{
	this->add(pixelCoordinate.x, pixelCoordinate.y);
}


string Region::Label() const
{
	tm * ptm;
	ptm = localtime(&date_obs);
	ostringstream ss;
	ss<<setfill('0')<<setw(4)<<ptm->tm_year<<setw(2)<<ptm->tm_mon + 1<<setw(2)<<ptm->tm_mday<<"T"<<setw(2)<<ptm->tm_hour<<setw(2)<<ptm->tm_min<<setw(2)<<ptm->tm_sec<<"_"<<id;
	return ss.str();
}


const string Region::header = "(center.x,center.y)\t(boxmin.x,boxmin.y)\t(boxmax.x,boxmax.y)\tid\tnumberPixels\tlabel\tdate_obs\tcolor";

ostream& operator<<(ostream& out, const Region& r)
{
	
	out<<r.Center()<<"\t"<<r.Boxmin()<<"\t"<<r.Boxmax()<<"\t"<<r.Id()<<"\t"<<r.NumberPixels()<<"\t"<<r.Label()<<"\t"<<r.DS79()<<"\t"<<r.Color();
	return out;
}

// Extraction of the regions from a connected component colored Map
vector<Region*> getRegions(const SunImage* colorizedComponentsMap)
{
	vector<Region*> regions;

	unsigned id = 0;
	
	//Let's get the connected regions
	for (unsigned y = 0; y < colorizedComponentsMap->Yaxes(); ++y)
	{
		for (unsigned x = 0; x < colorizedComponentsMap->Xaxes(); ++x)
		{
			if(colorizedComponentsMap->pixel(x,y) != colorizedComponentsMap->nullvalue)
			{
				unsigned color = unsigned(colorizedComponentsMap->pixel(x,y));
				
				//We check the array size before
				if(color >= regions.size())
					regions.resize(color + 100, NULL);
					
				// If the regions does not yet exist we create it
				if (!regions[color])
				{
					regions[color] = new Region(colorizedComponentsMap->ObsDate(),id, 0);
					++id;
				}
				
				// We add the pixel to the region
				regions[color]->add(Coordinate(x,y));
			}
		}

	}
	

	//We cleanup the null regions
	vector<Region*>::iterator r1 = regions.begin();
	while (r1 != regions.end())
	{
		if(!(*r1))
		{
			vector<Region*>::iterator r2 = r1;
			while( r2 != regions.end() && !(*r2))
				++r2;
			r1 = regions.erase(r1,r2);
		}
		else
			++r1;
	}

	return regions;

}


