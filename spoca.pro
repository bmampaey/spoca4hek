; +
; Description:
;	IDL code to call spoca and the tracking
; Author:
; 	Benjamin Mampaey
; Date:
; 	16 February 2010
; Params:
; 	aia_image1, aia_image2: in, required, type string, aia images filename of wavelength 171 and 195
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
;	output_directory: in, required, type string, folder where spoca can store temporary files (The modules manage the cleanup of old files) 
;	write_events_frequency: in, required, type integer, number of seconds between events write to the HEK
;	spoca_args_preprocessing: in, optional, type string, type of image preprocessing for spoca
;	spoca_args_numberclasses: in, optional, type string, number of classes for spoca
;	spoca_args_precision: in, optional, type string, precision for spoca
;	spoca_args_binsize: in, optional, type string, bin size for spoca
;	tracking_args_deltat: in, optional, type string, maximal time difference between 2 images for tracking
;	tracking_number_images: in, optional, type integer, number of images to track at the same time
;	tracking_overlap: in, optional, type real, proportion of the number of images to overlap between tracking succesive run
; -
 
; TODO :
; - Handle the imageRejected and quality when we know what is that quality keyword
; - Check for numActiveEvents


PRO SPoCA, aia_image1, aia_image2, $
	events = events, $
	write_file = write_file, $
	restart = restart, $ ; Is it obsolete ?
	error = error, $
	imageRejected = imageRejected, $
	status = status, $
	runMode = runMode, $
	inputStatusFilename = inputStatusFilename, $
	outputStatusFilename = outputStatusFilename, $
	numActiveEvents = numActiveEvents, $
	output_directory = output_directory, $
	write_events_frequency = write_events_frequency, $
	spoca_args_preprocessing = spoca_args_preprocessing, $
	spoca_args_numberclasses = spoca_args_numberclasses, $
	spoca_args_precision = spoca_args_precision, $
	spoca_args_binsize = spoca_args_binsize, $
	tracking_args_deltat = tracking_args_deltat, $
	tracking_number_images = tracking_number_images, $
	tracking_overlap = tracking_overlap
	
	

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
				
				status = {spoca_lastrun_number : spoca_lastrun_number, write_events_last : write_events_last}
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
				BREAK
   			END 

	'Clear Events':	BEGIN
				; TODO 
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
	
ENDIF

; We verify our module arguments

; We test the filenames

IF N_PARAMS() LT 2 THEN BEGIN 

	error = [ error, 'Wrong number of images passed as arguments, expecting 2']
	RETURN
	
ENDIF 


IF (~ FILE_TEST( aia_image1, /READ, /REGULAR)) || (~ FILE_TEST( aia_image2, /READ, /REGULAR) )  THEN BEGIN
	error = [ error, 'Cannot find images ' + aia_image1 + ' or ' + aia_image2 ]
	RETURN
ENDIF


IF N_ELEMENTS(output_directory) EQ 0 THEN output_directory = 'results/'
IF N_ELEMENTS(write_events_frequency) EQ 0 THEN write_events_frequency = 4 * 3600

; SPoCA parameters

spoca_bin = 'bin/SPoCA_HEK.x'

IF N_ELEMENTS(spoca_args_preprocessing) EQ 0 THEN spoca_args_preprocessing = '5'  
IF N_ELEMENTS(spoca_args_numberclasses) EQ 0 THEN spoca_args_numberclasses = '4'
IF N_ELEMENTS(spoca_args_precision) EQ 0 THEN spoca_args_precision = '0.000000001'
IF N_ELEMENTS(spoca_args_binsize) EQ 0 THEN spoca_args_binsize = '0.01,0.01'
spoca_args_centersfile = output_directory + 'centers.txt'


; Tracking parameters

tracking_bin = 'bin/Tracking_HEK.x'
IF N_ELEMENTS(tracking_deltat) EQ 0 THEN tracking_deltat = 3600 ; It is in seconds
IF N_ELEMENTS(tracking_number_images) EQ 0 THEN tracking_number_images = 3
IF N_ELEMENTS(tracking_overlap) EQ 0 THEN tracking_overlap = 0.3


IF (debug GT 0) THEN BEGIN
	PRINT, endl, "*********************************************************"
ENDIF


; --------- We take care of running spoca -----------------

; We initialise correctly the arguments for SPoCA_HEK

++spoca_lastrun_number

spoca_args =	' -P ' + spoca_args_preprocessing + $
		' -C ' + spoca_args_numberclasses + $
		' -p ' + spoca_args_precision + $
		' -z ' + spoca_args_binsize + $
		' -B ' + spoca_args_centersfile + $
		' -O ' + output_directory + STRING(spoca_lastrun_number, FORMAT='(I010)') + $
		' ' + aia_image1 + ' ' + aia_image2

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

