#pragma once

#pragma pack(1)

// 32bit値のエンディアン変換
#define TurnsDWORD(x)		((( (x) & 0x0FF000000 ) >> 24 ) | \
							 (( (x) & 0x000FF0000 ) >>  8 ) | \
							 (( (x) & 0x00000FF00 ) <<  8 ) | \
							 (( (x) & 0x0000000FF ) << 24 ) )

// 16bit値のエンディアン変換
#define TurnsWORD(x)		((( (x) & 0x0FF00 ) >> 8 ) | \
							 (( (x) & 0x000FF ) << 8 ) )

DWORD TEST_UNIT_READY(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	);

DWORD TRAY_OPEN(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	);

DWORD PRIVENT_ALLOW_MEDUIM_REMOVABLE(
	HTDODDDRIVE DriveHandle,
	BYTE byLock,
	CDBERRCODE *pCdbError
	);

//DWORD MODE_SENSE(
DWORD MODE_SENSE_05(			// 3.1.2.0
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	);

// 3.1.2.0->
DWORD MODE_SENSE_2A(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	);
// <-3.1.2.0

DWORD MODE_SELECT(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	);

DWORD GET_CONFIGURATION(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	);

DWORD READ_DISC_INFORMATION(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	);

DWORD RESERVE_TRACK_RZONE(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	);

typedef struct _TRACKINFORMATION {
	WORD	wTrackRZoneInformationLength;		//	00 01
	BYTE	byTrackRZoneNumberL;				//	02
	BYTE	bySessionBorderNumberL;				//	03
	BYTE	byReserved4;						//	04
	BYTE	TrackMode	: 4;					//	05	0123
	BYTE	Copy		: 1;					//	05	4
	BYTE	Damage		: 1;					//	05	5
	BYTE	LJRS		: 2;					//	05	67
	BYTE	DataMode	: 4;					//	06	0123
	BYTE	FP			: 1;					//	06	4
	BYTE	PacketInc	: 1;					//	06	5
	BYTE	Blank		: 1;					//	06	6
	BYTE	RT			: 1;					//	06	7
	BYTE	NWA_V		: 1;					//	07	0
	BYTE	LRA_V		: 1;					//	07	1
	BYTE	Reserved7	: 6;					//	07	234567
	DWORD	dwTrackRZoneStartAddress;			//	08 09 10 11
	DWORD	dwNextWritableAddress;				//	12 13 14 15
	DWORD	dwFreeBlocks;						//	16 17 18 19
	DWORD	dwFixedPacketSizeBlockingFactor;	//	20 21 22 23
	DWORD	dwTrackRZoneSize;					//	24 25 26 27
	DWORD	dwLastRecordedAddress;				//	28 29 30 31
	BYTE	byTrackRZoneNumberH;				//	32
	BYTE	bySessionBorderNumberH;				//	33
	WORD	wReserved34;						//	34 35
	DWORD	dwReadCompatibilityLBA;				//	36 37 38 39
} TRACKINFORMATION, *PTRACKINFORMATION;

DWORD READ_TRACK_INFORMATION(
	HTDODDDRIVE DriveHandle,
	DWORD dwTrackNo,
	PTRACKINFORMATION pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	);

DWORD READ10(
	HTDODDDRIVE DriveHandle,
	DWORD dwAddress,
	PBYTE pBuffer,
	WORD wSectorLength,
	CDBERRCODE *pCdbError
	);

DWORD WRITE10(
	HTDODDDRIVE DriveHandle,
	DWORD dwAddress,
	PBYTE pBuffer,
	WORD wSectorLength,
	CDBERRCODE *pCdbError
	);

DWORD SYNCHRONIZE_CACHE(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	);

// 3.1.2.0->
#pragma warning( disable : 4200 )

//	Mode Parameter Header
typedef struct _MODEPARAMETERHEADER{
	WORD	wModeDataLength;			// 00 01
	BYTE	byMediumTypeCode;			// 02
	BYTE	byReserved3;				// 03
	BYTE	byReserved4;				// 04
	BYTE	byReserved5;				// 05
	WORD	wBlockDescriptorLength0;	// 06 07
}MODEPARAMETERHEADER,*PMODEPARAMETERHEADER;

