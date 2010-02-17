; +
; Description:
;	IDL code to call spoca and the tracking
; Author:
; 	Benjamin Mampaey
; Date:
; 	16 February 2010
; Params:
; 	aia_image1, aia_image2: in, required, type string, aia images filename of wavelength 171 and 195
 



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
	tracking_frequency = tracking_frequency, $
	tracking_max_number_images = tracking_max_number_images
	

; set debugging
debug = 1

; newline shortcut for the c++ programmer
endl=STRING(10B)

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

; Test for filenames
IF N_PARAMS() LT 1 THEN BEGIN ; TODO Change back to 2

	error = [ error, 'Wrong number of images passed as arguments, expecting 2']
	RETURN
	
ENDIF ELSE BEGIN
	
;	IF (~ IDL_ENSURE_STRING(aia_image1)) || (~ IDL_ENSURE_STRING(aia_image2) )  THEN BEGIN
;		error = [ error, 'Images file names must be of type string' ]
;		RETURN
;	ENDIF

ENDELSE

IF N_ELEMENTS(output_directory) EQ 0 THEN output_directory = 'results/'

; SPoCA parameters

spoca_bin = 'bin/SPoCA_HEK.x'

; I propose to set the right default values in the version we deliver
spoca_args_preprocessing = '1'  
spoca_args_numberclasses = '3'
spoca_args_precision = '0.000001'
spoca_args_centersfile = output_directory + 'centers.txt'




; Tracking parameters

tracking_bin = 'bin/Tracking_HEK.x'
IF N_ELEMENTS(tracking_frequency) EQ 0 THEN tracking_frequency = 30 * 60 ; It is in seconds
IF N_ELEMENTS(tracking_max_number_images) EQ 0 THEN tracking_max_number_images = 3


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
	; TODO Should we cleanup and exit ?
	
ENDIF


; As output of spoca we receive the AR out of the limb for warning (they are not tracked)

IF (debug GT 0) THEN BEGIN
	PRINT, 'Spoca Output is : ', endl + spoca_output
ENDIF

; TODO Check that output is not null

; We convert the cartesian pixel coodinates into heliographic coordinates

; We need the header of one of the image to call the coord_cart_helio procedure

header = HEADFITS(aia_image1)
index=fitshead2struct(header)
r=1.3

; We allocate space for the events
events = strarr(N_ELEMENTS(spoca_output))

FOR k = 0, N_ELEMENTS(spoca_output) - 1 DO BEGIN 

	; The output of SpoCA is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label
	output = strsplit( spoca_output[k] , ' 	(),', /EXTRACT) 
	; We separate x and y
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
	
	; We transform into long and lat
	coord_cart_helio,index,r,cartesian_x,cartesian_y,arcsec_x, arcsec_y, helio_long, helio_lat
	
	IF (debug GT 0) THEN BEGIN
		PRINT , "long, latitude heliographic coordinates for the region ", k
		PRINT, helio_long
		PRINT, helio_lat
	ENDIF
	
	; Create an Hek event and fill it
		
	event = struct4event('AR')
	
	;event.required.KB_ArchivDate = '???'
	;event.required.KB_ArchivID = '???'
	;event.required.KB_Archivist = '???'
	;event.required.KB_ArchivURL = '???'

	event.required.OBS_Observatory = 'SDO'
	event.required.OBS_Instrument = 'AIA'
	event.required.OBS_ChannelID = '???'
	event.required.OBS_MeanWavel = 171+195/2 ; ??? IS IT CORRECT
	event.required.OBS_WavelUnit = 'Angstroms'

	event.required.FRM_Name = 'SPoCA'
	event.optional.FRM_VersionNumber = 1
	;event.required.FRM_Identifier = '???'
	event.required.FRM_Institute ='ROB'
	event.required.FRM_HumanFlag = 'F'
	;event.required.FRM_ParamSet = '???'
	event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
	event.required.FRM_Contact = 'veronique.delouille@sidc.be'
	event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SpocA'

	;event.required.Event_StartTime = '???' ; TBD from the tracking ?
	;event.required.Event_EndTime = '???' ; TBD from the tracking ?
	;event.optional.Event_Expires = '???'
	event.required.Event_CoordSys = 'UTC-HGS-TOPO'
	event.required.Event_CoordUnit = 'deg,deg'
	event.required.Event_Coord1 = helio_long[0]
	event.required.Event_Coord2 = helio_lat[0]
	event.required.Event_C1Error = 0
	event.required.Event_C2Error = 0
	event.optional.Event_Npixels = size
	;event.optional.Event_PixelUnit = '???'
	;event.optional.OBS_DataPrepURL = '???'
	;event.optional.Event_ClippedSpatial = '???'
	;event.optional.Event_ClippedTemporal = '???'

	event.required.BoundBox_C1LL = helio_long[1]
	event.required.BoundBox_C2LL = helio_lat[1]
	event.required.BoundBox_C1UR = helio_long[2]
	event.required.BoundBox_C2UR = helio_lat[2]
	
	
	; We write the VOevent
	
	IF KEYWORD_SET(write_file) THEN BEGIN
		export_event, event, /write, suff=label, buff=buff, filenameout=fn0
	ENDIF ELSE BEGIN
		export_event, event, suffix=label, buff=buff, filenameout=fn0
	ENDELSE
	
	events[k]=STRJOIN(buff, /SINGLE) ;


