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


vector<SunImage*> getImagesFromFiles(string type, const vector<string>& sunImagesFileNames, bool align)
{

	vector<SunImage*> images;

	// We read the files
	for (unsigned p = 0; p < sunImagesFileNames.size(); ++p)
	{
		#if defined(DEBUG) && DEBUG >= 1
		if(sunImagesFileNames[p].find(".fits")==string::npos && sunImagesFileNames[p].find(".fts")==string::npos)
		{
			cerr<<sunImagesFileNames[p]<<" is not a fits file! (must end in .fits or .fts)"<<endl;
		}
		#endif
		
		if (type == "EIT")
			images.push_back(new EITImage(sunImagesFileNames[p]));
		else if (type == "EUVI")
			images.push_back(new EUVIImage(sunImagesFileNames[p]));
		else if (type == "AIA")
			images.push_back(new AIAImage(sunImagesFileNames[p]));
		else if (type == "SWAP")
			images.push_back(new SWAPImage(sunImagesFileNames[p]));
		else 
			images.push_back(new SunImage(sunImagesFileNames[p]));
	}
	
	
	if(align) // We align the images so they all have the same sun center
	{
		Coordinate sunCenter = images[0]->SunCenter();
		
		for (unsigned p = 1; p < sunImagesFileNames.size(); ++p)
		{
			if( sunCenter.d2(images[p]->SunCenter()) > 2 )
			{
				cerr<<"Warning : Image "<<sunImagesFileNames[p]<<" will be recentered to have the same sun centre than image "<<sunImagesFileNames[0]<<endl;
				images[p]->recenter(sunCenter);
			}

			#if defined(DEBUG) && DEBUG >= 2
			string filename = outputFileName + "recentered.";
			filename +=  sunImagesFileNames[p].substr(sunImagesFileNames[p].rfind('/')!=string::npos?sunImagesFileNames[p].rfind('/')+1:0);
			images[p]->writeFitsImage(filename);
			#endif
		}
	}
	return images;
}


