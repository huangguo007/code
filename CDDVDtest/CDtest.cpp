// CDtest.cpp : アプリケーション用クラスの定義を行います。
//

#include "stdafx.h"
#include "CDtest.h"
#include "CDtestDlg.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp

BEGIN_MESSAGE_MAP(CCDtestApp, CWinApp)
	//{{AFX_MSG_MAP(CCDtestApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp クラスの構築

CCDtestApp::CCDtestApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
	m_bProgramStatus = FALSE;
	m_iReturnValue = ERROR_FAIL_END;
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CCDtestApp オブジェクト

CCDtestApp theApp;

CAllFileControl CLog;

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp クラスの初期化

BOOL CCDtestApp::InitInstance()
{
	AfxEnableControlContainer();

	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さくしたけ
	//  れば以下の特定の初期化ルーチンの中から不必要なものを削除して
	//  ください。
/******************
#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL 内で MFC を使う場合はここをコールしてください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクする場合はここをコールしてください。
#endif
*******************/

	CCDtestDlg		dlg;
	BOOL			bResult;

	//-----------------------------------------
	// 開発用・ログの初期化
	//-----------------------------------------
	LOG_START();
	LOG_OUT("(S)\n");

	//-----------------------------------------
	// アプリケーション情報の取得
	//-----------------------------------------
	bResult = GetAppInfo();
	if ( bResult == FALSE )
	{
		LOG_OUT("GetAppInfo() failed.\n");
		LOG_OUT("Failure, Acquisition of application informationd.\n");

		goto END_InitInstance;
	}

	//-----------------------------------------
	// メイン・ダイアログの起動
	//-----------------------------------------
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//       記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <ｷｬﾝｾﾙ> で消された時のコードを
		//       記述してください。
	}

END_InitInstance:

	LOG_OUT("(E)\n");

	// ダイアログが閉じられてからアプリケーションのメッセージ ポンプを開始するよりは、
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

int CCDtestApp::ExitInstance()
{
	LOG_OUT("(S)\n");

	LOG_OUT("m_iReturnValue = %ld\n", m_iReturnValue);

	LOG_OUT("(E)\n");

	//-----------------------------------------
	// 開発用・ログの後始末
	//-----------------------------------------
	LOG_END();

	CWinApp::ExitInstance();

	return m_iReturnValue;
}

////////////////////////////////////////////////////////////////////////
// アプリケーションの情報を取得する
////////////////////////////////////////////////////////////////////////
BOOL CCDtestApp::GetAppInfo()
{
	LOG_OUT("(S)\n");

	BOOL			bFuncResult = FALSE;
	BOOL			bResult;
	DWORD			dwResult;

	// 現在の実行モジュールを含む EXE ファイルのフル・パスを取得
	dwResult = ::GetModuleFileName( NULL, m_szModuleFileName_FullPath, _MAX_PATH );
	if ( dwResult == 0 )
	{
		LOG_OUT("GetModuleFileName() failed.\n");
		goto END_GetAppInfo;
	}
	else
	{
		LOG_OUT("GetModuleFileName() successed.\n");
		LOG_OUT("szModuleFileName = %s\n", m_szModuleFileName_FullPath );
	}

	// 現在の実行モジュールを含む EXE ファイルのフル・パスを要素ごとに分解
	::_tsplitpath( m_szModuleFileName_FullPath, m_szModuleFileName_Drive, m_szModuleFileName_Dir, m_szModuleFileName_FileName, m_szModuleFileName_EXt );
	LOG_OUT("m_szModuleFileName_Drive    = %s\n", m_szModuleFileName_Drive);
	LOG_OUT("m_szModuleFileName_Dir      = %s\n", m_szModuleFileName_Dir);
	LOG_OUT("m_szModuleFileName_FileName = %s\n", m_szModuleFileName_FileName);
	LOG_OUT("m_szModuleFileName_EXt      = %s\n", m_szModuleFileName_EXt);

	// INIファイルパスの準備
	m_strIniFilePath.Format(".\\%s.ini", m_szModuleFileName_FileName);
	LOG_OUT("m_strIniFilePath            = %s\n", m_strIniFilePath);

	// ログファイルパスの準備
	m_strLogFilePath = T_XML_LOG_PATH;
	LOG_OUT("m_strLogFilePath            = %s\n", m_strLogFilePath);

	// ログファイル名の準備
	if ( stricmp( m_szModuleFileName_FileName, "cdtest" ) == 0 )
	{
		m_strLogFileName = "CDtest";
	}
	else if ( stricmp( m_szModuleFileName_FileName, "dvdtest" ) == 0 )
	{
		m_strLogFileName = "DVDtest";
	}
	LOG_OUT("m_strLogFileName            = %s\n", m_strLogFileName);

	// ファイルバージョンの取得
	bResult = GetFileVersion();
	if ( bResult == FALSE )
	{
		LOG_OUT("GetFileVersion() failed.\n");
		goto END_GetAppInfo;
	}

	bFuncResult = TRUE;

END_GetAppInfo:

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

BOOL CCDtestApp::GetFileVersion()
{
	LOG_OUT("(S)\n");

	BOOL			bFuncResult = FALSE;
	UINT			uLen;
	LPWORD			lpWord;
	LPTSTR			lpStr;
	DWORD			dwLangAndCharSet;
	char			szKey[256];
	LPVOID			lpBuff = NULL;
	BOOL			bResult;

// バージョン情報取得に必要なライブラリの読み込み
#pragma comment(lib, "version.lib")

	// ログのバージョン情報サイズの取得
	DWORD dwZero = 0;
	DWORD dwReserved;
	DWORD dwVerInfoSize = ::GetFileVersionInfoSize( theApp.m_szModuleFileName_FullPath, &dwReserved );
	if ( dwVerInfoSize <= 0 )
	{
		LOG_OUT("GetFileVersionInfoSize() failed.\n");
		goto END_GetFileVersion;
	}

	// ログのバージョン情報のバッファの準備
	lpBuff = malloc( dwVerInfoSize );
	if ( !lpBuff )
	{
		LOG_OUT("malloc() failed.\n");
		goto END_GetFileVersion;
	}

	// ログのバージョン情報の取得
	bResult = GetFileVersionInfo( theApp.m_szModuleFileName_FullPath, 0, dwVerInfoSize, lpBuff );
	if ( bResult == FALSE )
	{
		LOG_OUT("GetFileVersionInfo() failed.\n");
		goto END_GetFileVersion;
	}

	// 言語・キャラクタセット変換テーブルを取得
	bResult = VerQueryValue( lpBuff, "\\VarFileInfo\\Translation", (void**)&lpWord, &uLen );
	if ( bResult == FALSE )
	{
		LOG_OUT("VerQueryValue() failed.\n");
		goto END_GetFileVersion;
	}

	// lpWord[0]は言語セット、lpWord[1]はキャラクタセット
	dwLangAndCharSet = MAKELONG( lpWord[1], lpWord[0] );

	// ファイルバージョンの取得
	sprintf( szKey, "\\StringFileInfo\\%08x\\%s", dwLangAndCharSet, "FileVersion" );
	bResult = VerQueryValue( lpBuff, szKey, (void **)&lpStr, &uLen );
	if ( bResult == FALSE )
	{
		LOG_OUT("VerQueryValue() failed.\n");
		goto END_GetFileVersion;
	}
	else
	{
		m_strFileVersion = lpStr;
		LOG_OUT("m_strFileVersion = %s\n", m_strFileVersion);
	}

	bFuncResult = TRUE;

END_GetFileVersion:

	if ( lpBuff )
	{
		free( lpBuff );
		lpBuff = NULL;
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}