ENDFOR 

; We take care of the tracking

deltat = SYSTIME(/SECONDS) - tracking_lastrun
ARmaps = FILE_SEARCH(output_directory, '*ARmap.tracking.fits', /TEST_READ, /TEST_REGULAR , /TEST_WRITE  ) 

IF (debug GT 0) THEN BEGIN
	PRINT, endl, "*********************************************************"
	PRINT , deltat, " seconds elapsed since last tracking"
	PRINT , "Found files : ", endl + ARmaps
ENDIF

IF (deltat GE tracking_frequency && N_ELEMENTS(ARmaps) GE tracking_max_number_images) THEN BEGIN
	
	
	; We cleanup old files
	number_of_files_to_delete = N_ELEMENTS(ARmaps) - tracking_max_number_images
	
	IF (number_of_files_to_delete GT 0) THEN BEGIN
	
		files_to_delete = ARmaps[0:number_of_files_to_delete-1]
		
		IF (debug GT 0) THEN BEGIN
			PRINT , "Deleting files : ", endl + files_to_delete
		ENDIF
		
		FILE_DELETE, files_to_delete , /ALLOW_NONEXISTENT , /NOEXPAND_PATH , VERBOSE = debug
				
		ARmaps = ARmaps[number_of_files_to_delete:*]
	
	ENDIF
	
	tracking_lastrun = SYSTIME(/SECONDS)
	
	IF (debug GT 0) THEN BEGIN

		PRINT, 'About to run : ' , tracking_bin + ' ' + STRJOIN( ARmaps , ' ', /SINGLE) 
	
	ENDIF
	
	SPAWN, tracking_bin + ' ' + STRJOIN( ARmaps , ' ', /SINGLE) , tracking_output, tracking_errors, EXIT_STATUS=tracking_exit 

	
	IF (tracking_exit NE 0) THEN BEGIN

		error = [ error, 'Error executing ', tracking_bin + ' ' + STRJOIN( ARmaps , ' ', /SINGLE) ]
		error = [ error, tracking_errors ]
		; What do we do in case of error ?
		
	ENDIF


	; As output of tracking we receive the AR intra limb 
	; TODO Check that output is not null

	IF (debug GT 0) THEN BEGIN
		PRINT, 'Tracking Output is :', endl + tracking_output
	ENDIF

	; We convert the caartesian pixel coodinates into heliographic coordinates

	; We need the header of one of the image to call the coord_cart_helio procedure
	; Depends witch ar we decide to output from tracking, if it is the ones from the last image, THEN it is not necessary to update the header and stuff
	; otherwise it si complicated because we have to make sure we keep all the headers of each file

	r=1.3 ; TODO Put r=1 when answer from alisdair

	FOR k = 0, N_ELEMENTS(tracking_output) - 1 DO BEGIN 

		; The output of Tracking is (center.x, center.y) (boxLL.x, boxLL.y) (boxUR.x, boxUR.y) id size label color	
		output = strsplit( tracking_output[k] , ' 	(),', /EXTRACT) 
		; We separate x and y
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
		
		; We transform into long and lat
		coord_cart_helio,index,r,cartesian_x,cartesian_y,arcsec_x, arcsec_y, helio_long, helio_lat
		
		IF (debug GT 0) THEN BEGIN
			PRINT , "long, latitude heliographic coordinates for the region ", label
			PRINT, helio_long
			PRINT, helio_lat
		ENDIF
		
		; Create an Hek event and fill it
			
		event = struct4event('AR')
		
		;event.required.KB_ArchivDate = '???'
		;event.required.KB_ArchivID = '???'
		;event.required.KB_Archivist = '???'
		;event.required.KB_ArchivURL = '???'

		event.required.OBS_Observatory = 'SDO'
		event.required.OBS_Instrument = 'AIA'
		event.required.OBS_ChannelID = '???'
		event.required.OBS_MeanWavel = 171+195/2 ; ??? IS IT CORRECT
		event.required.OBS_WavelUnit = 'Angstroms'

		event.required.FRM_Name = 'SPoCA'
		event.optional.FRM_VersionNumber = 1
		;event.required.FRM_Identifier = '???'
		event.required.FRM_Institute ='ROB'
		event.required.FRM_HumanFlag = 'F'
		;event.required.FRM_ParamSet = '???'
		event.required.FRM_DateRun = anytim(sys2ut(), /ccsds)
		event.required.FRM_Contact = 'veronique.delouille@sidc.be'
		event.required.FRM_URL = 'http://sdoatsidc.oma.be/web/sidcsdosoftware/SpocA'

		;event.required.Event_StartTime = '???' ; TBD from the tracking ?
		;event.required.Event_EndTime = '???' ; TBD from the tracking ?
		;event.optional.Event_Expires = '???'
		event.required.Event_CoordSys = 'UTC-HGS-TOPO'
		event.required.Event_CoordUnit = 'deg,deg'
		event.required.Event_Coord1 = helio_long[0]
		event.required.Event_Coord2 = helio_lat[0]
		event.required.Event_C1Error = 0
		event.required.Event_C2Error = 0
		event.optional.Event_Npixels = size
		;event.optional.Event_PixelUnit = '???'
		;event.optional.OBS_DataPrepURL = '???'
		;event.optional.Event_ClippedSpatial = '???'
		;event.optional.Event_ClippedTemporal = '???'

		event.required.BoundBox_C1LL = helio_long[1]
		event.required.BoundBox_C2LL = helio_lat[1]
		event.required.BoundBox_C1UR = helio_long[2]
		event.required.BoundBox_C2UR = helio_lat[2]
		
		; We write the VOevent
		
		IF KEYWORD_SET(write_file) THEN BEGIN
			export_event, event, /write, suff=label, buff=buff, filenameout=fn0 
		ENDIF ELSE BEGIN
			export_event, event, suffix=label, buff=buff, filenameout=fn0
		ENDELSE
		
		events[k]=STRJOIN(buff, /SINGLE) ;
 


	ENDFOR 


ENDIF

; We save the variables for next run

status.spoca_lastrun_number = spoca_lastrun_number
status.tracking_lastrun = tracking_lastrun
 
SAVE, status , DESCRIPTION='Spoca last run status variable at ' + SYSTIME() , FILENAME=outputStatusFilename, VERBOSE = debug
 
 
END 

