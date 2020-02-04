// CDtest.h : CDTEST アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_CDTEST_H__FAA262C4_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
#define AFX_CDTEST_H__FAA262C4_6413_11D5_AE4C_0010A405E9A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

#include <stdlib.h>
#include "Logger.h"
//#include "LogFile.h"

#define T_XML_LOG_PATH	"C:\\Logs\\"

// 戻り値
#define ERROR_NORMAL_END		0	// PASS終了
#define ERROR_FAIL_END			1	// FAIL終了
#define ERROR_PARAMETER			2	// 指定されたパラメータに異常がある場合
#define ERROR_INIFILE			3	// INIファイル関連の異常がある場合
#define ERROR_LOGFILE			4	// LOGファイル関連の異常がある場合

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp:
// このクラスの動作の定義に関しては CDtest.cpp ファイルを参照してください。
//

class CCDtestApp : public CWinApp
{
public:
	CCDtestApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CCDtestApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション

	//{{AFX_MSG(CCDtestApp)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	int					m_iReturnValue;								// アプリケーション終了コード（エラーコード）
	BOOL				m_bProgramStatus;							// 最終テスト結果
	CString				m_strErrorCode[5];							// エラーコード

	CString				m_strIniFilePath;							// INIファイルパス
	CString				m_strLogFilePath;							// ログファイルパス
	CString				m_strLogFileName;							// ログファイル名

	CString				m_strFileVersion;							// ファイルバージョン
	TCHAR				m_szModuleFileName_FullPath[_MAX_PATH];		// モジュールファイル：ファイル名（フルパス）
	TCHAR				m_szModuleFileName_Drive[_MAX_DRIVE];		// モジュールファイル：ドライブ名
	TCHAR				m_szModuleFileName_Dir[_MAX_DIR];			// モジュールファイル：ディレクトリ
	TCHAR				m_szModuleFileName_FileName[_MAX_FNAME];	// モジュールファイル：ファイル名
	TCHAR				m_szModuleFileName_EXt[_MAX_EXT];			// モジュールファイル：拡張子


	virtual int ExitInstance();
	BOOL		GetAppInfo();										// アプリケーションの情報を取得する
	BOOL		GetFileVersion();									// ファイルバージョンを取得する
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CDTEST_H__FAA262C4_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
