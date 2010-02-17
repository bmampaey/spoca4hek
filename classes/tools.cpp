#include "tools.h"

using namespace std;

ostream& operator<<(ostream& out, const vector<Real>& v)
{
	if (v.size() > 0)
	{
		out<<v.at(0);
		for (unsigned i = 1 ; i < v.size() ; ++i)
			out<<"\t"<<v.at(i);

	}
	else
	{
		out<<"Empty vector";
	}

	return out;
}


istream& operator>>(istream& in, vector<Real>& v)
{
	v.resize(0);
	char separator;
	Real value;
	in>>value;
	while (in.good())
	{
		v.push_back(value);
		in>>separator>>value;
	}
	v.push_back(value);
	return in;

}


string itos(const int& i)
{
	ostringstream ss;
	ss << i;
	return ss.str();
}


ostream& operator<<(ostream& out, const Coordinate& c)
{
	out<<"("<<c.x<<","<<c.y<<")";
	return out;
}


istream& operator>>(istream& in, Coordinate& c)
{
	char separator;
	in>>separator>>c.x>>separator>>c.y>>separator;
	return in;
}
