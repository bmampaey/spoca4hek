PRO test_spoca

files = FILE_SEARCH('/home/benjamin/data/stereo/200910/', '*A.fts', /TEST_READ, /TEST_REGULAR)
inputStatusFilename = "spoca.sav"

output_directory = "results/"
write_events_frequency = 0
spoca_args_preprocessing = '3'
spoca_args_numberclasses ='4'
spoca_args_precision = '0.000000001'
tracking_args_deltat = '21600'
tracking_number_images = 9
tracking_overlap = 3

spoca, files[0], files[1], $
	events = events, $
	write_file = 1, $
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = 'Construct', $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = inputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	output_directory = output_directory, $
	write_events_frequency = write_events_frequency, $
	spoca_args_preprocessing = spoca_args_preprocessing, $
	spoca_args_numberclasses = spoca_args_numberclasses, $
	spoca_args_precision = spoca_args_precision, $
	tracking_args_deltat = tracking_args_deltat, $
	tracking_number_images = tracking_number_images, $
	tracking_overlap = tracking_overlap

FOR i=1, N_ELEMENTS(files)/2 DO BEGIN

spoca, files[i*2], files[i*2+1], $
	events = events, $
	write_file = 1, $
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = 'Normal', $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = inputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	output_directory = output_directory, $
	write_events_frequency = write_events_frequency, $
	spoca_args_preprocessing = spoca_args_preprocessing, $
	spoca_args_numberclasses = spoca_args_numberclasses, $
	spoca_args_precision = spoca_args_precision, $
	tracking_args_deltat = tracking_args_deltat, $
	tracking_number_images = tracking_number_images, $
	tracking_overlap = tracking_overlap

ENDFOR
              

end
