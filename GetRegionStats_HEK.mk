CC=g++
CFLAGS=-Wall -fkeep-inline-functions -O3
LFLAGS=-l cfitsio
DFLAGS=-DDEBUG=1 -DNUMBERWAVELENGTH=2

all:bin/GetRegionStats_HEK.x
clean: rm bin/GetRegionStats_HEK.x objects/GetRegionStats_HEK.o objects/FeatureVector.o objects/Coordinate.o objects/RegionStats.o objects/SunImage.o objects/Image.o objects/Region.o objects/MainUtilities.o objects/SWAPImage.o objects/EUVIImage.o objects/EITImage.o objects/ArgumentHelper.o objects/AIAImage.o objects/tools.o


bin/GetRegionStats_HEK.x : GetRegionStats_HEK.mk objects/GetRegionStats_HEK.o objects/FeatureVector.o objects/Coordinate.o objects/RegionStats.o objects/SunImage.o objects/Image.o objects/Region.o objects/MainUtilities.o objects/SWAPImage.o objects/EUVIImage.o objects/EITImage.o objects/ArgumentHelper.o objects/AIAImage.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/GetRegionStats_HEK.o objects/FeatureVector.o objects/Coordinate.o objects/RegionStats.o objects/SunImage.o objects/Image.o objects/Region.o objects/MainUtilities.o objects/SWAPImage.o objects/EUVIImage.o objects/EITImage.o objects/ArgumentHelper.o objects/AIAImage.o objects/tools.o $(LFLAGS) -o bin/GetRegionStats_HEK.x

objects/GetRegionStats_HEK.o : GetRegionStats_HEK.mk programs/GetRegionStats_HEK.cpp classes/tools.h classes/constants.h classes/AIAImage.h dsr/ArgumentHelper.h classes/MainUtilities.h classes/RegionStats.h classes/Coordinate.h classes/FeatureVector.h
	$(CC) -c $(CFLAGS) $(DFLAGS) programs/GetRegionStats_HEK.cpp -o objects/GetRegionStats_HEK.o

objects/FeatureVector.o : GetRegionStats_HEK.mk classes/FeatureVector.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FeatureVector.cpp -o objects/FeatureVector.o

objects/Coordinate.o : GetRegionStats_HEK.mk classes/Coordinate.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Coordinate.cpp -o objects/Coordinate.o

objects/RegionStats.o : GetRegionStats_HEK.mk classes/RegionStats.cpp classes/constants.h classes/Region.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/RegionStats.cpp -o objects/RegionStats.o

objects/SunImage.o : GetRegionStats_HEK.mk classes/SunImage.cpp classes/fitsio.h classes/longnam.h classes/Image.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/Image.o : GetRegionStats_HEK.mk classes/Image.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/Region.o : GetRegionStats_HEK.mk classes/Region.cpp classes/constants.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Region.cpp -o objects/Region.o

objects/MainUtilities.o : GetRegionStats_HEK.mk classes/MainUtilities.cpp classes/FeatureVector.h classes/SunImage.h classes/EITImage.h classes/EUVIImage.h classes/AIAImage.h classes/SWAPImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/MainUtilities.cpp -o objects/MainUtilities.o

objects/SWAPImage.o : GetRegionStats_HEK.mk classes/SWAPImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SWAPImage.cpp -o objects/SWAPImage.o

objects/EUVIImage.o : GetRegionStats_HEK.mk classes/EUVIImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/EUVIImage.cpp -o objects/EUVIImage.o

objects/EITImage.o : GetRegionStats_HEK.mk classes/EITImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/EITImage.cpp -o objects/EITImage.o

objects/ArgumentHelper.o : GetRegionStats_HEK.mk dsr/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) dsr/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/AIAImage.o : GetRegionStats_HEK.mk classes/AIAImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/AIAImage.cpp -o objects/AIAImage.o

objects/tools.o : GetRegionStats_HEK.mk classes/tools.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o
