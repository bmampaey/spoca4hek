#pragma once
#ifndef Constants_H
#define Constants_H

#include "fitsio.h"

/* Definition of constants*/
/* Modify the following to adapt to your program */

// The instrument who took the images. This will set the type of the pixel values, and the keywors describing the image
// Acceptable values are EIT (soho) EUVI (stereo)  AIA (sdo)
#if ! defined(INSTRUMENT)
#define INSTRUMENT EIT
#endif
// The precision you want for the float numbers
// Acceptable values are TFLOAT (fast) TDOUBLE (more precise)
#if ! defined(REALTYPE)
#define REALTYPE TDOUBLE
#endif
// The number of wavelength, or images to process in parralel
#if ! defined(NUMBERWAVELENGTH)
#define NUMBERWAVELENGTH 2
#endif

// The quantity of output to screen and files, and verification to be done
// 1 is for verification and output of the results only
// 2 is 1 + lots of output to files
// 3 is 2 + lots of output to screen
#if ! defined(DEBUG)
#define DEBUG 3
#endif


// The function to use for limb correction percentage
// Acceptable values are VINCE_CORR , CIS1_CORR, BEN_CORR, CIS2_CORR

#if ! defined(LIMB_CORRECTION)
#define LIMB_CORRECTION BEN_CORR
#endif


// The function you want for the merging of 2 centers in sur segmentation
// Acceptable values are MERGEVINCE  MERGEMEAN  MERGEMEANX  MERGEMAX  MERGECIS
#if ! defined(MERGE)
#define MERGE MERGECIS
#endif

// Wheter you want eta to be fixed or not

#if ! defined(FIXETA)
#define FIXETA FALSE
#endif

//The minimal size for an AR in (arc/sec)square (otherwise it is a bright point)
#if ! defined(MIN_AR_SIZE)
#define MIN_AR_SIZE 1500
#endif

// The type of the pixel values of the images. This will overide the type set by the instrument.
// Acceptable values are TBYTE TSBYTE TUSHORT TSHORT TUINT TINT TULONG TLONG TFLOAT TLONGLONG TDOUBLE

//#define PIXELTYPE TDOUBLE

/* Do not modify the constants here under please */

#define EIT 1
#define EUVI 2
#define AIA 3

#define VINCE_CORR 1
#define CIS1_CORR 2
#define BEN_CORR 3
#define CIS2_CORR 4


#define MERGEVINCE 1
#define MERGEMEAN 2
#define MERGEMEANX 2
#define MERGEMAX 4
#define MERGECIS 5

#define FALSE 0
#define TRUE 1

#if ! defined(LIMB_CORRECTION)
#warning "LIMB_CORRECTION not defined, using default CIS2_CORR"
#define LIMB_CORRECTION CIS2_CORR
#endif

#if ! defined(REALTYPE)
#warning "REALTYPE not defined, using default type double"
#define REALTYPE TDOUBLE
#endif

#if REALTYPE==TFLOAT
#define Real float
#else
#define Real double
#endif

#if ! defined(INSTRUMENT)
#warning "INSTRUMENT not defined, using default EIT"
#define INSTRUMENT EIT
#endif

#if INSTRUMENT==EIT
#define PIXELTYPE TDOUBLE

#define VINCE_CORR_R1  95
#define CIS1_CORR_R1 90
#define CIS1_CORR_R2 95
#define BEN_CORR_R1 90
#define BEN_CORR_R2 95
#define BEN_CORR_R3 105
#define BEN_CORR_R4 110


#elif INSTRUMENT==EUVI
#define PIXELTYPE TFLOAT

#define VINCE_CORR_R1  95
#define CIS1_CORR_R1 90
#define CIS1_CORR_R2 95
#define BEN_CORR_R1 95
#define BEN_CORR_R2 100
#define BEN_CORR_R3 105
#define BEN_CORR_R4 110

#elif INSTRUMENT==AIA
#define PIXELTYPE TUINT

#define VINCE_CORR_R1  95
#define CIS1_CORR_R1 90
#define CIS1_CORR_R2 95
#define BEN_CORR_R1 95
#define BEN_CORR_R2 100
#define BEN_CORR_R3 105
#define BEN_CORR_R4 110

#endif

#if ! defined(PIXELTYPE)
#warning "PIXELTYPE not defined, using default TDOUBLE"
#define PIXELTYPE TDOUBLE
#endif

#if PIXELTYPE==TBYTE
#define PixelType signed char
#elif PIXELTYPE==TSBYTE
#define PixelType unsigned char
#elif PIXELTYPE==TSHORT
#define PixelType short
#elif PIXELTYPE==TUSHORT
#define PixelType unsigned short
#elif PIXELTYPE==TINT
#define PixelType int
#elif PIXELTYPE==TUINT
#define PixelType unsigned int
#elif PIXELTYPE==TLONG
#define PixelType long
#elif PIXELTYPE==TULONG
#define PixelType unsigned long
#elif PIXELTYPE==TLONGLONG
#define PixelType long long
#elif PIXELTYPE==TFLOAT
#define PixelType float
#elif PIXELTYPE==TDOUBLE
#define PixelType double
#endif
#endif
