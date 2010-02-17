// Functions to help main programs

#pragma once
#ifndef MainUtilities_H
#define MainUtilities_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "FeatureVector.h"
#include "SunImage.h"

extern std::string outputFileName;
extern const char * instruments[];
extern const char * limb_corrections[];

//Read the centers and the wavelength from a file
void readCentersFromFile(std::vector<RealFeature>& B, RealFeature& wavelengths, const std::string& centersFileName);

//Put the images in the same order of the wavelength vector
void ordonateImages(std::vector<SunImage*>& images, const RealFeature& wavelengths);

//Read the images from fits files
void fetchImagesFromFile(std::vector<SunImage*>& images, const std::vector<std::string>& sunImagesFileNames, const unsigned  preprocessingType, const double radiusRatio);
#endif
