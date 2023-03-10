#pragma once
#ifndef trackable_H
#define trackable_H


#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>


#include "tools.h"
#include "constants.h"
#include "ColorMap.h"
#include "Region.h"
#include "gradient.h"
#include "../cgt/graph.h"

extern std::string filenamePrefix;
extern ColorType newColor;

typedef cgt::graph<Region*, int> RegionGraph;

//Ordonate the images according to time
void ordonate(std::vector<ColorMap*>& images);

// Return a vector of indices of the images vector ordonated according to time
std::vector<unsigned> imageOrder(const std::vector<ColorMap*>& images);

// Compute the number of pixels common to 2 regions from 2 images, with derotation
unsigned overlay_derotate(ColorMap* image1, const Region* region1, ColorMap* image2, const Region* region2);

// Compute the number of pixels common to 2 regions from 2 images
unsigned overlay(ColorMap* image1, const Region* region1, ColorMap* image2, const Region* region2);


// Find the biggest parrent of a node (the one I have the biggest intersection with)
RegionGraph::node* biggestParent(const RegionGraph::node* n);

// Find the biggest son of a node (the one I have the biggest intersection with)
RegionGraph::node* biggestSon(const RegionGraph::node* n);

// Color a node
void colorize(RegionGraph::node& me);

// Tell if there is a path between a node and a region
bool path(const RegionGraph::node* n, const Region* r);

// Output a graph in the dot format
void ouputGraph(const RegionGraph& g, const std::vector<std::vector<Region*> >& regions, const std::string graphName, bool isColored = true);

// Output regions in the region format
void ouputRegions(const std::vector<std::vector<Region*> >& regions, std::string filename);

void recolorFromRegions(ColorMap* image, const std::vector<Region*>& regions);
#endif
