CC=g++
CFLAGS=-Wall -fkeep-inline-functions -O3
LFLAGS=-l cfitsio -l pthread
DFLAGS=-DINSTRUMENT=AIA -DDEBUG=1 -DNUMBERWAVELENGTH=2

all:bin/Tracking_HEK.x
clean: rm bin/Tracking_HEK.x objects/Tracking_HEK.o objects/MainUtilities.o objects/FeatureVector.o objects/ArgumentHelper.o objects/Region.o objects/Coordinate.o objects/SunImage.o objects/Image.o objects/tools.o


bin/Tracking_HEK.x : Tracking_HEK.mk objects/Tracking_HEK.o objects/MainUtilities.o objects/FeatureVector.o objects/ArgumentHelper.o objects/Region.o objects/Coordinate.o objects/SunImage.o objects/Image.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/Tracking_HEK.o objects/MainUtilities.o objects/FeatureVector.o objects/ArgumentHelper.o objects/Region.o objects/Coordinate.o objects/SunImage.o objects/Image.o objects/tools.o $(LFLAGS) -o bin/Tracking_HEK.x

objects/Tracking_HEK.o : Tracking_HEK.mk programs/Tracking_HEK.cpp classes/tools.h classes/constants.h classes/SunImage.h classes/Region.h classes/gradient.h dsr/ArgumentHelper.h classes/MainUtilities.h cgt/graph.h
	$(CC) -c $(CFLAGS) $(DFLAGS) programs/Tracking_HEK.cpp -o objects/Tracking_HEK.o

objects/MainUtilities.o : Tracking_HEK.mk classes/MainUtilities.cpp classes/FeatureVector.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/MainUtilities.cpp -o objects/MainUtilities.o

objects/FeatureVector.o : Tracking_HEK.mk classes/FeatureVector.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FeatureVector.cpp -o objects/FeatureVector.o

objects/ArgumentHelper.o : Tracking_HEK.mk dsr/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) dsr/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/Region.o : Tracking_HEK.mk classes/Region.cpp classes/constants.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Region.cpp -o objects/Region.o

objects/Coordinate.o : Tracking_HEK.mk classes/Coordinate.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Coordinate.cpp -o objects/Coordinate.o

objects/SunImage.o : Tracking_HEK.mk classes/SunImage.cpp classes/fitsio.h classes/longnam.h classes/Image.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/Image.o : Tracking_HEK.mk classes/Image.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/tools.o : Tracking_HEK.mk classes/tools.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o
