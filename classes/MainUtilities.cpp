#include "MainUtilities.h"

using namespace std;

void readCentersFromFile(vector<RealFeature>& B, RealFeature& wavelengths, const string& centersFileName)
{
	ifstream centersFile(centersFileName.c_str());
	if (centersFile.is_open())
	{
		centersFile>>wavelengths;
		RealFeature Bi;
		centersFile>>Bi;
		while(centersFile.good())
		{
			B.push_back(Bi);
			centersFile>>Bi;
		}
		centersFile.close();
	}
}


void ordonateImages(vector<SunImage*>& images, const RealFeature& wavelengths)
{
	for (unsigned p = 0; p < NUMBERWAVELENGTH; ++p)
	{
		if(wavelengths.v[p] != images[p]->Wavelength())
		{
			unsigned pp = p+1;
			while(pp < NUMBERWAVELENGTH && wavelengths.v[p] != images[pp]->Wavelength())
				++pp;
			if(pp < NUMBERWAVELENGTH)
			{
				SunImage* temp = images[pp];
				images[pp] = images[p];
				images[p] = temp;
			}
			else
			{

				cerr<<"Error : the wavelengths of the sun images provided do not match the wavelengths of the centers file!"<<endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}


void fetchImagesFromFile(vector<SunImage*>& images, const vector<string>& sunImagesFileNames, const unsigned  preprocessingType, const double radiusRatio)
{
	for (unsigned p = 0; p < sunImagesFileNames.size(); ++p)
	{
		#if defined(DEBUG) && DEBUG >= 1
		if(sunImagesFileNames[p].find(".fits")==string::npos && sunImagesFileNames[p].find(".fts")==string::npos)
		{
			cerr<<sunImagesFileNames[p]<<" is not a fits file! (must end in .fits or .fts)"<<endl;
		}
		#endif
		images.push_back(new SunImage(sunImagesFileNames[p]));

	}
	for (unsigned p = 0; p < sunImagesFileNames.size(); ++p)
	{

		images[p]->preprocessing(preprocessingType,radiusRatio);

		#if defined(DEBUG) && DEBUG >= 2
		string filename = outputFileName + "preprocessed.";
		filename +=  sunImagesFileNames[p].substr(sunImagesFileNames[p].rfind('/')!=string::npos?sunImagesFileNames[p].rfind('/')+1:0);
		images[p]->writeFitsImage(filename);
		#endif

	}
}


const char * instruments[] = {"ERROR","EIT","EUVI","AIA"};
const char * limb_corrections[] = {"ERROR","VINCE_CORR","CIS1_CORR","BEN_CORR", "CIS2_CORR"};
