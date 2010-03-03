CC=g++
CFLAGS=-Wall -fkeep-inline-functions -O3
LFLAGS=-l cfitsio
DFLAGS=-DINSTRUMENT=EUVI -DDEBUG=0 -DNUMBERWAVELENGTH=2

all:bin/SPoCA_HEK.x
clean: rm bin/SPoCA_HEK.x objects/SPoCA_HEK.o objects/RegionStats.o objects/Region.o objects/MainUtilities.o objects/ArgumentHelper.o objects/FeatureVector.o objects/HistogramFCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Image.o objects/SunImage.o objects/tools.o


bin/SPoCA_HEK.x : SPoCA_HEK.mk objects/SPoCA_HEK.o objects/RegionStats.o objects/Region.o objects/MainUtilities.o objects/ArgumentHelper.o objects/FeatureVector.o objects/HistogramFCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Image.o objects/SunImage.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/SPoCA_HEK.o objects/RegionStats.o objects/Region.o objects/MainUtilities.o objects/ArgumentHelper.o objects/FeatureVector.o objects/HistogramFCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Image.o objects/SunImage.o objects/tools.o $(LFLAGS) -o bin/SPoCA_HEK.x

objects/SPoCA_HEK.o : SPoCA_HEK.mk programs/SPoCA_HEK.cpp classes/tools.h classes/constants.h classes/SunImage.h classes/HistogramFCMClassifier.h classes/FeatureVector.h dsr/ArgumentHelper.h classes/MainUtilities.h classes/RegionStats.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) programs/SPoCA_HEK.cpp -o objects/SPoCA_HEK.o

objects/RegionStats.o : SPoCA_HEK.mk classes/RegionStats.cpp classes/Region.h classes/FeatureVector.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/RegionStats.cpp -o objects/RegionStats.o

objects/Region.o : SPoCA_HEK.mk classes/Region.cpp classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Region.cpp -o objects/Region.o

objects/MainUtilities.o : SPoCA_HEK.mk classes/MainUtilities.cpp classes/FeatureVector.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/MainUtilities.cpp -o objects/MainUtilities.o

objects/ArgumentHelper.o : SPoCA_HEK.mk dsr/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) dsr/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/FeatureVector.o : SPoCA_HEK.mk classes/FeatureVector.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FeatureVector.cpp -o objects/FeatureVector.o

objects/HistogramFCMClassifier.o : SPoCA_HEK.mk classes/HistogramFCMClassifier.cpp classes/Image.h classes/SunImage.h classes/HistogramFeatureVector.h classes/FeatureVector.h classes/FCMClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/HistogramFCMClassifier.cpp -o objects/HistogramFCMClassifier.o

objects/FCMClassifier.o : SPoCA_HEK.mk classes/FCMClassifier.cpp classes/Image.h classes/SunImage.h classes/FeatureVector.h classes/Classifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FCMClassifier.cpp -o objects/FCMClassifier.o

objects/Classifier.o : SPoCA_HEK.mk classes/Classifier.cpp classes/tools.h classes/constants.h classes/Image.h classes/SunImage.h classes/FeatureVector.h classes/RegionStats.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Classifier.cpp -o objects/Classifier.o

objects/Image.o : SPoCA_HEK.mk classes/Image.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/SunImage.o : SPoCA_HEK.mk classes/SunImage.cpp classes/fitsio.h classes/longnam.h classes/Image.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/tools.o : SPoCA_HEK.mk classes/tools.cpp classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o
