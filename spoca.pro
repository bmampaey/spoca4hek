; +
; Description:
;	IDL code to call spoca and the tracking
; Author:
; 	Benjamin Mampaey
; Date:
; 	16 February 2010
; Params:
; 	image1, image2: in, required, type string, images filename of wavelength 171 and 195
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


PRO SPoCA, image1=image1, image2=image2, $
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

; We look at what is the runMode and take care of the status

SWITCH runMode OF 
	'Construct':	BEGIN
				
				spoca_lastrun_number = 0
				write_events_last = SYSTIME(/SECONDS)
				numActiveEvents = 0
				last_color_assigned = 0
				status = {spoca_lastrun_number : spoca_lastrun_number, write_events_last : write_events_last, numActiveEvents : numActiveEvents, last_color_assigned : last_color_assigned}
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
				write_events_last = status.write_events_last
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
	PRINT, 'write_events_last : ', write_events_last
	PRINT, 'numActiveEvents : ', numActiveEvents
	PRINT, 'last_color_assigned : ', last_color_assigned
ENDIF

; We verify our module arguments

; We test the filenames

IF N_ELEMENTS(image1) EQ 0 THEN BEGIN 
	error = [ error, 'No image1 provided as argument']
	RETURN	
ENDIF 

IF N_ELEMENTS(image2) EQ 0 THEN BEGIN 
	error = [ error, 'No image2 provided as argument']
	RETURN	
ENDIF 

IF (~ FILE_TEST( image1, /READ, /REGULAR)) || (~ FILE_TEST( image2, /READ, /REGULAR) )  THEN BEGIN
	error = [ error, 'Cannot find images ' + image1 + ' or ' + image2 ]
	RETURN
ENDIF


IF N_ELEMENTS(outputDirectory) EQ 0 THEN outputDirectory = 'results/'
IF N_ELEMENTS(writeEventsFrequency) EQ 0 THEN writeEventsFrequency = 4 * 3600
IF N_ELEMENTS(cCodeLocation) EQ 0 THEN cCodeLocation = 'bin/'

; SPoCA parameters

spoca_bin = cCodeLocation + 'SPoCA_HEK.x'

IF N_ELEMENTS(spocaArgsPreprocessing) EQ 0 THEN spocaArgsPreprocessing = '3'  
IF N_ELEMENTS(spocaArgsNumberclasses) EQ 0 THEN spocaArgsNumberclasses = '4'
IF N_ELEMENTS(spocaArgsPrecision) EQ 0 THEN spocaArgsPrecision = '0.000000001'
IF N_ELEMENTS(spocaArgsBinsize) EQ 0 THEN spocaArgsBinsize = '0.01,0.01'
spoca_args_centersfile = outputDirectory + 'centers.txt'


; Tracking parameters

tracking_bin = cCodeLocation + 'Tracking_HEK.x'
IF N_ELEMENTS(trackingArgsDeltat) EQ 0 THEN trackingArgsDeltat = '21600' ; It is in seconds
IF N_ELEMENTS(trackingNumberImages) EQ 0 THEN trackingNumberImages = 9
IF N_ELEMENTS(trackingOverlap) EQ 0 THEN trackingOverlap = 3


IF (debug GT 0) THEN BEGIN
	PRINT, endl, "*********************************************************"
ENDIF


; --------- We take care of running spoca -----------------

; We initialise correctly the arguments for SPoCA_HEK

++spoca_lastrun_number

spoca_args =	' -P ' + spocaArgsPreprocessing + $
		' -C ' + spocaArgsNumberclasses + $
		' -p ' + spocaArgsPrecision + $
		' -z ' + spocaArgsBinsize + $
		' -B ' + spoca_args_centersfile + $
		' -O ' + outputDirectory + STRING(spoca_lastrun_number, FORMAT='(I010)') + $
		' ' + image1 + ' ' + image2

IF (debug GT 0) THEN BEGIN

	PRINT, 'About to run : ' , spoca_bin + spoca_args
	
ENDIF

; We call SPoCA with the correct arguments

SPAWN, spoca_bin + spoca_args, spoca_output, spoca_errors, EXIT_STATUS=spoca_exit 

; In case of error
IF (spoca_exit NE 0) THEN BEGIN

	error = [ error, 'Error executing '+ spoca_bin + spoca_args ]
	error = [ error, spoca_errors ]
	; TODO Should we cleanup  ???
	
ENDIF


; As output of spoca we receive the AR out of the limb for warning (they are not tracked)

IF (debug GT 0) THEN BEGIN
	PRINT, 'Spoca Output is : ', endl + spoca_output
ENDIF


; We check that output is not null

IF (N_ELEMENTS(spoca_output) LT 1 || STRLEN(spoca_output[0]) LE 1 ) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Spoca Output is void, going to Tracking'
	ENDIF
	GOTO, Tracking
ENDIF

