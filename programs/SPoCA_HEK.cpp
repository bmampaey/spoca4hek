#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <fenv.h>
#include <iomanip>
#include <sys/stat.h>

#include "../classes/tools.h"
#include "../classes/constants.h"
#include "../classes/AIAImage.h"
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

	unsigned initNumberClasses =  3, maxNumberIteration = 100;
	string preprocessingSteps = "DivMedian,DivExpTime";
	double radiusRatio = 1.31, precision = 0.001, fuzzifier = 2;
	string centersFileName, sbinSize, histogramFile;
	vector<string> sunImagesFileNames;

	vector<SunImage*> images;
	vector<RealFeature> B;
	RealFeature wavelengths, binSize;

	string programDescription = "SPoCA classification of images.\n";
	programDescription+="Compiled with options :";
	programDescription+="\nNUMBERWAVELENGTH: " + itos(NUMBERWAVELENGTH);
	programDescription+="\nDEBUG: "+ itos(DEBUG);
	programDescription+=string("\nINSTRUMENT: AIA");
	programDescription+="\nPixelType: " + string(typeid(PixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	ArgumentHelper arguments;
	arguments.new_named_unsigned_int('C', "initNumberClasses", "initNumberClasses", "The number of classes for the classification.", initNumberClasses);
	arguments.new_named_string('B',"centersFile","centersFile", "The name of the file containing the centers.\n\tIf it it not provided the centers will be initialized randomly.", centersFileName);
	arguments.new_named_string('O',"outputFile","outputFile", "The name for the output file(s).", outputFileName);
	arguments.new_named_double('r',"radiusratio","radiusratio","The ratio of the radius of the sun that will be processed",radiusRatio);
	arguments.new_named_double('p',"precision","precision","The precision to be reached to stop the SPoCA classification.",precision);
	arguments.new_named_unsigned_int('I', "maxNumberIteration", "maxNumberIteration", "The maximal number of iteration for the SPoCA classification.", maxNumberIteration);
	arguments.new_named_string('P', "preprocessingSteps", "preprocessingSteps", "The step of preprocessing to apply to the sun images.\n\tNullify above radius : NAR, ALC : Annulus Limb Correction, Division median : DivMedian, Take the square root : TakeSqrt, Take the log : TakeLog, Division by the mode : DivMode, Division by the Exposure Time : DivExpTime", preprocessingSteps);
	arguments.new_named_double('f',"fuzzifier","fuzzifier","The fuzzifier (m).",fuzzifier);
	arguments.new_named_string('H',"histogramFile","histogramFile", "The name of a file containing a histogram\n\tThe first line must be the binSize followed by the number of bins (supersedes the binSize argument).", histogramFile);
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

	//We declare the type of Classifier we want
	HistogramFCMClassifier F(fuzzifier);

	//We initialise the histogram
	if(!sbinSize.empty())
	{
		istringstream Z(sbinSize);
		Z>>binSize;
		if(Z.fail())
		{
			cerr<<"Error reading the binSize."<<endl;
		}
	}
	
	struct stat buffer ;
	if(!histogramFile.empty() && (stat(histogramFile.c_str(), &buffer) == 0)) //We make sure the file exists
	{
		F.initHistogram(histogramFile, binSize, true);
	}
	

	//We fetch the wavelength and the initial centers from the centers file
	if(! centersFileName.empty() && (stat(centersFileName.c_str(), &buffer) == 0))
	{
		readCentersFromFile(B, wavelengths, centersFileName);
		#if defined(DEBUG) && DEBUG >= 1
		if(B.size() != initNumberClasses)
		{
			cerr<<"Error : The initial number of classes is different than the number of centers read in the center file."<<endl;
			return EXIT_FAILURE;
		}
		#endif
	}
	

	//We read and preprocess the sun images
	images = getImagesFromFiles("AIA", sunImagesFileNames, true);
	for (unsigned p = 0; p < images.size(); ++p)
	{
		images[p]->preprocessing(preprocessingSteps, radiusRatio);
		#if defined(DEBUG) && DEBUG >= 2
		images[p]->writeFitsImage(outputFileName + "preprocessed."+sunImagesFileNames[p].substr(sunImagesFileNames[p].rfind('/')!=string::npos?sunImagesFileNames[p].rfind('/')+1:0));
		#endif
	}


	if(B.size() == initNumberClasses)
	{
		//The order of the sun images must follow the order of the wavelengths
		ordonateImages(images, wavelengths);
	}
	else
	{
		cerr<<"Centers will be initialised ramdomly."<<endl;
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

	//We save the histogram for the next run
	if(!histogramFile.empty())
	{
		F.saveHistogram(histogramFile, binSize);
	}

	//We save the AR map for tracking
	F.saveARmap(images[0]);
	
		//We save the Histogram
	if(!histogramFile.empty())
	{
		F.saveHistogram(histogramFile, binSize);
	}
	
	return EXIT_SUCCESS;
}
