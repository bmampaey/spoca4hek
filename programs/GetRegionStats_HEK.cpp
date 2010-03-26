#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <fenv.h>
#include <iomanip>

#include "../classes/tools.h"
#include "../classes/constants.h"
#include "../classes/SunImage.h"
#include "../dsr/ArgumentHelper.h"
#include "../classes/MainUtilities.h"
#include "../classes/RegionStats.h"
#include "../classes/Coordinate.h"
#include "../classes/FeatureVector.h"

using namespace std;
using namespace dsr;

string outputFileName;



int main(int argc, const char **argv)
{
	#if defined(DEBUG) && DEBUG >= 1
	feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
	cout<<setiosflags(ios::fixed);
	#endif
	
	unsigned preprocessingType = 1;
	double radiusRatio = 1.31;
	string colorizedComponentsMapFileName;
	vector<string> sunImagesFileNames;
	
	vector<SunImage*> images;
	SunImage* colorizedComponentsMap;
	
	string programDescription = "This Programm output regions info and statistics.\n";
	programDescription+="Compiled with options :";
	programDescription+="\nNUMBERWAVELENGTH: " + itos(NUMBERWAVELENGTH);
	programDescription+="\nDEBUG: "+ itos(DEBUG);
	programDescription+=string("\nINSTRUMENT: ") + instruments[INSTRUMENT];
	programDescription+=string("\nLIMB_CORRECTION: ") + limb_corrections[LIMB_CORRECTION];
	programDescription+="\nPixelType: " + string(typeid(PixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	ArgumentHelper arguments;
	arguments.new_named_double('r',"radiusratio","radiusratio","The ratio of the radius of the sun that will be processed",radiusRatio);
	arguments.new_named_string('M',"colorizedComponentsMap","colorizedComponentsMap", "The name of the file containing a colorizedComponentsMap of regions (i.e. each one must have a different color).", colorizedComponentsMapFileName);
	arguments.new_named_unsigned_int('P', "preprocessingType", "preprocessingType", "The type of preprocessing to apply to the sun images.\n\tNo preprocessing = 0, AnnulusLimbCorrection(ALC) = 1, ALC+TakeLog = 2, ALC+DivMedian = 3, ALC+TakeSqrt = 4, ALC+DivMode = 5", preprocessingType);	
	arguments.set_string_vector("fitsFileName1 fitsFileName2 ...", "The name of the fits files containing the images of the sun.", sunImagesFileNames);

	arguments.set_description(programDescription.c_str());
	arguments.set_author("Benjamin Mampaey, benjamin.mampaey@sidc.be");
	arguments.set_build_date(__DATE__);
	arguments.set_version("1.0");
	arguments.process(argc, argv);

	if(sunImagesFileNames.size() < 1 )
	{
		cerr<<"No fits image file given as parameter!"<<endl;
		return EXIT_FAILURE;
	}
	
	//We read and preprocess the sun images
	fetchImagesFromFile(images, sunImagesFileNames, preprocessingType, radiusRatio);

	//We do the same for the colorizedComponentsMap
	#if defined(DEBUG) && DEBUG >= 1
		if(colorizedComponentsMapFileName.find(".fits")==string::npos && colorizedComponentsMapFileName.find(".fts")==string::npos)
		{
			cerr<<colorizedComponentsMapFileName<<" is not a fits file! (must end in .fits or .fts)"<<endl;
		}
	#endif
	
	colorizedComponentsMap = new SunImage(colorizedComponentsMapFileName);
	
	#if defined(DEBUG) && DEBUG >= 1

		
	Coordinate sunCenter = colorizedComponentsMap->SunCenter();
	double sunRadius = colorizedComponentsMap->SunRadius();

	for (unsigned p = 0; p < sunImagesFileNames.size(); ++p)
	{
		if( sunCenter.d2(images[p]->SunCenter()) > 2 )
		{
			cerr<<"Warning : Image "<<sunImagesFileNames[p]<<" will be recentered to have the same sun centre than image "<<colorizedComponentsMapFileName<<endl;
			images[p]->recenter(sunCenter);
		}
		if( abs(images[p]->SunRadius() - sunRadius) > 1 )
		{
			cerr<<"Error : Image "<<sunImagesFileNames[p]<<" does not have the same sun radius than image "<<colorizedComponentsMapFileName<<endl;
			exit(EXIT_FAILURE);
		}
	}
	#endif

	colorizedComponentsMap->nullifyAboveRadius(radiusRatio);

	for (unsigned p = 0; p < sunImagesFileNames.size(); ++p)
	{
		vector<RegionStats*> regions = getRegions(colorizedComponentsMap, images[p]);
		for (unsigned r = 0; r < regions.size(); ++r)
		{
			cout<<*(regions[r])<<endl;
			delete regions[r];
		}
		delete images[p];
	}

	return EXIT_SUCCESS;
}