//	C/DVD Capabilities and Mechanical Status Mode Page Format
typedef struct _CDVDCAPABILITIESANDMECHANICALSTATUS {
	BYTE	PageCode		: 6;		// 00 012345
	BYTE	Reserved0		: 1;		// 00 6
	BYTE	PS				: 1;		// 00 7
	BYTE	byPageLength;				// 01

	BYTE	CDR_Read		: 1;		// 02 0
	BYTE	CDRW_Read		: 1;		// 02 1
	BYTE	Method2			: 1;		// 02 2
	BYTE	DVDROM_Read		: 1;		// 02 3
	BYTE	DVDR_Read		: 1;		// 02 4
	BYTE	DVDRAM_Read		: 1;		// 02 5
	BYTE	Reserved1		: 2;		// 02 67

	BYTE	CDR_Write		: 1;		// 03 0
	BYTE	CDRW_Write		: 1;		// 03 1
	BYTE	TestWrite		: 1;		// 03 2
	BYTE	Reserved2_3		: 1;		// 03 3
	BYTE	DVDR_Write		: 1;		// 03 4
	BYTE	DVDRAM_Write	: 1;		// 03 5
	BYTE	Reserved2_6		: 2;		// 03 67

	BYTE	AudioPlay		: 1;		// 04 0
	BYTE	Composite		: 1;		// 04 1
	BYTE	DigitalPort1	: 1;		// 04 2
	BYTE	DigitalPort2	: 1;		// 04 3
	BYTE	Mode2Form1		: 1;		// 04 4
	BYTE	Mode2Form2		: 1;		// 04 5
	BYTE	Multisession	: 1;		// 04 6
	BYTE	BUF				: 1;		// 04 7

	BYTE	CDDA			: 1;		// 05 0
	BYTE	CDDA_StreamAccurate : 1;	// 05 1
	BYTE	RW_Supported	: 1;		// 05 2
	BYTE	RW_DC			: 1;		// 05 3
	BYTE	C2PointersSupported : 1;	// 05 4
	BYTE	ISRC			: 1;		// 05 5
	BYTE	UPC				: 1;		// 05 6
	BYTE	ReadBarCodeCapable	: 1;	// 05 7

	BYTE	Lock			: 1;		// 06 0
	BYTE	LockState		: 1;		// 06 1
	BYTE	PreventJumper	: 1;		// 06 2
	BYTE	Eject			: 1;		// 06 3
	BYTE	Reserved6		: 1;		// 06 4
	BYTE	LMT				: 3;		// 06 567

	BYTE	SepVol			: 1;		// 07 0
	BYTE	SeparateChannelMute : 1;	// 07 1
	BYTE	SupportsDiscPresent	: 1;	// 07 2
	BYTE	SWSlotSelection	: 1;		// 07 3
	BYTE	SideChangeCapable	: 1;	// 07 4
	BYTE	RWinLeadinReadable	: 1;	// 07 5
	BYTE	Reserved7		: 2;		// 07 67

	WORD	wObsolete8;					// 08 09
	WORD	wNumberOfVolumeLevelsSupported;		// 10 11
	WORD	wBufferSizeSupportedByLogicalUnit;	// 12 13
	WORD	wObsolete14;				// 14 15
	BYTE	byObsolete16;				// 16

	BYTE	Reserved17_0	: 1;		// 17 0
	BYTE	BCKF			: 1;		// 17 1
	BYTE	RCK				: 1;		// 17 2
	BYTE	LSBF			: 1;		// 17 3
	BYTE	Length			: 2;		// 17 45
	BYTE	Reserved17_6	: 2;		// 17 67

	WORD	wObsolete18;				// 18 19
	WORD	wObsolete20;				// 20 21
	WORD	wCopyManagementRevisionSupported;	// 22 23
	BYTE	byReserved24;				// 24
	BYTE	byReserved25;				// 25
	BYTE	byReserved26;				// 26

	BYTE	RotationControlSelected : 2;// 27 01
	BYTE	Reserved27				: 6;// 27 234567

	WORD	wCurrentWriteSpeedSelected;	// 28 29

//	WORD	wNumberOfLogicalUnitWriteSpeedPerformanceDescriptorTables;	// 30 31
//	PLOGICALUNITWRITESPEEDPERFORMANCEDESCRIPTORTABLE	LogicalUnitWriteSpeedPerformanceDescriptorBlock[0];	// 32 -
} CDVDCAPABILITIESANDMECHANICALSTATUS, *PCDVDCAPABILITIESANDMECHANICALSTATUS;

typedef struct _CDVDCAPABILITIESANDMECHANICALSTATUSRESULTDATA {
	MODEPARAMETERHEADER					Header;
	CDVDCAPABILITIESANDMECHANICALSTATUS	Status;
} CDVDCAPABILITIESANDMECHANICALSTATUSRESULTDATA, *PCDVDCAPABILITIESANDMECHANICALSTATUSRESULTDATA;