; We allocate space for the events
spoca_events = strarr(N_ELEMENTS(spoca_output))

; We need the header of one of the image to transform the coordinates
header = headfits(image1)
wcs = fitshead2wcs(header)

FOR k = 0, N_ELEMENTS(spoca_output) - 1 DO BEGIN 

	; The output of SpoCA is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label date_obs
	output = strsplit( spoca_output[k] , ' 	(),', /EXTRACT) 
	; We parse the output
	cartesian_x = FLOAT(output[0:4:2])
	cartesian_y = FLOAT(output[1:5:2])
	cartesian = FLTARR(2,N_ELEMENTS(cartesian_x))
	cartesian[0,*]=cartesian_x
	cartesian[1,*]=cartesian_y
	id = output[6]
	size = output[7]
	label = output[8]
	date_obs = output[9] 
	
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
        ;temp disabled by rt due to stupid 255 character limit, 
	event.required.FRM_ParamSet = 'image1 : calibrated image 171 A' $
		+ ', image2 : calibrated image 195 A' $
		+ ', dilation_factor= 12' $
		+ ', initialisation_type= FCM' $
		+ ', numerical_precision= double'; $
		;+ ', spocaArgsPreprocessing=' + spocaArgsPreprocessing $
		;+ ', spocaArgsNumberclasses=' + spocaArgsNumberclasses $
		;+ ', spocaArgsPrecision='  + spocaArgsPrecision $
		;+ ', spocaArgsBinsize='  + spocaArgsBinsize $
		;+ ', trackingArgsDeltat=' + trackingArgsDeltat $
		;+ ', trackingNumberImages=' + STRING(trackingNumberImages, FORMAT='(I)') $
		;+ ', trackingOverlap=' + STRING(trackingOverlap, FORMAT='(I)') 


	event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
	event.required.FRM_Contact = 'veronique.delouille@sidc.be'
	event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SPoCA'

	event.required.Event_StartTime = anytim(date_obs, /ccsds) 
	event.required.Event_EndTime = anytim(date_obs, /ccsds) 
	;event.optional.Event_Expires = anytim(sys2ut(), /ccsds)  
	event.required.Event_CoordSys = 'UTC-HPC-TOPO'
	event.required.Event_CoordUnit = 'arcsec,arcsec'
	event.required.Event_Coord1 = hpc_x[0]
	event.required.Event_Coord2 = hpc_y[0]
	event.required.Event_C1Error = 0 ; TBD
	event.required.Event_C2Error = 0 ; TBD
	event.optional.Event_Npixels = size
	event.optional.Event_PixelUnit = 'DN/s' ; ??? TBC for AIA
	event.optional.OBS_DataPrepURL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SPoCA' ; 

	event.required.BoundBox_C1LL = hpc_x[1]
	event.required.BoundBox_C2LL = hpc_y[1]
	event.required.BoundBox_C1UR = hpc_x[2]
	event.required.BoundBox_C2UR = hpc_y[2]
	
	
	; We write the VOevent
	
	IF KEYWORD_SET(write_file) THEN BEGIN
		export_event, event, /write, suff=label, buff=buff
	ENDIF ELSE BEGIN
		export_event, event, suffix=label, buff=buff
	ENDELSE
	
	spoca_events[k]=STRJOIN(buff, /SINGLE) ;


ENDFOR 

IF (debug GT 0) THEN BEGIN
	PRINT, endl, "*********************************************************"
ENDIF


; --------- We take care of the tracking -----------------

Tracking:	; Label for the case spoca_output is empty

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

tracking_args =	' -n ' + STRING(last_color_assigned, FORMAT = '(I)') + $
		' -d ' + trackingArgsDeltat + $
		' -D ' + STRING(trackingOverlap, FORMAT = '(I)') + $
		' ' + STRJOIN( ARmaps , ' ', /SINGLE)
	
IF (debug GT 0) THEN BEGIN
	PRINT, 'About to run : ' , tracking_bin + tracking_args 
ENDIF

SPAWN, tracking_bin + tracking_args , tracking_output, tracking_errors, EXIT_STATUS=tracking_exit 

IF (tracking_exit NE 0) THEN BEGIN

	error = [ error, 'Error executing ', tracking_bin + tracking_args ]
	error = [ error, tracking_errors ]
	; What do we do in case of error ?
	
ENDIF

; As output of tracking we receive the AR intra limb 
IF (debug GT 0) THEN BEGIN
	PRINT, 'Tracking Output is :', endl + tracking_output
ENDIF


; We check that output is not null
IF (N_ELEMENTS(tracking_output) LT 1 || STRLEN(tracking_output[0]) LE 1 ) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Tracking Output is void, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF

; We update the number of Active events we follow
numActiveEvents = N_ELEMENTS(tracking_output)


; We check if it is time to write events to the hek
events_write_deltat = SYSTIME(/SECONDS) -  write_events_last

