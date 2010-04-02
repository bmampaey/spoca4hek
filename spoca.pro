; +
; Description:
;	IDL code to call spoca and the tracking
; Author:
; 	Benjamin Mampaey
; Date:
; 	16 February 2010
; Params:
; 	image171, image195: in, required, type string, images filename of wavelength 171 and 195
;	events: out, required, type string array, see document SDO EDS API
;	write_file: in, optional, type boolean, see document SDO EDS API
;	restart: in, optional, type boolean, see document SDO EDS API
;	error: out, required, type string array, see document SDO EDS API
;	imageRejected: out, required, type boolean, see document SDO EDS API
;	status: in/out, required, type struct, see document SDO EDS API
;	runMode: in, required, type string, see document SDO EDS API
;	inputStatusFilename: in, optional, type string, see document SDO EDS API
;	outputStatusFilename: in, required, type string, see document SDO EDS API
;	numActiveEvents: out, required, type integer, see document SDO EDS API
;	outputDirectory: in, required, type string, folder where spoca can store temporary files (The modules manage the cleanup of old files) 
;	writeEventsFrequency: in, required, type integer, number of seconds between events write to the HEK
;	cCodeLocation: in, optional, type string, directory of the c executables
;	spocaArgsPreprocessing: in, optional, type string, type of image preprocessing for spoca
;	spocaArgsNumberclasses: in, optional, type string, number of classes for spoca
;	spocaArgsPrecision: in, optional, type string, precision for spoca
;	spocaArgsBinsize: in, optional, type string, bin size for spoca
;	trackingArgsDeltat: in, optional, type string, maximal time difference between 2 images for tracking
;	trackingNumberImages: in, optional, type integer, number of images to track at the same time
;	trackingOverlap: in, optional, type integer, proportion of the number of images to overlap between tracking succesive run
; -
 
; TODO :
; - Handle the imageRejected and quality when we know what is that quality keyword
; - Take care of the Clear Events case
; 


PRO SPoCA, image171=image171, image195=image195, $
	events = events, $
	write_file = write_file, $
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = runMode, $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = outputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	outputDirectory = outputDirectory, $
	writeEventsFrequency = writeEventsFrequency, $
	cCodeLocation = cCodeLocation, $
	spocaArgsPreprocessing = spocaArgsPreprocessing, $
	spocaArgsNumberclasses = spocaArgsNumberclasses, $
	spocaArgsPrecision = spocaArgsPrecision, $
	spocaArgsBinsize = spocaArgsBinsize, $
	trackingArgsDeltat = trackingArgsDeltat, $
	trackingNumberImages = trackingNumberImages, $
	trackingOverlap = trackingOverlap

	
	
; set debugging
debug = 1

; newline shortcut for the c++ programmer
endl=STRING(10B)

; Because we cannot declare variable in IDL
IF N_ELEMENTS(error) EQ 0 THEN BEGIN
	error = ''
ENDIF


; --------- We take care of the arguments -----------------

IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('BEGINNING OF PARAMETERS CHECK', 100, fill='_')
ENDIF


; We look at what is the runMode and take care of the status

