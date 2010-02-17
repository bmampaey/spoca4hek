#include "SunImage.h"

using namespace std;

const Real PI = 3.14159265358979323846;
const Real MIPI = 1.57079632679489661923;
const Real BIPI = 6.28318530717958647692;

SunImage::SunImage(const long xAxes, const long yAxes, const double radius, const double wavelength)
:Image<PixelType>(xAxes,yAxes),radius(radius),wavelength(wavelength),date_obs(0),median(1),datap01(0), datap95(numeric_limits<PixelType>::max())
{
	suncenter.x = xAxes/2;
	suncenter.y = yAxes/2;
	cdelt[0] = cdelt[1] = 1;
}


SunImage::SunImage(const string& fitsFileName)
:Image<PixelType>(fitsFileName),median(1),datap01(0), datap95(numeric_limits<PixelType>::max())
{
	int   status  = 0;
	fitsfile  *fptr;
	char * comment = NULL  ;					  /**<By specifying NULL we say that we don't want the comments	*/

	if (!fits_open_file(&fptr, fitsFileName.c_str(), READONLY, &status))
	{

		if (fits_read_key(fptr, TDOUBLE, "WAVELNTH", &wavelength,comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;
			cerr<<"Error reading key WAVELNTH : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		if (fits_read_key(fptr, TINT, "CRPIX1", &(suncenter.x),comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key CRPIX1 : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		if (fits_read_key(fptr, TINT, "CRPIX2", &(suncenter.y), comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key CRPIX2 : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		if (fits_read_key(fptr, TDOUBLE, "CDELT1", &(cdelt[0]), comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key CDELT1 : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		if (fits_read_key(fptr, TDOUBLE, "CDELT2", &(cdelt[1]), comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key CDELT2 : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		#if INSTRUMENT==EIT
		if (fits_read_key(fptr, TDOUBLE, "SOLAR_R", &radius, comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key SOLAR_R : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		#elif INSTRUMENT==EUVI
		if (fits_read_key(fptr, TDOUBLE, "RSUN", &radius, comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key RSUN : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		radius/=cdelt[0];
		#elif INSTRUMENT==AIA

		#else
		#error "INSTRUMENT is not defined"
		#endif

		if (fits_read_key(fptr, TSTRING, "DATE-OBS", date_obs_string, comment, &status))
		{
			//HACK for eit images
			status = 0;
			if (fits_read_key(fptr, TSTRING, "DATE_OBS", date_obs_string, comment, &status))
			{
				cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;
				cerr<<"Error reading key DATE-OBS : "<< status <<endl;
				fits_report_error(stderr, status);
				status = 0;
			}
			else
			{
				char * letter;
				if((letter = strpbrk (date_obs_string, "zZ")))
					*letter = '\0';
			}
		}
		//We convert DATE_OBS to time
		tm time;
		double seconds;
		int month;
		if (fits_str2time(date_obs_string, &(time.tm_year), &(month), &(time.tm_mday), &(time.tm_hour), &(time.tm_min), &seconds, &status))
			cerr<<"Error converting date_obs_string to time : "<< status <<endl;
		time.tm_sec = int(seconds);
		time.tm_mon = month -1;	//Because stupid c++ standard lib has the month going from 0-11
		date_obs = mktime(&time);
		
		#if INSTRUMENT!=EIT
		if (fits_read_key(fptr, datatype, "DATAP01", &datap01,comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key DATAP01 : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		if (fits_read_key(fptr, datatype, "DATAP95", &datap95,comment, &status))
		{
			cerr<<"Error reading image file "<<fitsFileName<<" :"<<endl;			
			cerr<<"Error reading key DATAP95 : "<< status <<endl;
			fits_report_error(stderr, status);
			status = 0;
		}
		#endif

		fits_close_file(fptr, &status);
	}
	else
		cerr<<"Error : Could not open fits File image "<<fitsFileName<<"."<<endl;

}


SunImage::SunImage(const SunImage& i)
:Image<PixelType>(i),radius(i.radius),wavelength(i.wavelength),date_obs(i.date_obs),suncenter(i.suncenter),median(i.median),datap01(i.datap01),datap95(i.datap95)
{
	strncpy (date_obs_string, i.date_obs_string, 80);
	cdelt[0] = i.cdelt[0];
	cdelt[1] = i.cdelt[1];
}


SunImage::SunImage(const SunImage* i)
:Image<PixelType>(i),radius(i->radius),wavelength(i->wavelength),date_obs(i->date_obs),suncenter(i->suncenter),median(i->median),datap01(i->datap01),datap95(i->datap95)
{
	strncpy (date_obs_string, i->date_obs_string, 80);
	cdelt[0] = i->cdelt[0];
	cdelt[1] = i->cdelt[1];

}


SunImage* SunImage::writeFitsImage (const string& filename)
{
	fitsfile *fptr;
	int status = 0;
	char comment[100];
	remove(filename.c_str());

	if (fits_create_file(&fptr, filename.c_str(), &status))
		cerr<<"Error : Creating file with status "<< status <<endl;;

	if ( fits_create_img(fptr,  bitpix, naxis, axes, &status) )
		cerr<<"Error : Creating image with status "<< status <<endl;;

	if ( fits_write_img(fptr, datatype, 1, numberPixels, pixels, &status) )
		cerr<<"Error : Writing image with status "<< status <<endl;;

	strcpy(comment, "naxis1");
	if ( fits_update_key(fptr, TLONG, "NAXIS1", &(axes[0]) , comment, &status) )
		cerr<<"Error : writing keyword NAXIS1 = "<< status <<endl;

	strcpy(comment, "naxis2");
	if ( fits_update_key(fptr, TLONG, "NAXIS2", &(axes[1]), comment, &status) )
		cerr<<"Error : writing keyword NAXIS2 = "<< status <<endl;

	strcpy(comment, "crpix1");
	if ( fits_update_key(fptr, TINT, "CRPIX1", &suncenter.x, comment, &status) )
		cerr<<"Error : writing keyword CRPIX1 = "<< status <<endl;

	strcpy(comment, "crpix2");
	if ( fits_update_key(fptr, TINT, "CRPIX2", &suncenter.y, comment, &status) )
		cerr<<"Error : writing keyword CRPIX2 = "<< status <<endl;

	strcpy(comment, "solar_r");
	#if INSTRUMENT==EIT
	if ( fits_update_key(fptr, TDOUBLE, "SOLAR_R", &radius, comment, &status) )
		cerr<<"Error : writing keyword SOLAR_R = "<< status <<endl;
	#elif INSTRUMENT==EUVI
	//TODO check if I have to multiply the radius by cdelt[0] again
	if ( fits_update_key(fptr, TDOUBLE, "RSUN", &radius, comment, &status) )
		cerr<<"Error : writing keyword RSUN = "<< status <<endl;

	#elif INSTRUMENT==AIA

	#else
	#error "INSTRUMENT is not defined"
	#endif

	strcpy(comment, "wavelength");
	if ( fits_update_key(fptr, TDOUBLE, "WAVELNTH", &wavelength, comment, &status) )
		cerr<<"Error : writing keyword WAVELNTH = "<< status <<endl;

	strcpy(comment, "date-obs");
	if (fits_update_key(fptr, TSTRING, "DATE_OBS", date_obs_string, comment, &status))
		cerr<<"Error : writing keyword DATE_OBS = "<< status <<endl;
	strcpy(comment, "cdelt1");
	if (fits_update_key(fptr, TDOUBLE, "CDELT1", &(cdelt[0]), comment, &status))
		cerr<<"Error : writing keyword CDELT1 : "<< status <<endl;
	strcpy(comment, "cdelt2");
	if (fits_update_key(fptr, TDOUBLE, "CDELT2", &(cdelt[1]), comment, &status))
		cerr<<"Error : writing keyword CDELT2 : "<< status <<endl;

	if ( fits_close_file(fptr, &status) )
		cerr<<"Error : Closing file with status "<< status <<endl;

	return this;
}


unsigned SunImage::numberValidPixelsEstimate() const
{return unsigned(PI*radius*radius);}

void SunImage::nullifyAboveRadius(const Real radiusRatio)
{
	Real radius2 = radiusRatio*radiusRatio*radius*radius;
	for (unsigned y=0; y < Yaxes(); ++y)
	{
		for (unsigned x=0; x < Xaxes(); ++x)
		{
			if ((x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y)> radius2)
				pixel(x,y) = nullvalue;

		}
	}
}


/* Function that returns the percentage of correction necessary for an annulus, given the percentage of the annulus radius to the radius of the sun */

#if LIMB_CORRECTION==VINCE_CORR
/*Method of Vincent
No Correction before r1, Full correction after r1 */
inline Real SunImage::percentCorrection(const Real r)const
{

	const Real r1 = 0.95;
	if (r < r1)
		return 0;
	else
		return 1;
}


#elif LIMB_CORRECTION==CIS1_CORR

/*1st Method of Cis
No Correction before r1, Full correction after r2, progressive correction in between */
inline Real SunImage::percentCorrection(const Real r)const
{

	const Real r1 = 0.90;
	const Real r2 = 0.95;
	if (r < r1)
		return 0;
	else if (r >= r1 && r <= r2)
	{
		return (r - r1)/(r2 - r1);
	}
	else
		return 1;

}


#elif LIMB_CORRECTION==BEN_CORR

/*Method of Benjamin
No correction before r1 and after r2, Full correction between r3 and r4,
progressive correction following the ascending phase of the sine between r1 and r2
progressive correction following the descending phase of the sine between r3 and r4 */

inline Real SunImage::percentCorrection(const Real r)const
{

	const Real r1 = 0.90;
	const Real r2 = 0.95;
	const Real r3 = 1.05;
	const Real r4 = 1.10;
	if (r <= r1 || r >= r4)
		return 0;
	else if (r >= r2 && r <= r3)
		return 1;
	else if (r <= r2)
	{
		Real T = - 2*(r1-r2);
		Real phi = MIPI*(r1+r2)/(r1-r2);
		return (sin((BIPI/T)*r + phi) + 1)/2;
	}
	else										  // (r => r3)
	{
		Real T = 2*(r3-r4);
		Real phi = - MIPI*(r3+r4)/(r3-r4);
		return (sin((BIPI/T)*r + phi) + 1)/2;
	}

}


#elif LIMB_CORRECTION==CIS2_CORR

/*2nd Method of Cis */
inline Real SunImage::percentCorrection(const Real r)const
{
	return exp(-0.01 * pow(100 * (r-1),4));
}


#else

inline Real SunImage::percentCorrection(const Real r)const
{
	return 0;
}
#endif

void SunImage::recenter(const Coordinate& newCenter)
{
	int delta = (suncenter.x - newCenter.x) + (suncenter.y - newCenter.y) * Xaxes();
	if(delta < 0)
	{
		memmove(pixels - delta, pixels, (numberPixels + delta + 1) * sizeof(PixelType));
		fill(pixels, pixels-delta, nullvalue);
	}
	else if (delta > 0)
	{
		memmove(pixels, pixels + delta, (numberPixels - delta + 1) * sizeof(PixelType));
		fill(pixels + numberPixels - delta, pixels + numberPixels, nullvalue);
	}
	suncenter = newCenter;
}


/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 */

#define ELEM_SWAP(a,b) { register PixelType t=(a);(a)=(b);(b)=t; }

PixelType quick_select(vector<PixelType>& arr, int n)
{
	int low, high ;
	int median;
	int middle, ll, hh;

	low = 0 ; high = n-1 ; median = (low + high) / 2;
	for (;;)
	{
		if (high <= low)						  /* One element only */
			return arr[median] ;

		if (high == low + 1)					  /* Two elements only */
		{
			if (arr[low] > arr[high])
				ELEM_SWAP(arr[low], arr[high]) ;
			return arr[median] ;
		}

		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high) / 2;
		if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
		if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
		if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP(arr[middle], arr[low+1]) ;

		/* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;
		for (;;)
		{
			do ll++; while (arr[low] > arr[ll]) ;
			do hh--; while (arr[hh]  > arr[low]) ;

			if (hh < ll)
				break;

			ELEM_SWAP(arr[ll], arr[hh]) ;
		}

		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP(arr[low], arr[hh]) ;

		/* Re-set active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
	}
}


#undef ELEM_SWAP

void SunImage::annulusLimbCorrection(Real maxLimbRadius, Real minLimbRadius)
{

	minLimbRadius *= radius;
	maxLimbRadius *= radius;
	Real minLimbRadius2 = minLimbRadius*minLimbRadius;
	Real maxLimbRadius2 = maxLimbRadius*maxLimbRadius;
	Real deltaR = 0.3;							  //This means that we consider the width of an annulus to be the size of a third of a pixel
	vector<PixelType> onDiscList;
	onDiscList.reserve(numberValidPixelsEstimate());
	vector<Real> annulusMean(unsigned((maxLimbRadius-minLimbRadius)/deltaR)+2,0);
	vector<unsigned> annulusNbrPixels(unsigned((maxLimbRadius-minLimbRadius)/deltaR)+2,0);

	PixelType* pixelValue;
	Real pixelRadius2 = 0;
	unsigned indice = 0;
	for (unsigned y=0; y < Yaxes(); ++y)
	{
		for (unsigned x=0; x < Xaxes(); ++x)
		{

			pixelValue = &pixel(x,y);
			if ((*pixelValue) != nullvalue)
			{
				pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);

				if (pixelRadius2 <=  minLimbRadius2)
				{
					onDiscList.push_back((*pixelValue));
				}
				else if (pixelRadius2 <=  maxLimbRadius2)
				{
					indice = unsigned((sqrt(pixelRadius2) - minLimbRadius)/deltaR);
					annulusMean.at(indice) = annulusMean.at(indice) + (*pixelValue);
					annulusNbrPixels.at(indice) = annulusNbrPixels.at(indice) + 1;
				}

			}
		}
	}

	median = quick_select(onDiscList, onDiscList.size());

	for (unsigned i=0; i<annulusMean.size(); ++i)
	{
		if(annulusNbrPixels.at(i)>0)
			annulusMean.at(i) = annulusMean.at(i) / Real(annulusNbrPixels.at(i));
	}
	for (unsigned y=0; y < Yaxes(); ++y)
	{
		for (unsigned x=0; x < Xaxes(); ++x)
		{
			pixelValue = &pixel(x,y);
			if ((*pixelValue) != nullvalue)
			{
				pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);

				if (pixelRadius2 < minLimbRadius2)// We don't correct
				{
				}
												  // We don't take the pixel into account
				else if (maxLimbRadius2 < pixelRadius2)
				{
					(*pixelValue) = nullvalue;
				}
				else							  //We correct the limb
				{
					Real pixelRadius = sqrt(pixelRadius2);
					Real fraction = percentCorrection(pixelRadius/radius);
					indice = unsigned((pixelRadius-minLimbRadius)/deltaR);
					(*pixelValue) = (1. - fraction) * (*pixelValue) + (fraction * (*pixelValue) * median) / annulusMean.at(indice);

				}

			}
		}
	}

}


void SunImage::preprocessing(const unsigned type, Real maxLimbRadius, Real minLimbRadius)
{
	minLimbRadius *= radius;
	maxLimbRadius *= radius;
	Real minLimbRadius2 = minLimbRadius*minLimbRadius;
	Real maxLimbRadius2 = maxLimbRadius*maxLimbRadius;
	Real deltaR = 0.3;
	vector<PixelType> onDiscList;
	onDiscList.reserve(numberValidPixelsEstimate());
	vector<Real> annulusMean(unsigned((maxLimbRadius-minLimbRadius)/deltaR)+2,0);
	vector<unsigned> annulusNbrPixels(unsigned((maxLimbRadius-minLimbRadius)/deltaR)+2,0);

	PixelType* pixelValue;
	Real pixelRadius2 = 0;
	unsigned indice = 0;
	for (unsigned y=0; y < Yaxes(); ++y)
	{
		for (unsigned x=0; x < Xaxes(); ++x)
		{

			pixelValue = &pixel(x,y);
			if ((*pixelValue) != nullvalue)
			{
				pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);

				if (pixelRadius2 <=  minLimbRadius2)
				{
					onDiscList.push_back((*pixelValue));
				}
				else if (pixelRadius2 <=  maxLimbRadius2)
				{
					indice = unsigned((sqrt(pixelRadius2)-minLimbRadius)/deltaR);
					annulusMean.at(indice) = annulusMean.at(indice) + (*pixelValue);
					annulusNbrPixels.at(indice) = annulusNbrPixels.at(indice) + 1;
				}

			}
		}
	}

	median = quick_select(onDiscList, onDiscList.size());

	for (unsigned i=0; i<annulusMean.size(); ++i)
	{
		if(annulusNbrPixels.at(i)>0)
			annulusMean.at(i) = annulusMean.at(i) / Real(annulusNbrPixels.at(i));
	}

	if(type == DivMedian)
	{
		for (unsigned y=0; y < Yaxes(); ++y)
		{
			for (unsigned x=0; x < Xaxes(); ++x)
			{
				pixelValue = &pixel(x,y);
				if ((*pixelValue) != nullvalue)
				{
					pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);
												  //Division of the picture by its median value
					if (pixelRadius2 < minLimbRadius2)
					{
						(*pixelValue) = ((*pixelValue) / median);
					}
												  // We don't take the pixel into account
					else if (maxLimbRadius2 < pixelRadius2)
					{
						(*pixelValue) = nullvalue;
					}
					else						  //We correct the limb
					{
						Real pixelRadius = sqrt(pixelRadius2);
						Real fraction = percentCorrection(pixelRadius/radius);
						indice = unsigned((pixelRadius-minLimbRadius)/deltaR);
						(*pixelValue) = (1. - fraction) * (*pixelValue) / median + (fraction * (*pixelValue)) / annulusMean.at(indice);

					}

				}
			}
		}
	}
	else if(type == TakeLog)
	{
		for (unsigned y=0; y < Yaxes(); ++y)
		{
			for (unsigned x=0; x < Xaxes(); ++x)
			{
				pixelValue = &pixel(x,y);
				if ((*pixelValue) != nullvalue)
				{
					pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);
												  //We take the log of the picture
					if (pixelRadius2 < minLimbRadius2)
					{
						(*pixelValue) = log((*pixelValue));
					}
												  // We don't take the pixel into account
					else if (maxLimbRadius2 < pixelRadius2)
					{
						(*pixelValue) = nullvalue;
					}
					else						  //We correct the limb
					{
						Real pixelRadius = sqrt(pixelRadius2);
						Real fraction = percentCorrection(pixelRadius/radius);
						indice = unsigned((pixelRadius-minLimbRadius)/deltaR);
						(*pixelValue) = log(((1. - fraction) * (*pixelValue) + (fraction * (*pixelValue) * median) / annulusMean.at(indice)));

					}

				}
			}
		}
	}
	else if(type == TakeSqrt)
		for (unsigned y=0; y < Yaxes(); ++y)
	{
		for (unsigned x=0; x < Xaxes(); ++x)
		{
			pixelValue = &pixel(x,y);
			if ((*pixelValue) != nullvalue)
			{
				pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);
				if (pixelRadius2 < minLimbRadius2)//We take the sqrt of the picture
				{
					(*pixelValue) = sqrt((*pixelValue));
				}
												  // We don't take the pixel into account
				else if (maxLimbRadius2 < pixelRadius2)
				{
					(*pixelValue) = nullvalue;
				}
				else							  //We correct the limb
				{
					Real pixelRadius = sqrt(pixelRadius2);
					Real fraction = percentCorrection(pixelRadius/radius);
					indice = unsigned((pixelRadius-minLimbRadius)/deltaR);
					(*pixelValue) = sqrt(((1. - fraction) * (*pixelValue) + (fraction * (*pixelValue) * median) / annulusMean.at(indice)));

				}

			}
		}
	}
	else
		for (unsigned y=0; y < Yaxes(); ++y)
	{
		for (unsigned x=0; x < Xaxes(); ++x)
		{
			pixelValue = &pixel(x,y);
			if ((*pixelValue) != nullvalue)
			{
				pixelRadius2 = (x-suncenter.x)*(x-suncenter.x) + (y-suncenter.y)*(y-suncenter.y);

				if (pixelRadius2 < minLimbRadius2)// We don't correct
				{
				}
												  // We don't take the pixel into account
				else if (maxLimbRadius2 < pixelRadius2)
				{
					(*pixelValue) = nullvalue;
				}
				else							  //We correct the limb
				{
					Real pixelRadius = sqrt(pixelRadius2);
					Real fraction = percentCorrection(pixelRadius/radius);
					indice = unsigned((pixelRadius-minLimbRadius)/deltaR);
					(*pixelValue) = (1. - fraction) * (*pixelValue) + (fraction * (*pixelValue) * median) / annulusMean.at(indice);

				}

			}
		}
	}

}


void SunImage::copyKeywords(const SunImage* i)
{
	radius = i->radius;
	suncenter = i->suncenter;
	wavelength = i->wavelength;
	median = i->median;
	date_obs = i->date_obs;
	datap01=i->datap01;
	datap95=i->datap95;
	cdelt[0]=i->cdelt[0];
	cdelt[1]=i->cdelt[1];
}


//calculates the differential solar rotation speed for a given pixel
// Formula coming from Wikipedia, should be verified
inline Real SunImage::angularSpeed(Real latitude)
{
	const Real A = 14.713;
	const Real B = -2.396;
	const Real C =  -1.787;
	latitude *= latitude;
	Real result = A + B * latitude + C * latitude * latitude;
	return result * (PI / (24. * 3600. * 180.));
}


inline unsigned SunImage::newPos(Real x, Real y, const Real t)
{
	x = suncenter.x - x;
	y = suncenter.y - y;
	Real latitude = y / radius;
	Real omega = angularSpeed(latitude);
	Real alpha = omega * t;
	Real r = 1;
	Real teta = asin(x / sqrt(x * x + r * r));
	Real delta = r * sin(teta - alpha);
	return suncenter.x - delta;
}


SunImage* SunImage::rotate(const unsigned t)
{
	SunImage * img = new SunImage(axes[0], axes[1], radius, wavelength);
	vector<Real> radiusAtLatitude (radius + 1, 0);
	vector<unsigned> left ( radius, Xaxes());
	vector<unsigned> right ( radius, 0);

	Image<Real> Phi (Xaxes(), radius + 1);
	Phi.zero(MIPI);
	for (unsigned latitude = 0; latitude < radius; ++latitude)
	{
		radiusAtLatitude[latitude] = sqrt((radius * radius) - (latitude * latitude));
		left[latitude] = suncenter.x - radiusAtLatitude[latitude] ;
		right[latitude] = suncenter.x + radiusAtLatitude[latitude];
		for (int longitude = floor(1. - radiusAtLatitude[latitude]); longitude <=  ceil(radiusAtLatitude[latitude] - 1.); ++longitude)
		{
			Real r1 = Real(latitude) / Real(radius);
			Phi.pixel(suncenter.x + longitude,latitude)=asin(Real(longitude) /(sqrt(1 - r1 * r1) * Real(radius)));
		}
	}
	Phi.writeFitsImage("phi.fits");

	cout<<"left\tright:"<<endl;
	for (unsigned i = 0; i < left.size(); ++i)
		cout<<left[i]<<"\t"<<right[i]<<endl;

	Image<Real> PhiRotated (Xaxes(), radius + 1);
	for (unsigned latitude = 0; latitude < radius; ++latitude)
	{
		for (unsigned longitude = left[latitude]; longitude <=  right[latitude]; ++longitude)
		{
			PhiRotated.pixel(longitude,latitude)=Phi.pixel(longitude,latitude) + angularSpeed(Real(latitude)/radius) * t;
		}
	}
	PhiRotated.writeFitsImage("PhiRotated.fits");
	/*

	img->zero();
	Real newx;
	for(int y = 0; y <= radius ; ++y)
	{
		for(int x = -radiusAtLatitude(y); x <= radiusAtLatitude(r); ++x)
		{
			newx = newPos(x,y,t);
			img->pixel(x,y) = pixel(newx,y);
			img->pixel(x,-y) = pixel(newx,-y);
	}
	}*/

	return img;

}
