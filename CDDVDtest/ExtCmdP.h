/************************************************************************/
/*	ファイル名：Cmdpst.h												*/
/*																		*/
/*	CommamndPassthough用DLLアプリケーション＆DLL共通ヘッダ				*/
/*																		*/
/*	Copyright(C) TOSHIBA Corporation 2002. All right reserved.			*/
/*																		*/
/*		2002.04.15		(DME)		新規作成							*/
/************************************************************************/
#ifndef __PASSTEXT_H__
#define __PASSTEXT_H__

//#include <devioctl.h>

/************************************************************************/
/*	定数定義															*/
/************************************************************************/
// 起動OSの種類
#define				WIN9X_FAMILY			0x01					// Windows95,98,98SE,ME
#define				WIN2K_FAMILY			0x02					// Windows2000,XP

// ハンドル識別用シグネーチャ
#define				SIGNATURE				"PSTCMD  "				// シグネーチャーコード

// コマンドパケット
#define			 	CDB6					06						// 6バイトコマンド
#define 			CDB10					10						// 10バイトコマンド
#define 			CDB12					12						// 12バイトコマンド

// データ転送の方向
#define				DATA_OUT		        0						// データ書き出し
#define				DATA_IN					1						// データ読み込み
#define				DATA_NON				1						// データ転送なし

// タイムアウト値
#define				TIME_1S					1
#define				TIME_3S					3
#define				TIME_5S					5
#define				TIME_7S					7
#define				TIME_10S				10
#define				TIME_30S				30

// デバイス情報
typedef struct _DEVICEINFO {
	UCHAR			DeviceType;										// デバイスタイプ
	UCHAR			VenderIdentify[8];								// ベンダー名
	UCHAR			ProductIdentify[16];							// プロダクト名
	UCHAR			Revision[4];									// レビジョン
} DEVICEINFO, *PDEVICEINFO;

// コマンド発行用構造体
typedef struct _USERPACKET {
	UCHAR			Packet[16];										// 発行するコマンド
	UCHAR			PacketLength;									// パケットの長さを指定
	PUCHAR			UserBuffer;										// ユーザーバッファへのポインタ
	ULONG			UserBufferLength;								// ユーザーバッファの長さ
	ULONG			Timeout;										// タイムアウト値を指定
	UCHAR			DataDirection;									// データの方向
	UCHAR			SenseData[24];									// エラー時の詳細
} USERPACKET,*PUSERPACKET;

// エラーコード
#define				STATUS_SUCCESS			0x00000000				// 正常終了
#define				ERROR_NOT_CONNECT		0x80000001				// 指定されたデバイスが見つからない
#define				ERROR_HANDLE_INVALID	0x80000002				// ハンドルが違う。
#define				ERROR_CMD_PACKET		0x80000021				// 発行したコマンドにエラーが発生		
#define				ERROR_DEVIOCTL			0x800000FF				// 予期せぬエラー

#define				ERROR_ASPI_DLL			0x80001000				// ASPIのDLLが異常

//#define				ERROR_HANDLE_INVALID	0x80000012				// ハンドルが違う。
#define				ERROR_IO_CONTROL		0x80001000				// コマンドエラー

#pragma warning(default:4200)
#endif

/*
** End-Of-File
*/