SWITCH runMode OF 
	'Construct':	BEGIN
				
				spoca_lastrun_number = 0
				; We set the start of the first event == the observation date of the first image
				header = headfits(image171)
				; observationdate = get_obs_date(header) get_obs_date is bugged
				observationdate = FXPAR(header,'DATE-OBS')
				IF !err LT 0 THEN observationdate = FXPAR(header,'DATE_OBS')
				IF !err LT 0 THEN BEGIN
					error = [ error, "ERROR : could not find DATE_OBS nor DATE-OBS keyword in file " + image171 ]
					RETURN
				ENDIF
				
				IF (debug GT 0) THEN BEGIN
					PRINT, "First obs_date is ", observationdate, " : ", anytim(observationdate, /ccsds)
				ENDIF
				last_event_written = anytim(observationdate, /ccsds)
				numActiveEvents = 0
				last_color_assigned = 0
				status = {spoca_lastrun_number : spoca_lastrun_number, last_event_written : last_event_written, numActiveEvents : numActiveEvents, last_color_assigned : last_color_assigned}
				BREAK

   			END
	'Recovery':	BEGIN
				IF FILE_TEST( inputStatusFilename , /REGULAR ) THEN BEGIN 

					RESTORE ,inputStatusFilename , VERBOSE = debug 
				
				ENDIF ELSE BEGIN 
				
					error = [ error,  "I am in recovery mode but i didn't receive my inputStatusFilename" ]
					RETURN
				
				ENDELSE
				; I don't break because now I am in normal mode
   			END 
   	'Normal':	BEGIN ; We read the status
				spoca_lastrun_number = status.spoca_lastrun_number
				last_event_written = status.last_event_written
				numActiveEvents = status.numActiveEvents
				last_color_assigned = status.last_color_assigned
				BREAK
   			END 

	'Clear Events':	BEGIN
				; TODO close out events (altought I don't think we have that)
				ARmaps = FILE_SEARCH(outputDirectory, '*ARmap.tracking.fits', /TEST_READ, /TEST_REGULAR , /TEST_WRITE  )
				IF (debug GT 0) THEN BEGIN
					PRINT, "Clear Events called"
					PRINT , "Deleting all remaining ARmaps : ", endl + ARmaps
				ENDIF
		
				FILE_DELETE, ARmaps , /ALLOW_NONEXISTENT , /NOEXPAND_PATH , VERBOSE = debug 
			END
			
	ELSE:		BEGIN
				error = [ error, "I just don't know what to do with myself. runMode is " + runMode ]
				RETURN
			END   	
ENDSWITCH

IF (debug GT 0) THEN BEGIN
	PRINT, 'Status :'
	PRINT, 'spoca_lastrun_number : ' , spoca_lastrun_number
	PRINT, 'last_event_written : ', last_event_written
	PRINT, 'numActiveEvents : ', numActiveEvents
	PRINT, 'last_color_assigned : ', last_color_assigned
ENDIF

; We verify our module arguments

; We test the filenames

IF N_ELEMENTS(image171) EQ 0 THEN BEGIN 
	error = [ error, 'No image171 provided as argument']
	RETURN	
ENDIF 

IF N_ELEMENTS(image195) EQ 0 THEN BEGIN 
	error = [ error, 'No image195 provided as argument']
	RETURN	
ENDIF 

IF (~ FILE_TEST( image171, /READ, /REGULAR)) || (~ FILE_TEST( image195, /READ, /REGULAR) )  THEN BEGIN
	error = [ error, 'Cannot find images ' + image171 + ' or ' + image195 ]
	RETURN
ENDIF


IF N_ELEMENTS(outputDirectory) EQ 0 THEN outputDirectory = 'results/'
IF N_ELEMENTS(writeEventsFrequency) EQ 0 THEN writeEventsFrequency = 4 * 3600
IF N_ELEMENTS(cCodeLocation) EQ 0 THEN cCodeLocation = 'bin/'

; SPoCA parameters

spoca_bin = cCodeLocation + 'SPoCA_HEK.x'

IF ~ FILE_TEST( spoca_bin, /EXECUTABLE)  THEN BEGIN
	error = [ error, 'Cannot find executable ' + spoca_bin ]
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Cannot find executable ' + spoca_bin
	ENDIF
	RETURN
ENDIF
IF N_ELEMENTS(spocaArgsPreprocessing) EQ 0 THEN spocaArgsPreprocessing = '3'  
IF N_ELEMENTS(spocaArgsNumberclasses) EQ 0 THEN spocaArgsNumberclasses = '4'
IF N_ELEMENTS(spocaArgsPrecision) EQ 0 THEN spocaArgsPrecision = '0.000000001'
IF N_ELEMENTS(spocaArgsBinsize) EQ 0 THEN spocaArgsBinsize = '0.01,0.01'
spoca_args_centersfile = outputDirectory + 'centers.txt'


; Tracking parameters

tracking_bin = cCodeLocation + 'Tracking_HEK.x'

IF ~ FILE_TEST( tracking_bin, /EXECUTABLE)  THEN BEGIN
	error = [ error, 'Cannot find executable ' + tracking_bin ]
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Cannot find executable ' + tracking_bin
	ENDIF
	RETURN
ENDIF
IF N_ELEMENTS(trackingArgsDeltat) EQ 0 THEN trackingArgsDeltat = '21600' ; It is in seconds
IF N_ELEMENTS(trackingNumberImages) EQ 0 THEN trackingNumberImages = 9
IF N_ELEMENTS(trackingOverlap) EQ 0 THEN trackingOverlap = 3

; GetRegionStats parameters

getregionstats_bin = cCodeLocation + 'GetRegionStats_HEK.x'
IF ~ FILE_TEST( getregionstats_bin, /EXECUTABLE)  THEN BEGIN
	error = [ error, 'Cannot find executable ' + getregionstats_bin ]
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Cannot find executable ' + getregionstats_bin
	ENDIF
	RETURN
ENDIF
getregionstatsArgsPreprocessing = '1' ; We only do Annulus Limb Correction here

; We verify the quality of the images
header = headfits(image171)
quality = FXPAR(header,'QUALITY')

IF !err LT 0 THEN BEGIN
	error = [ error, 'Cannot find keyword QUALITY in image ' + image171 ]
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Cannot find keyword QUALITY in image ' + image171
	ENDIF
	;RETURN		; To be removed when there is a quality keyword in the files
ENDIF

IF quality NE 0 THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Bad QUALITY of file ' + image171
	ENDIF
	imageRejected = 1
	;RETURN		; To be removed when there is a quality keyword in the files
ENDIF

header = headfits(image195)
quality = FXPAR(header,'QUALITY')

IF !err LT 0 THEN BEGIN
	error = [ error, 'Cannot find keyword QUALITY in image ' + image195 ]
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Cannot find keyword QUALITY in image ' + image195
	ENDIF
	;RETURN		; To be removed when there is a quality keyword in the files
ENDIF

IF quality NE 0 THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT , 'Bad QUALITY of file ' + image195
	ENDIF
	imageRejected = 1
	;RETURN		; To be removed when there is a quality keyword in the files
ENDIF

; The images are good
imageRejected = 0


IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('END OF PARAMETERS CHECK', 100, fill='_')
ENDIF


; --------- We take care of running spoca -----------------

IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('BEGINNING OF SPOCA', 100, fill='_')
ENDIF


; We initialise correctly the arguments for SPoCA_HEK

++spoca_lastrun_number

spoca_args = [	'-P', spocaArgsPreprocessing, $
		'-C', spocaArgsNumberclasses, $
		'-p', spocaArgsPrecision, $
		'-z', spocaArgsBinsize, $
		'-B', spoca_args_centersfile, $
		'-O', outputDirectory + STRING(spoca_lastrun_number, FORMAT='(I010)'), $
		image171, image195 ]

IF (debug GT 0) THEN BEGIN

	PRINT, 'About to run : ' , STRJOIN( [spoca_bin , spoca_args] , ' ', /SINGLE ) 
	
ENDIF

; We call SPoCA with the correct arguments

SPAWN, [spoca_bin , spoca_args], spoca_output, spoca_errors, /NOSHELL, EXIT_STATUS=spoca_exit 

; In case of error
IF (spoca_exit NE 0) THEN BEGIN

	error = [ error, 'Error executing '+  STRJOIN( [spoca_bin , spoca_args] , ' ', /SINGLE )  ]
	error = [ error, spoca_errors ]
	; TODO Should we cleanup  ???
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "SPoCA exited with error : ", spoca_exit, endl, spoca_errors
	ENDIF
	
ENDIF



IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('END OF SPOCA', 100, fill='_')
ENDIF


; --------- We take care of the tracking -----------------

IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('BEGINNING OF TRACKING', 100, fill='_')
ENDIF


ARmaps = FILE_SEARCH(outputDirectory, '*ARmap.tracking.fits', /TEST_READ, /TEST_REGULAR , /TEST_WRITE  ) 

IF (debug GT 0) THEN BEGIN
	PRINT , "Found files : ", endl + ARmaps
ENDIF

IF (N_ELEMENTS(ARmaps) LT trackingNumberImages) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Not enough files to do tracking, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF
		
; We initialise correctly the arguments for Tracking_HEK

tracking_args =	[	'-n', STRING(last_color_assigned, FORMAT = '(I)'), $
			'-d', trackingArgsDeltat, $
			'-D', STRING(trackingOverlap, FORMAT = '(I)'), $
			ARmaps ]
	
IF (debug GT 0) THEN BEGIN
	PRINT, 'About to run : ', STRJOIN( [tracking_bin , tracking_args] , ' ', /SINGLE )
ENDIF

SPAWN, [tracking_bin , tracking_args] , tracking_output, tracking_errors, /NOSHELL, EXIT_STATUS=tracking_exit 

IF (tracking_exit NE 0) THEN BEGIN

	error = [ error, 'Error executing ' + STRJOIN( [tracking_bin , tracking_args] , ' ', /SINGLE ) ]
	error = [ error, tracking_errors ]
	; What do we do in case of error ?
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "Tracking exited with error : ", tracking_exit, endl, tracking_errors
	ENDIF
	
ENDIF


IF (debug GT 0) THEN BEGIN
	PRINT, 'Tracking Output is :', endl + tracking_output
ENDIF


; We check that output is not null
IF (N_ELEMENTS(tracking_output) LT 1 || STRLEN(tracking_output[0]) LE 1 ) THEN BEGIN

	error = [ error, 'Error No output from Tracking']	
	RETURN
	
ENDIF ELSE BEGIN

	; The output of Tracking is numActiveEvents last_event_found last_color_assigned
	output = strsplit( tracking_output[0] , ' 	(),', /EXTRACT) 
	numActiveEvents = LONG(output[0])
	last_event_found = output[1]
	last_color_assigned = LONG(output[2])
	
ENDELSE


IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('END OF TRACKING', 100, fill='_')
ENDIF

; --------- We take care of the computing of the Regions Stats -----------------

IF (debug GT 0) THEN BEGIN

	PRINT, endl, STRPAD('BEGINNING OF GETREGIONSTATS', 100, fill='_')
ENDIF

; We run RegionsStats if it is time to write the events to the hek and that we have at least 1 active event

events_write_deltat = anytim(last_event_found, /sec) - anytim(last_event_written, /sec) 

IF (debug GT 0) THEN BEGIN
	PRINT,  "time of last_event_written : ", last_event_written
	PRINT,  "time of last_event_found : ", last_event_found
	PRINT,  STRING(events_write_deltat, FORMAT='(I20)') + ' seconds elapsed between last_event_found and last_event_written'
ENDIF


IF events_write_deltat LT writeEventsFrequency THEN BEGIN
	
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Not running GetRegionStats yet'
	ENDIF
	GOTO, Finish
	
ENDIF ELSE BEGIN

	IF (debug GT 0) THEN BEGIN
		PRINT, 'Running GetRegionStats'
	ENDIF

ENDELSE


IF numActiveEvents EQ 0 THEN BEGIN

	IF (debug GT 0) THEN BEGIN
		PRINT, 'No active event, going to Finish'
	ENDIF

	last_event_written = last_event_found	; Even if there is no event to write, it was time to write them
	GOTO, Finish
ENDIF


getregionstats_args = [	'-P', getregionstatsArgsPreprocessing, $
			'-M', ARmaps[N_ELEMENTS(ARmaps) - 1], $
			image171 ]

IF (debug GT 0) THEN BEGIN

	PRINT, 'About to run : ' , STRJOIN( [getregionstats_bin , getregionstats_args] , ' ', /SINGLE )
	
ENDIF

; We call RegionsStats with the correct arguments

SPAWN, [getregionstats_bin , getregionstats_args], getregionstats_output, getregionstats_errors, /NOSHELL, EXIT_STATUS=getregionstats_exit 

; In case of error
IF (getregionstats_exit NE 0) THEN BEGIN

	error = [ error, 'Error executing '+ STRJOIN( [getregionstats_bin , getregionstats_args] , ' ', /SINGLE ) ]
	error = [ error, getregionstats_errors ]
	; TODO Should we cleanup  ???
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "RegionsStats exited with error : ", getregionstats_exit, endl, getregionstats_errors
	ENDIF
	
ENDIF

; As output of GetRegionStats we receive the stats on the AR intra limb 
IF (debug GT 0) THEN BEGIN
	PRINT, 'GetRegionStats Output is :', endl + getregionstats_output
ENDIF


; We check that output is not null, This should not be the case because we know that the numActiveEvents > 0
IF (N_ELEMENTS(getregionstats_output) LT 1 || STRLEN(getregionstats_output[0]) LE 1 ) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'ERROR GetRegionStats Output is void, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF


; We allocate space for the events
events = strarr(N_ELEMENTS(getregionstats_output))


; We need the header of one of the image to transform the coordinates

header = headfits(image171)
wcs = fitshead2wcs(header)




FOR k = 0, N_ELEMENTS(getregionstats_output) - 1 DO BEGIN 

	; The output of GetRegionStats is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id numberpixels label observationdate color minintensity maxintensity mean variance skewness kurtosis totalintensity (centererror.x, centererror.y) area_raw area_rawuncert area_atdiskcenter area_atdiskcenteruncert
	output = strsplit( getregionstats_output[k] , ' 	(),', /EXTRACT) 
	; We parse the output
	cartesian_x = FLOAT(output[0:4:2])
	cartesian_y = FLOAT(output[1:5:2])
	cartesian = FLTARR(2,N_ELEMENTS(cartesian_x))
	cartesian[0,*]=cartesian_x
	cartesian[1,*]=cartesian_y
	id = output[6]
	numberpixels = LONG(output[7])
	label = output[8]
	observationdate = output[9]
	color = LONG(output[10])
	minintensity = FLOAT(output[11])
	maxintensity = FLOAT(output[12])
	mean = FLOAT(output[13])
	variance = FLOAT(output[14])
	skewness = FLOAT(output[15])
	kurtosis = FLOAT(output[16])
	totalintensity = FLOAT(output[17])
	centerx_error = FLOAT(output[18])
	centery_error = FLOAT(output[19])
	area_raw = FLOAT(output[20])
	area_rawuncert = FLOAT(output[21])
	area_atdiskcenter = FLOAT(output[22])
	area_atdiskcenteruncert = FLOAT(output[23])
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "cartesians coordinates for the region ", k
		PRINT, cartesian
	ENDIF
		
	; We convert the cartesian pixel coodinates into WCS
        wcs_coord = WCS_GET_COORD(wcs, cartesian)
        
	; We convert the WCS coodinates into helioprojective cartesian
	WCS_CONVERT_FROM_COORD, wcs, wcs_coord, 'HPC', /ARCSECONDS, hpc_x, hpc_y
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y, z HPC coordinates for the region ", k
		PRINT, hpc_x
		PRINT, hpc_y
	ENDIF
	
	; Create an Hek event and fill it
		
	event = struct4event('AR')

	event.required.OBS_Observatory = 'STEREO'
	event.required.OBS_Instrument = 'EUVI'
	event.required.OBS_ChannelID = 'EUVI 171, EUVI 195'
	event.required.OBS_MeanWavel = 171; ??? There should be 2 values, one for 195 also
	event.required.OBS_WavelUnit = 'Angstroms'

	event.required.FRM_Name = 'SPoCA'
	event.optional.FRM_VersionNumber = 1
	event.required.FRM_Identifier = 'vdelouille'
	event.required.FRM_Institute ='ROB'
	event.required.FRM_HumanFlag = 'F'
	event.required.FRM_ParamSet = 'image171 : calibrated image 171 A' $
		+ ', image195 : calibrated image 195 A' $
		+ ', dilation_factor= 12' $
		+ ', initialisation_type= FCM' $
		+ ', numerical_precision= double' $
		+ ', spocaArgsPreprocessing=' + spocaArgsPreprocessing $
		+ ', spocaArgsNumberclasses=' + spocaArgsNumberclasses $
		+ ', spocaArgsPrecision='  + spocaArgsPrecision $
		+ ', spocaArgsBinsize='  + spocaArgsBinsize $
		+ ', trackingArgsDeltat=' + trackingArgsDeltat $
		+ ', trackingNumberImages=' + STRING(trackingNumberImages, FORMAT='(I)') $
		+ ', trackingOverlap=' + STRING(trackingOverlap, FORMAT='(I)') 


	event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
	event.required.FRM_Contact = 'veronique.delouille@sidc.be'
	event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SPoCA'

	event.required.Event_StartTime = anytim(last_event_written, /ccsds) ; The start time is the previous time we ran GetRegionStats
	event.required.Event_EndTime = anytim(observationdate, /ccsds)  
	  
	event.required.Event_CoordSys = 'UTC-HPC-TOPO'
	event.required.Event_CoordUnit = 'arcsec,arcsec'
	event.required.Event_Coord1 = hpc_x[0]
	event.required.Event_Coord2 = hpc_y[0]
	event.required.Event_C1Error = centerx_error
	event.required.Event_C2Error = centery_error	
	event.required.BoundBox_C1LL = hpc_x[1]
	event.required.BoundBox_C2LL = hpc_y[1]
	event.required.BoundBox_C1UR = hpc_x[2]
	event.required.BoundBox_C2UR = hpc_y[2]
	
	event.optional.Event_Npixels = numberpixels
	event.optional.Event_PixelUnit = 'DN/s' ; ??? TBC for AIA
	event.optional.OBS_DataPrepURL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SPoCA' 
	event.optional.FRM_SpecificID = color
	event.optional.Area_AtDiskCenter = area_atdiskcenter
	event.optional.Area_AtDiskCenterUncert = area_atdiskcenteruncert
	event.optional.Area_Raw = area_raw
	event.optional.Area_Uncert = area_rawuncert
	event.optional.Area_Unit = 'Mm2'
	; These are not yet in my solar soft (updated 1 April 2010)
	; event.optional.AR_IntensMin = minintensity
	; event.optional.AR_IntensMax = maxintensity
	; event.optional.AR_IntensMean = mean
	; event.optional.AR_IntensVar = variance
	; event.optional.AR_IntensSkew = skewness
	; event.optional.AR_IntensKurt = kurtosis
	; event.optional.AR_IntensTotal = totalintensity
	; event.optional.AR_IntensUnit = 'DN/s' ; ??? TBC for AIA  Is this a float ?



	
	; We write the VOevent
	
	IF KEYWORD_SET(write_file) THEN BEGIN
		export_event, event, /write, suff=label, buff=buff
	ENDIF ELSE BEGIN
		export_event, event, suffix=label, buff=buff
	ENDELSE
	
	events[k]=STRJOIN(buff, /SINGLE) ;
	

ENDFOR 

last_event_written = last_event_found ; We update the time we wrote an event

IF (debug GT 0) THEN BEGIN
	PRINT, endl, STRPAD('END OF GETREGIONSTATS', 100, fill='_')
ENDIF



Finish :	; Label for the case not enough images were present for the tracking, or if we do not write 

; --------- We finish up -----------------


IF (debug GT 0) THEN BEGIN
	PRINT, endl, STRPAD('FINISHING', 100, fill='_')
ENDIF

; We cleanup old files

IF (N_ELEMENTS(ARmaps) GE trackingNumberImages) THEN BEGIN

	number_of_files_to_delete = N_ELEMENTS(ARmaps) - trackingOverlap
	IF (number_of_files_to_delete GT 0) THEN BEGIN

		files_to_delete = ARmaps[0:number_of_files_to_delete-1]
		
		IF (debug GT 0) THEN BEGIN
			PRINT , "Deleting files : ", endl + files_to_delete
		ENDIF
		
		FILE_DELETE, files_to_delete , /ALLOW_NONEXISTENT , /NOEXPAND_PATH , VERBOSE = debug

	ENDIF
ENDIF


; We save the variables for next run

status.spoca_lastrun_number = spoca_lastrun_number
status.last_event_written = last_event_written
status.numActiveEvents = numActiveEvents
status.last_color_assigned = last_color_assigned

SAVE, status , DESCRIPTION='Spoca last run status variable at ' + SYSTIME() , FILENAME=outputStatusFilename, VERBOSE = debug
 
 
IF (debug GT 0) THEN BEGIN
	PRINT, endl, STRPAD('END OF FINISH', 100, fill='_')
ENDIF
 
END 