IF (N_ELEMENTS(spoca_output) LE 1 && N_ELEMENTS(spoca_output[0]) LE 1 ) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Spoca Output is void, going to Tracking'
	ENDIF
	GOTO, Tracking
ENDIF

; We allocate space for the events
spoca_events = strarr(N_ELEMENTS(spoca_output))

; We need the header of one of the image to transform the coordinates
header = headfits(aia_image1)
wcs = fitshead2wcs(header)

FOR k = 0, N_ELEMENTS(spoca_output) - 1 DO BEGIN 

	; The output of SpoCA is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label
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
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "cartesians coordinates for the region ", k
		PRINT, cartesian
	ENDIF
	
	; We convert the cartesian pixel coodinates into WCS

        wcs_coord = WCS_GET_COORD(wcs, cartesian)
        
	; We convert the WCS coodinates into helioprojective cartesian
	WCS_CONVERT_FROM_COORD, wcs, wcs_coord, 'HPC', hpc_x, hpc_y
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y, z HPC coordinates for the region ", k
		PRINT, hpc_x
		PRINT, hpc_y
	ENDIF
	
	; Create an Hek event and fill it
		
	event = struct4event('AR')
	
	;event.required.KB_ArchivDate = anytim(sys2ut(), /ccsds) ; ??? Not sure what to put here
	event.required.KB_ArchivID = 'SPoCA' ; ??? Not sure what to put here
	event.required.KB_Archivist = '' ; ??? Not sure what to put here
	event.required.KB_ArchivURL = '' ; ??? Not sure what to put here

	event.required.OBS_Observatory = 'SDO'
	event.required.OBS_Instrument = 'AIA'
	event.required.OBS_ChannelID = 'AIA 171, AIA 195'
	event.required.OBS_MeanWavel = 171; ??? There should be 2 values, one for 195 also
	event.required.OBS_WavelUnit = 'Angstroms'

	event.required.FRM_Name = 'SPoCA'
	event.optional.FRM_VersionNumber = 1
	event.required.FRM_Identifier = 'vdelouille'
	event.required.FRM_Institute ='ROB'
	event.required.FRM_HumanFlag = 'F'
	event.required.FRM_ParamSet = 'calibration=1' $
		+ ' tracking_deltat=' + STRING(tracking_deltat, FORMAT='(I)') $
		+ ' tracking_number_images=' + STRING(tracking_number_images, FORMAT='(I)') $
		+ ' spoca_args_preprocessing=' + STRING(spoca_args_preprocessing, FORMAT='(I)') $
		+ ' spoca_args_numberclasses=' + STRING(spoca_args_numberclasses, FORMAT='(I)') $
		+ ' spoca_args_precision='  + STRING(spoca_args_precision, FORMAT='(F)') 

	event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
	event.required.FRM_Contact = 'veronique.delouille@sidc.be'
	event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SpocA'

	event.required.Event_StartTime = anytim(sys2ut(), /ccsds)
	event.required.Event_EndTime = anytim(sys2ut(), /ccsds)  
	;event.optional.Event_Expires = anytim(sys2ut(), /ccsds)  
	event.required.Event_CoordSys = 'UTC-HPC-TOPO'
	event.required.Event_CoordUnit = 'deg,deg'
	event.required.Event_Coord1 = hpc_x[0]
	event.required.Event_Coord2 = hpc_y[0]
	event.required.Event_C1Error = 0
	event.required.Event_C2Error = 0
	event.optional.Event_Npixels = size
	event.optional.Event_PixelUnit = 'DN/s' ; ??? TBC
	event.optional.OBS_DataPrepURL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SpocA' ; ??? TBC
	event.optional.Event_ClippedSpatial = 'F'  ; ??? TBC
	event.optional.Event_ClippedTemporal = 'F'

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

ARmaps = FILE_SEARCH(output_directory, '*ARmap.tracking.fits', /TEST_READ, /TEST_REGULAR , /TEST_WRITE  ) 

IF (debug GT 0) THEN BEGIN
	PRINT , "Found files : ", endl + ARmaps
ENDIF

IF (N_ELEMENTS(ARmaps) LT tracking_number_images) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Not enough files to do tracking, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF
		
; We initialise correctly the arguments for Tracking_HEK

tracking_overlap = UINT(tracking_number_images / tracking_overlap)

tracking_args =	' -d ' + tracking_args_deltat + $
		' -D ' + STRING(tracking_overlap, FORMAT = '(I)') + $
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
IF (N_ELEMENTS(tracking_output) LE 1 && N_ELEMENTS(tracking_output[0]) LE 1 ) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Tracking Output is void, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF

; We check if it is time to write events to the hek
events_write_deltat = SYSTIME(/SECONDS) -  write_events_last

