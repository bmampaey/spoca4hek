#pragma once
#ifndef Tools_H
#define Tools_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "Coordinate.h"

std::ostream& operator<<(std::ostream& out, const std::vector<Real>& v);
std::istream& operator>>(std::istream& in, std::vector<Real>& v);

std::string itos(const int& i);
#endif
