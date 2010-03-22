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
#include "../classes/HistogramFCMClassifier.h"
#include "../classes/FeatureVector.h"
#include "../dsr/ArgumentHelper.h"
#include "../classes/MainUtilities.h"
#include "../classes/Coordinate.h"


using namespace std;
using namespace dsr;
//This program will classify a pair of images using PCM2
// The output on the standar output will be out of limb AR center and boxes corner
// And a map of AR for tracking 

string outputFileName;

int main(int argc, const char **argv)
{
	#if defined(DEBUG) && DEBUG >= 1
	feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
	cout<<setiosflags(ios::fixed);
	#endif

	unsigned initNumberClasses =  3, maxNumberIteration = 100, preprocessingType = 1;
	double radiusRatio = 1.31, precision = 0.001, fuzzifier = 2;
	string centersFileName, sbinSize;
	vector<string> sunImagesFileNames;

	vector<SunImage*> images;
	vector<RealFeature> B;
	RealFeature wavelengths, binSize;

	string programDescription = "SPoCA classification of images.\n";
	programDescription+="Compiled with options :";
	programDescription+="\nNUMBERWAVELENGTH: " + itos(NUMBERWAVELENGTH);
	programDescription+="\nDEBUG: "+ itos(DEBUG);
	programDescription+=string("\nINSTRUMENT: ") + instruments[INSTRUMENT];
	programDescription+=string("\nLIMB_CORRECTION: ") + limb_corrections[LIMB_CORRECTION];
	programDescription+="\nPixelType: " + string(typeid(PixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	ArgumentHelper arguments;
	arguments.new_named_unsigned_int('C', "initNumberClasses", "initNumberClasses", "The number of classes for the classification.", initNumberClasses);
	arguments.new_named_string('B',"centersFile","centersFile", "The name of the file containing the centers.\n\tIf it it not provided the centers will be initialized randomly.", centersFileName);
	arguments.new_named_string('O',"outputFile","outputFile", "The name for the output file(s).", outputFileName);
	arguments.new_named_double('r',"radiusratio","radiusratio","The ratio of the radius of the sun that will be processed",radiusRatio);
	arguments.new_named_double('p',"precision","precision","The precision to be reached to stop the SPoCA classification.",precision);
	arguments.new_named_unsigned_int('I', "maxNumberIteration", "maxNumberIteration", "The maximal number of iteration for the SPoCA classification.", maxNumberIteration);
	arguments.new_named_unsigned_int('P', "preprocessingType", "preprocessingType", "The type of preprocessing to apply to the sun images.\n\tNo preprocessing = 0, AnnulusLimbCorrection(ALC) = 1, ALC+TakeLog = 2, ALC+DivMedian = 3, ALC+TakeSqrt = 4, ALC+DivMode = 5", preprocessingType);
	arguments.new_named_double('f',"fuzzifier","fuzzifier","The fuzzifier (m).",fuzzifier);
	arguments.new_named_string('z',"binSize","binSize", "The width of the bin.\n\tList of number separated by commas, and no spaces. ex -z 1.2,1.3", sbinSize);
	arguments.set_string_vector("fitsFileName1 fitsFileName2 ...", "The name of the fits files containing the images of the sun.", sunImagesFileNames);
	arguments.set_description(programDescription.c_str());
	arguments.set_author("Benjamin Mampaey, benjamin.mampaey@sidc.be");
	arguments.set_build_date(__DATE__);
	arguments.set_version("1.0");
	arguments.process(argc, argv);

	//Let's set the name of output files
	//If none as been provided as a program argument, we set it to the executable file name minus the last .extension
	if(outputFileName.empty())
	{
		outputFileName = argv[0];
		outputFileName = outputFileName.substr(outputFileName.rfind('/')!=string::npos?outputFileName.rfind('/')+1:0);
		outputFileName = outputFileName.substr(0, outputFileName.rfind('.'));
	}
	outputFileName += ".";
	//We assert that the number of sun images provided is correct
	if(sunImagesFileNames.size() != NUMBERWAVELENGTH)
	{
		cerr<<"Error : "<<sunImagesFileNames.size()<<" fits image file given as parameter, "<<NUMBERWAVELENGTH<<" must be given!"<<endl;
		return EXIT_FAILURE;
	}
	
	if(sbinSize.empty())
	{
		cerr<<"No binSize given as parameter."<<endl;
		return EXIT_FAILURE;
	}
	else
	{
		istringstream Z(sbinSize);
		Z>>binSize;
		if(Z.fail())
		{
			cerr<<"Error reading the binSize."<<endl;
			return EXIT_FAILURE;
		}
	}

	//We read and preprocess the sun images
	fetchImagesFromFile(images, sunImagesFileNames, preprocessingType, radiusRatio);

	//We declare the type of Classifier we want
	HistogramFCMClassifier F(fuzzifier);

	//We initialise the initial centers initB from the centers file
	if(! centersFileName.empty())
	{
		readCentersFromFile(B, wavelengths, centersFileName);
		if(B.size() != initNumberClasses)
		{
			cerr<<"Error : The initial number of classes is different than the number of centers read in the center file."<<endl;
			for (unsigned p = 0; p< NUMBERWAVELENGTH; ++p)
				wavelengths.v[p] = images[p]->Wavelength();
		}
		else //The order of the sun images must follow the order of the wavelengths
		{
			ordonateImages(images, wavelengths);
		}

	}
	else
	{
		#if defined(DEBUG) && DEBUG >= 3
		cout<<"No centers file given as parameter, centers will be initialised ramdomly."<<endl;
		#endif
		for (unsigned p = 0; p< NUMBERWAVELENGTH; ++p)
			wavelengths.v[p] = images[p]->Wavelength();

	}

	//We add the images to the classifier
	F.addImages(images, binSize);

	if(B.size() == initNumberClasses)
	{
		F.init(B);
	}
	else
	{
		F.randomInit(initNumberClasses);
	}
	
	#if defined(DEBUG) && DEBUG >= 3
		cout<<"The centers have been initialized to B :"<<F.getB()<<endl;
	#endif
	
	
	
	//We have all the information we need, we can do the classification
	F.classification(precision, maxNumberIteration);


	//We save the centers for the next run (for PCM and derivatives, must be done before classification)
	
	ofstream centersFile(centersFileName.c_str());
	if (centersFile.good())
	{
		centersFile<<wavelengths<<"\t"<<F.getB()<<endl;
		centersFile.close();
	}

	#if defined(DEBUG) && DEBUG >= 2
	//We save all the results
	F.saveResults(images[0]);
	#endif

	//We save the AR map for tracking
	F.saveARmap(images[0]);
	
	return EXIT_SUCCESS;
}
