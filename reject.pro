; +
; Description:
;	IDL code to test the quality of a file
; Authors:
; 	Paolo Grigis, Ryan Timmons, Benjamin Mampaey
; Date:
; 	9 November 2010
; Params:
; 	header: in, required, struct containing the keywords of a fits file
;	imageRejected: out, required, flag to tell if the image is to be rejected
;	rejectionString: out, optional, type string, reason for the rejection


PRO checkQuality, header, imageRejected, rejectionString

;Actual checking of headers, standardized with code borrowed from Ryan/Paolo's work on the flares
;Note that this version of the code opts for a matched 171/193 both
;good, instead of allowing a 171/193 pair say 40 sec apart.

imageRejected = 0

;reject open filters
IF tag_exist(header, 'WAVE_STR') && strmatch(header.wave_str, 'open', /FOLD_CASE) EQ 1 THEN BEGIN
	rejectionString = 'Open filter (WAVE_STR =~ open)'
	imageRejected = 1
	RETURN
ENDIF

;check for darks or non -light images
IF tag_exist(header, 'IMG_TYPE') && header.img_type NE 'LIGHT' THEN BEGIN 
	rejectionString = 'Dark image (IMG_TYPE != LIGHT)'
	imageRejected = 1
	RETURN
ENDIF

;New eclipse flag
IF tag_exist (header, 'aiagp6') && header.aiagp6 NE 0 THEN BEGIN
	rejectionString = 'Eclipse (AIAGP6 != 0)'
	imageRejected = 1
	RETURN
ENDIF


IF tag_exist(header, "exptime") && header.exptime LT 1.5 THEN BEGIN
	rejectionString = 'Exposure time too short (exptime <= 1.5)'
	imageRejected = 1
	RETURN
ENDIF


IF tag_exist(header, "aiftsid") && header.aiftsid GE 49152 THEN BEGIN
	rejectionString = 'Calibration image (aiftsid >= 49152)'
	imageRejected = 1
	RETURN
ENDIF


IF tag_exist (header, "percentd") && header.percentd LT 99.9999 THEN BEGIN
	rejectionString = 'Missing pixels (percentd < 99.9999)'
	imageRejected = 1
	RETURN
ENDIF


; Quality keyword in AIA - details TBD
; Need to understand in more details what "quality" means as a flag
; Now is e.g. set to 131072=2^17 just means ISS loops is open
; Seems to be OK for now
; Eventually we want to reject everything but 0 - but for now just reject based on a list of forbidden bits

IF tag_exist(header,'QUALITY') THEN BEGIN 

	;create an array of number such that the j-th elementh as bit j set to 1 and all others set to 0
	;i.e. 1,2,4,8,...,2^J,...
	BitArray=2UL^ulindgen(32)
	BitSet=(header.quality AND BitArray) NE 0
	
	; If any of these bits is set - reject the image
	ForbiddenBits=[0,1,2,3,4,12,13,14,15,16,17,18,20,21,31] 
	;RPT - added bits for ISS loop (17), ACS_MODE not SCIENCE (12)
	;RPT - 9/25/10 - bits 20, 21, below from Rock's new def file
	;	20	(AIFCPS <= -20 or AIFCPS >= 100)	;	AIA focus out of range 
	;	21	AIAGP6 != 0					;	AIA register flag


	IF total(BitSet[ForbiddenBits]) GT 0 THEN BEGIN 
		rejectionString = 'Bad quality1 ('+header.quality+')'
		imageRejected = 1
		RETURN
	ENDIF 

ENDIF 

IF tag_exist(header,'QUALLEV0') THEN BEGIN 

	;create an array of number such that the j-th elementh as bit j set to 1 and all others set to 0
	;i.e. 1,2,4,8,...,2^J,...
	BitArray=2UL^ulindgen(32)
	BitSet=(header.quallev0 AND BitArray) NE 0
	
	; If any of these bits is set - reject the image
	ForbiddenBits=[0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23,24,25,26,27,28] 
	;RPT - added bits for ISS loop (17), ACS_MODE not SCIENCE (12)


	IF total(BitSet[ForbiddenBits]) GT 0 THEN BEGIN 
		rejectionString = 'Bad quality0 ('+header.quallev0+')'
		imageRejected = 1
		RETURN
	ENDIF 

ENDIF 

END


