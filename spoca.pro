; +
; Description:
;	IDL code to call spoca and the tracking
; Author:
; 	Benjamin Mampaey
; Date:
; 	16 February 2010
; Params:
; 	aia_image1, aia_image2: in, required, type string, aia images filename of wavelength 171 and 195
; -
 



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
	spoca_args_preprocessing = spoca_args_preprocessing, $
	spoca_args_numberclasses = spoca_args_numberclasses, $
	spoca_args_precision = spoca_args_precision, $
	tracking_args_deltat = tracking_args_deltat, $
	tracking_number_images = tracking_number_images, $
	tracking_overlay = tracking_overlay
	
	

; set debugging
debug = 1

; newline shortcut for the c++ programmer
endl=STRING(10B)


; --------- We take care of the arguments -----------------

; We look at what is the runMode and take care of the status

SWITCH runMode OF 
	'Construct':	BEGIN
				
				spoca_lastrun_number = 0
				tracking_lastrun = SYSTIME(/SECONDS)
				
				status = {spoca_lastrun_number : spoca_lastrun_number, tracking_lastrun : tracking_lastrun}
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
				tracking_lastrun = status.tracking_lastrun
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
	PRINT, 'tracking_lastrun : ', tracking_lastrun
	
ENDIF

; We verify our module arguments

; We test the filenames

IF N_PARAMS() LT 1 THEN BEGIN ; TODO Change back to 2

	error = [ error, 'Wrong number of images passed as arguments, expecting 2']
	RETURN
	
ENDIF ELSE BEGIN
	
;	IF (~ IDL_ENSURE_STRING(aia_image1)) || (~ IDL_ENSURE_STRING(aia_image2) )  THEN BEGIN
;		error = [ error, 'Images file names must be of type string' ]
;		RETURN
;	ENDIF
; TODO test for file existence

ENDELSE

IF N_ELEMENTS(output_directory) EQ 0 THEN output_directory = 'results/'

; SPoCA parameters

spoca_bin = 'bin/SPoCA_HEK.x'

IF N_ELEMENTS(spoca_args_preprocessing) EQ 0 THEN spoca_args_preprocessing = '1'  
IF N_ELEMENTS(spoca_args_numberclasses) EQ 0 THEN spoca_args_numberclasses = '3'
IF N_ELEMENTS(spoca_args_precision) EQ 0 THEN spoca_args_precision = '0.000001'
spoca_args_centersfile = output_directory + 'centers.txt'


; Tracking parameters

tracking_bin = 'bin/Tracking_HEK.x'
IF N_ELEMENTS(tracking_deltat) EQ 0 THEN tracking_deltat = 3600 ; It is in seconds
IF N_ELEMENTS(tracking_number_images) EQ 0 THEN tracking_number_images = 3
IF N_ELEMENTS(tracking_overlay) EQ 0 THEN tracking_overlay = 0.3


IF (debug GT 0) THEN BEGIN
	PRINT, endl, "*********************************************************"
ENDIF


; --------- We take care of running spoca -----------------

; We initialise correctly the arguments for SPoCA_HEK

++spoca_lastrun_number

spoca_args = ' -P ' + spoca_args_preprocessing + ' -C ' + spoca_args_numberclasses + ' -p ' + spoca_args_precision + ' -B ' + spoca_args_centersfile + ' -O ' + output_directory + STRING(spoca_lastrun_number, FORMAT='(I010)') + ' ' + aia_image1 ;+ ' ' + aia_image2

IF (debug GT 0) THEN BEGIN

	PRINT, 'About to run : ' , spoca_bin , spoca_args
	
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

