CC=g++
CFLAGS=-Wall -fkeep-inline-functions -O3
LFLAGS=-l cfitsio
DFLAGS=-DINSTRUMENT=EIT -DDEBUG=0 -DNUMBERWAVELENGTH=2

all:bin/SPoCA_V3D.x
clean: rm bin/SPoCA_V3D.x objects/SPoCA_V3D.o objects/Coordinate.o objects/MainUtilities.o objects/ArgumentHelper.o objects/FeatureVector.o objects/HistogramFCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Region.o objects/Image.o objects/SunImage.o objects/tools.o


bin/SPoCA_V3D.x : SPoCA_V3D.mk objects/SPoCA_V3D.o objects/Coordinate.o objects/MainUtilities.o objects/ArgumentHelper.o objects/FeatureVector.o objects/HistogramFCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Region.o objects/Image.o objects/SunImage.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/SPoCA_V3D.o objects/Coordinate.o objects/MainUtilities.o objects/ArgumentHelper.o objects/FeatureVector.o objects/HistogramFCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Region.o objects/Image.o objects/SunImage.o objects/tools.o $(LFLAGS) -o bin/SPoCA_V3D.x

objects/SPoCA_V3D.o : SPoCA_V3D.mk programs/SPoCA_HEK.cpp classes/tools.h classes/constants.h classes/SunImage.h classes/HistogramFCMClassifier.h classes/FeatureVector.h dsr/ArgumentHelper.h classes/MainUtilities.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) programs/SPoCA_HEK.cpp -o objects/SPoCA_V3D.o

objects/Coordinate.o : SPoCA_V3D.mk classes/Coordinate.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Coordinate.cpp -o objects/Coordinate.o

objects/MainUtilities.o : SPoCA_V3D.mk classes/MainUtilities.cpp classes/FeatureVector.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/MainUtilities.cpp -o objects/MainUtilities.o

objects/ArgumentHelper.o : SPoCA_V3D.mk dsr/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) dsr/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/FeatureVector.o : SPoCA_V3D.mk classes/FeatureVector.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FeatureVector.cpp -o objects/FeatureVector.o

objects/HistogramFCMClassifier.o : SPoCA_V3D.mk classes/HistogramFCMClassifier.cpp classes/Image.h classes/SunImage.h classes/HistogramFeatureVector.h classes/FeatureVector.h classes/FCMClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/HistogramFCMClassifier.cpp -o objects/HistogramFCMClassifier.o

objects/FCMClassifier.o : SPoCA_V3D.mk classes/FCMClassifier.cpp classes/Image.h classes/SunImage.h classes/FeatureVector.h classes/Classifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FCMClassifier.cpp -o objects/FCMClassifier.o

objects/Classifier.o : SPoCA_V3D.mk classes/Classifier.cpp classes/tools.h classes/constants.h classes/Image.h classes/SunImage.h classes/FeatureVector.h classes/Region.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Classifier.cpp -o objects/Classifier.o

objects/Region.o : SPoCA_V3D.mk classes/Region.cpp classes/constants.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Region.cpp -o objects/Region.o

objects/Image.o : SPoCA_V3D.mk classes/Image.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/SunImage.o : SPoCA_V3D.mk classes/SunImage.cpp classes/fitsio.h classes/longnam.h classes/Image.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/tools.o : SPoCA_V3D.mk classes/tools.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o
