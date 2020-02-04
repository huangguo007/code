/*
*
* API return code:
*	NO_ERROR			Success
*	ERROR_NOT_READY			Drive not ready
*	ERROR_INVALID_DRIVE		Invalid drive
*	ERROR_NOT_ENOUGH_MEMORY		Not enough memory
*	ERROR_INSUFFICIENT_BUFFER	Client buffer too small
*	ERROR_INVALID_PARAMETER		Invalid parameter
*	ERROR_BAD_DRIVER		MSCDEX unavailable
*	ERROR_CALL_NOT_IMPLEMENTED	Failed to issue DPMI service
*	ERROR_AUTODATASEG_EXCEEDS_64k	Buffer exceeds 64k bytes
*
* ---------------------------------------------------------------------
*/
#if !defined(__MSCD_h__)
#define  __MSCD_h__

#if defined(__cplusplus)
  extern "C" {
#endif	//__cplusplus

#if defined(__MSCD_Internal__)
  #define  MSCDEXPORT	__declspec(dllexport)
#else
  #define  MSCDEXPORT
#endif

#define  MSCDAPI	MSCDEXPORT long WINAPI


/*
* ---------------------------------------------------------------------
* ReadCDRomSectors - MSCDEX 1508h
*	disk read
* Note:
*	- When clibuf is NULL, the function returns required buffer size.
*	- MSCD_SECTOR_SIZE * sectnum should be smaller than 64k.
*	- Returns error under Win9x if the starting sector is less than 10h.
* ---------------------------------------------------------------------
*/
#define  MSCD_SECTOR_SIZE	2048
#define  MSCD_MAX_SECTORS	(65535 / MSCD_SECTOR_SIZE)
MSCDAPI ReadCDRomSectors(
	char		drive,		//i: drive letter
	unsigned long	sector,		//i: starting sector number
	unsigned short	sectnum,	//i: number of sectors to read
	void *		clibuf,		//o: sector buffer
	unsigned short*	bufsize		//i/o: client/required buffer size
);

#define  MSCD_DIRSIZE_DIRECT	255
#define  MSCD_DIRSIZE_CANONICAL	285
#define  MSCD_DIRF_CANONICAL	0x01

#if defined(__cplusplus)
  } //extern "C"
#endif	//__cplusplus

#endif //__MSCD_h__
//eof
