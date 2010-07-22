CC=g++
CFLAGS=-Wall -fkeep-inline-functions -g
TRACKINGLFLAGS=-lpthread
IDLLFLAGS=-L /usr/local/idl/idl706/bin/bin.linux.x86_64 -lpthread -lidl -lXp -lXpm -lXmu -lXext -lXt -lSM -lICE  -lXinerama -lX11 -ldl -ltermcap -lrt -lm /usr/lib/libXm.a
MAGICKLFLAGS=`Magick++-config --cppflags --cxxflags --ldflags --libs`
MAGICKCFLAGS=-I /usr/include/ImageMagick/
DFLAGS=
LFLAGS=-lcfitsio

all:bin/SPoCA_HEK.x
clean: rm bin/SPoCA_HEK.x objects/SPoCA_HEK.o objects/HistogramPCMClassifier.o objects/PCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Coordinate.o objects/Region.o objects/Image.o objects/HistogramPCM2Classifier.o objects/PCM2Classifier.o objects/HistogramFCMClassifier.o objects/HistogramClassifier.o objects/FeatureVector.o objects/SunImage.o objects/ArgumentHelper.o objects/mainutilities.o objects/SWAPImage.o objects/AIAImage.o objects/EUVIImage.o objects/EITImage.o objects/tools.o


bin/SPoCA_HEK.x : SPoCA_HEK.mk objects/SPoCA_HEK.o objects/HistogramPCMClassifier.o objects/PCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Coordinate.o objects/Region.o objects/Image.o objects/HistogramPCM2Classifier.o objects/PCM2Classifier.o objects/HistogramFCMClassifier.o objects/HistogramClassifier.o objects/FeatureVector.o objects/SunImage.o objects/ArgumentHelper.o objects/mainutilities.o objects/SWAPImage.o objects/AIAImage.o objects/EUVIImage.o objects/EITImage.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/SPoCA_HEK.o objects/HistogramPCMClassifier.o objects/PCMClassifier.o objects/FCMClassifier.o objects/Classifier.o objects/Coordinate.o objects/Region.o objects/Image.o objects/HistogramPCM2Classifier.o objects/PCM2Classifier.o objects/HistogramFCMClassifier.o objects/HistogramClassifier.o objects/FeatureVector.o objects/SunImage.o objects/ArgumentHelper.o objects/mainutilities.o objects/SWAPImage.o objects/AIAImage.o objects/EUVIImage.o objects/EITImage.o objects/tools.o $(LFLAGS) -o bin/SPoCA_HEK.x

objects/SPoCA_HEK.o : SPoCA_HEK.mk programs/SPoCA_HEK.cpp classes/tools.h classes/constants.h classes/mainutilities.h classes/ArgumentHelper.h classes/SunImage.h classes/FeatureVector.h classes/HistogramFCMClassifier.h classes/HistogramPCM2Classifier.h classes/HistogramPCMClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) programs/SPoCA_HEK.cpp -o objects/SPoCA_HEK.o

objects/HistogramPCMClassifier.o : SPoCA_HEK.mk classes/HistogramPCMClassifier.cpp classes/SunImage.h classes/FeatureVector.h classes/HistogramFCMClassifier.h classes/PCMClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/HistogramPCMClassifier.cpp -o objects/HistogramPCMClassifier.o

objects/PCMClassifier.o : SPoCA_HEK.mk classes/PCMClassifier.cpp classes/SunImage.h classes/FeatureVector.h classes/FCMClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/PCMClassifier.cpp -o objects/PCMClassifier.o

objects/FCMClassifier.o : SPoCA_HEK.mk classes/FCMClassifier.cpp classes/Image.h classes/SunImage.h classes/FeatureVector.h classes/Classifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FCMClassifier.cpp -o objects/FCMClassifier.o

objects/Classifier.o : SPoCA_HEK.mk classes/Classifier.cpp classes/tools.h classes/constants.h classes/Image.h classes/SunImage.h classes/FeatureVector.h classes/Region.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Classifier.cpp -o objects/Classifier.o

objects/Coordinate.o : SPoCA_HEK.mk classes/Coordinate.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Coordinate.cpp -o objects/Coordinate.o

objects/Region.o : SPoCA_HEK.mk classes/Region.cpp classes/constants.h classes/Coordinate.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Region.cpp -o objects/Region.o

objects/Image.o : SPoCA_HEK.mk classes/Image.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/HistogramPCM2Classifier.o : SPoCA_HEK.mk classes/HistogramPCM2Classifier.cpp classes/SunImage.h classes/FeatureVector.h classes/HistogramPCMClassifier.h classes/PCM2Classifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/HistogramPCM2Classifier.cpp -o objects/HistogramPCM2Classifier.o

objects/PCM2Classifier.o : SPoCA_HEK.mk classes/PCM2Classifier.cpp classes/SunImage.h classes/FeatureVector.h classes/PCMClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/PCM2Classifier.cpp -o objects/PCM2Classifier.o

objects/HistogramFCMClassifier.o : SPoCA_HEK.mk classes/HistogramFCMClassifier.cpp classes/Image.h classes/SunImage.h classes/HistogramFeatureVector.h classes/FeatureVector.h classes/FCMClassifier.h classes/HistogramClassifier.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/HistogramFCMClassifier.cpp -o objects/HistogramFCMClassifier.o

objects/HistogramClassifier.o : SPoCA_HEK.mk classes/HistogramClassifier.cpp classes/SunImage.h classes/HistogramFeatureVector.h classes/FeatureVector.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/HistogramClassifier.cpp -o objects/HistogramClassifier.o

objects/FeatureVector.o : SPoCA_HEK.mk classes/FeatureVector.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FeatureVector.cpp -o objects/FeatureVector.o

objects/SunImage.o : SPoCA_HEK.mk classes/SunImage.cpp classes/fitsio.h classes/longnam.h classes/Image.h classes/Coordinate.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/ArgumentHelper.o : SPoCA_HEK.mk classes/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/mainutilities.o : SPoCA_HEK.mk classes/mainutilities.cpp classes/FeatureVector.h classes/SunImage.h classes/EITImage.h classes/EUVIImage.h classes/AIAImage.h classes/SWAPImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/mainutilities.cpp -o objects/mainutilities.o

objects/SWAPImage.o : SPoCA_HEK.mk classes/SWAPImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SWAPImage.cpp -o objects/SWAPImage.o

objects/AIAImage.o : SPoCA_HEK.mk classes/AIAImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/AIAImage.cpp -o objects/AIAImage.o

objects/EUVIImage.o : SPoCA_HEK.mk classes/EUVIImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/EUVIImage.cpp -o objects/EUVIImage.o

objects/EITImage.o : SPoCA_HEK.mk classes/EITImage.cpp classes/fitsio.h classes/longnam.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/EITImage.cpp -o objects/EITImage.o

objects/tools.o : SPoCA_HEK.mk classes/tools.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o
