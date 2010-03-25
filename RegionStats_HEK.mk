CC=g++
CFLAGS=-Wall -fkeep-inline-functions -O3
LFLAGS=-l cfitsio
DFLAGS=-DINSTRUMENT=EUVI -DDEBUG=0 -DNUMBERWAVELENGTH=2

all:bin/RegionStats_HEK.x
clean: rm bin/RegionStats_HEK.x objects/RegionStats_HEK.o objects/FeatureVector.o objects/Coordinate.o objects/RegionStats.o objects/Region.o objects/MainUtilities.o objects/ArgumentHelper.o objects/SunImage.o objects/Image.o objects/tools.o


bin/RegionStats_HEK.x : RegionStats_HEK.mk objects/RegionStats_HEK.o objects/FeatureVector.o objects/Coordinate.o objects/RegionStats.o objects/Region.o objects/MainUtilities.o objects/ArgumentHelper.o objects/SunImage.o objects/Image.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/RegionStats_HEK.o objects/FeatureVector.o objects/Coordinate.o objects/RegionStats.o objects/Region.o objects/MainUtilities.o objects/ArgumentHelper.o objects/SunImage.o objects/Image.o objects/tools.o $(LFLAGS) -o bin/RegionStats_HEK.x

objects/RegionStats_HEK.o : RegionStats_HEK.mk programs/RegionStats_HEK.cpp classes/tools.h classes/constants.h classes/SunImage.h dsr/ArgumentHelper.h classes/MainUtilities.h classes/RegionStats.h classes/Coordinate.h classes/FeatureVector.h
	$(CC) -c $(CFLAGS) $(DFLAGS) programs/RegionStats_HEK.cpp -o objects/RegionStats_HEK.o

objects/FeatureVector.o : RegionStats_HEK.mk classes/FeatureVector.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FeatureVector.cpp -o objects/FeatureVector.o

objects/Coordinate.o : RegionStats_HEK.mk classes/Coordinate.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Coordinate.cpp -o objects/Coordinate.o

objects/RegionStats.o : RegionStats_HEK.mk classes/RegionStats.cpp classes/constants.h classes/Region.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/RegionStats.cpp -o objects/RegionStats.o

objects/Region.o : RegionStats_HEK.mk classes/Region.cpp classes/constants.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Region.cpp -o objects/Region.o

objects/MainUtilities.o : RegionStats_HEK.mk classes/MainUtilities.cpp classes/FeatureVector.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/MainUtilities.cpp -o objects/MainUtilities.o

objects/ArgumentHelper.o : RegionStats_HEK.mk dsr/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) dsr/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/SunImage.o : RegionStats_HEK.mk classes/SunImage.cpp classes/fitsio.h classes/longnam.h classes/Image.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/Image.o : RegionStats_HEK.mk classes/Image.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/tools.o : RegionStats_HEK.mk classes/tools.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o