IF (N_ELEMENTS(spoca_output) GE 1 && N_ELEMENTS(spoca_output[0]) LE 1 ) THEN BEGIN
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
	id = output[6]
	size = output[7]
	label = output[8]
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y cartesians coordinates for the region ", k
		PRINT, cartesian_x
		PRINT, cartesian_y
	ENDIF
	
	
	; We convert the cartesian pixel coodinates into WCS
	cartesian = FLTARR(2,N_ELEMENTS(cartesian_x))
	cartesian[0,*]=cartesian_x
	cartesian[1,*]=cartesian_y
        wcs_coord = WCS_GET_COORD(wcs, cartesian)
        
	; We convert the WCS coodinates into helioprojective cartesian
	WCS_CONVERT_FROM_COORD, wcs, wcs_coord, 'HPC', hpc_x, hpc_y, hpc_z
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y, z HPC coordinates for the region ", k
		PRINT, hpc_x
		PRINT, hpc_y
		PRINT, hpc_z
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

	event.required.Event_StartTime = anytim(sys2ut(), /ccsds) ; TBD from the tracking ???
	event.required.Event_EndTime = anytim(sys2ut(), /ccsds) ; TBD from the tracking ???
	;event.optional.Event_Expires = anytim(sys2ut(), /ccsds) ; ??? Not sure what to put here
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
	;event.optional.Event_ClippedTemporal = '???'

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

tracking_overlay = UINT(tracking_number_images / tracking_overlay)

tracking_args = ' -d ' + tracking_args_deltat + ' -D ' + STRING(tracking_overlay, FORMAT = '(I)') + ' ' + STRJOIN( ARmaps , ' ', /SINGLE)
	
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
IF (N_ELEMENTS(tracking_output) GE 1 && N_ELEMENTS(tracking_output[0]) LE 1 ) THEN BEGIN
	IF (debug GT 0) THEN BEGIN
		PRINT, 'Tracking Output is void, going to Finish'
	ENDIF
	GOTO, Finish
ENDIF


; We allocate space for the events
tracking_events = strarr(N_ELEMENTS(tracking_output))

; We need the header of one of the image to transform the coordinates
; Depends witch AR we decide to output from tracking, if it is the ones from the last image, THEN it is not necessary to update the header and stuff
; otherwise we do have to know wich header to load 

;header = headfits(aia_image1)
;wcs = fitshead2wcs(header)


FOR k = 0, N_ELEMENTS(tracking_output) - 1 DO BEGIN 

	; The output of Tracking is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label color	
	output = strsplit( tracking_output[k] , ' 	(),', /EXTRACT) 
	; We parse the output
	cartesian_x = FLOAT(output[0:4:2])
	cartesian_y = FLOAT(output[1:5:2])
	id = output[6]
	size = output[7]
	label = output[8]
	color = output[9]
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y cartesians coordinates for the region ", label
		PRINT, cartesian_x
		PRINT, cartesian_y
	ENDIF
	
	; We convert the cartesian pixel coodinates into WCS
	cartesian = FLTARR(2,N_ELEMENTS(cartesian_x))
	cartesian[0,*]=cartesian_x
	cartesian[1,*]=cartesian_y
        wcs_coord = WCS_GET_COORD(wcs, cartesian)
        
	; We convert the WCS coodinates into helioprojective cartesian
	WCS_CONVERT_FROM_COORD, wcs, wcs_coord, 'HPC', hpc_x, hpc_y, hpc_z
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "x, y, z HPC coordinates for the region ", k
		PRINT, hpc_x
		PRINT, hpc_y
		PRINT, hpc_z
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

	event.required.Event_StartTime = anytim(sys2ut(), /ccsds) ; TBD from the tracking ???
	event.required.Event_EndTime = anytim(sys2ut(), /ccsds) ; TBD from the tracking ???
	;event.optional.Event_Expires = anytim(sys2ut(), /ccsds) ; ??? Not sure what to put here
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
	;event.optional.Event_ClippedTemporal = '???'

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
number_of_files_to_delete = N_ELEMENTS(ARmaps) - tracking_overlay

IF (number_of_files_to_delete GT 0) THEN BEGIN

	files_to_delete = ARmaps[0:number_of_files_to_delete-1]
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "Deleting files : ", endl + files_to_delete
	ENDIF
	
	FILE_DELETE, files_to_delete , /ALLOW_NONEXISTENT , /NOEXPAND_PATH , VERBOSE = debug

ENDIF


; We concatenate events from spoca and tracking
events = [ spoca_events , tracking_events ] 

; We save the variables for next run

status.spoca_lastrun_number = spoca_lastrun_number
status.tracking_lastrun = tracking_lastrun
 
SAVE, status , DESCRIPTION='Spoca last run status variable at ' + SYSTIME() , FILENAME=outputStatusFilename, VERBOSE = debug
 
 
END 

