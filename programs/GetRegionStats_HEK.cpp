#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <fenv.h>
#include <iomanip>

#include "../classes/tools.h"
#include "../classes/constants.h"
#include "../classes/mainutilities.h"
#include "../classes/ArgumentHelper.h"

#include "../classes/SunImage.h"
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
	
	// The list of names of the sun images to process
	string imageType = "AIA";
	vector<string> sunImagesFileNames;

	// Options for the preprocessing of images
	string preprocessingSteps = "NAR";
	double radiusRatio = 0.95;
	

	// The map of colored regions
	string colorizedComponentsMapFileName;

	
	string programDescription = "This Programm output regions info and statistics.\n";
	programDescription+="Compiled with options :";
	programDescription+="\nNUMBERWAVELENGTH: " + itos(NUMBERWAVELENGTH);
	programDescription+="\nDEBUG: "+ itos(DEBUG);
	programDescription+="\nPixelType: " + string(typeid(PixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	ArgumentHelper arguments;
	arguments.new_named_string('I', "imageType","string", "\n\tThe type of the images.\n\tPossible values are : EIT, EUVI, AIA, SWAP\n\t", imageType);
	arguments.new_named_string('P', "preprocessingSteps", "comma separated list of string (no spaces)", "\n\tThe steps of preprocessing to apply to the sun images.\n\tPossible values :\n\t\tNAR (Nullify above radius)\n\t\tALC (Annulus Limb Correction)\n\t\tDivMedian (Division by the median)\n\t\tTakeSqrt (Take the square root)\n\t\tTakeLog (Take the log)\n\t\tDivMode (Division by the mode)\n\t\tDivExpTime (Division by the Exposure Time)\n\t", preprocessingSteps);
	arguments.new_named_double('r', "radiusratio", "positive real", "\n\tThe ratio of the radius of the sun that will be processed.\n\t",radiusRatio);
	arguments.new_named_string('M',"colorizedComponentsMap","file name", "\n\tA colorized Components Map of regions (i.e. each one must have a different color).\n\t", colorizedComponentsMapFileName);
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
	vector<SunImage*> images = getImagesFromFiles(imageType, sunImagesFileNames, false);
	for (unsigned p = 0; p < images.size(); ++p)
	{
		images[p]->preprocessing(preprocessingSteps, radiusRatio);
		#if defined(DEBUG) && DEBUG >= 2
		images[p]->writeFitsImage(outputFileName + "preprocessed."+sunImagesFileNames[p].substr(sunImagesFileNames[p].rfind('/')!=string::npos?sunImagesFileNames[p].rfind('/')+1:0));
		#endif
	}

	
	SunImage* colorizedComponentsMap = getImageFromFile(imageType, colorizedComponentsMapFileName);
	
	
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
		if( abs(1. - (images[p]->SunRadius() / sunRadius)) > 0.01 )
		{
			cerr<<"Error : Image "<<sunImagesFileNames[p]<<" does not have the same sun radius than image "<<colorizedComponentsMapFileName<<endl;
			exit(EXIT_FAILURE);
		}
	}
	#endif

	colorizedComponentsMap->nullifyAboveRadius(1);

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
