// atapi.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Atapi.h"

extern LPTDODDSENDCOMMAND m_lpfnTDODDSendCommand;

//
//	TEST_UNIT_READY
//
DWORD TEST_UNIT_READY(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 6;
	Command.DataIn = TDODDCOMMAND_DATA_UNSPECIFIED;
	Command.Cdb[0] = 0x00;	// TEST UNIT READY

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	TRAY_OPEN
//
DWORD TRAY_OPEN(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 6;
	Command.DataIn = TDODDCOMMAND_DATA_UNSPECIFIED;
	Command.Cdb[0] = 0x1B;	// START STOP UNIT
	Command.Cdb[4] = 0x02;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	PRIVENT ALLOW MEDUIM REMOVABLE
//
DWORD PRIVENT_ALLOW_MEDUIM_REMOVABLE(
	HTDODDDRIVE DriveHandle,
	BYTE byLock,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 6;
	Command.DataIn = TDODDCOMMAND_DATA_UNSPECIFIED;
	Command.Cdb[0] = 0x1E;		// PRIVENT ALLOW MEDUIM REMOVABLE
	Command.Cdb[4] = byLock;	// LOCK = 1, UNLOCK = 0

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	MODE SENSE
//
//DWORD MODE_SENSE(
DWORD MODE_SENSE_05(			// 3.1.2.0
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_IN;
	Command.Cdb[0] = 0x5A;		// MODE_SENSE
	Command.Cdb[2] = 0x05;		// PC = 0, Page Code = 5
	Command.Cdb[7] = (UCHAR)((wBufferLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wBufferLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wBufferLength;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

// 3.1.2.0->
DWORD MODE_SENSE_2A(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_IN;
	Command.Cdb[0] = 0x5A;		// MODE_SENSE
	Command.Cdb[1] = 0x08;		// DBD = 1
	Command.Cdb[2] = 0x2A;		// PC = 0, Page Code = 2A
	Command.Cdb[7] = (UCHAR)((wBufferLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wBufferLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wBufferLength;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}
// <-3.1.2.0

//
//	MODE SELECT
//
DWORD MODE_SELECT(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_OUT;
	Command.Cdb[0] = 0x55;		// MODE_SELECT
	Command.Cdb[1] = 0x10;		// PF = 1
	Command.Cdb[7] = (UCHAR)((wBufferLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wBufferLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wBufferLength;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	GET CONFIGURATION
//
DWORD GET_CONFIGURATION(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_IN;
	Command.Cdb[0] = 0x46;		// GET_CONFIGURATION
	Command.Cdb[1] = 0x00;		// Feature Number = 0
	Command.Cdb[7] = (UCHAR)((wBufferLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wBufferLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wBufferLength;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	READ DISC INFORMATION
//
DWORD READ_DISC_INFORMATION(
	HTDODDDRIVE DriveHandle,
	PBYTE pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_IN;
	Command.Cdb[0] = 0x51;		// READ_DISC_INFORMATION
	Command.Cdb[1] = 0x00;		// Data Type = 0(Standard Disc Information)
	Command.Cdb[7] = (UCHAR)((wBufferLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wBufferLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wBufferLength;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	RESERVE TRACK RZONE
//
DWORD RESERVE_TRACK_RZONE(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_UNSPECIFIED;
	Command.Cdb[0] = 0x53;		// RESERVE_TRACK_RZONE
	Command.Cdb[5] = 0x00;		// (MSB)
	Command.Cdb[6] = 0x11;		// Reservation Size
	Command.Cdb[7] = 0x7F;		//   0x00117FF0 (1146864) = 約2240MB
	Command.Cdb[8] = 0xF0;		// (LSB)

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	READ TRACK INFORMATION
//
DWORD READ_TRACK_INFORMATION(
	HTDODDDRIVE DriveHandle,
	DWORD dwTrackNo,
	PTRACKINFORMATION pBuffer,
	WORD wBufferLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_IN;
	Command.Cdb[0] = 0x52;		// READ_TRACK_INFORMATION
	Command.Cdb[1] = 0x01;		// Address/Number Type Value = 1

	// Track Number
	Command.Cdb[2] = (UCHAR)((dwTrackNo & 0xFF000000) >> 24);
	Command.Cdb[3] = (UCHAR)((dwTrackNo & 0x00FF0000) >> 16);
	Command.Cdb[4] = (UCHAR)((dwTrackNo & 0x0000FF00) >> 8);
	Command.Cdb[5] = (UCHAR)(dwTrackNo & 0x000000FF);

	Command.Cdb[7] = (UCHAR)((wBufferLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wBufferLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wBufferLength;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	if (dwStatus == ERROR_SUCCESS && GetSenseCode(*pCdbError) == 0) {
		// エンディアン変換
		pBuffer->dwTrackRZoneStartAddress = TurnsDWORD(pBuffer->dwTrackRZoneStartAddress);
		pBuffer->dwNextWritableAddress = TurnsDWORD(pBuffer->dwNextWritableAddress);
		pBuffer->dwFreeBlocks = TurnsDWORD(pBuffer->dwFreeBlocks);
		pBuffer->dwFixedPacketSizeBlockingFactor = TurnsDWORD(pBuffer->dwFixedPacketSizeBlockingFactor);
		pBuffer->dwTrackRZoneSize = TurnsDWORD(pBuffer->dwTrackRZoneSize);
		pBuffer->dwLastRecordedAddress = TurnsDWORD(pBuffer->dwLastRecordedAddress);
		pBuffer->dwReadCompatibilityLBA = TurnsDWORD(pBuffer->dwReadCompatibilityLBA);
	}

	return dwStatus;
}

//
//	READ10
//
DWORD READ10(
	HTDODDDRIVE DriveHandle,
	DWORD dwAddress,
	PBYTE pBuffer,
	WORD wSectorLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_IN;
	Command.Cdb[0] = 0x28;		// READ10

	Command.Cdb[2] = (UCHAR)((dwAddress & 0xFF000000) >> 24);
	Command.Cdb[3] = (UCHAR)((dwAddress & 0x00FF0000) >> 16);
	Command.Cdb[4] = (UCHAR)((dwAddress & 0x0000FF00) >> 8);
	Command.Cdb[5] = (UCHAR)(dwAddress & 0x000000FF);

	Command.Cdb[7] = (UCHAR)((wSectorLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wSectorLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wSectorLength * 2048;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	WRITE10
//
DWORD WRITE10(
	HTDODDDRIVE DriveHandle,
	DWORD dwAddress,
	PBYTE pBuffer,
	WORD wSectorLength,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_OUT;
	Command.Cdb[0] = 0x2A;		// WRITE10

	Command.Cdb[2] = (UCHAR)((dwAddress & 0xFF000000) >> 24);
	Command.Cdb[3] = (UCHAR)((dwAddress & 0x00FF0000) >> 16);
	Command.Cdb[4] = (UCHAR)((dwAddress & 0x0000FF00) >> 8);
	Command.Cdb[5] = (UCHAR)(dwAddress & 0x000000FF);

	Command.Cdb[7] = (UCHAR)((wSectorLength & 0xFF00) >> 8);
	Command.Cdb[8] = (UCHAR)(wSectorLength & 0x00FF);

	Command.DataTransferLength = (ULONG)wSectorLength * 2048;
	Command.DataBufferOffset = (PVOID)pBuffer;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

//
//	SYNCHRONIZE CACHE
//
DWORD SYNCHRONIZE_CACHE(
	HTDODDDRIVE DriveHandle,
	CDBERRCODE *pCdbError
	)
{
	DWORD dwStatus = ERROR_SUCCESS;
	TDODDCOMMAND Command;
	ZeroMemory(&Command, sizeof(TDODDCOMMAND));

	Command.CdbLength = 10;
	Command.DataIn = TDODDCOMMAND_DATA_UNSPECIFIED;
	Command.Cdb[0] = 0x35;		// SYNCHRONIZE_CACHE
	Command.Cdb[1] = 0x02;

	dwStatus = m_lpfnTDODDSendCommand(DriveHandle, &Command, pCdbError);

	return dwStatus;
}

// 3.1.2.0->
// PROFILELIST_xxxをMEDIA_xxxに変換
DWORD ProfileToMedia(
	WORD wProfile
	)
{
	switch(wProfile) {
		case PROFILELIST_CDROM:
			return MEDIA_CDROM;
			break;
		case PROFILELIST_CDR:
			return MEDIA_CDR;
			break;
		case PROFILELIST_CDRW:
			return MEDIA_CDRW;
			break;
		case PROFILELIST_DVDROM:
			return MEDIA_DVDROM;
			break;
		case PROFILELIST_DVDR:
			return MEDIA_DVDR;
			break;
		case PROFILELIST_DVDRAM:
			return MEDIA_DVDRAM;
			break;
		case PROFILELIST_DVDRW_RESTRICTEDOVERWRITE:
			return MEDIA_DVDRW_RESTRICTEDOVERWRITE;
			break;
		case PROFILELIST_DVDRW_SEQUENTIALRECORDING:
			return MEDIA_DVDRW_SEQUENTIALRECORDING;
			break;
		case PROFILELIST_DVDR_DL_SEQUENTIALRECORDING:
			return MEDIA_DVDR_DL_SEQUENTIALRECORDING;
			break;
		case PROFILELIST_DVDR_DL_JUMPRECORDING:
			return MEDIA_DVDR_DL_JUMPRECORDING;
			break;
		case PROFILELIST_DVDxRW:
			return MEDIA_DVDxRW;
			break;
		case PROFILELIST_DVDxR:
			return MEDIA_DVDxR;
			break;
		case PROFILELIST_DDCDROM:
			return MEDIA_DDCDROM;
			break;
		case PROFILELIST_DDCDR:
			return MEDIA_DDCDR;
			break;
		case PROFILELIST_DDCDRW:
			return MEDIA_DDCDRW;
			break;
		case PROFILELIST_DVDxR_DL:
			return MEDIA_DVDxR_DL;
			break;
//		case PROFILELIST_HD_DVDROM:
//			return MEDIA_HD_DVDROM;
//			break;
//		case PROFILELIST_HD_DVDR:
//			return MEDIA_HD_DVDR;
//			break;
//		case PROFILELIST_HD_DVDRAM:
//			return MEDIA_HD_DVDRAM;
//			break;
//		case PROFILELIST_HD_DVDR_DL:
//			return MEDIA_HD_DVDR_DL;
//			break;
		case PROFILELIST_BDROM:
			return MEDIA_BDROM;
			break;
		case PROFILELIST_BDR_SEQUENTIAL:
			return MEDIA_BDR_SEQUENTIAL;
			break;
		case PROFILELIST_BDR_RANDOM:
			return MEDIA_BDR_RANDOM;
			break;
		case PROFILELIST_BDRE:
			return MEDIA_BDRE;
			break;



	}
	return 0;
}
// <-3.1.2.0