IF events_write_deltat LT write_events_frequency THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT,  STRING(events_write_deltat) + ' seconds elapsed since last events write, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF ELSE BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, STRING(events_write_deltat) + ' seconds elapsed since last events write, writing to the HEK'
	ENDIF
	Event_StartTime = anytim(write_events_last, /ccsds)
	write_events_last = SYSTIME(/SECONDS)
ENDELSE


; We allocate space for the events
tracking_events = strarr(N_ELEMENTS(tracking_output))

; We need the header of one of the image to transform the coordinates
; Depends witch AR we decide to output from tracking, if it is the ones from the last image, THEN it is not necessary to update the header and stuff
; otherwise we do have to know wich header to load 

header = headfits(aia_image1)
wcs = fitshead2wcs(header)


FOR k = 0, N_ELEMENTS(tracking_output) - 1 DO BEGIN 

	; The output of Tracking is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label color	
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
	color = output[9]
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "cartesians coordinates for the region ", k
		PRINT, cartesian
	ENDIF
		
	; We convert the cartesian pixel coodinates into WCS
        wcs_coord = WCS_GET_COORD(wcs, cartesian)
        
	; We convert the WCS coodinates into helioprojective cartesian
	WCS_CONVERT_FROM_COORD, wcs, wcs_coord, 'HPC', hpc_x, hpc_y
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y, z HPC coordinates for the region ", k
		PRINT, hpc_x
		PRINT, hpc_y
	ENDIF
	
	; Create an Hek event and fill it
		
	event = struct4event('AR')
	
	;event.required.KB_ArchivDate = anytim(sys2ut(), /ccsds) ; ??? Not sure what to put here
	event.required.KB_ArchivID = 'SPoCA' ; ??? Not sure what to put here
	event.required.KB_Archivist = '' ; ??? Not sure what to put here
	event.required.KB_ArchivURL = '' ; ??? Not sure what to put here

	event.required.OBS_Observatory = 'SDO'
	event.required.OBS_Instrument = 'AIA'
	event.required.OBS_ChannelID = 'AIA 171, AIA 195'
	event.required.OBS_MeanWavel = 171; ??? There should be 2 values, one for 195 also
	event.required.OBS_WavelUnit = 'Angstroms'

	event.required.FRM_Name = 'SPoCA'
	event.optional.FRM_VersionNumber = 1
	event.required.FRM_Identifier = 'vdelouille'
	event.required.FRM_Institute ='ROB'
	event.required.FRM_HumanFlag = 'F'
	event.required.FRM_ParamSet = 'calibration=1' $
		+ ' tracking_deltat=' + STRING(tracking_deltat, FORMAT='(I)') $
		+ ' tracking_number_images=' + STRING(tracking_number_images, FORMAT='(I)') $
		+ ' spoca_args_preprocessing=' + STRING(spoca_args_preprocessing, FORMAT='(I)') $
		+ ' spoca_args_numberclasses=' + STRING(spoca_args_numberclasses, FORMAT='(I)') $
		+ ' spoca_args_precision='  + STRING(spoca_args_precision, FORMAT='(F)') 

	event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
	event.required.FRM_Contact = 'veronique.delouille@sidc.be'
	event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SpocA'

	event.required.Event_StartTime = Event_StartTime ; We say that the event started when we last wrote to the hek
	event.required.Event_EndTime = anytim(sys2ut(), /ccsds)  
	;event.optional.Event_Expires = anytim(sys2ut(), /ccsds) 
	event.required.Event_CoordSys = 'UTC-HPC-TOPO'
	event.required.Event_CoordUnit = 'deg,deg'
	event.required.Event_Coord1 = hpc_x[0]
	event.required.Event_Coord2 = hpc_y[0]
	event.required.Event_C1Error = 0
	event.required.Event_C2Error = 0
	event.optional.Event_Npixels = size
	event.optional.Event_PixelUnit = 'DN/s' ; ??? TBC
	event.optional.OBS_DataPrepURL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SpocA' ; ??? TBC
	event.optional.Event_ClippedSpatial = 'T'  ; ??? TBC
	event.optional.Event_ClippedTemporal = 'F'

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

ENDFOR 

Finish :	; Label for the case tracking_output is empty, or if not enough images where present for the tracking

; --------- We finish up -----------------

; We cleanup old files

IF (N_ELEMENTS(ARmaps) GE tracking_number_images) THEN BEGIN

	number_of_files_to_delete = N_ELEMENTS(ARmaps) - tracking_overlap
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
numActiveEvents = N_ELEMENTS(events)
imageRejected = 0

; We save the variables for next run

status.spoca_lastrun_number = spoca_lastrun_number
status.write_events_last = write_events_last
 
SAVE, status , DESCRIPTION='Spoca last run status variable at ' + SYSTIME() , FILENAME=outputStatusFilename, VERBOSE = debug
 
 
END 