/*****************************************************************************/
/* GET CONFIGURATION command                                                 */
/*****************************************************************************/
//	GET CONFIGURATION Command Descriptor Block
#define GETCONFIGURATION	0x46
typedef struct _GETCONFIGURATIONCDB {
	BYTE	byOpCode;					// 00
	BYTE	RT			: 2;			// 01 01
	BYTE	Reserved1	: 3;			// 01 234
	BYTE	LUN			: 3;			// 01 567
	WORD	wStartingFeatureNumber;		// 02 03
	BYTE	byReserved4;				// 04
	BYTE	byReserved5;				// 05
	BYTE	byReserved6;				// 06
	WORD	wAllocationLength;			// 07 08
	BYTE	Link		: 1;			// 09 0
	BYTE	Flag		: 1;			// 09 1
	BYTE	NACA		: 1;			// 09 2
	BYTE	Reserved9	: 3;			// 09 345
	BYTE	VendorSpecific : 2;			// 09 67
} GETCONFIGURATIONCDB, *PGETCONFIGURATIONCDB;
//	Feature Header
typedef struct _FEATUREHEADER {
	DWORD	dwDataLength;			// 00 01 02 03
	BYTE	byReserved4;			// 04
	BYTE	byReserved5;			// 05
	WORD	wCurrentProfile;		// 06 07
} FEATUREHEADER, *PFEATUREHEADER;

//	Feature List
#define FEATURELIST_PROFILELIST			0x0000
#define FEATURELIST_CORE				0x0001
#define FEATURELIST_MORPHING			0x0002
#define FEATURELIST_REMOVABLEMEDIUM		0x0003
#define FEATURELIST_WRITEPROTECT		0x0004
#define FEATURELIST_RANDOMREADABLE		0x0010
#define FEATURELIST_MULTIREAD			0x001D
#define FEATURELIST_CDREAD				0x001E
#define FEATURELIST_DVDREAD				0x001F
#define FEATURELIST_RANDOMWRITABLE		0x0020
#define FEATURELIST_INCREMENTALSTREAMINGWRITABLE	0x0021
#define FEATURELIST_SECTORERASABLE		0x0022
#define FEATURELIST_FORMATTABLE			0x0023
#define FEATURELIST_HWDEFECTMANAGEMENT	0x0024
#define FEATURELIST_WRITEONCE			0x0025
#define FEATURELIST_RESTRICTEDOVERWRITE	0x0026
#define FEATURELIST_CDRWCAVWRITE		0x0027
#define FEATURELIST_MRW					0x0028
#define FEATURELIST_ENHANCEDDEFECTREPORTING 		0x0029
#define FEATURELIST_RIGIDRESTRICTEDOVERWRITE 		0x002C
#define FEATURELIST_CDTRACKATONCE		0x002D
#define FEATURELIST_CDMASTERING			0x002E
#define FEATURELIST_DVDRWRITE			0x002F
#define FEATURELIST_POWERMANAGEMENT		0x0100
#define FEATURELIST_SMART				0x0101
#define FEATURELIST_EMBEDDEDCHANGER		0x0102
#define FEATURELIST_CDAUDIOANALOGPLAY	0x0103
#define FEATURELIST_MICROCODEUPDATE		0x0104
#define FEATURELIST_TIMEOUT				0x0105
#define FEATURELIST_DVDCSS				0x0106
#define FEATURELIST_REALTIMESTREAMING	0x0107
#define FEATURELIST_LOGICALUNITSERIALNUMBER			0x0108
#define FEATURELIST_DISCCONTROLBLOCKS	0x010A
#define FEATURELIST_DVDCPRM				0x010B
#define FEATURELIST_FIRMWAREINFORMATION	0x010C

//	Profile Descriptor
typedef struct _PROFILEDESCRIPTOR {
	WORD	wProfileNumber;				// 00 01
	BYTE	CurrentP	: 1;			// 02 0
	BYTE	Reserved2	: 7;			// 02 1234567
	BYTE	byReserved3;				// 03
} PROFILEDESCRIPTOR, *PPROFILEDESCRIPTOR;

//	Profile List Feature Descriptor
typedef struct _PROFILELISTFEATUREDESCRIPTOR {
	WORD	wFeatureCode;				// 00 01
	BYTE	Current		: 1;			// 02 0
	BYTE	Persistent	: 1;			// 02 1
	BYTE	Version		: 4;			// 02 2345
	BYTE	Reserved	: 2;			// 02 67
	BYTE	byAdditionalLength;			// 03
	PROFILEDESCRIPTOR	ProfileDescriptors[0];	// 04
} PROFILELISTFEATUREDESCRIPTOR, *PPROFILELISTFEATUREDESCRIPTOR;

typedef struct _PROFILELISTDATA {
	FEATUREHEADER					FeatureHeader;
	PROFILELISTFEATUREDESCRIPTOR	ProfileList;
} PROFILELISTDATA,*PPROFILELISTDATA;