IF events_write_deltat LT writeEventsFrequency THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT,  STRING(events_write_deltat) + ' seconds elapsed since last events write, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF ELSE BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, STRING(events_write_deltat) + ' seconds elapsed since last events write, writing to the HEK'
	ENDIF
	write_events_last = SYSTIME(/SECONDS)
ENDELSE


; We allocate space for the events
tracking_events = strarr(N_ELEMENTS(tracking_output))

; We need the header of one of the image to transform the coordinates
; Depends witch AR we decide to output from tracking, if it is the ones from the last image, THEN it is not necessary to update the header and stuff
; otherwise we do have to know which header to load 

header = headfits(image1)
wcs = fitshead2wcs(header)


FOR k = 0, N_ELEMENTS(tracking_output) - 1 DO BEGIN 

	; The output of Tracking is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label date_obs color	
	output = strsplit( tracking_output[k] , ' 	(),', /EXTRACT) 
	; We parse the output
	cartesian_x = FLOAT(output[0:4:2])
	cartesian_y = FLOAT(output[1:5:2])
	cartesian = FLTARR(2,N_ELEMENTS(cartesian_x))
	cartesian[0,*]=cartesian_x
	cartesian[1,*]=cartesian_y
	id = output[6]
	size = output[7]
	label = output[8]
	date_obs = output[9]
	color = output[10]
	
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
	event.required.FRM_ParamSet = 'image1 : calibrated image 171 A' $
		+ ', image2 : calibrated image 195 A' $
		+ ', dilation_factor= 12' $
		+ ', initialisation_type= FCM' $
		+ ', numerical_precision= double'; $
		;+ ', spocaArgsPreprocessing=' + spocaArgsPreprocessing $
		;+ ', spocaArgsNumberclasses=' + spocaArgsNumberclasses $
		;+ ', spocaArgsPrecision='  + spocaArgsPrecision $
		;+ ', spocaArgsBinsize='  + spocaArgsBinsize $
		;+ ', trackingArgsDeltat=' + trackingArgsDeltat $
		;+ ', trackingNumberImages=' + STRING(trackingNumberImages, FORMAT='(I)') $
		;+ ', trackingOverlap=' + STRING(trackingOverlap, FORMAT='(I)') 


	event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
	event.required.FRM_Contact = 'veronique.delouille@sidc.be'
	event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SPoCA'

	event.required.Event_StartTime = anytim(date_obs - writeEventsFrequency, /ccsds) ; The start time is the previous time we observed an event
	event.required.Event_EndTime = anytim(date_obs, /ccsds)  
	;event.optional.Event_Expires = anytim(sys2ut(), /ccsds)  
	event.required.Event_CoordSys = 'UTC-HPC-TOPO'
	event.required.Event_CoordUnit = 'arcsec,arcsec'
	event.required.Event_Coord1 = hpc_x[0]
	event.required.Event_Coord2 = hpc_y[0]
	event.required.Event_C1Error = 0 ; TBD
	event.required.Event_C2Error = 0 ; TBD
	event.optional.Event_Npixels = size
	event.optional.Event_PixelUnit = 'DN/s' ; ??? TBC for AIA
	event.optional.OBS_DataPrepURL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SPoCA' ; 

	event.required.BoundBox_C1LL = hpc_x[1]
	event.required.BoundBox_C2LL = hpc_y[1]
	event.required.BoundBox_C1UR = hpc_x[2]
	event.required.BoundBox_C2UR = hpc_y[2]
	
	; We write the VOevent
	
	IF KEYWORD_SET(write_file) THEN BEGIN
		export_event, event, /write, suff=label, buff=buff
	ENDIF ELSE BEGIN
		export_event, event, suffix=label, buff=buff
	ENDELSE
	
	tracking_events[k]=STRJOIN(buff, /SINGLE) ;
	
	; We update the last color assigned
	IF color GT last_color_assigned THEN BEGIN
		last_color_assigned = color
	ENDIF

ENDFOR 

Finish :	; Label for the case tracking_output is empty, or if not enough images where present for the tracking

; --------- We finish up -----------------

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

; We concatenate events from spoca and tracking
IF N_ELEMENTS(spoca_events) GT 0 THEN BEGIN
	events = spoca_events
ENDIF

IF N_ELEMENTS(tracking_events) GT 0 THEN BEGIN
	IF N_ELEMENTS(events) GT 0 THEN BEGIN
		events = [ events , tracking_events ]
	ENDIF ELSE BEGIN
		events = tracking_events
	ENDELSE
ENDIF
 

; This may need to change
imageRejected = 0

; We save the variables for next run

status.spoca_lastrun_number = spoca_lastrun_number
status.write_events_last = write_events_last
status.numActiveEvents = numActiveEvents
status.last_color_assigned = last_color_assigned

SAVE, status , DESCRIPTION='Spoca last run status variable at ' + SYSTIME() , FILENAME=outputStatusFilename, VERBOSE = debug
 
 
END 

