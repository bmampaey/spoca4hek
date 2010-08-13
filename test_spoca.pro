PRO test_spoca

files171 = FILE_SEARCH('/home/benjamin/spoca/spoca_hek/data', '*171.fits', /TEST_READ, /TEST_REGULAR)
files195 = FILE_SEARCH('/home/benjamin/spoca/spoca_hek/data', '*193.fits', /TEST_READ, /TEST_REGULAR)
inputStatusFilename = "spoca.sav"
outputStatusFilename = "spoca.sav"
write_file = 1

outputDirectory = "results/"
writeEventsFrequency = 900 ; For my test I write every 15 minutes
cCodeLocation = "bin/"
instrument = 'AIA'
spocaArgsPreprocessing = 'ALC,DivMedian,DivExpTime'
spocaArgsNumberclasses ='4'
spocaArgsPrecision = '0.000000001'
spocaArgsBinsize = '0.01,0.01'
trackingArgsDeltat = '3600'; == 1h
trackingNumberImages = 9
trackingOverlap = 3
getregionArgsPreprocessing = 'NAR'
getregionArgsRadiusRatio = '0.95'



spoca, image171 = files171[0], image195 = files195[0], $
	events = events, $
	write_file = write_file, $
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = 'Construct', $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = outputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	outputDirectory = outputDirectory, $
	writeEventsFrequency = writeEventsFrequency, $
	cCodeLocation = cCodeLocation, $
	instrument = instrument, $
	spocaArgsPreprocessing = spocaArgsPreprocessing, $
	spocaArgsNumberclasses = spocaArgsNumberclasses, $
	spocaArgsPrecision = spocaArgsPrecision, $
	spocaArgsBinsize = spocaArgsBinsize, $
	trackingArgsDeltat = trackingArgsDeltat, $
	trackingNumberImages = trackingNumberImages, $
	trackingOverlap = trackingOverlap, $
	getregionArgsPreprocessing = getregionArgsPreprocessing, $
	getregionArgsRadiusRatio = getregionArgsRadiusRatio

FOR i=1, MIN([(N_ELEMENTS(files171) - 1), (N_ELEMENTS(files195) - 1)]) DO BEGIN

spoca, image171 = files171[i], image195 = files195[i], $
	events = events, $
	write_file = write_file, $
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = 'Normal', $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = inputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	outputDirectory = outputDirectory, $
	writeEventsFrequency = writeEventsFrequency, $
	cCodeLocation = cCodeLocation, $
	instrument = instrument, $
	spocaArgsPreprocessing = spocaArgsPreprocessing, $
	spocaArgsNumberclasses = spocaArgsNumberclasses, $
	spocaArgsPrecision = spocaArgsPrecision, $
	spocaArgsBinsize = spocaArgsBinsize, $
	trackingArgsDeltat = trackingArgsDeltat, $
	trackingNumberImages = trackingNumberImages, $
	trackingOverlap = trackingOverlap, $
	getregionArgsPreprocessing = getregionArgsPreprocessing, $
	getregionArgsRadiusRatio = getregionArgsRadiusRatio


ENDFOR

spoca, image171 = '', image195 = '', $
	events = events, $
	write_file = write_file, $
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = 'Clear Events', $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = inputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	outputDirectory = outputDirectory, $
	writeEventsFrequency = writeEventsFrequency, $
	cCodeLocation = cCodeLocation, $
	instrument = instrument, $
	spocaArgsPreprocessing = spocaArgsPreprocessing, $
	spocaArgsNumberclasses = spocaArgsNumberclasses, $
	spocaArgsPrecision = spocaArgsPrecision, $
	spocaArgsBinsize = spocaArgsBinsize, $
	trackingArgsDeltat = trackingArgsDeltat, $
	trackingNumberImages = trackingNumberImages, $
	trackingOverlap = trackingOverlap, $
	getregionArgsPreprocessing = getregionArgsPreprocessing, $
	getregionArgsRadiusRatio = getregionArgsRadiusRatio


end