//	Profile List
#define PROFILELIST_NONREMOVABLEDISK	0x0001
#define PROFILELIST_REMOVABLEDISK		0x0002
#define PROFILELIST_MO_ERASABLE			0x0003
#define PROFILELIST_MO_WRITEONCE		0x0004
#define PROFILELIST_ASMO				0x0005
#define PROFILELIST_CDROM				0x0008
#define PROFILELIST_CDR					0x0009
#define PROFILELIST_CDRW				0x000A
#define PROFILELIST_DVDROM				0x0010
#define PROFILELIST_DVDR				0x0011
#define PROFILELIST_DVDRAM				0x0012
#define PROFILELIST_DVDRW_RESTRICTEDOVERWRITE		0x0013
#define PROFILELIST_DVDRW_SEQUENTIALRECORDING		0x0014
#define PROFILELIST_DVDR_DL_SEQUENTIALRECORDING		0x0015	//	DVD-R Dual Layer Sequential Recording
#define PROFILELIST_DVDR_DL_JUMPRECORDING			0x0016	//	DVD-R Dual Layer Jump Recording

#define PROFILELIST_DVDRW_DL			0x0017
#define PROFILELIST_DVDD				0x0018

#define PROFILELIST_DVDxRW				0x001A
#define PROFILELIST_DVDxR				0x001B
#define PROFILELIST_DDCDROM				0x0020
#define PROFILELIST_DDCDR				0x0021
#define PROFILELIST_DDCDRW				0x0022
#define PROFILELIST_DVDxR_DL			0x002B

#define PROFILELIST_BDROM				0x0040
#define PROFILELIST_BDR_SEQUENTIAL		0x0041
#define PROFILELIST_BDR_RANDOM			0x0042
#define PROFILELIST_BDRE				0x0043

#define PROFILELIST_HD_DVDROM			0x0050
#define PROFILELIST_HD_DVDR				0x0051
#define PROFILELIST_HD_DVDRW			0x0052

//#define PROFILELIST_HD_DVDR_DL			0x0054		// Virtual HD DVD
//#define PROFILELIST_HD_DVDR_DL			0x0058
//#define PROFILELIST_HD_DVDRW_DL			0x005A

#define PROFILELIST_NOTSTANDARD			0xFFFF

#pragma warning( default : 4200 )

// メディア種別
#define MEDIA_CDROM							0x00000001		/*!< CD-ROM */
#define MEDIA_CDR							0x00000002		/*!< CD-R */
#define MEDIA_CDRW							0x00000004		/*!< CD-RW */
#define MEDIA_DVDROM						0x00000008		/*!< DVD-ROM */
#define MEDIA_DVDR							0x00000010		/*!< DVD-R */
#define MEDIA_DVDRAM						0x00000020		/*!< DVD-RAM */
#define MEDIA_DVDRW_RESTRICTEDOVERWRITE		0x00000040		/*!< DVD-RW(Restricted Overwrite) */
#define MEDIA_DVDRW_SEQUENTIALRECORDING		0x00000080		/*!< DVD-RW(Sequential Recording) */
#define MEDIA_DVDxRW						0x00000100		/*!< DVD+RW */
#define MEDIA_DVDxR							0x00000200		/*!< DVD+R */
#define MEDIA_DDCDROM						0x00000400		/*!< DDCD-ROM(未サポート) */
#define MEDIA_DDCDR							0x00000800		/*!< DDCD-R(未サポート) */
#define MEDIA_DDCDRW						0x00001000		/*!< DDCD-RW(未サポート) */
#define MEDIA_DVDxR_DL						0x00002000		/*!< DVD+R Double Layer */
#define MEDIA_DVDR_DL_SEQUENTIALRECORDING	0x00004000		/*!< DVD-R Dual Layer(Sequential Recording) */
#define MEDIA_DVDR_DL_JUMPRECORDING			0x00008000		/*!< DVD-R Dual Layer(Jump Recording) */
#define MEDIA_HD_DVDROM						0x00010000		/*!< HD DVD-ROM */
#define MEDIA_HD_DVDR						0x00020000		/*!< HD DVD-R */
#define MEDIA_HD_DVDRAM						0x00040000		/*!< HD DVD-RAM */
#define MEDIA_HD_DVDR_DL					0x00080000		/*!< HD DVD-R DL */
#define MEDIA_BDROM							0x00100000		/*!< BD-ROM */
#define MEDIA_BDR_SEQUENTIAL				0x00200000		/*!< BD-R(Sequential Recording) */
#define MEDIA_BDR_RANDOM					0x00400000		/*!< BD-R(Random Recording) */
#define MEDIA_BDRE							0x00800000		/*!< BD-RE */

DWORD ProfileToMedia(
	WORD wProfile
	);
// <-3.1.2.0

#pragma pack()
