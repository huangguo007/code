// CDtestDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "CDtest.h"
#include "CDtestDlg.h"

#include "ExtCmdP.h"

#include <winioctl.h>  // From the Win32 SDK \Mstools\Include

#include "LogFile.h"

extern CCDtestApp theApp;
extern CAllFileControl CLog;

// TDODDLib.dll
HMODULE m_hTDODDLib = NULL;
LPTDODDSETFLAGS m_lpfnTDODDSetFlags;
LPTDODDOPENDRIVE m_lpfnTDODDOpenDrive;
LPTDODDCLOSEDRIVE m_lpfnTDODDCloseDrive;
LPTDODDDEVICEIOCONTROL m_lpfnTDODDDeviceIoControl;
LPTDODDLOCKDRIVE m_lpfnTDODDLockDrive;
LPTDODDUNLOCKDRIVE m_lpfnTDODDUnlockDrive;
LPTDODDLOCKVOLUME m_lpfnTDODDLockVolume;
LPTDODDUNLOCKVOLUME m_lpfnTDODDUnlockVolume;
LPTDODDSENDCOMMAND m_lpfnTDODDSendCommand;


#define ErrCode_Test_AppInit						"001"
#define ErrCode_Test_DriveDetect					"002"
#define ErrCode_Test_DriveReady						"003"
#define ErrCode_Test_TrayLock						"004"
#define ErrCode_Test_Volume_Lock					"005"
#define ErrCode_Test_MediaInformation				"006"
#define ErrCode_Test_DataCompare					"007"
#define ErrCode_Test_Cancel							"008"
#define ErrCode_Test_DataSave						"009"

#define TEST_MEDIA_VOLUME_LABEL_OF_DVD				"DVDTest Media"
#define TEST_MEDIA_VOLUME_LABEL_OF_CD				"CDTest Media"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#include "resultcsv.h"
//#pragma comment(lib, "resultcsv.lib")

//#include "MSCD.h"
//#pragma comment(lib, "MSCD.lib")

//#include "tndmsgdlg.h"
//#pragma comment(lib, "tndmsgdlg.lib")


// ドライブタイプ
#define EX_DRIVE_INVALID    0
#define EX_DRIVE_REMOVABLE  1 
#define EX_DRIVE_FIXED      2
#define EX_DRIVE_REMOTE     3
#define EX_DRIVE_CDROM      4
#define EX_DRIVE_FLOPPY     5
#define EX_DRIVE_RAMDISK    6


#define WTCOMSTRMSG "WAITING TESTER ACTION"
//#define WTCOMSTRSTR "\"/KS=WTCOMSTR\\\"WAITING TESTER ACTION\\\"\""
#define WTCOMSTRSTR "\"/KS=WTCOMSTR \\\"WAITING TESTER ACTION\\\"\""


//int CheckMediaReady(char* szDrives);
long CheckMediaReady(char* szDrives);

long  save( const char* fname, const void* buf, size_t size );
long  lord( const char* fname, void* buf, size_t size );
void EjectCD(void);
int EjectCDWin98(void);
BOOL EjectCDWin2K(void);
int SendWin2K(char * szMsg);
int SendWin98(char * szMsg);
BOOL IsNT();														// Windows NT OSであるかどうかの判定


void FreeTDODDLib(void);
BOOL LoadTDODDLib(void);
bool GetWorkDir(char *buf, int size, char *File_Name);


int		m_MonitorMax = 1;
RECT	*m_rectMonitor = NULL;


////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	TEST_TYPE m_TestType;
	VOID SetTestType( TEST_TYPE type ){ m_TestType = type; }
	CString m_strTestTypeAndVersion;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strTestTypeAndVersion(_T(""))
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT

	m_TestType = TEST_CD;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STATIC_1, m_strTestTypeAndVersion);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:
	if ( m_TestType == TEST_CD )
	{
		// ウインドウタイトルの設定
		this->SetWindowText("About CDtest");

		// テストタイプ＆テストバージョンの設定
		m_strTestTypeAndVersion = "CDtest Version " + theApp.m_strFileVersion;

		// アイコンの設定
		((CStatic*)this->GetDlgItem( IDC_STATIC_0 ))->SetIcon( LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_CD)) );
	}
	else if ( m_TestType == TEST_DVD )
	{
		// ウインドウタイトルの設定
		this->SetWindowText("About DVDtest");

		// テストタイプ＆テストバージョンの設定
		m_strTestTypeAndVersion = "DVDtest Version " + theApp.m_strFileVersion;

		// アイコンの設定
		((CStatic*)this->GetDlgItem( IDC_STATIC_0 ))->SetIcon( LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_DVD)) );
	}
	else
	{
	}
	
	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// メッセージ ハンドラがありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDtestDlg ダイアログ

CCDtestDlg::CCDtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDtestDlg::IDD, pParent)
{
	LOG_OUT("(S)\n");

	//{{AFX_DATA_INIT(CCDtestDlg)
		// メモ: この位置に ClassWizard によってメンバの初期化が追加されます。
	//}}AFX_DATA_INIT
	// メモ: LoadIcon は Win32 の DestroyIcon のサブシーケンスを要求しません。
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_DVD);

	//------------------------------------------------
	// メンバ変数の初期化
	//------------------------------------------------
	m_TDODDHandle							= (HTDODDDRIVE)INVALID_HANDLE_VALUE;
    m_bRetryFlag							= FALSE;
	m_bTrayOpenFlag_TestEnd					= TRUE;
	m_TestType								= TEST_CD;
	m_CurrentStep							= Step_Drive_Detect;
	m_bVolumeLock							= FALSE;
	m_bTrayLock								= FALSE;
	m_ulSectorOffset						= 16;
	m_ulSectorNum							= 5;
	m_iMaxRetryCount						= 5;
	m_iCurrentRetryCount					= 0;
	m_iDeviceOpen_MaxRetryCount				= 5;
	m_iDeviceOpen_WaitTime					= 3000;
	m_iDeviceOpen_CurrentRetryCount			= 0;
	m_iVolumeLock_MaxRetryCount				= 5;
	m_iVolumeLock_WaitTime					= 1000;
	m_iVolumeLock_CurrentRetryCount			= 0;
	m_iReadDataCompare_MaxRetryCount		= 5;
	m_iReadDataCompare_WaitTime				= 5000;
	m_iReadDataCompare_CurrentRetryCount	= 0;
	m_iTestUnitReadyCount					= 0;
	m_uDebugOption							= 0;
	m_bMainLogOpenFlag						= FALSE;
	m_bTestDataSaveFlag						= FALSE;
	m_iReadDataSave_MaxRetryCount			= 3;
	m_iReadDataSave_WaitTime				= 1000;
	m_iReadDataSave_CurrentRetryCount		= 0;
	m_bIniFileExist							= FALSE;
	m_SelectMonitor							= 1;
	m_uProfileCheckOption					= 0;	// 3.1.2.0
	CdbErrorSave							= 0;	// 3.1.2.0

	//------------------------------------------------
	// メンバ変数のログ出力
	//------------------------------------------------
	LOG_OUT("m_TDODDHandle                          = 0x%08x\n", m_TDODDHandle );
	LOG_OUT("m_bRetryFlag                           = %s\n", BOOL_TO_A(m_bRetryFlag) );
	LOG_OUT("m_bTrayOpenFlag_TestEnd                = %s\n", BOOL_TO_A(m_bTrayOpenFlag_TestEnd) );
	LOG_OUT("m_TestType                             = %s\n", ((m_TestType==0)?"TEST_CD":"TEST_DVD") );
	LOG_OUT("m_CurrentStep                          = %s\n", GetTestStepString(m_CurrentStep) );
	LOG_OUT("m_bVolumeLock                          = %s\n", BOOL_TO_A(m_bVolumeLock) );
	LOG_OUT("m_bTrayLock                            = %s\n", BOOL_TO_A(m_bTrayLock) );
	LOG_OUT("m_ulSectorOffset                       = %ld\n", m_ulSectorOffset );
	LOG_OUT("m_ulSectorNum                          = %ld\n", m_ulSectorNum );
	LOG_OUT("m_iMaxRetryCount                       = %ld\n", m_iMaxRetryCount );
	LOG_OUT("m_iCurrentRetryCount                   = %ld\n", m_iCurrentRetryCount );
	LOG_OUT("m_iDeviceOpen_MaxRetryCount            = %ld\n", m_iDeviceOpen_MaxRetryCount );
	LOG_OUT("m_iDeviceOpen_WaitTime                 = %ld\n", m_iDeviceOpen_WaitTime );
	LOG_OUT("m_iDeviceOpen_CurrentRetryCount        = %ld\n", m_iDeviceOpen_CurrentRetryCount );
	LOG_OUT("m_iVolumeLock_MaxRetryCount            = %ld\n", m_iVolumeLock_MaxRetryCount );
	LOG_OUT("m_iVolumeLock_WaitTime                 = %ld\n", m_iVolumeLock_WaitTime );
	LOG_OUT("m_iVolumeLock_CurrentRetryCount        = %ld\n", m_iVolumeLock_CurrentRetryCount );
	LOG_OUT("m_iReadDataCompare_MaxRetryCount       = %ld\n", m_iReadDataCompare_MaxRetryCount );
	LOG_OUT("m_iReadDataCompare_WaitTime            = %ld\n", m_iReadDataCompare_WaitTime );
	LOG_OUT("m_iReadDataCompare_CurrentRetryCount   = %ld\n", m_iReadDataCompare_CurrentRetryCount );
	LOG_OUT("m_iTestUnitReadyCount                  = %ld\n", m_iTestUnitReadyCount );
	LOG_OUT("m_uDebugOption                         = %lu\n", m_uDebugOption );
	LOG_OUT("m_bMainLogOpenFlag                     = %s\n", BOOL_TO_A(m_bMainLogOpenFlag) );
	LOG_OUT("m_bTestDataSaveFlag                    = %s\n", BOOL_TO_A(m_bTestDataSaveFlag) );
	LOG_OUT("m_iReadDataSave_MaxRetryCount          = %ld\n", m_iReadDataSave_MaxRetryCount );
	LOG_OUT("m_iReadDataSave_WaitTime               = %ld\n", m_iReadDataSave_WaitTime );
	LOG_OUT("m_iReadDataSave_CurrentRetryCount      = %ld\n", m_iReadDataSave_CurrentRetryCount );
	LOG_OUT("m_bIniFileExist                        = %s\n", BOOL_TO_A(m_bIniFileExist) );
	LOG_OUT("m_SelectMonitor                        = %ld\n", m_SelectMonitor );

	LOG_OUT("(E)\n");
}

void CCDtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCDtestDlg)
		// メモ: この場所には ClassWizard によって DDX と DDV の呼び出しが追加されます。
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCDtestDlg, CDialog)
	//{{AFX_MSG_MAP(CCDtestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDtestDlg メッセージ ハンドラ

BOOL CCDtestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニュー項目をシステム メニューへ追加します。

	// IDM_ABOUTBOX はコマンド メニューの範囲でなければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンを設定
	
	// TODO: 特別な初期化を行う時はこの場所に追加してください。
	LOG_OUT("(S)\n");

	BOOL		bFuncResult = FALSE;
	BOOL		bResult;
	CString		strMessage;

	//-----------------------------------------
	// パラメータ解析
	//-----------------------------------------
	bResult = AnalizeAppParam();
	if ( bResult == FALSE )
	{
		LOG_OUT("Failure, Analysis of application parameter.\n");

		// 終了コードの設定
		theApp.m_iReturnValue = ERROR_PARAMETER;

		goto END_OnInitDialog;
	}

	//-----------------------------------------
	// INIファイル値の取得
	//-----------------------------------------
	bResult = GetIniFileInfo();
	if ( bResult == FALSE )
	{
		LOG_OUT("Failure, Acquisition of INI file information.\n");

		// 終了コードの設定
		theApp.m_iReturnValue = ERROR_INIFILE;

		goto END_OnInitDialog;
	}

	//-----------------------------------------
	// メイン・ログの初期化
	//-----------------------------------------
	bResult = InitMainLog();
	if ( bResult == FALSE )
	{
		LOG_OUT("Failure, Create Log ( TEXT, XML ) File.\n");

		// 終了コードの設定
		theApp.m_iReturnValue = ERROR_LOGFILE;

		goto END_OnInitDialog;
	}

	//-----------------------------------------
	// メイン・ログへセクション情報の出力
	//-----------------------------------------
	strMessage = "Initialization of application";
	CLog.WriteLogSection( strMessage );
	//AddMessage(m_strMessage);
	LOG_OUT("[%s]\n", strMessage );

	//-----------------------------------------
	// 現在のテスト・タイプの取得
	//-----------------------------------------
	bResult = GetTestType();
	if ( bResult == FALSE )
	{
		LOG_OUT("Failure, Acquisition of test type.\n");

		goto END_OnInitDialog;
	}

	//-----------------------------------------
	// TDODDLib.Dllロード
	//-----------------------------------------
	if (LoadTDODDLib() == FALSE)
	{
		// TDODDLib.dllロードエラー
		strMessage.Format("Faiilure, Load of TDODDLib.dll.");

		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
		CLog.WriteFail(); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );

		goto END_OnInitDialog;
	}

	//-----------------------------------------
	// 最小化表示
	//-----------------------------------------
	ShowWindow(SW_MINIMIZE);

	//-----------------------------------------
	// タイマーの設定
	//-----------------------------------------
	m_timerID = SetTimer( TIMER_ID, TEST_LOOP_TIME, NULL );

	//-----------------------------------------
	// この関数が正常終了したことを示す
	//-----------------------------------------
	bFuncResult = TRUE;

END_OnInitDialog:

	if ( bFuncResult == FALSE )
	{
		if ( theApp.m_strErrorCode[4] == "" ) 
		{
			theApp.m_strErrorCode[4] = ErrCode_Test_AppInit;
		}

		EndDialog( IDOK );
	}
	else
	{
		strMessage = "Success, Initialization of application.";
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
		LOG_OUT("%s\n", strMessage );

		//-----------------------------------------
		// モニター情報の取得
		//-----------------------------------------
		Display_Setting();
	}

	LOG_OUT("(E)(TRUE)\n");

	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}

void CCDtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.SetTestType( m_TestType );
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

void CCDtestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// システムは、ユーザーが最小化ウィンドウをドラッグしている間、
// カーソルを表示するためにここを呼び出します。
HCURSOR CCDtestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCDtestDlg::OnOK() 
{
	LOG_OUT("(S)\n");

	// TODO: この位置にその他の検証用のコードを追加してください

	CDialog::OnOK();

	LOG_OUT("(E)\n");
}

void CCDtestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	BOOL		bResult;
	DWORD		dwResult = ERROR_SUCCESS;
	DWORD		dwStatus;
	int			iResult;
	CDBERRCODE	CdbError = 0;
	CString		strMessage;
	CString		strMessage2;

	if ( nIDEvent == TIMER_ID )
	{
		KillTimer(TIMER_ID);

		LOG_OUT("***** << m_CurrentStep == %s >> *********************************************************************\n", GetTestStepString( m_CurrentStep ) );

		switch (m_CurrentStep)
		{
		case Step_Drive_Detect:
			{
				LOG_OUT("m_iDeviceOpen_CurrentRetryCount = %ld\n", m_iDeviceOpen_CurrentRetryCount);

				// メイン・ログにセクションの書き込み
				if ( m_iDeviceOpen_CurrentRetryCount == 0 )
				{
					CLog.WriteLog("\r\n");

					strMessage = "Drive and Media Detect";
					CLog.WriteLogSection( strMessage );
					//m_strMessage.Format("[Drive and Media Detect]");
					//AddMessage(m_strMessage);
					LOG_OUT("[%s]\n", strMessage );

					if ( m_iCurrentRetryCount > 0 )
					{
						strMessage.Format("Retrying of the whole : %d", m_iCurrentRetryCount );
						CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					}
				}

				// ドライブ検索&オープン
				bResult = DriveDetect(&dwStatus);
				if ( m_uDebugOption == 1 ) { bResult = FALSE; }

				if (bResult == FALSE)
				{ // 失敗：ドライブ検索＆オープン

					if ( m_iDeviceOpen_CurrentRetryCount < m_iDeviceOpen_MaxRetryCount ) 
					{ // 自動リトライ

						// リトライカウントのインクリメント
						m_iDeviceOpen_CurrentRetryCount++;
						LOG_OUT("m_iDeviceOpen_CurrentRetryCount = %ld\n", m_iDeviceOpen_CurrentRetryCount);

						// スリープ
						LOG_OUT("Sleep(%ld)\n", m_iDeviceOpen_WaitTime);
						Sleep(m_iDeviceOpen_WaitTime);
					}
					else
					{ // リトライ数オーバー

						strMessage.Format("Failure, Device not found or open error.");
						CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
						//AddMessage(strMessage);
						LOG_OUT("%s\n", strMessage);

						if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_DriveDetect;

						m_CurrentStep = Step_NG;				
					}
				}
				else
				{ // 正常
					LOG_OUT("DriveDetect() successed.\n");

// 3.1.2.0->
#if 0
					// TEST UNIT READY準備
					m_iTestUnitReadyCount = 0;
					LOG_OUT("m_iTestUnitReadyCount = %ld\n", m_iTestUnitReadyCount );

					m_CurrentStep = Step_Drive_Ready;
#else
// 3.1.2.1->
					bool bCheckProfile = false;
					if (m_uProfileCheckOption == 2) {
						// TDConfの値により、Profileチェックを行うかどうか判定する
						strMessage.Format("Check TDConf value.");
						CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
						LOG_OUT("%s\n", strMessage);
						bCheckProfile = CheckTDConf();
						if (bCheckProfile == false) {
							strMessage.Format("Skip profile check.");
							CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
							LOG_OUT("%s\n", strMessage);
						}
					}
					else {
						if (m_uProfileCheckOption == 1)
							// 無条件にProfileチェックを行う
							bCheckProfile = true;
					}
					
//					if (m_uProfileCheckOption != 0)
					if (bCheckProfile)
// <-3.1.2.1
					{
						strMessage.Format("Check C/DVD Profile & Capabilities and Mechanical.");
						CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
						LOG_OUT("%s\n", strMessage);

						// ドライブで扱えるメディアをチェック(FW書き換えでSuper Multi->ROM化ドライブの確認)
						bResult = ProfilesCheck();
						if ( bResult != TRUE )
						{ // 失敗
							strMessage.Format("Failure, C/DVD Profile & Capabilities and Mechanical Check.");
							CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
							LOG_OUT("%s\n", strMessage);

							if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_MediaInformation;

							m_CurrentStep = Step_NG;
						}
						else
						{ // 正常
							strMessage.Format("Success, C/DVD Profile & Capabilities and Mechanical Check.");
							CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
							LOG_OUT("%s\n", strMessage);

							// TEST UNIT READY準備
							m_iTestUnitReadyCount = 0;
							LOG_OUT("m_iTestUnitReadyCount = %ld\n", m_iTestUnitReadyCount );

							m_CurrentStep = Step_Drive_Ready;
						}
					}
					else
					{
						// TEST UNIT READY準備
						m_iTestUnitReadyCount = 0;
						LOG_OUT("m_iTestUnitReadyCount = %ld\n", m_iTestUnitReadyCount );

						m_CurrentStep = Step_Drive_Ready;
					}
#endif
// <-3.1.2.0
				}
			}
			break;

		case Step_Drive_Ready:
			{
				LOG_OUT("m_iTestUnitReadyCount = %ld\n", m_iTestUnitReadyCount);
				if ( m_iTestUnitReadyCount < TEST_UNIT_READY_MAX_RETRY_COUNT )
				{
					// メディア装填確認
					dwResult = TEST_UNIT_READY(m_TDODDHandle, &CdbError);
					if (dwResult != ERROR_SUCCESS)
					{
						LOG_OUT("TEST_UNIT_READY() failed. : 0x%08x\n", dwResult);
						LOG_OUT("CdbStatus = 0x%08x\n", CdbError);

						if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_DriveReady;

						m_CurrentStep = Step_NG;
					}
					else
					{ // 成功

						LOG_OUT("TEST_UNIT_READY() successed. : CdbStatus = 0x%08x\n", CdbError);

						if ( GetSenseKeyFormCdbErrCode(CdbError) == 0x02 && GetSenseASCFormCdbErrCode(CdbError) == 0x3A )
						{ // メディア未挿入→トレイをあける

							// ODDドライブのオープン
							TRAY_OPEN(m_TDODDHandle, &CdbError);
							LOG_OUT("TRAY_OPEN()\n");

							// メディア挿入メッセージの表示
							if ( m_bIniFileExist == TRUE )
							{
								iResult = IniRead_MessageDlg_Section("Media Insert Message");
							}
							else
							{
								strMessage.Format("Please set TEST %s", TEST_MEDIA_NAME(m_TestType) );
								strMessage2.Format("%s", TEST_NAME(m_TestType) );

								iResult = MessageBox( strMessage, strMessage2, MB_OK | MB_ICONINFORMATION);
								LOG_OUT("message = \"%s\"\n", strMessage );
							}
						}
						else if ( IsCdbSuccess(CdbError) )
						{ // 正常

							// メンバー変数にボリュームラベルを格納
							m_strVolumeLabel = GetVolumeLabel( m_szOddDrive );
							LOG_OUT("m_strVolumeLabel = %s\n", m_strVolumeLabel );

							// Driveの準備が完了
							strMessage.Format("Success, Drive ready.");
							CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
							LOG_OUT("%s\n", strMessage );

							// Step_Tray_Lockへ進む
							m_CurrentStep = Step_Tray_Lock;
							LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
						}
						else
						{ // メディア未挿入以外のエラー(処理中など)
// 3.1.2.0->
#if 0
							strMessage.Format("Failure, Drive is not ready(CdbStatus = 0x%08x).", CdbError);
							CLog.WriteLogOnly( strMessage ); CLog.WriteLogOnly("\r\n");
							//AddMessage(strMessage);
							LOG_OUT("%s\n", strMessage);
#endif
// <-3.1.2.0
							// CDBエラーコード保存
							CdbErrorSave = CdbError;	// 3.1.2.0
							// スリープ
							LOG_OUT("Sleep(100)\n");
							Sleep(100);

							m_iTestUnitReadyCount++;
						}
					}
				}
				else
				{ // 準備ができないのでメッセージ出力

//					strMessage.Format("Device problem or operation error(Status = 0x%08x, CdbStatus = 0x%08x).", dwResult, CdbError);	// 3.1.2.0
					strMessage.Format("Device problem or operation error(Status = 0x%08x, CdbStatus = 0x%08x).", dwResult, CdbErrorSave);
					CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					//AddMessage(strMessage);
					LOG_OUT("%s\n", strMessage);

					if ( m_bIniFileExist == TRUE )
					{
						iResult = IniRead_MessageDlg_Section("Media detection failure Message");
					}
					else
					{
						iResult = MessageBox(strMessage, "Media detection failure", MB_OK | MB_ICONERROR);
						LOG_OUT("message = \"%s\"\n", strMessage );
					}


					if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_DriveReady;

					m_CurrentStep = Step_NG;			
				}
			}
			break;

		case Step_Tray_Lock:
			{ 
				// トレイロック
				bResult = TrayLock();

				if ( bResult == TRUE )
				{ // 正常

					m_bTrayLock = TRUE;
					LOG_OUT("m_bTrayLock = %s\n", BOOL_TO_A( m_bTrayLock ) );

					// VolumeLockの準備
					m_iVolumeLock_CurrentRetryCount = 0;

					m_CurrentStep = Step_Volume_Lock;
					LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
				}
				else
				{ // 失敗
					strMessage.Format("Failure, Tray Lock.");
					CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					//AddMessage(strMessage);
					LOG_OUT("%s\n", strMessage);

					if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_TrayLock;

					m_CurrentStep = Step_NG;
				}
			}
			break;

		case Step_Volume_Lock:
			{
				LOG_OUT("m_iVolumeLock_CurrentRetryCount = %ld\n", m_iVolumeLock_CurrentRetryCount );

				// ボリュームのロック
				dwResult = m_lpfnTDODDLockVolume(m_TDODDHandle);
				if ( m_uDebugOption == 2 ) dwResult = ERROR_FILE_NOT_FOUND;

				if (dwResult != ERROR_SUCCESS) 
				{
					LOG_OUT("TDODDLockVolume() failed. : 0x%08x\n", dwResult);

					if ( m_iVolumeLock_CurrentRetryCount < m_iVolumeLock_MaxRetryCount ) 
					{ // 自動リトライ

						// リトライカウントのインクリメント
						m_iVolumeLock_CurrentRetryCount++;
						LOG_OUT("m_iVolumeLock_CurrentRetryCount = %ld\n", m_iVolumeLock_CurrentRetryCount);

						// スリープ
						LOG_OUT("Sleep(%ld)\n", m_iVolumeLock_WaitTime);
						Sleep(m_iVolumeLock_WaitTime);
					}
					else
					{ // リトライ数オーバー

						strMessage.Format("Failure, Volume lock(Status = 0x%08x).", dwResult);
						CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
						//AddMessage(strMessage);
						LOG_OUT("%s\n", strMessage);

						if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_Volume_Lock;

						m_CurrentStep = Step_NG;
					}
				}
				else
				{ // 正常
					LOG_OUT("TDODDLockVolume() successed.\n");

					m_bVolumeLock = TRUE;
					LOG_OUT("m_bVolumeLock = %s\n", BOOL_TO_A(m_bVolumeLock) );

					m_CurrentStep = Step_Media_Information;
					LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
				}
			}
			break;

		case Step_Media_Information:
			{
				// メディアチェック
				bResult = MediaCheck();
				if ( bResult == TRUE )
				{ // 正常

					if ( m_bTestDataSaveFlag == FALSE )
					{
						// ReadDataCompare準備
						m_iReadDataCompare_CurrentRetryCount = 0;
						LOG_OUT("m_iReadDataCompare_CurrentRetryCount = %ld\n", m_iReadDataCompare_CurrentRetryCount );

						m_CurrentStep = Step_ReadDataCompare;
						LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
					}
					else
					{
						// ReadDataSave準備
						m_iReadDataSave_CurrentRetryCount = 0;
						LOG_OUT("m_iReadDataSave_CurrentRetryCount = %ld\n", m_iReadDataSave_CurrentRetryCount );

						m_CurrentStep = Step_ReadDataSave;
						LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
					}
				}
				else
				{ // 失敗

					strMessage.Format("Failure, Media Check.");
					CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					//AddMessage(strMessage);
					LOG_OUT("%s\n", strMessage);

					if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_MediaInformation;

					m_CurrentStep = Step_NG;
				}
			}
			break;

		case Step_ReadDataCompare:
			{
				LOG_OUT("m_iReadDataCompare_CurrentRetryCount = %ld\n", m_iReadDataCompare_CurrentRetryCount);

				// メイン・ログにセクションの書き込み
				if ( m_iReadDataCompare_CurrentRetryCount == 0 )
				{
					CLog.WriteLog("\r\n");

					strMessage = "Data Read & Compare";
					CLog.WriteLogSection( strMessage );
					LOG_OUT("[%s]\n", strMessage );

					if ( m_iCurrentRetryCount > 0 )
					{
						strMessage.Format("Retrying of the whole : %d", m_iCurrentRetryCount );
						CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					}
				}

				bResult = ReadDataCompare();
				LOG_OUT("ReadDataCompare() : %s\n", BOOL_TO_A(bResult) );

				if ( bResult == FALSE )
				{
					if ( m_iReadDataCompare_CurrentRetryCount < m_iReadDataCompare_MaxRetryCount )
					{ // 自動リトライ

						// リトライカウントのインクリメント
						m_iReadDataCompare_CurrentRetryCount++;

						// スリープ
						LOG_OUT("Sleep(%ld)\n", m_iReadDataCompare_WaitTime);
						Sleep(m_iReadDataCompare_WaitTime);
					}
					else
					{ // リトライ・オーバー

						if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_DataCompare;

						m_CurrentStep = Step_NG;
						LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
					}
				}
				else
				{ // 正常
					m_CurrentStep = Step_OK;
					LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
				}
			}
			break;

		case Step_ReadDataSave:
			{
				LOG_OUT("m_iReadDataSave_CurrentRetryCount = %ld\n", m_iReadDataSave_CurrentRetryCount);

				// メイン・ログにセクションの書き込み
				if ( m_iReadDataSave_CurrentRetryCount == 0 )
				{
					CLog.WriteLog("\r\n");

					strMessage = "Data Read & Save";
					CLog.WriteLogSection( strMessage );
					LOG_OUT("[%s]\n", strMessage );

					if ( m_iCurrentRetryCount > 0 )
					{
						strMessage.Format("Retrying of the whole : %d", m_iCurrentRetryCount );
						CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					}
				}

				// 比較（正解値）ファイル作成
				bResult = ReadDataSave();
				LOG_OUT("ReadDataSave() : %s\n", BOOL_TO_A(bResult) );

				if ( bResult == FALSE )
				{
					if ( m_iReadDataSave_CurrentRetryCount < m_iReadDataSave_MaxRetryCount )
					{ // 自動リトライ

						// リトライカウントのインクリメント
						m_iReadDataSave_CurrentRetryCount++;

						// スリープ
						LOG_OUT("Sleep(%ld)\n", m_iReadDataSave_WaitTime);
						Sleep(m_iReadDataSave_WaitTime);
					}
					else
					{ // リトライ・オーバー

						if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_DataSave;

						m_CurrentStep = Step_NG;
						LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
					}
				}
				else
				{ // 正常
					m_CurrentStep = Step_OK;
					LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
				}
			}
			break;

		case Step_OK:
			{
				theApp.m_bProgramStatus = TRUE;

				CLog.WritePass(); CLog.WriteLog("\r\n");

				Epilogue();
				OnOK();
			}
			break;

		case Step_NG:
			{
				Epilogue();

				if ( m_bRetryFlag == TRUE )
				{ // リトライオプションが指定されている場合

					if ( m_iCurrentRetryCount < m_iMaxRetryCount )
					{
						int flg;

						if ( m_bIniFileExist == TRUE )
						{
							flg = IniRead_MessageDlg_Section("Test Retry Message");
						}
						else
						{
							strMessage.Format("%s Retry ?", TEST_NAME(m_TestType) );
							strMessage2.Format("%s", TEST_NAME(m_TestType) );

							flg = MessageBox( strMessage, strMessage2, MB_RETRYCANCEL | MB_ICONWARNING);
							LOG_OUT("message = \"%s\"\n", strMessage );
						}

						if ( flg == IDRETRY )
						{
							m_CurrentStep = Step_Retry;
							LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
						}
						else
						{
							//AddMessage("FAIL");
							CLog.WriteFail(); CLog.WriteLog("\r\n");
							LOG_OUT("FAIL\n");
							OnOK();
						}
					}
					else
					{
						//AddMessage("FAIL");
						CLog.WriteFail(); CLog.WriteLog("\r\n");
						LOG_OUT("FAIL\n");
						OnOK();
					}
				}
				else
				{ // リトライオプションが指定されていない場合
					//AddMessage("FAIL");
					CLog.WriteFail(); CLog.WriteLog("\r\n");
					LOG_OUT("FAIL\n");
					OnOK();
				}
			}
			break;

		case Step_Retry:
			{
				m_iDeviceOpen_CurrentRetryCount = 0;
				m_iVolumeLock_CurrentRetryCount = 0;
				m_iReadDataCompare_CurrentRetryCount = 0;
				m_iCurrentRetryCount++;

				m_CurrentStep = Step_Drive_Detect;
				LOG_OUT("m_CurrentStep = %s\n", GetTestStepString( m_CurrentStep ) );
			}
			break;

		default:
			LOG_OUT("Step is Unknown.\n");
			break;
		}

		m_timerID=SetTimer( TIMER_ID, TEST_LOOP_TIME, NULL );
	}

	CDialog::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////////////
// CDiagCDRom::GetDriveMediaType
// 概要：ドライブタイプを返す
// pDrive：ドライブ名 [IN]
// 戻り値：CD-ROM/DVD   DRIVE_CDROM
//         その他       EX_DRIVE_REMOVABLE ,
//                      EX_DRIVE_FLOPPY    ,
//                      DRIVE_FIXED        ,
//                      DRIVE_REMOTE       ,
//                      DRIVE_RAMDISK      ,
//                      EX_DRIVE_INVALID
////////////////////////////////////////////////////////////////////////
int CCDtestDlg::GetDriveMediaType(const char * pDrive)
{
	unsigned int uiDriveType = GetDriveType(pDrive);	// ドライブタイプを取得

	switch(uiDriveType)
	{
	case DRIVE_REMOVABLE:

		if((*pDrive == 'A') || (*pDrive == 'a') || (*pDrive == 'B') || (*pDrive == 'b'))
		{
			return (EX_DRIVE_FLOPPY);
		}
		else
		{
			return (EX_DRIVE_REMOVABLE);
		}
		break;

	case DRIVE_FIXED:
		return (EX_DRIVE_FIXED);

	case DRIVE_REMOTE:
		return (EX_DRIVE_REMOTE);

	case DRIVE_CDROM:	// CD-ROM と判明
		return (EX_DRIVE_CDROM);

	case DRIVE_RAMDISK:	// ラムディスクと判明
		return (EX_DRIVE_RAMDISK);
	}

	return (EX_DRIVE_INVALID);	// Drive is invalid if we get here.
}

////////////////////////////////////////////////////////////////////////
// CDiagCDRom::GetModuleFileDirectory
// 概要：実行モジュールのディレクトリを取得する
// pszPath：取得バッファ   [OUT]
// nSize  ：バッファサイズ [IN]
// 戻り値 ：0 成功、-1 失敗
////////////////////////////////////////////////////////////////////////
int CCDtestDlg::GetModuleFileDirectory(char * pszPath, int nSize)
{
	if(GetModuleFileName(NULL, pszPath, nSize))
	{
		char * p = strrchr(pszPath, '\\');
		if(p)
		{
			*p = '\0';
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
// PCB Tester に WAITING を　送信
////////////////////////////////////////////////////////////////////////
void CCDtestDlg::SendWaitPCBTester(void)
{
// PS1 修正要求対応07/28/00
	char *Strgp;
	
	// PS1からのリクエスト。ComPortへの出力 07/27/00
	Strgp = ((CCDtestApp*)AfxGetApp())->m_lpCmdLine;
	if(Strgp != _T('\0')) 
	{
		if (strcmp(WTCOMSTRSTR, Strgp) == 0)
		{
			// Windows NT OSであるかどうかの判定
			if(IsNT())
			{
				//	AfxMessageBox("Wtcomstr Message Out for Tester WinNT",MB_OK|MB_ICONINFORMATION);
				SendWin2K(WTCOMSTRMSG);
			}
			else
			{
				//  AfxMessageBox("Wtcomstr Message Out for Tester Win98",MB_OK|MB_ICONINFORMATION);
				SendWin98(WTCOMSTRMSG);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
// 小文字を大文字に変換
////////////////////////////////////////////////////////////////////////
void CCDtestDlg::StrUpper(char * pszString)
{
	char * cp;	// 作業用キャラクターポインタ

	for(cp = pszString; *cp; cp++) {	// 文字列の終わりまで全てを見る
		if(islower(*cp))	// 小文字なら
			*cp = _toupper(*cp);	// 大文字に変換
	}
	return;
}

////////////////////////////////////////////////////////////////////////
// メディアの確認
//
// 戻り値：
//         
////////////////////////////////////////////////////////////////////////
long CheckMediaReady(char* szDrives)
{
  BOOL    nMediaCheck;
  char  szVolumeLabel[256];
	
  nMediaCheck = GetVolumeInformation(szDrives, szVolumeLabel, sizeof(szVolumeLabel), NULL, NULL, NULL, NULL, NULL );
  return (GetLastError());
}

////////////////////////////////////////////////////////////////////////
// buf data to file save
////////////////////////////////////////////////////////////////////////
long  save( const char* fname,void* buf, size_t size )
{
	FILE *	fp;
	bool	success;

	if ( NULL == (fp = fopen(fname, "wb")) )
	{
		return  ERROR_WRITE_FAULT;
	}

	success = size == fwrite(buf, 1, size, fp);

	fclose(fp);
	
	return  success ? NO_ERROR : ERROR_WRITE_FAULT;
}

////////////////////////////////////////////////////////////////////////
// file read to buf
////////////////////////////////////////////////////////////////////////
long  lord( const char* fname,void* buf, size_t size )
{
	FILE *	fp;
	bool	success;
	if ( NULL == (fp = fopen(fname, "rb")) ) {
		return  ERROR_READ_FAULT;
	}
	success = size == fread(buf, 1, size, fp);
	fclose(fp);
	return  success ? NO_ERROR : ERROR_READ_FAULT;
} //lord


////////////////////////////////////////////////////////////////////////
// 概要：Ejectcd3.exe を起動する
////////////////////////////////////////////////////////////////////////
void EjectCD(void)
{
	LOG_OUT("(S)\n");

	// Windows NT OSであるかどうかの判定
//	if( 0 )
	if( IsNT() )
	{
		EjectCDWin2K();
	}
	else 
	{
		EjectCDWin98();
	}

	LOG_OUT("(E)\n");
}

////////////////////////////////////////////////////////////////////////
// 概要：Ejectcd3.exe を起動する
// 戻り値：成功    0
//         失敗   -1
////////////////////////////////////////////////////////////////////////
int EjectCDWin98(void)
{
	LOG_OUT("(S)\n");

	int ret = 0;
	char szCommandLine[1024] = {0};
	char szCommandArg[2048] = {0};
	char szCurrentDir[1024] = {0};
	STARTUPINFO strucStartupInfo = {0};
	PROCESS_INFORMATION strucProcessInfo = {0};

	CRITICAL_SECTION csLockUnion;
	InitializeCriticalSection(&csLockUnion);

	// strucStartupInfo
	GetStartupInfo(&strucStartupInfo);
	strucStartupInfo.cb												= sizeof(strucStartupInfo);
	strucStartupInfo.lpReserved										= NULL;
	strucStartupInfo.lpDesktop										= NULL;
	strucStartupInfo.lpTitle										= NULL;
	strucStartupInfo.dwX = strucStartupInfo.dwY						= 0;
	strucStartupInfo.dwXSize = strucStartupInfo.dwYSize				= 0;
	strucStartupInfo.dwXCountChars =strucStartupInfo.dwYCountChars	= 0;
	strucStartupInfo.dwFillAttribute								= 0;
	strucStartupInfo.dwFlags										= STARTF_USESHOWWINDOW;
	strucStartupInfo.wShowWindow									= SW_HIDE;
	strucStartupInfo.cbReserved2									= 0;
	strucStartupInfo.lpReserved2									= NULL;
	strucStartupInfo.hStdInput = strucStartupInfo.hStdOutput		= NULL;
	strucStartupInfo.hStdError										= NULL;

	// szCurrentDir
	DWORD dwRet = GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
	if(!dwRet)
	{
		LOG_OUT("(E)(-1)\n");
		return -1;
	}

	// szCommandLine
	//strcpy(szCommandLine, szCurrentDir);
	//strcat(szCommandLine, "\\");
	//strcat(szCommandLine, "Ejectcd3.exe");

	strcpy(szCommandLine,"Ejectcd3.exe");

	// szCommandArg
	// wsprintf(szCommandArg, "\"%s\" \"%s\"", szCommandLine, szMsg);

	BOOL blRet = CreateProcess( (LPCTSTR)szCommandLine,
								(LPTSTR)szCommandArg,
								(LPSECURITY_ATTRIBUTES)NULL,
								(LPSECURITY_ATTRIBUTES)NULL,
								(BOOL)FALSE,
								(DWORD)(NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE),
								(LPTSTR)NULL,
								(LPTSTR)szCurrentDir,
								(LPSTARTUPINFO)&strucStartupInfo,
								(LPPROCESS_INFORMATION)&strucProcessInfo
								);

	if(blRet != TRUE)
	{ // エラー
		ret = -1;
	
		LOG_OUT("CreateProcess() failed.\n");
	}
	else
	{
		LOG_OUT("CreateProcess() successed.\n");

		DWORD dwReturnCode = WaitForSingleObject(strucProcessInfo.hProcess,INFINITE);
		if ( dwReturnCode == WAIT_FAILED )
		{ // エラー
			LOG_OUT("WaitForSingleObject() failed.\n");
		}
		else if ( dwReturnCode == WAIT_TIMEOUT )
		{ // タイムアウト
			LOG_OUT("WaitForSingleObject() timeout.\n");
		}
		else if ( dwReturnCode == WAIT_OBJECT_0 )
		{ // 指定したオブジェクトがシグナル状態になったことを意味します
			LOG_OUT("WaitForSingleObject() successed.\n");
		}
	}

    if ( !(CloseHandle(strucProcessInfo.hThread)) )
    { // エラー
		LOG_OUT("CloseHandle() failed.\n");
    }

    if ( !(CloseHandle(strucProcessInfo.hProcess)) )
    { // エラー
		LOG_OUT("CloseHandle() failed.\n");
    }

	LOG_OUT("(E)(%ld)\n", ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////
BOOL EjectCDWin2K(void)
{
	LOG_OUT("(S)\n");

	BOOL				bFuncResult = FALSE;
	BOOL				bResult;
	DWORD				dwResult;
	char				szCommandLine[1024] = {0};
	char				szCommandArg[2048]	= {0};
	char				szCurrentDir[1024]	= {0};
	STARTUPINFO			strucStartupInfo	= {0};
	PROCESS_INFORMATION strucProcessInfo	= {0};

	CRITICAL_SECTION csLockUnion;
	InitializeCriticalSection(&csLockUnion);

	// strucStartupInfo
	GetStartupInfo(&strucStartupInfo);
	strucStartupInfo.cb												= sizeof(strucStartupInfo);
	strucStartupInfo.lpReserved										= NULL;
	strucStartupInfo.lpDesktop										= NULL;
	strucStartupInfo.lpTitle										= NULL;
	strucStartupInfo.dwX = strucStartupInfo.dwY                     = 0;
	strucStartupInfo.dwXSize = strucStartupInfo.dwYSize             = 0;
	strucStartupInfo.dwXCountChars =strucStartupInfo.dwYCountChars  = 0;
	strucStartupInfo.dwFillAttribute								= 0;
	strucStartupInfo.dwFlags										= STARTF_USESHOWWINDOW;
	strucStartupInfo.wShowWindow									= SW_HIDE;
	strucStartupInfo.cbReserved2									= 0;
	strucStartupInfo.lpReserved2									= NULL;
	strucStartupInfo.hStdInput = strucStartupInfo.hStdOutput		= NULL;
	strucStartupInfo.hStdError										= NULL;

	// szCommandLine
	dwResult = GetSystemDirectory(szCommandLine, sizeof(szCommandLine));
	if( !dwResult )
	{
		LOG_OUT("GetSystemDirectory() failed.\n");
		goto END_EjectCDWin2K;
	}
	if (szCommandLine[strlen(szCommandLine)-1] != '\\')
	{
		strcat(szCommandLine,"\\");
	}
	strcat(szCommandLine,"cmd.exe");

	// szCurrentDir
	dwResult = GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
	if( !dwResult )
	{
		LOG_OUT("GetCurrentDirectory() failed.\n");
		goto END_EjectCDWin2K;
	}


	strcpy(szCommandArg," /C c:\\TOOLS\\Ejectcd3.exe");

	LOG_OUT("szCommandLine = %s\n", szCommandLine);
	LOG_OUT("szCommandArg  = %s\n", szCommandArg);
	LOG_OUT("szCurrentDir  = %s\n", szCurrentDir);

	bResult = CreateProcess(	(LPCTSTR)szCommandLine,
								(LPSTR)szCommandArg,
								(LPSECURITY_ATTRIBUTES)NULL,
								(LPSECURITY_ATTRIBUTES)NULL,
								(BOOL)FALSE,
								(DWORD)(CREATE_NEW_CONSOLE),//(DWORD)(NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE),
								(LPTSTR)NULL,
								(LPTSTR)szCurrentDir,
								(LPSTARTUPINFO)&strucStartupInfo,
								(LPPROCESS_INFORMATION)&strucProcessInfo 
								);

	if( bResult == FALSE )
	{
		LOG_OUT("CreateProcess() failed. : ErrorCode = 0x%08x\n", GetLastError() );
		goto END_EjectCDWin2K;
	}
	else
	{
		LOG_OUT("CreateProcess() success.\n");

		dwResult = WaitForSingleObject(strucProcessInfo.hProcess, INFINITE);
		if ( dwResult == WAIT_FAILED )
		{ // エラー
			LOG_OUT("WaitForSingleObject() failed.\n");
			goto END_EjectCDWin2K;
		}
		else if ( dwResult == WAIT_TIMEOUT )
		{ // タイムアウト
			LOG_OUT("WaitForSingleObject() timeout.\n");
		}
		else if ( dwResult == WAIT_OBJECT_0 )
		{
			DWORD dwExitCode;
			bResult = GetExitCodeProcess( strucProcessInfo.hProcess, &dwExitCode );
			if ( bResult == TRUE )
			{
				LOG_OUT("GetExitCodeProcess() success. : ExitCode = %ld\n", dwExitCode );
			}
			else
			{
				LOG_OUT("GetExitCodeProcess() failed. : LastErrorCode = 0x%08x\n", GetLastError());
			}
		}
	}

	if ( !(CloseHandle(strucProcessInfo.hThread)) )
	{ // エラー
		LOG_OUT("CloseHandle() failed.\n");
		goto END_EjectCDWin2K;
	}

	if ( !(CloseHandle(strucProcessInfo.hProcess)) )
	{ // エラー
		LOG_OUT("CloseHandle() failed.\n");
		goto END_EjectCDWin2K;
	}

	bFuncResult = TRUE;

END_EjectCDWin2K:

	LOG_OUT("(E)(%s)\n", BOOL_TO_A( bFuncResult ) );

	return bFuncResult;
}




////////////////////////////////////////////////////////////////////////
// CWTCOMSTR::SendWin2K
// 概要：コマンドプロンプト cmd.exe を [/C] オプションで起動し、
//       そのパラメータとして WTCOMSTR.exe のフルパスを渡しています。
//       引数szMsgは、WTCOMSTR.exeのコマンドラインパラメータとなります。
// szMsg：送信メッセージ [IN]
// 戻り値：成功    0
//         失敗   -1
////////////////////////////////////////////////////////////////////////
int SendWin2K(char * szMsg)
{
	int ret = 0;
	char szCommandLine[1024] = {0};
	char szCommandArg[2048] = {0};
	char szCurrentDir[1024] = {0};
	STARTUPINFO strucStartupInfo = {0};
	PROCESS_INFORMATION strucProcessInfo = {0};

	CRITICAL_SECTION csLockUnion;
	InitializeCriticalSection(&csLockUnion);

	// strucStartupInfo
	GetStartupInfo(&strucStartupInfo);
	strucStartupInfo.cb                     = sizeof(strucStartupInfo);
	strucStartupInfo.lpReserved             = NULL;
	strucStartupInfo.lpDesktop              = NULL;
	strucStartupInfo.lpTitle                = NULL;
	strucStartupInfo.dwX = strucStartupInfo.dwY                     = 0;
	strucStartupInfo.dwXSize = strucStartupInfo.dwYSize             = 0;
	strucStartupInfo.dwXCountChars =strucStartupInfo.dwYCountChars  = 0;
	strucStartupInfo.dwFillAttribute        = 0;
	strucStartupInfo.dwFlags                = STARTF_USESHOWWINDOW;
	strucStartupInfo.wShowWindow            = SW_HIDE;
	strucStartupInfo.cbReserved2            = 0;
	strucStartupInfo.lpReserved2            = NULL;
	strucStartupInfo.hStdInput = strucStartupInfo.hStdOutput    = NULL;
	strucStartupInfo.hStdError              = NULL;

	// szCommandLine
	DWORD dwRet = GetSystemDirectory(szCommandLine, sizeof(szCommandLine));
	if(!dwRet)
	{
		return -1;
	}
	if (szCommandLine[strlen(szCommandLine)-1] != '\\')
	{
		strcat(szCommandLine,"\\");
	}
	strcat(szCommandLine,"cmd.exe");

	// szCurrentDir
	dwRet = GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
	if(!dwRet)
	{
		return -1;
	}

	// szCommandArg // WTCOMSTREXE は WTCOMSTR.exe のフルパスです。
	//   wsprintf(szCommandArg, " /C \"\"%s\\%s\" \"%s\"\"", szCurrentDir, WTCOMSTREXE, szMsg);

	wsprintf(szCommandArg, " /C \"\"%s\\%s\" \"%s\"\"", szCurrentDir, "WTCOMSTR.EXE", szMsg);


	BOOL blRet = CreateProcess( (LPCTSTR)szCommandLine,
                               (LPSTR)szCommandArg,
                               (LPSECURITY_ATTRIBUTES)NULL,
                               (LPSECURITY_ATTRIBUTES)NULL,
                               (BOOL)FALSE,
 
	(DWORD)(NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE),
                               (LPTSTR)NULL,
                               (LPTSTR)szCurrentDir,
                               (LPSTARTUPINFO)&strucStartupInfo,
                               (LPPROCESS_INFORMATION)&strucProcessInfo 
                             );

   if(blRet != TRUE)
   {
      ret = -1;
      // エラー
   }
   else
   {
      DWORD dwReturnCode = WaitForSingleObject(strucProcessInfo.hProcess, INFINITE);
      if ( dwReturnCode == WAIT_FAILED )
      {
         // エラー
      }
      if ( dwReturnCode == WAIT_TIMEOUT )
      {
         // タイムアウト
      }
   }

   if ( !(CloseHandle(strucProcessInfo.hThread)) )
   {
      // エラー
   }

   if ( !(CloseHandle(strucProcessInfo.hProcess)) )
   {
      // エラー
   }

   return ret;
}

////////////////////////////////////////////////////////////////////////
// CWTCOMSTR::SendWin98
// 概要：WTCOMSTR.exe を起動し、コマンドラインパラメータとして
//       szMsgを渡します。
// szMsg：送信メッセージ [IN]
// 戻り値：成功    0
//         失敗   -1
////////////////////////////////////////////////////////////////////////
int SendWin98(char * szMsg)
{
   int ret = 0;
   char szCommandLine[1024] = {0};
   char szCommandArg[2048] = {0};
   char szCurrentDir[1024] = {0};
   STARTUPINFO strucStartupInfo = {0};
   PROCESS_INFORMATION strucProcessInfo = {0};

   CRITICAL_SECTION csLockUnion;
   InitializeCriticalSection(&csLockUnion);

   // strucStartupInfo
   GetStartupInfo(&strucStartupInfo);
   strucStartupInfo.cb                     = sizeof(strucStartupInfo);
   strucStartupInfo.lpReserved             = NULL;
   strucStartupInfo.lpDesktop              = NULL;
   strucStartupInfo.lpTitle                = NULL;
   strucStartupInfo.dwX = strucStartupInfo.dwY                     = 0;
   strucStartupInfo.dwXSize = strucStartupInfo.dwYSize             = 0;
   strucStartupInfo.dwXCountChars =strucStartupInfo.dwYCountChars  = 0;
   strucStartupInfo.dwFillAttribute        = 0;
   strucStartupInfo.dwFlags                = STARTF_USESHOWWINDOW;
   strucStartupInfo.wShowWindow            = SW_HIDE;
   strucStartupInfo.cbReserved2            = 0;
   strucStartupInfo.lpReserved2            = NULL;
   strucStartupInfo.hStdInput = strucStartupInfo.hStdOutput     = NULL;
   strucStartupInfo.hStdError              = NULL;

   // szCurrentDir
   DWORD dwRet = GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
   if(!dwRet){
      return -1;
   }

   // szCommandLine
   strcpy(szCommandLine, szCurrentDir);
   strcat(szCommandLine, "\\");
//zzz
//   strcat(szCommandLine, WTCOMSTREXE); // WTCOMSTREXE は WTCOMSTR.exe
//                                       // のフルパスです。

   strcat(szCommandLine, "WTCOMSTR.EXE"); // WTCOMSTREXE は WTCOMSTR.exe
                                       // のフルパスです。


   // szCommandArg
   wsprintf(szCommandArg, "\"%s\" \"%s\"", szCommandLine, szMsg);

   BOOL blRet = CreateProcess( (LPCTSTR)szCommandLine,
								(LPTSTR)szCommandArg,
								(LPSECURITY_ATTRIBUTES)NULL,
								(LPSECURITY_ATTRIBUTES)NULL,
								(BOOL)FALSE,
								(DWORD)(NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE),
								(LPTSTR)NULL,
								(LPTSTR)szCurrentDir,
								(LPSTARTUPINFO)&strucStartupInfo,
								(LPPROCESS_INFORMATION)&strucProcessInfo
                             );

   if(blRet != TRUE)
   {
      ret = -1;
      // エラー
   }
   else
   {
		DWORD dwReturnCode = WaitForSingleObject(strucProcessInfo.hProcess, INFINITE);
		if ( dwReturnCode == WAIT_FAILED )
		{
			 // エラー
		}
		if ( dwReturnCode == WAIT_TIMEOUT )
		{
			 // タイムアウト
		}
   }

	if ( !(CloseHandle(strucProcessInfo.hThread)) )
	{
		// エラー
	}

	if ( !(CloseHandle(strucProcessInfo.hProcess)) )
	{
		// エラー
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
//NT OSであるかどうか
////////////////////////////////////////////////////////////////////////
BOOL IsNT()
{
    OSVERSIONINFO          osvi;
    
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    
    GetVersionEx(&osvi);

    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
// 「Exit」ボタンが押下された時の処理        						
////////////////////////////////////////////////////////////////////////
void CCDtestDlg::OnBnClickedOk()
{
	LOG_OUT("(S)\n");

	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	CString strMessage;

	strMessage = "The test was canceled.";
	CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
	CLog.WriteFail(); CLog.WriteLog("\r\n");

	LOG_OUT("%s\n", strMessage );

	if ( theApp.m_strErrorCode[4] == "" ) theApp.m_strErrorCode[4] = ErrCode_Test_Cancel;

	Epilogue();
	OnOK();

	LOG_OUT("(E)\n");
}

////////////////////////////////////////////////////////////////////////
// テストデータ読み取り＆比較処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::ReadDataCompare(VOID)
{
	LOG_OUT("(S)\n");

	BOOL		bFuncResult = FALSE;									// このメソッドの合否
	DWORD		dwResult = ERROR_SUCCESS;								// 関数実行の合否
	CDBERRCODE	CdbError = 0;											// CDBエラー情報
	LPVOID		lpMediaData = NULL;										// メディアデータ
	void		*LdBuf = NULL;											// 外部ファイルデータ
	long		lResult = 0;											// 関数実行の合否
	CString		strMessage;												// メッセージ出力用バッファ

	LOG_OUT("m_strCompareTargetFile = %s\n", m_strCompareTargetFile );
	LOG_OUT("m_ulSectorOffset       = %lu\n", m_ulSectorOffset );
	LOG_OUT("m_ulSectorNum          = %lu\n", m_ulSectorNum );

	//-------------------------------------------------------------
	// 比較（正解値）ファイルの存在チェック
	//-------------------------------------------------------------
	if ( PathFileExists( m_strCompareTargetFile ) == FALSE )
	{
		strMessage.Format("Failure, Not Found CompareTargetFile : %s", m_strCompareTargetFile );
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );

		goto END_ReadDataCompare;
	}

	//-------------------------------------------------------------
	// テストに使用するデータのサイズを計算する
	//-------------------------------------------------------------
	DWORD  dwTestDataSize = m_ulSectorNum * 2048;
	LOG_OUT("dwTestDataSize = %ld\n", dwTestDataSize );

	//-------------------------------------------------------------
	// メディアから読み込むデータ用のメモリ領域を確保する
	//-------------------------------------------------------------
	lpMediaData = VirtualAlloc( NULL, dwTestDataSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE );
	if ( lpMediaData == NULL )
	{
		strMessage.Format("Failure, Memory allocation(ErrorCode = 0x%08x)", GetLastError() );
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );

		goto END_ReadDataCompare;
	}

	//-------------------------------------------------------------
	// メディアからデータの読み込み
	//-------------------------------------------------------------
	dwResult = READ10( m_TDODDHandle, m_ulSectorOffset, (PBYTE)lpMediaData, (WORD)m_ulSectorNum, &CdbError );
	if ( dwResult != ERROR_SUCCESS || GetSenseCode(CdbError) != 0 )
	{
		if (GetSenseCode(CdbError) != 0)
		{
			strMessage.Format("Failure, Reading Sector(CdbStatus = 0x%08x).", CdbError);
			CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage );
		}
		else
		{
			strMessage.Format("Failure, Reading Sector(Status = 0x%08x).", dwResult);
			CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage );
		}

		goto END_ReadDataCompare;
	}
	else
	{
		strMessage.Format("Success, Reading Sector( SectorOffset=%lu,  SectorNum=%lu ).", m_ulSectorOffset, m_ulSectorNum);
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
		LOG_OUT("%s\n", strMessage );
	}

	//-------------------------------------------------------------
	// 外部ファイルから読み込むデータ用のメモリ領域を確保する
	//-------------------------------------------------------------
	LdBuf = malloc( dwTestDataSize );
	if ( LdBuf == NULL )
	{
		strMessage.Format("Failure, Memory allocation(ErrorCode = 0x%08x).", GetLastError() );
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );

		goto END_ReadDataCompare;
	}
	
	//-------------------------------------------------------------
	// 外部ファイルからデータの読み込み
	//-------------------------------------------------------------
	lResult = lord( m_strCompareTargetFile, LdBuf, dwTestDataSize );
	if ( NO_ERROR != lResult )
	{
		strMessage.Format("Failure, Reading Data File.");
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );
		goto END_ReadDataCompare;
	}

	//-------------------------------------------------------------
	// メディアデータと外部ファイルデータとの比較
	//-------------------------------------------------------------
	if( memcmp( lpMediaData, LdBuf, dwTestDataSize ) == 0 )
	{
		strMessage.Format("Success, Data Compare Check.");
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );
	}
	else
	{
		strMessage.Format("Failure, Data Compare Check.");
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );
		goto END_ReadDataCompare;
	}

	bFuncResult = TRUE;

END_ReadDataCompare:

	//-------------------------------------------------------------
	// 外部ファイルからのデータ読み込みに使用したメモリ領域を解放する
	//-------------------------------------------------------------
	if ( LdBuf != NULL )
	{
		free( LdBuf );
		LdBuf = NULL;
	}

	//-------------------------------------------------------------
	// メディアからのデータ読み込みに使用したメモリ領域を解放する
	//-------------------------------------------------------------
	if ( lpMediaData != NULL )
	{
		VirtualFree( lpMediaData, 0, MEM_RELEASE );
		lpMediaData = NULL;
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

////////////////////////////////////////////////////////////////////////
// テストデータ読み取り＆比較（正解値）ファイル作成処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::ReadDataSave(VOID)
{
	LOG_OUT("(S)\n");

	BOOL		bFuncResult = FALSE;
	BOOL		bResult;
	LONG		lResult;
	LPVOID		lpMediaData = NULL;										// メディアデータ
	DWORD		dwResult = ERROR_SUCCESS;								// 関数実行の合否
	CDBERRCODE	CdbError = 0;											// CDBエラー情報
	CString		strMessage;												// メッセージ出力用バッファ

	LOG_OUT("m_strCompareTargetFile = %s\n", m_strCompareTargetFile );
	LOG_OUT("m_ulSectorOffset       = %lu\n", m_ulSectorOffset );
	LOG_OUT("m_ulSectorNum          = %lu\n", m_ulSectorNum );

	//-------------------------------------------------------------
	// 比較（正解値）ファイルの存在チェック
	//-------------------------------------------------------------
	if ( PathFileExists( m_strCompareTargetFile ) == TRUE )
	{
		// リネームファイル名の準備
		__time64_t t1;
		_time64( &t1 );
		struct tm t2;
		t2 = *_localtime64( &t1 );

		CString strBackupFileName;
		CString strFileName = m_strCompareTargetFile;
		strFileName.Replace(".dat","");
	
		strBackupFileName.Format("%s_%d%02d%02d-%02d%02d%02d.dat",strFileName, t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday, t2.tm_hour, t2.tm_min, t2.tm_sec );

		// 比較（正解値）ファイルのリネーム
		bResult = MoveFile( m_strCompareTargetFile, strBackupFileName );
		if ( bResult == FALSE )
		{
			strMessage.Format("Failure, Rename of CompareTargetFile : %s", m_strCompareTargetFile );
			CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

			LOG_OUT("%s\n", strMessage );

			goto END_ReadDataSave;
		}
	}

	//-------------------------------------------------------------
	// テストに使用するデータのサイズを計算する
	//-------------------------------------------------------------
	DWORD  dwTestDataSize = m_ulSectorNum * 2048;
	LOG_OUT("dwTestDataSize = %ld\n", dwTestDataSize );

	//-------------------------------------------------------------
	// メディアから読み込むデータ用のメモリ領域を確保する
	//-------------------------------------------------------------
	lpMediaData = VirtualAlloc( NULL, dwTestDataSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE );
	if ( lpMediaData == NULL )
	{
		strMessage.Format("Failure, Memory allocation(ErrorCode = 0x%08x)", GetLastError() );
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );

		goto END_ReadDataSave;
	}

	//-------------------------------------------------------------
	// メディアからデータの読み込み
	//-------------------------------------------------------------
	dwResult = READ10( m_TDODDHandle, m_ulSectorOffset, (PBYTE)lpMediaData, (WORD)m_ulSectorNum, &CdbError );
	if ( dwResult != ERROR_SUCCESS || GetSenseCode(CdbError) != 0 )
	{
		if (GetSenseCode(CdbError) != 0)
		{
			strMessage.Format("Failure, Reading Sector(CdbStatus = 0x%08x).", CdbError);
			CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage );
		}
		else
		{
			strMessage.Format("Failure, Reading Sector(Status = 0x%08x).", dwResult);
			CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage );
		}

		goto END_ReadDataSave;
	}
	else
	{
		strMessage.Format("Success, Reading Sector.");
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
		LOG_OUT("%s\n", strMessage );
	}

	//-------------------------------------------------------------
	// メディアからデータの読み込み
	//-------------------------------------------------------------
	lResult = save( m_strCompareTargetFile, lpMediaData, dwTestDataSize );
	if ( lResult == NO_ERROR )
	{
		strMessage.Format("Success, Create CompareTargetFile : %s", m_strCompareTargetFile);
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

		LOG_OUT("%s\n", strMessage );
	}
	else
	{
		strMessage.Format("Failure, Create CompareTargetFile : %s", m_strCompareTargetFile);
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
		LOG_OUT("%s\n", strMessage );

		goto END_ReadDataSave;
	}

	bFuncResult = TRUE;

END_ReadDataSave:

	//-------------------------------------------------------------
	// メディアからのデータ読み込みに使用したメモリ領域を解放する
	//-------------------------------------------------------------
	if ( lpMediaData != NULL )
	{
		VirtualFree( lpMediaData, 0, MEM_RELEASE );
		lpMediaData = NULL;
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

////////////////////////////////////////////////////////////////////////
// ODDドライブ検索処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::DriveDetect( PDWORD pdwStatus )
{
	LOG_OUT("(S)\n");

	BOOL	bFuncResult = FALSE;
	CString strMessage;

	// システムで利用可能なドライブを表す文字列をバッファに格納します。
	char	strDrive[TDODD_MAX_STRING];
	ZeroMemory( strDrive, sizeof(strDrive) );
	int iLen = GetLogicalDriveStrings( sizeof(strDrive), strDrive );

	// CD-ROM、またはDVD-ROMドライブを検索します。
	for ( int i = 0 ; i < iLen ; i += 4 )
	{
		if ( GetDriveType(&strDrive[i]) == DRIVE_CDROM )
		{
			// デバイスオープン
			if ( m_TDODDHandle == (HTDODDDRIVE)INVALID_HANDLE_VALUE )
			{
				m_TDODDHandle = m_lpfnTDODDOpenDrive( strDrive[i], pdwStatus );
				LOG_OUT("m_TDODDHandle = 0x%08x\n", m_TDODDHandle );
			}

			if (m_TDODDHandle == (HTDODDDRIVE)INVALID_HANDLE_VALUE)
			{
				strMessage.Format("Failure, %s drive can not open(Status = 0x%08x).", TEST_DRIVE_NAME(m_TestType), *pdwStatus);
				CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
				//AddMessage(strMessage);
				LOG_OUT("%s\n", strMessage );
				goto END_DriveDetect;
			}

			strMessage.Format("Success, ODD device detection(%s).", &strDrive[i]);
			CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
			//AddMessage(m_strMessage);
			LOG_OUT("%s\n", strMessage );

			// メンバー変数にドライブ文字列を格納
			strcpy( m_szOddDrive, &strDrive[i] );
			LOG_OUT("m_szOddDrive = %s\n", m_szOddDrive );

			bFuncResult = TRUE;
			goto END_DriveDetect;
		}
	}
	
	// すべてのドライブを調べたが見つからない
	for (int i = 0; i < iLen; i++)
	{
		if (strDrive[i] == '\0')
		{
			strDrive[i] = ' ';
		}
	}
	strDrive[iLen] = '\0';

	strMessage.Format("Failure, %s drive can not be found(%s).", TEST_DRIVE_NAME(m_TestType), strDrive);
	CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");
	//AddMessage(strMessage);
	LOG_OUT("%s", strMessage );

	*pdwStatus = TDODDSTATUS_FILE_NOT_FOUND;

END_DriveDetect:

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

////////////////////////////////////////////////////////////////////////
// アプリケーション終了処理
////////////////////////////////////////////////////////////////////////
void CCDtestDlg::Epilogue()
{
	LOG_OUT("(S)\n");

	DWORD	dwResult;
	//DWORD	dwStatus;
	CDBERRCODE CdbError;

	if (m_bVolumeLock)
	{
		// ボリュームのアンロック
		dwResult = m_lpfnTDODDUnlockVolume( m_TDODDHandle );
		LOG_OUT("TDODDUnlockVolume() : result = 0x%08x\n", dwResult );

		m_bVolumeLock = FALSE;
		LOG_OUT("m_bVolumeLock = %s\n", BOOL_TO_A( m_bVolumeLock ) );
	}

	if (m_bTrayLock)
	{
		// ドライブのアンロック
		dwResult = m_lpfnTDODDUnlockDrive( m_TDODDHandle );
		LOG_OUT("TDODDUnlockDrive() : result = 0x%08x\n", dwResult );

		m_bTrayLock = FALSE;
		LOG_OUT("m_bTrayLock = %s\n", BOOL_TO_A( m_bTrayLock ) );
	}

	// メディア取り出し
	m_iDeviceOpen_CurrentRetryCount = 0;
	LOG_OUT("m_iDeviceOpen_MaxRetryCount     = %ld\n", m_iDeviceOpen_MaxRetryCount );
	LOG_OUT("m_iDeviceOpen_CurrentRetryCount = %ld\n", m_iDeviceOpen_CurrentRetryCount );

	while ( (m_bTrayOpenFlag_TestEnd == TRUE) )
	{
		dwResult = TRAY_OPEN( m_TDODDHandle, &CdbError );

//add  auto open DVD retry times 5 
		int i = 0;
		while(true)
		{
			if (dwResult==0 || i >5 )
			{
				break;
			}
			Sleep(1000);
			dwResult = TRAY_OPEN( m_TDODDHandle, &CdbError );
			i++;
		}
 //add end 

		LOG_OUT("TRAY_OPEN() : result = 0x%08x\n", dwResult );

		if ( dwResult != ERROR_SUCCESS || GetSenseCode(CdbError) != 0 )
		{
			if ( m_iDeviceOpen_CurrentRetryCount < m_iDeviceOpen_MaxRetryCount )
			{ // 自動リトライ
				m_iDeviceOpen_CurrentRetryCount++;
				LOG_OUT("m_iDeviceOpen_CurrentRetryCount = %ld\n", m_iDeviceOpen_CurrentRetryCount );

				LOG_OUT("Sleep(%ld)\n", m_iDeviceOpen_WaitTime );
				Sleep( m_iDeviceOpen_WaitTime );
			}
			else
			{
				// あきらめる
				LOG_OUT("Retry Over : m_iDeviceOpen_CurrentRetryCount = %ld\n", m_iDeviceOpen_CurrentRetryCount );
				break;
			}
		}
		else
		{
			LOG_OUT("TRAY_OPEN() success.\n");
			break;
		}
	}

	// ドライブクローズ
	if ( m_TDODDHandle != (HTDODDDRIVE)INVALID_HANDLE_VALUE )
	{
		dwResult = m_lpfnTDODDCloseDrive( m_TDODDHandle );
		LOG_OUT("TDODDCloseDrive() : result = 0x%08x\n", dwResult );

		m_TDODDHandle = (HTDODDDRIVE)INVALID_HANDLE_VALUE;
	}

	LOG_OUT("(E)\n");
}

////////////////////////////////////////////////////////////////////////
// ODDドライブのトレイロック処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::TrayLock(VOID)
{
	LOG_OUT("(S)\n");

	BOOL			bFuncResult = FALSE;
	DWORD			dwResult = ERROR_SUCCESS;
	CDBERRCODE		CdbError = 0;
	CString			strMessage;

	// ドライブのロック
	dwResult = m_lpfnTDODDLockDrive( m_TDODDHandle );
	LOG_OUT("TDODDLockDrive() : result = 0x%08x\n", dwResult );

	if (dwResult != ERROR_SUCCESS)
	{
		strMessage.Format("Failure, Drive lock(Status = 0x%08x).", dwResult);
		CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
		//AddMessage(strMessage);
		LOG_OUT("%s\n", strMessage );
		goto END_TrayLock;
	}

	bFuncResult = TRUE;

END_TrayLock:

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

////////////////////////////////////////////////////////////////////////
// テストメディアのチェック処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::MediaCheck(VOID)
{
	LOG_OUT("(S)\n");

	BOOL bFuncResult = FALSE;
	USHORT usCurrentProfile;
	DWORD dwResult = ERROR_SUCCESS;
	CDBERRCODE CdbError = 0;
	CString strMessage;
	CString strVolumeLabel;

	//----------------------------------------------
	// メディアの種類を取得
	//----------------------------------------------
	dwResult = GET_CONFIGURATION(m_TDODDHandle, m_ucBuffer, 0x08, &CdbError);
	LOG_OUT("GET_CONFIGURATION() : result = 0x%08x : CdbError = 0x%08x\n", dwResult, CdbError );

	if (dwResult != ERROR_SUCCESS || GetSenseCode(CdbError) != 0)
	{
		if (GetSenseCode(CdbError) != 0)
		{
			strMessage.Format("Failure, Can not get configuration(CdbStatus = 0x%08x).", CdbError);
			LOG_OUT("%s\n", strMessage);
		}
		else
		{
			strMessage.Format("Failure, Can not get configuration(Status = 0x%08x).", dwResult);
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			//AddMessage(strMessage);
			LOG_OUT("%s\n", strMessage);
			goto END_MediaCheck;
		}
	}
	else 
	{
		// メディア種類のチェック
		usCurrentProfile = (m_ucBuffer[6] << 8) + m_ucBuffer[7];
		LOG_OUT("usCurrentProfile = 0x%04x\n", usCurrentProfile);
		LOG_OUT("Kind of media : %s\n", this->GetProfileStringA(usCurrentProfile) );

		if ( m_TestType == TEST_CD )
		{
			if (usCurrentProfile == 0x0008	||	// CD-ROM 
				usCurrentProfile == 0x0009	||	// CD-R
				usCurrentProfile == 0x000A	)	// CD-RW
			{ // CDメディアだった場合
				strMessage.Format("Success, MediaType Check : This is a CD media. : %s(0x%04x).", this->GetProfileStringA(usCurrentProfile), usCurrentProfile);
				CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
				//AddMessage(strMessage);
				LOG_OUT("%s\n", strMessage);
			}
			else
			{ // CDメディアでなかった場合
				strMessage.Format("Failure, MediaType Check : This is not a CD media. : %s(0x%04x).", this->GetProfileStringA(usCurrentProfile), usCurrentProfile);
				CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
				//AddMessage(strMessage);
				LOG_OUT("%s\n", strMessage);
				goto END_MediaCheck;
			}
		}
		else if ( m_TestType == TEST_DVD )
		{
			if (usCurrentProfile == 0x0010	||	// DVD-ROM
				usCurrentProfile == 0x0011	||	// DVD-R Sequential recording
				usCurrentProfile == 0x0012	||	// DVD-RAM
				usCurrentProfile == 0x0013	||	// DVD-RW Restricted Overwrite
				usCurrentProfile == 0x0014	||	// DVD-RW Sequential recording
				usCurrentProfile == 0x0015	||	// DVD-R  Dual Layer Sequential recording
				usCurrentProfile == 0x0016	||	// DVD-R  Dual Layer Jump recording
				usCurrentProfile == 0x0017	||	// DVD-RW Dual Layer
				usCurrentProfile == 0x0018	||	// DVD-Download disc recordingDual Layer
				usCurrentProfile == 0x001A	||	// DVD+RW
				usCurrentProfile == 0x001B	)	// DVD+R
			{ // DVDメディアだった場合
				strMessage.Format("Success, MediaType Check : This is a DVD media. : %s(0x%04x).", this->GetProfileStringA(usCurrentProfile), usCurrentProfile);
				CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
				//AddMessage(strMessage);
				LOG_OUT("%s\n", strMessage);
			}
			else
			{ // DVDメディアでなかった場合
				strMessage.Format("Failure, MediaType Check : This is not a DVD media. : %s(0x%04x).", this->GetProfileStringA(usCurrentProfile), usCurrentProfile);
				CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
				//AddMessage(strMessage);
				LOG_OUT("%s\n", strMessage);
				goto END_MediaCheck;
			}
		}
		else
		{ // このパターンはない
		}
	}

	//----------------------------------------------
	// メディアがテストメディアかどうかチェックする
	//----------------------------------------------
	if ( m_TestType == TEST_CD )
	{
		if ( m_strVolumeLabel.Find( TEST_MEDIA_VOLUME_LABEL_OF_CD ) == -1 )
		{ // テストメディアではない
			strMessage.Format("Failure, VolumeLabel Check : This is not a Test media. : VolumeLabel = %s", m_strVolumeLabel );
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			//AddMessage(strMessage);
			LOG_OUT("%s\n", strMessage);
			goto END_MediaCheck;
		}
		else
		{ // テストメディア
			strMessage.Format("Success, VolumeLabel Check : This is a Test media. : VolumeLabel = %s", m_strVolumeLabel );
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			//AddMessage(strMessage);
			LOG_OUT("%s\n", strMessage);
		}
	}
	else if ( m_TestType == TEST_DVD )
	{
		if ( m_strVolumeLabel.Find( TEST_MEDIA_VOLUME_LABEL_OF_DVD ) == -1 )
		{ // テストメディアではない
			strMessage.Format("Failure, VolumeLabel Check : This is not a Test media. : VolumeLabel = %s", m_strVolumeLabel );
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			//AddMessage(strMessage);
			LOG_OUT("%s\n", strMessage);
			goto END_MediaCheck;
		}
		else
		{ // テストメディア
			strMessage.Format("Success, VolumeLabel Check : This is a Test media. : VolumeLabel = %s", m_strVolumeLabel );
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			//AddMessage(strMessage);
			LOG_OUT("%s\n", strMessage);
		}
	}
	else
	{ // このパターンはない
	}

	bFuncResult = TRUE;

END_MediaCheck:

	if (bFuncResult)
	{
		//AddMessage("Success, Media Check.");
		LOG_OUT("Success, Media Check.\n");
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

// 3.1.2.0->
////////////////////////////////////////////////////////////////////////
// ドライブで扱えるメディアのチェック処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::ProfilesCheck(VOID)
{
	LOG_OUT("(S)\n");

	BOOL bFuncResult = TRUE;
	DWORD dwResult = ERROR_SUCCESS;
	CDBERRCODE CdbError = 0;
	CString strMessage;

	DWORD dwRMedia_ms = 0;		// Read media for mode sense
	DWORD dwWMedia_ms = 0;		// Write media for mode sense
	DWORD dwRMedia_gc = 0;		// Read media for get configuration
	DWORD dwWMedia_gc = 0;		// Write media for get configuration

	// CD Testは何もしない
	if ( m_TestType == TEST_DVD )
	{
		//----------------------------------------------
		// C/DVD Capabilities and Mechanical Statusを取得
		//----------------------------------------------
		dwResult = MODE_SENSE_2A(m_TDODDHandle, m_ucBuffer, sizeof(CDVDCAPABILITIESANDMECHANICALSTATUSRESULTDATA), &CdbError);
		LOG_OUT("MODE_SENSE_2A() : result = 0x%08x : CdbError = 0x%08x\n", dwResult, CdbError );

		if (dwResult != ERROR_SUCCESS || GetSenseCode(CdbError) != 0) {
			if (GetSenseCode(CdbError) != 0) {
				strMessage.Format("Failure, Can not mode sense(2A)(CdbStatus = 0x%08x).", CdbError);
				LOG_OUT("%s\n", strMessage);
			}
			else {
				strMessage.Format("Failure, Can not mode sense(2A)(Status = 0x%08x).", dwResult);
				CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
				LOG_OUT("%s\n", strMessage);
			}
			bFuncResult = FALSE;
		}
		else {
			PCDVDCAPABILITIESANDMECHANICALSTATUSRESULTDATA pStatus = (PCDVDCAPABILITIESANDMECHANICALSTATUSRESULTDATA)&m_ucBuffer[0];
			PCDVDCAPABILITIESANDMECHANICALSTATUS pMechanicalStatus = &pStatus->Status;

			// 値設定
			if (pMechanicalStatus->CDR_Read)
				dwRMedia_ms |= MEDIA_CDR;
			if (pMechanicalStatus->CDRW_Read)
				dwRMedia_ms |= MEDIA_CDRW;
			if (pMechanicalStatus->DVDROM_Read)
				dwRMedia_ms |= MEDIA_DVDROM;
			if (pMechanicalStatus->DVDR_Read )
				dwRMedia_ms |= MEDIA_DVDR;
			if (pMechanicalStatus->DVDRAM_Read)
				dwRMedia_ms |= MEDIA_DVDRAM;
			if (pMechanicalStatus->CDR_Write)
				dwWMedia_ms |= MEDIA_CDR;
			if (pMechanicalStatus->CDRW_Write)
				dwWMedia_ms |= MEDIA_CDRW;
			if (pMechanicalStatus->DVDR_Write)
				dwWMedia_ms |= MEDIA_DVDR;
			if (pMechanicalStatus->DVDRAM_Write)
				dwWMedia_ms |= MEDIA_DVDRAM;

			PBYTE p = (PBYTE)&pStatus->Status;
			strMessage.Format("Mode sense raw data = %02x %02x %02x %02x", *p, *(p + 1), *(p + 2), *(p + 3));
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage);

			strMessage.Format("C/DVD Capabilities and Mechanical Status. Read = %08x, Write = %08x", dwRMedia_ms, dwWMedia_ms);
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage);
		}

		if (bFuncResult) {
			//----------------------------------------------
			// Profileを取得
			//----------------------------------------------
			dwResult = GET_CONFIGURATION(m_TDODDHandle, m_ucBuffer, 256, &CdbError);
			LOG_OUT("GET_CONFIGURATION() : result = 0x%08x : CdbError = 0x%08x\n", dwResult, CdbError );

			if (dwResult != ERROR_SUCCESS || GetSenseCode(CdbError) != 0) {
				if (GetSenseCode(CdbError) != 0) {
					strMessage.Format("Failure, Can not get configuration(CdbStatus = 0x%08x).", CdbError);
					CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					LOG_OUT("%s\n", strMessage);
				}
				else {
					strMessage.Format("Failure, Can not get configuration(Status = 0x%08x).", dwResult);
					CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
					LOG_OUT("%s\n", strMessage);
				}
				bFuncResult = FALSE;
			}
			else {
				PPROFILELISTDATA pProfile = (PPROFILELISTDATA)&m_ucBuffer[0];
				DWORD dwMedia;

				pProfile->FeatureHeader.dwDataLength = TurnsDWORD(pProfile->FeatureHeader.dwDataLength);
				pProfile->FeatureHeader.wCurrentProfile	= TurnsWORD(pProfile->FeatureHeader.wCurrentProfile);
				pProfile->ProfileList.wFeatureCode = TurnsWORD(pProfile->ProfileList.wFeatureCode);
				int iMaxList = pProfile->ProfileList.byAdditionalLength / sizeof(PROFILEDESCRIPTOR);

				strMessage.Format("Get configuration profile list = ");
				CLog.WriteLog(strMessage);
				LOG_OUT("%s", strMessage);

				for (int i = 0; i < iMaxList; i++) {
					pProfile->ProfileList.ProfileDescriptors[i].wProfileNumber = TurnsWORD(pProfile->ProfileList.ProfileDescriptors[i].wProfileNumber);

					strMessage.Format("%04xh ", pProfile->ProfileList.ProfileDescriptors[i].wProfileNumber);
					CLog.WriteLog(strMessage);
					LOG_OUT("%s", strMessage);

					switch (pProfile->ProfileList.ProfileDescriptors[i].wProfileNumber) {
						case PROFILELIST_CDROM:
						case PROFILELIST_DVDROM:
//						case PROFILELIST_HD_DVDROM:
						case PROFILELIST_BDROM:
							dwRMedia_gc |= ProfileToMedia(pProfile->ProfileList.ProfileDescriptors[i].wProfileNumber);
							break;
						case PROFILELIST_CDR:
						case PROFILELIST_CDRW:
						case PROFILELIST_DVDR:
						case PROFILELIST_DVDRAM:
						case PROFILELIST_DVDRW_RESTRICTEDOVERWRITE:
						case PROFILELIST_DVDRW_SEQUENTIALRECORDING:
						case PROFILELIST_DVDxRW:
						case PROFILELIST_DVDxR:
						case PROFILELIST_DDCDROM:
						case PROFILELIST_DDCDR:
						case PROFILELIST_DDCDRW:
						case PROFILELIST_DVDxR_DL:
						case PROFILELIST_DVDR_DL_SEQUENTIALRECORDING:
						case PROFILELIST_DVDR_DL_JUMPRECORDING:
//						case PROFILELIST_HD_DVDR:
//						case PROFILELIST_HD_DVDRAM:
//						case PROFILELIST_HD_DVDR_DL:
						case PROFILELIST_BDR_SEQUENTIAL:
						case PROFILELIST_BDR_RANDOM:
						case PROFILELIST_BDRE:
							dwMedia = ProfileToMedia(pProfile->ProfileList.ProfileDescriptors[i].wProfileNumber);
							dwWMedia_gc |= dwMedia;
							dwRMedia_gc |= dwMedia;
							break;
					}
				}

				CLog.WriteLog("\r\n");
				LOG_OUT("\n");

				strMessage.Format("Profiles. Read = %08x, Write = %08x", dwRMedia_gc, dwWMedia_gc);
				CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
				LOG_OUT("%s\n", strMessage);
			}
		}
	}

	if (bFuncResult) {
		// Super Multi -> CD/DVD-ROMにしたための検査なので、Mode Senseのみで判定
		if (dwWMedia_ms != 0) {
			strMessage.Format("Failure, This drive is not CD/DVD-ROM(Write enable).");
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage);

			bFuncResult = FALSE;
		}
	}

	if (bFuncResult) {
		//AddMessage("Success, Profile Check.");
		LOG_OUT("Success, Profile Check.\n");
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}
// <-3.1.2.0

////////////////////////////////////////////////////////////////////////
// アプリケーション・パラメータの解析処理
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::AnalizeAppParam()
{
	LOG_OUT("(S)\n");

	for ( int i=0 ; i<__argc ; i++ )
	{
		LOG_OUT("Param%ld : %s\n", i, __argv[i] );

		if ( strcmpi(__argv[i], "/R") == 0 )
		{ // FAIL終了時にリトライ選択ダイアログを表示する
			m_bRetryFlag = TRUE;
		}

		if ( strcmpi(__argv[i], "/T") == 0 )
		{ // 読み取り＆比較処理・自動リトライ間隔
			if ( __argv[ i + 1 ] != NULL )
			{
				sscanf( __argv[++i], "%d", &m_iReadDataCompare_WaitTime );
			}
		}

		if ( strcmpi(__argv[i], "/N") == 0 )
		{ // 読み取り＆比較処理・自動リトライ回数
			if ( __argv[ i + 1 ] != NULL )
			{
				sscanf( __argv[++i], "%d", &m_iReadDataCompare_MaxRetryCount );
			}
		}

		if ( strcmpi(__argv[i], "/D") == 0 )
		{ // デバッグ時に使用する
			if ( __argv[ i + 1 ] != NULL )
			{
				sscanf( __argv[++i], "%u", &m_uDebugOption );
			}
		}

		if ( strcmpi(__argv[i], "/E") == 0 )
		{ // テスト終了時にトレイをEjectを行わないようにする
			m_bTrayOpenFlag_TestEnd = FALSE;
		}

		if ( strcmpi(__argv[i], "/I") == 0 )
		{ // INIファイルの場所を指定する
			if ( __argv[ i + 1 ] != NULL )
			{
				char szIniFilePath[_MAX_PATH] = {0};

				sprintf( szIniFilePath, "%s", __argv[++i] );
				theApp.m_strIniFilePath.Format("%s", szIniFilePath );
			}
		}

		if ( strcmpi(__argv[i], "/S") == 0 )
		{ // テストデータ作成モードのオプション
			m_bTestDataSaveFlag = TRUE;
		}
	}

	LOG_OUT("m_bRetryFlag                              = %s\n", BOOL_TO_A(m_bRetryFlag) );
	LOG_OUT("m_iReadDataCompare_WaitTime               = %d\n", m_iReadDataCompare_WaitTime );
	LOG_OUT("m_iReadDataCompare_MaxRetryCount          = %d\n", m_iReadDataCompare_MaxRetryCount );
	LOG_OUT("m_uDebugOption                            = %u\n", m_uDebugOption );
	LOG_OUT("m_bTrayOpenFlag_TestEnd                   = %s\n", BOOL_TO_A(m_bTrayOpenFlag_TestEnd) );
	LOG_OUT("theApp.m_strIniFilePath                   = %s\n", theApp.m_strIniFilePath );
	LOG_OUT("m_bTestDataSaveFlag                       = %s\n", BOOL_TO_A(m_bTestDataSaveFlag) );

	LOG_OUT("(E)\n");

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// 指定されたテストステップに対応した文字列の取得
////////////////////////////////////////////////////////////////////////
CString CCDtestDlg::GetTestStepString( TEST_STEP step )
{
	CString strFuncResult;

	switch(step)
	{
	case Step_Drive_Detect:
		strFuncResult = "Step_Drive_Detect";
		break;
	case Step_Drive_Ready:
		strFuncResult = "Step_Drive_Ready";
		break;
	case Step_Tray_Lock:
		strFuncResult = "Step_Tray_Lock";
		break;
	case Step_Volume_Lock:
		strFuncResult = "Step_Volume_Lock";
		break;
	case Step_Media_Information:
		strFuncResult = "Step_Media_Information";
		break;
	case Step_ReadDataCompare:
		strFuncResult = "Step_ReadDataCompare";
		break;
	case Step_ReadDataSave:
		strFuncResult = "Step_ReadDataSave";
		break;
	case Step_OK:
		strFuncResult = "Step_OK";
		break;
	case Step_NG:
		strFuncResult = "Step_NG";
		break;
	case Step_Retry:
		strFuncResult = "Step_Retry";
		break;
	default:
		strFuncResult = "Step_Unknown";
		break;
	}

	return strFuncResult;
}

////////////////////////////////////////////////////////////////////////
// 指定されたテストタイプに対応した文字列の取得
////////////////////////////////////////////////////////////////////////
CString CCDtestDlg::GetTestTypeString( TEST_TYPE type )
{
	CString strFuncResult;

	switch(type)
	{
	case TEST_CD:
		strFuncResult = "TEST_CD";
		break;
	case TEST_DVD:
		strFuncResult = "TEST_DVD";
		break;
	default:
		strFuncResult = "TEST_TYPE_Unknown";
	}

	return strFuncResult;
}


////////////////////////////////////////////////////////////////////////
// INIファイル情報の取得
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::GetIniFileInfo()
{
	LOG_OUT("(S)\n");

	BOOL	bFuncResult = FALSE;
	char	Buf[128] = {0};
	char	Buf2[_MAX_PATH] = {0};
	BOOL	bResult;
	CString strDefaultCompareTargetFile;

	//-------------------------------------------------
	// Iniファイルの存在チェック
	//-------------------------------------------------
	bResult = ::PathFileExists( theApp.m_strIniFilePath );
	if ( bResult == FALSE )
	{
		m_bIniFileExist = FALSE;
		LOG_OUT("m_bIniFileExist = %s\n", BOOL_TO_A(m_bIniFileExist) );

		CString strMessage; 
		strMessage.Format("Failure, IniFile( %s ) dosen't exist.", theApp.m_strIniFilePath );
		LOG_OUT("%s\n", strMessage);

		bFuncResult = TRUE; // INIファイルが存在しないので、INIファイル情報が取得出来ないのは正常系の動作とする
		goto END_GetIniFileInfo;
	}
	else
	{
		m_bIniFileExist = TRUE;
		LOG_OUT("m_bIniFileExist = %s\n", BOOL_TO_A(m_bIniFileExist) );

		LOG_OUT("IniFile( %s ) exists.\n", theApp.m_strIniFilePath );
	}

	//-------------------------------------------------
	// テストで使用するデータファイルのパスを取得
	//-------------------------------------------------
	char	szCompareTargetFile[_MAX_PATH];

	if ( stricmp( theApp.m_szModuleFileName_FileName, "cdtest" ) == 0 )
	{
		strDefaultCompareTargetFile = "CdTestMedia.dat";
	}
	else if ( stricmp( theApp.m_szModuleFileName_FileName, "dvdtest" ) == 0 )
	{
		strDefaultCompareTargetFile = "DvdTestMedia.dat";
	}

	GetPrivateProfileString( "Common", "CompareTargetFile", strDefaultCompareTargetFile, szCompareTargetFile, _MAX_PATH, theApp.m_strIniFilePath );
	m_strCompareTargetFile = szCompareTargetFile;
	LOG_OUT("m_strCompareTargetFile = %s\n", m_strCompareTargetFile);

	//-------------------------------------------------
	// テストで使用するデータの位置情報（先頭からのオフセット）を取得
	//-------------------------------------------------
	m_ulSectorOffset = GetPrivateProfileInt( "Common", "SectorOffset", 16, theApp.m_strIniFilePath );
	LOG_OUT("m_ulSectorOffset = %lu\n", m_ulSectorOffset);

	//-------------------------------------------------
	// テストで使用するデータのセクター数を取得
	//-------------------------------------------------
	m_ulSectorNum = GetPrivateProfileInt( "Common", "SectorNum", 5, theApp.m_strIniFilePath );
	LOG_OUT("m_ulSectorNum = %lu\n", m_ulSectorNum);

	//-------------------------------------------------
	// テストのデバック時に使用するエラー生成オプション
	//-------------------------------------------------
	m_uDebugOption = GetPrivateProfileInt( "Common", "DebugOption", 0, theApp.m_strIniFilePath );
	LOG_OUT("m_uDebugOption = %u\n", m_uDebugOption);

	//-------------------------------------------------
	// デバイスオープン処理のリトライカウント（上限）
	//-------------------------------------------------
	GetPrivateProfileString( "DEVICEOPEN", "RETRYCOUNT", "", Buf, sizeof( Buf ), theApp.m_strIniFilePath );
	if( strlen( Buf ) > 0 )
	{
		sscanf( Buf, "%d", &m_iDeviceOpen_MaxRetryCount );
		if( m_iDeviceOpen_MaxRetryCount <= 0)
		{
			m_iDeviceOpen_MaxRetryCount = 5;
		}
	}
	else
	{
		m_iDeviceOpen_MaxRetryCount = 5;
	}
	LOG_OUT("m_iDeviceOpen_MaxRetryCount = %d\n", m_iDeviceOpen_MaxRetryCount);

	//-------------------------------------------------
	// デバイスオープン処理のリトライ時・ウエイトタイム
	//-------------------------------------------------
	GetPrivateProfileString( "DEVICEOPEN", "INTERVAL", "", Buf, sizeof( Buf ), theApp.m_strIniFilePath );
	if( strlen( Buf ) > 0 )
	{
		sscanf( Buf, "%d", &m_iDeviceOpen_WaitTime );
		if( m_iDeviceOpen_WaitTime <= 0) 
		{
			m_iDeviceOpen_WaitTime = 3000;
		}
	}
	else
	{
		m_iDeviceOpen_WaitTime = 3000;
	}
	LOG_OUT("m_iDeviceOpen_WaitTime = %d\n", m_iDeviceOpen_WaitTime);

	//-------------------------------------------------
	// ボリュームロック処理のリトライカウント（上限）
	//-------------------------------------------------
	GetPrivateProfileString( "VOLUMELOCK", "RETRYCOUNT", "", Buf, sizeof( Buf ), theApp.m_strIniFilePath );
	if( strlen( Buf ) > 0 )
	{
		sscanf( Buf, "%d", &m_iVolumeLock_MaxRetryCount );
		if( m_iVolumeLock_MaxRetryCount <= 0) 
		{
			m_iVolumeLock_MaxRetryCount = 5;
		}
	}
	else
	{
		m_iVolumeLock_MaxRetryCount = 5;
	}
	LOG_OUT("m_iVolumeLock_MaxRetryCount = %d\n", m_iVolumeLock_MaxRetryCount);

	//-------------------------------------------------
	// ボリュームロック処理のリトライ時・ウエイトタイム
	//-------------------------------------------------
	GetPrivateProfileString( "VOLUMELOCK", "INTERVAL", "", Buf, sizeof( Buf ), theApp.m_strIniFilePath );
	if( strlen( Buf ) > 0 )
	{
		sscanf( Buf, "%d", &m_iVolumeLock_WaitTime );
		if( m_iVolumeLock_WaitTime <= 0) 
		{
			m_iVolumeLock_WaitTime = 1000;
		}
	}
	else
	{
		m_iVolumeLock_WaitTime = 1000;
	}
	LOG_OUT("m_iVolumeLock_WaitTime = %d\n", m_iVolumeLock_WaitTime);

	//-------------------------------------------------
	// ログファイルパス
	//-------------------------------------------------
	GetPrivateProfileString( "LOG", "FilePath", "", Buf2, sizeof( Buf2 ), theApp.m_strIniFilePath );
	if( strlen( Buf2 ) > 0 )
	{
		theApp.m_strLogFilePath.Format("%s", Buf2);
	}

	// パスの右端に'\'がない場合があるので付加する
	theApp.m_strLogFilePath.TrimRight("\\");
	theApp.m_strLogFilePath.Insert( (theApp.m_strLogFilePath.GetLength() + 1), "\\" );

	LOG_OUT("theApp.m_strLogFilePath = %s\n", theApp.m_strLogFilePath);

	//-------------------------------------------------
	// ログファイル名
	//-------------------------------------------------
	GetPrivateProfileString( "LOG", "FileName", "", Buf2, sizeof( Buf2 ), theApp.m_strIniFilePath );
	if( strlen( Buf2 ) > 0 )
	{
		theApp.m_strLogFileName.Format("%s", Buf2);
	}
	LOG_OUT("theApp.m_strLogFileName = %s\n", theApp.m_strLogFileName);

// 3.2.1.0->
	//-------------------------------------------------
	// Profile チェック
	//-------------------------------------------------
	m_uProfileCheckOption = GetPrivateProfileInt( "Common", "CheckProfile", 0, theApp.m_strIniFilePath );
	LOG_OUT("m_uProfileCheckOption = %u\n", m_uProfileCheckOption);
// <-3.2.1.0

	bFuncResult = TRUE;

END_GetIniFileInfo:

	LOG_OUT("(E)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

////////////////////////////////////////////////////////////////////////
// 現在のテストタイプを判定する
////////////////////////////////////////////////////////////////////////
BOOL CCDtestDlg::GetTestType()
{
	LOG_OUT("(S)\n");
	
	if ( stricmp( theApp.m_szModuleFileName_FileName, "dvdtest" ) == 0 )
	{
		// テストタイプの設定
		m_TestType = TEST_DVD;

		// 正解値（比較データ）ファイルの設定
		m_strCompareTargetFile = "DvdTestMedia.dat";

		// ウインドウタイトルの設定
		SetWindowText("DVDtest");

		// アイコンのロード
		m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_DVD);
		SetIcon( m_hIcon, TRUE );
		SetIcon( m_hIcon, FALSE );
	}
	else
	{
		// テストタイプの設定
		m_TestType = TEST_CD;

		// 正解値（比較データ）ファイルの設定
		m_strCompareTargetFile = "CdTestMedia.dat";

		// ウインドウタイトルの設定
		SetWindowText("CDtest");

		// アイコンのロード
		m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CD);
		SetIcon( m_hIcon, TRUE );
		SetIcon( m_hIcon, FALSE );
	}
	LOG_OUT("m_TestType = %s\n", (m_TestType==TEST_CD  ? "TEST_CD" : "TEST_DVD" ));
	LOG_OUT("m_strCompareTargetFile = %s\n", m_strCompareTargetFile );

	LOG_OUT("(E)(TRUE)\n");

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// 指定されたドライブのヴォリュームラベルを取得します
////////////////////////////////////////////////////////////////////////
CString CCDtestDlg::GetVolumeLabel( char *pszDrive )
{
	LOG_OUT("(S)\n");

	CString strResult = "";
	char	szVolumeLabel[256];
	DWORD	dwResult;
	BOOL	bResult;
	BOOL	bFlag_DriveHandle = FALSE;

	// ドライブクローズ
	if ( m_TDODDHandle != (HTDODDDRIVE)INVALID_HANDLE_VALUE )
	{
		bFlag_DriveHandle = TRUE;

		m_lpfnTDODDCloseDrive( m_TDODDHandle );
		m_TDODDHandle = (HTDODDDRIVE)INVALID_HANDLE_VALUE;

		LOG_OUT("m_TDODDHandle = 0x%08x\n", m_TDODDHandle );
	}

// V3.1.2.1->
#if 0
	// ヴォリュームラベルの取得
	::ZeroMemory( szVolumeLabel, sizeof(szVolumeLabel) );

	bResult = ::GetVolumeInformation(	pszDrive,					// ルートディレクトリ
										szVolumeLabel,				// ボリュームラベルバッファ
										sizeof(szVolumeLabel),		// ボリュームラベルバッファのサイズ
										NULL,						// ボリュームのシリアル番号
										NULL,						// ファイル名の最大の長さ
										NULL,						// ファイルシステムのオプション
										NULL,						// ファイルシステム名を格納するバッファ
										NULL );						// ファイルシステム名を格納するバッファのサイズ
	if ( bResult == FALSE )
	{
		dwResult = GetLastError();
		LOG_OUT("GetVolumeInformation() failed. : LastError = 0x%08x\n", dwResult );
	}
	else
	{
		strResult = szVolumeLabel;
		LOG_OUT("szVolumeLabel = %s\n", szVolumeLabel);
	}
#else
	int iRetryCount = m_iDeviceOpen_MaxRetryCount;
	while (true) {
		// ヴォリュームラベルの取得
		::ZeroMemory( szVolumeLabel, sizeof(szVolumeLabel) );

		bResult = ::GetVolumeInformation(	pszDrive,					// ルートディレクトリ
											szVolumeLabel,				// ボリュームラベルバッファ
											sizeof(szVolumeLabel),		// ボリュームラベルバッファのサイズ
											NULL,						// ボリュームのシリアル番号
											NULL,						// ファイル名の最大の長さ
											NULL,						// ファイルシステムのオプション
											NULL,						// ファイルシステム名を格納するバッファ
											NULL );						// ファイルシステム名を格納するバッファのサイズ
		if ( bResult == FALSE )
		{
			iRetryCount--;
			if (iRetryCount < 1) {
				dwResult = GetLastError();
				LOG_OUT("GetVolumeInformation() failed. : LastError = 0x%08x\n", dwResult );
				break;
			}
			LOG_OUT("Retry GetVolumeInformation()\n");
			Sleep(1000);
		}
		else
		{
			strResult = szVolumeLabel;
			LOG_OUT("szVolumeLabel = %s\n", szVolumeLabel);
			break;
		}
	}
#endif
// <-V3.1.2.1

	// デバイスオープン
	if ( bFlag_DriveHandle == TRUE )
	{
		m_TDODDHandle = m_lpfnTDODDOpenDrive( m_szOddDrive[0], &dwResult );

		LOG_OUT("m_TDODDHandle = 0x%08x\n", m_TDODDHandle );
	}

	LOG_OUT("(E)\n");

	return strResult;
}

void CCDtestDlg::OnDestroy()
{
	LOG_OUT("(S)\n");

	CDialog::OnDestroy();

	// TODO: 
	BOOL bResult;

	if ( m_rectMonitor != NULL )
	{
		delete m_rectMonitor;
		m_rectMonitor = NULL;
	}

	if ( m_bMainLogOpenFlag == TRUE )
	{
		// メイン・ログの後始末
		bResult = UnInitMainLog();
		if ( bResult == FALSE )
		{
			LOG_OUT("UnInitMainLog() failed.\n");

			// 終了コードの設定
			theApp.m_iReturnValue = ERROR_LOGFILE;
		}
	}

	//  TDODDLib.Dllのアンロード
	FreeTDODDLib();

	LOG_OUT("(E)\n");
}

BOOL LoadTDODDLib(void)
{
	LOG_OUT("(S)\n");

	char	szPath[MAX_PATH];
	bool	bStatus;
	BOOL	bResult;
	BOOL	bFuncResult = FALSE;

	if (m_hTDODDLib) 
	{ // 既にTDODDLib.dllをロードしていたら解放
		FreeLibrary(m_hTDODDLib);
	}
	m_hTDODDLib = NULL;

	// TDODDLib.dllのパスの準備
	strcpy(szPath, LIB_TDODDLIB);
	bStatus = GetWorkDir(szPath, sizeof(szPath), szPath);
	if (bStatus == false)
	{ // TDODDLib.dllのパスの準備に失敗したら
		goto END_LoadTDODDLib;
	}

	// TDODDLib.dllの存在チェック
	bResult = ::PathFileExists( szPath );
	if ( bResult == FALSE )
	{ // TDODDLib.dllが存在しなかったら

		CString strMessage;
		strMessage.Format("Not found TDODDLib.dll");

		LOG_OUT("%s\n", strMessage );
		CLog.WriteLog( strMessage ); CLog.WriteLog("\r\n");

//		MessageBox(NULL, strMessage, "DVD-ROM Test", MB_OK | MB_ICONERROR);

		goto END_LoadTDODDLib;
	}

	m_hTDODDLib = LoadLibrary(szPath);
	if (m_hTDODDLib == NULL)
	{
		goto END_LoadTDODDLib;
	}

	m_lpfnTDODDSetFlags = NULL;
	m_lpfnTDODDOpenDrive = NULL;
	m_lpfnTDODDCloseDrive = NULL;
	m_lpfnTDODDDeviceIoControl = NULL;
	m_lpfnTDODDLockDrive = NULL;
	m_lpfnTDODDUnlockDrive = NULL;
	m_lpfnTDODDLockVolume = NULL;
	m_lpfnTDODDUnlockVolume = NULL;
	m_lpfnTDODDSendCommand = NULL;

	m_lpfnTDODDSetFlags = (LPTDODDSETFLAGS)GetProcAddress(m_hTDODDLib, TDODDSETFLAGS);
	m_lpfnTDODDOpenDrive = (LPTDODDOPENDRIVE)GetProcAddress(m_hTDODDLib, TDODDOPENDRIVE);
	m_lpfnTDODDCloseDrive = (LPTDODDCLOSEDRIVE)GetProcAddress(m_hTDODDLib, TDODDCLOSEDRIVE);
	m_lpfnTDODDDeviceIoControl = (LPTDODDDEVICEIOCONTROL)GetProcAddress(m_hTDODDLib, TDODDDEVICEIOCONTROL);
	m_lpfnTDODDLockDrive = (LPTDODDLOCKDRIVE)GetProcAddress(m_hTDODDLib, TDODDLOCKDRIVE);
	m_lpfnTDODDUnlockDrive = (LPTDODDUNLOCKDRIVE)GetProcAddress(m_hTDODDLib, TDODDUNLOCKDRIVE);
	m_lpfnTDODDLockVolume = (LPTDODDLOCKVOLUME)GetProcAddress(m_hTDODDLib, TDODDLOCKVOLUME);
	m_lpfnTDODDUnlockVolume = (LPTDODDUNLOCKVOLUME)GetProcAddress(m_hTDODDLib, TDODDUNLOCKVOLUME);
	m_lpfnTDODDSendCommand = (LPTDODDSENDCOMMAND)GetProcAddress(m_hTDODDLib, TDODDSENDCOMMAND);

	if (m_lpfnTDODDSetFlags == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDOpenDrive == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDCloseDrive == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDDeviceIoControl == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDLockDrive == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDUnlockDrive == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDLockVolume == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDUnlockVolume == NULL)
		goto END_LoadTDODDLib;
	if (m_lpfnTDODDSendCommand == NULL)
		goto END_LoadTDODDLib;


	bFuncResult = TRUE;

END_LoadTDODDLib:

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

void FreeTDODDLib(void)
{
	LOG_OUT("(S)\n");

	if (m_hTDODDLib) 
	{
		BOOL bResult = FreeLibrary(m_hTDODDLib);
		if ( bResult == TRUE )
		{
			m_hTDODDLib = NULL;
			LOG_OUT("Success, FreeLibrary() : 0x%08x\n", m_hTDODDLib );
		}
		else
		{
			LOG_OUT("Failure, FreeLibrary() : 0x%08x\n", m_hTDODDLib );
		}
	}

	LOG_OUT("(E)\n");
}

bool GetWorkDir(char *buf, int size, char *File_Name)
{
	LOG_OUT("(S)\n");

	if( !buf || (size <= 0) ) {
		LOG_OUT("(E)(false)\n");
		return false;
	}

	if( !File_Name || (strlen(File_Name) <= 0)){
		LOG_OUT("(E)(false)\n");
		return false;
	}

	char tmpBuf[MAX_PATH] = {0};
	if( GetModuleFileName(NULL, tmpBuf, sizeof(tmpBuf)) ) {
		char * p = strrchr( tmpBuf, '\\' );
		if( p ) {
			*p = '\0';
		}
		if( strlen(tmpBuf) < (unsigned)size ) {
			strcat( tmpBuf,"\\");
			strcat( tmpBuf,File_Name);
			strcpy( buf, tmpBuf );
			//*
			LOG_OUT("buf = %s\n", buf);
			LOG_OUT("(E)(true)\n");
			return true;
		}
	}
	LOG_OUT("(E)(false)\n");
	return false;
}

CString CCDtestDlg::GetProfileString( USHORT usProfile )
{
	LOG_OUT("(S)\n");

	LOG_OUT("usProfile = %u\n", usProfile);

	CString strResult = "";

	if ( usProfile == 0x0008 )
	{
		strResult = "CD-ROM";
	}
	else if ( usProfile == 0x0009 )
	{
		strResult = "CD-R";
	}
	else if ( usProfile == 0x000A )
	{
		strResult = "CD-RW";
	}
	else if ( usProfile == 0x010 )
	{
		strResult = "DVD-ROM";
	}
	else if ( usProfile == 0x011 )
	{
		strResult = "DVD-R Sequential recording";
	}
	else if ( usProfile == 0x012 )
	{
		strResult = "DVD-RAM";
	}
	else if ( usProfile == 0x013 )
	{
		strResult = "DVD-RW Restricted Overwrite";
	}
	else if ( usProfile == 0x014 )
	{
		strResult = "DVD-RW Sequential recording";
	}
	else if ( usProfile == 0x015 )
	{
		strResult = "DVD-R  Dual Layer Sequential recording";
	}
	else if ( usProfile == 0x016 )
	{
		strResult = "DVD-R  Dual Layer Jump recording";
	}
	else if ( usProfile == 0x017 )
	{
		strResult = "DVD-RW Dual Layer";
	}
	else if ( usProfile == 0x018 )
	{
		strResult = "DVD-Download disc recordingDual Layer";
	}
	else if ( usProfile == 0x01A )
	{
		strResult = "DVD+RW";
	}
	else if ( usProfile == 0x01B )
	{
		strResult = "DVD+R";
	}

	LOG_OUT("(E)(%s)\n", strResult);

	return strResult;
}

BOOL CCDtestDlg::InitMainLog()
{
	LOG_OUT("(S)\n");

	BOOL			bFuncResult = FALSE;
	BOOL			bResult;
	CString			strParam_LogOpen;
	CString			strLogDllFilePath;
	nameversion		PVer;

	//-------------------------------------------------------
	// ログ処理DLLのパスの準備
	//-------------------------------------------------------
	strLogDllFilePath.Format("%s%sXMLLogFile.dll", theApp.m_szModuleFileName_Drive, theApp.m_szModuleFileName_Dir);

	//-------------------------------------------------------
	// ログ処理DLL（XMLLogFile.dll）の存在チェック
	//-------------------------------------------------------
	bResult = ::PathFileExists( strLogDllFilePath );
	if ( bResult == FALSE )
	{ // XMLLogFile.dllが存在しなかったら
		LOG_OUT("Not found XMLLogFile.dll.\n");
		goto END_InitMainLog;
	}

	//-------------------------------------------------------
	// ログの初期化
	//-------------------------------------------------------
	CLog.InitLog();

	//-------------------------------------------------------
	// ログのオープン
	//-------------------------------------------------------
	strParam_LogOpen.Format( "%s%s%s%s%s%s%s",
							theApp.m_strLogFilePath, theApp.m_strLogFileName, ".log",
							"*",
							theApp.m_strLogFilePath, theApp.m_strLogFileName, ".xml");
	LOG_OUT("strParam_LogOpen = %s\n", strParam_LogOpen);

	if( CLog.LogOpen( strParam_LogOpen ) != 0 )
	{
		LOG_OUT("CLog.LogOpen() failed.\n");
		goto END_InitMainLog;
	}
	else
	{
		LOG_OUT("CLog.LogOpen() Success.\n");
		LOG_OUT("Txt Log File = %s.txt\n", theApp.m_strLogFileName );
		LOG_OUT("Xml Log File = %s.xml\n", theApp.m_strLogFileName );

		m_bMainLogOpenFlag = TRUE;
		LOG_OUT("m_bMainLogOpenFlag = %s\n", BOOL_TO_A( m_bMainLogOpenFlag ) );
	}

	//-------------------------------------------------------
	// ログ開始時間の取得
	//-------------------------------------------------------
	CLog.LogStart();

	//-------------------------------------------------------
	// ログヘッダ情報（テスト名）の設定
	//-------------------------------------------------------
	if ( stricmp( theApp.m_szModuleFileName_FileName, "cdtest" ) == 0 )
	{
		PVer.name = "CD Test Utility";
	}
	else if ( stricmp( theApp.m_szModuleFileName_FileName, "dvdtest" ) == 0 )
	{
		PVer.name = "DVD Test Utility";
	}
	else
	{
		PVer.name = "Unknown";
	}
	LOG_OUT("PVer.name = %s\n", PVer.name);

	//-------------------------------------------------------
	// ログヘッダ情報（テストバージョン）の設定
	//-------------------------------------------------------
	PVer.version = theApp.m_strFileVersion;
	LOG_OUT("PVer.version = %s\n", PVer.version);

	//-------------------------------------------------------
	// ログヘッダの出力
	//-------------------------------------------------------
	CLog.WriteLogHeader( PVer );
    CLog.WriteLog( "\r\n" );

	bFuncResult = TRUE;

END_InitMainLog:


	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

BOOL CCDtestDlg::UnInitMainLog()
{
	LOG_OUT("(S)\n");

	//-------------------------------------------------------
	// ログ終了時間の取得
	//-------------------------------------------------------
	CLog.LogFinish();

	//-------------------------------------------------------
	// 最終的なテスト結果のログ出力＆終了コードの設定
	//-------------------------------------------------------
	if( theApp.m_bProgramStatus )
	{
		// 最終的なテスト結果をログに出力
		CLog.WriteLogResult( STATE_PASS );
		LOG_OUT("STATE_PASS.\n");

		// 終了コードの設定
		theApp.m_iReturnValue = ERROR_NORMAL_END;
	}
	else
	{
		CString errorMessage;
		errorMessage.Format("<ERRORCODE> %s\r\n", theApp.m_strErrorCode[4]);
		CLog.WriteLogOnly( errorMessage );

		// 最終的なテスト結果をログに出力
		CLog.WriteLogResult( STATE_FAIL, theApp.m_strErrorCode );
		LOG_OUT("STATE_FAIL.\n");
		LOG_OUT("<ERRORCODE> %s\n", theApp.m_strErrorCode[4]);

		// 終了コードの設定
		theApp.m_iReturnValue = ERROR_FAIL_END;
	}

	//-------------------------------------------------------
	// ログのクローズ
	//-------------------------------------------------------
	CLog.LogClose();

	LOG_OUT("(E)(TRUE)\n");

	return TRUE;
}

int CCDtestDlg::IniRead_MessageDlg_Section( char* secname )
{
	LOG_OUT("(S)\n");

	int iResult = -1;
	char chBuf_Work[256] = {0};

	// Start MessageBox Display
	char chBufMsgSectionName[256] = {0};
	strcpy( chBufMsgSectionName, secname );

	MSGDLGSTATUS str_MsgItem;
	char chMsgBoxTitle[256] = {0};
	char chMsgBoxCaption[256] = {0};
	char chMsgBoxImage[MAX_PATH] = {0};
	int iMsgBoxType = 0;
	int iMsgDialogMode = 0;	//++v0004	(0:画像表示ありのダイアログ。1:メッセージ表示のダイアログ。2:標準メッセージボックス)

	// StartMsgBoxTitle Get
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxTitle", "", chMsgBoxTitle, sizeof( chMsgBoxTitle ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxTitle = %s\n", chMsgBoxTitle);

	// StartMsgBoxTitle Get
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxCaption", "", chMsgBoxCaption, sizeof( chMsgBoxCaption ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxCaption = %s\n", chMsgBoxCaption);

	// MsgBoxImagePath Get
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxImagePath", "", chMsgBoxImage, sizeof( chMsgBoxImage ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxImagePath = %s\n", chMsgBoxImage);

	// MsgBoxButton1Caption Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton1Caption", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	str_MsgItem.m_Button1Caption.Format( "%s", chBuf_Work );
	LOG_OUT("MsgBoxButton1Caption = %s\n", chBuf_Work);

	// MsgBoxType Get
	iMsgBoxType = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxType", 0, theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxType = %ld\n", iMsgBoxType);

	// MsgBoxButton2Caption Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton2Caption", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	str_MsgItem.m_Button2Caption.Format( "%s", chBuf_Work );
	LOG_OUT("MsgBoxButton2Caption = %s\n", chBuf_Work);

	// MsgBoxButton3Caption Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton3Caption", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	str_MsgItem.m_Button3Caption.Format( "%s", chBuf_Work );
	LOG_OUT("MsgBoxButton3Caption = %s\n", chBuf_Work);

	// MsgBoxButton1Visible Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton1Visible", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButton1Visible = %s\n", chBuf_Work);
	if( !strcmpi( chBuf_Work, "On" ))
	{
		str_MsgItem.m_bButton1 = true;
	}
	else
	{
		str_MsgItem.m_bButton1 = false;
	}

	// MsgBoxButton2Visible Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton2Visible", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButton2Visible = %s\n", chBuf_Work);
	if( !strcmpi( chBuf_Work, "On" ))
	{
		str_MsgItem.m_bButton2 = true;
	}
	else
	{
		str_MsgItem.m_bButton2 = false;
	}

	// MsgBoxButton3Visible Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton3Visible", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButton3Visible = %s\n", chBuf_Work);
	if( !strcmpi( chBuf_Work, "On" ))
	{
		str_MsgItem.m_bButton3 = true;
	}
	else
	{
		str_MsgItem.m_bButton3 = false;
	}

	// MsgBoxButtonBold Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButtonBold", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButtonBold = %s\n", chBuf_Work);
	if( !strcmpi( chBuf_Work, "On" ))
	{
		str_MsgItem.m_ButtonFontBold = true;
	}
	else
	{
		str_MsgItem.m_ButtonFontBold = false;
	}

	// MsgBoxButton1ReturnValue Get
	str_MsgItem.m_iButton1ReturnVal = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButton1ReturnValue", 1, theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButton1ReturnValue = %ld\n", str_MsgItem.m_iButton1ReturnVal);

	// MsgBoxButton2ReturnValue Get
	str_MsgItem.m_iButton2ReturnVal = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButton2ReturnValue", 1, theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButton2ReturnValue = %ld\n", str_MsgItem.m_iButton2ReturnVal);

	// MsgBoxButton3ReturnValue Get
	str_MsgItem.m_iButton3ReturnVal = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButton3ReturnValue", 1, theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButton3ReturnValue = %ld\n", str_MsgItem.m_iButton3ReturnVal);

	// MsgBoxButtonMouseCursolSetNo Get
	str_MsgItem.m_iButtonCursolSetNo = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButtonMouseCursolSetNo", 1, theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButtonMouseCursolSetNo = %ld\n", str_MsgItem.m_iButtonCursolSetNo);

	// MsgBoxButtonFontSize Get
	str_MsgItem.m_ButtonFontSize = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButtonFontSize", 10, theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButtonFontSize = %ld\n", str_MsgItem.m_ButtonFontSize);

	// MsgBoxTimeOutSet Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxTimeOutSet", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxTimeOutSet = %s\n", chBuf_Work);
	if( !strcmpi( chBuf_Work, "On" ))
	{
		str_MsgItem.m_bTimeOutSetFlag = true;
		LOG_OUT("str_MsgItem.m_bTimeOutSetFlag = %s\n", booltoa(str_MsgItem.m_bTimeOutSetFlag));
	}
	else
	{
		str_MsgItem.m_bTimeOutSetFlag = false;
		LOG_OUT("str_MsgItem.m_bTimeOutSetFlag = %s\n", booltoa(str_MsgItem.m_bTimeOutSetFlag));
	}

	if( str_MsgItem.m_bTimeOutSetFlag )
	{
		// MsgBoxTimeOutIntervalTime Get
		str_MsgItem.m_iTimeOutInterval = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxTimeOutIntervalTime", 0, theApp.m_strIniFilePath );
		LOG_OUT("MsgBoxTimeOutIntervalTime = %ld\n", str_MsgItem.m_iTimeOutInterval);

		// MsgBoxTimeOutReturnValue Get
		str_MsgItem.m_im_iTimeOutReturnValue = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxTimeOutReturnValue", 0, theApp.m_strIniFilePath );
		LOG_OUT("MsgBoxTimeOutReturnValue = %ld\n", str_MsgItem.m_im_iTimeOutReturnValue);
	}

	// MsgBoxButtonMouseCursolDisplayChangeSet Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButtonMouseCursolDisplayChangeSet", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
	LOG_OUT("MsgBoxButtonMouseCursolDisplayChangeSet = %s\n", chBuf_Work);
	if( !strcmpi( chBuf_Work, "On" ))
	{
		str_MsgItem.m_bButtonMouseCursolDspChgSetFlag = true;
		LOG_OUT("str_MsgItem.m_bButtonMouseCursolDspChgSetFlag = %s\n", booltoa(str_MsgItem.m_bButtonMouseCursolDspChgSetFlag));
	}
	else
	{
		str_MsgItem.m_bButtonMouseCursolDspChgSetFlag = false;
		LOG_OUT("str_MsgItem.m_bButtonMouseCursolDspChgSetFlag = %s\n", booltoa(str_MsgItem.m_bButtonMouseCursolDspChgSetFlag));
	}
	
	str_MsgItem.m_MsgWindowTitle.Format( "%s", chMsgBoxTitle );
	wsprintf( str_MsgItem.m_ImageFilePath, "%s", chMsgBoxImage );

	// Image File Check
	if( Image_FileCheck( chMsgBoxImage ))
	{
		// 画像表示ダイアログを使用するモード。
		iMsgDialogMode = 0;
		// 画像ありフラグ
		str_MsgItem.m_bImageFlag = true;
	}
	else
	{
		// 標準的な
		memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
		GetPrivateProfileString( chBufMsgSectionName, "MsgBoxUsgMessageDialogFlag", "On", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
		LOG_OUT("MsgBoxUsgMessageDialogFlag = %s\n", chBuf_Work);
		if( !strcmpi( chBuf_Work, "On" ))
		{
			// メッセージ表示ダイアログを使用するモード。
			iMsgDialogMode = 1;

			// 画像なしフラグ
			str_MsgItem.m_bImageFlag = false;
			
			// 1行目表示メッセージ
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg1", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			str_MsgItem.m_DisplayMsg1.Format( "%s", chBuf_Work );
			LOG_OUT("MsgBoxDisplayMsg1 = %s\n", chBuf_Work);
			
			// 2行目表示メッセージ
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg2", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			str_MsgItem.m_DisplayMsg2.Format( "%s", chBuf_Work );
			LOG_OUT("MsgBoxDisplayMsg2 = %s\n", chBuf_Work);
			
			// 3行目表示メッセージ
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg3", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			str_MsgItem.m_DisplayMsg3.Format( "%s", chBuf_Work );
			LOG_OUT("MsgBoxDisplayMsg3 = %s\n", chBuf_Work);

			// 1行目表示メッセージのアライメント設定(0:Center(デフォルト), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg1Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsg1Alignment = %s\n", chBuf_Work);
			if( !strcmpi( chBuf_Work, "Right" ))
			{
				str_MsgItem.m_DisplayMsg1Alignment = 2;
			} 
			else if ( !strcmpi( chBuf_Work, "Left" ))
			{
				str_MsgItem.m_DisplayMsg1Alignment = 1;
			} 
			else
			{
				str_MsgItem.m_DisplayMsg1Alignment = 0;
			}

			// 2行目表示メッセージのアライメント設定(0:Center(デフォルト), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg2Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsg2Alignment = %s\n", chBuf_Work);
			if( !strcmpi( chBuf_Work, "Right" ))
			{
				str_MsgItem.m_DisplayMsg2Alignment = 2;
			} 
			else if ( !strcmpi( chBuf_Work, "Left" ))
			{
				str_MsgItem.m_DisplayMsg2Alignment = 1;
			} 
			else
			{
				str_MsgItem.m_DisplayMsg2Alignment = 0;
			}

			// 3行目表示メッセージのアライメント設定(0:Center(デフォルト), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg3Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsg3Alignment = %s\n", chBuf_Work);
			if( !strcmpi( chBuf_Work, "Right" ))
			{
				str_MsgItem.m_DisplayMsg3Alignment = 2;
			} 
			else if ( !strcmpi( chBuf_Work, "Left" ))
			{
				str_MsgItem.m_DisplayMsg3Alignment = 1;
			} 
			else
			{
				str_MsgItem.m_DisplayMsg3Alignment = 0;
			}
			
			// メッセージ表示時のスタティックボックスの高さ
			str_MsgItem.m_iDisplayMsgHeight = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgHeight", 30, theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsgHeight = %ld\n", str_MsgItem.m_iDisplayMsgHeight);

			// メッセージ表示時のスタティックボックスの幅
			str_MsgItem.m_iDisplayMsgWidth = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgWidth", 700, theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsgWidth = %ld\n", str_MsgItem.m_iDisplayMsgWidth);
			
			// メッセージ表示時のスタティックボックスのフォントサイズ
			str_MsgItem.m_iDisplayMsgFontSize = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgFontSize", 23, theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsgFontSize = %ld\n", str_MsgItem.m_iDisplayMsgFontSize);

			// メッセージ表示時のスタティックボックスのフォントボールド
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsgBold", "", chBuf_Work, sizeof( chBuf_Work ), theApp.m_strIniFilePath );
			LOG_OUT("MsgBoxDisplayMsgBold = %s\n", chBuf_Work);
			if( !strcmpi( chBuf_Work, "On" ))
			{
				str_MsgItem.m_iDisplayMsgBold = true;
			}
			else
			{
				str_MsgItem.m_iDisplayMsgBold = false;
			}
			
		}
		else
		{
			// 標準メッセージボックスを使用するモード。
			iMsgDialogMode = 2;
		}
	}

	// モードに応じて処理を行う。
	LOG_OUT("iMsgDialogMode = %ld\n", iMsgDialogMode);
	switch(iMsgDialogMode) 
	{
	case 0:	// 画像有。
		iResult = Msg_Dlg_Display( &str_MsgItem );
		break;
	case 1:	// 画像無。メッセージダイアログ使用。
		iResult = Msg_Dlg_Display( &str_MsgItem );
		break;
	case 2:	// 画像無。メッセージダイアログ未使用。
		// タイムアウト設定がある場合は、Sleepを行うのみ。タイムアウト設定が無い場合は、メッセージボックスを表示する。
		if(str_MsgItem.m_bTimeOutSetFlag)
		{
			Sleep(str_MsgItem.m_iTimeOutInterval * 1000);
			// 現状はOKが押された値を設定する。ダイアログの呼び出し元で、戻り値による処理の分岐を行う場合は、要修正。
			iResult = IDOK;
		}
		else
		{
			iResult = ::MessageBox( theApp.GetMainWnd()->GetSafeHwnd(), chMsgBoxCaption, chMsgBoxTitle, iMsgBoxType );
		}
		break;
	default:
		break;
	}

	LOG_OUT("(E)(%ld)\n", iResult);

	return( iResult );
}

int CCDtestDlg::Msg_Dlg_Display( MSGDLGSTATUS* mSts )
{
	LOG_OUT("(S)\n");

	CImageMsgBoxDlg *pMessageDlg = NULL;
	pMessageDlg = new CImageMsgBoxDlg();

	pMessageDlg->m_MsgWindowTitle = mSts->m_MsgWindowTitle;
	pMessageDlg->m_Button1Caption = mSts->m_Button1Caption;
	pMessageDlg->m_Button2Caption = mSts->m_Button2Caption;
	pMessageDlg->m_Button3Caption = mSts->m_Button3Caption;
	pMessageDlg->m_bButton1 = mSts->m_bButton1;
	pMessageDlg->m_bButton2 = mSts->m_bButton2;
	pMessageDlg->m_bButton3 = mSts->m_bButton3;
	pMessageDlg->m_ButtonFontBold = mSts->m_ButtonFontBold;
	pMessageDlg->m_iButton1ReturnVal = mSts->m_iButton1ReturnVal;
	pMessageDlg->m_iButton2ReturnVal = mSts->m_iButton2ReturnVal;
	pMessageDlg->m_iButton3ReturnVal = mSts->m_iButton3ReturnVal;
	pMessageDlg->m_iButtonCursolSetNo = mSts->m_iButtonCursolSetNo;

	wchar_t *pwDataFileName = NULL;
	if(mSts->m_bImageFlag) 
	{
		// 画像が存在する場合は、画像を表示する。
		pMessageDlg->m_bDisplayImageFlag = true;

		int len = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,mSts->m_ImageFilePath,-1,NULL,0);
		pwDataFileName = new wchar_t[len + 1]; 
		MultiByteToWideChar( CP_ACP,				// ANSI code page
							MB_PRECOMPOSED,		// 既定の変換方法
							mSts->m_ImageFilePath,    // 変換元文字列
							-1,					// 変換元文字列サイズ(-1指定で自動計算)
							pwDataFileName,        // 変換先バッファ
							len+1);				// 変換先バッファサイズ
		pMessageDlg->pwDataFileName = pwDataFileName;
	}
	else
	{
		// 画像が存在しない場合は、メッセージを表示する。
		pMessageDlg->m_bDisplayImageFlag = false;
		pMessageDlg->m_csDisplayMsg1 = mSts->m_DisplayMsg1;
		pMessageDlg->m_csDisplayMsg2 = mSts->m_DisplayMsg2;
		pMessageDlg->m_csDisplayMsg3 = mSts->m_DisplayMsg3;
		pMessageDlg->m_iDisplayMsg1Alignment = mSts->m_DisplayMsg1Alignment;
		pMessageDlg->m_iDisplayMsg2Alignment = mSts->m_DisplayMsg2Alignment;
		pMessageDlg->m_iDisplayMsg3Alignment = mSts->m_DisplayMsg3Alignment;
		pMessageDlg->m_iDisplayMsgHeight = mSts->m_iDisplayMsgHeight;
		pMessageDlg->m_iDisplayMsgWidth = mSts->m_iDisplayMsgWidth;
		pMessageDlg->m_iDisplayMsgFontSize = mSts->m_iDisplayMsgFontSize;
		pMessageDlg->m_iDisplayMsgBold = mSts->m_iDisplayMsgBold;
	}

	pMessageDlg->m_ButtonFontSize = mSts->m_ButtonFontSize;
	pMessageDlg->m_prectDispWin = &m_rectMonitor[m_SelectMonitor - 1];

	pMessageDlg->m_bTimeOutSetFlag = mSts->m_bTimeOutSetFlag;
	pMessageDlg->m_iTimeOutInterval = mSts->m_iTimeOutInterval;
	pMessageDlg->m_iTimeOutReturnValue = mSts->m_im_iTimeOutReturnValue;

	pMessageDlg->m_bButtonMouseCursolDspChgSetFlag = mSts->m_bButtonMouseCursolDspChgSetFlag;

	INT_PTR nResponse = pMessageDlg->DoModal();

	if(pwDataFileName != NULL) 
	{
		delete pwDataFileName;
	}
	delete pMessageDlg;

	LOG_OUT("(E)(%ld)\n", nResponse);

	return( (INT)nResponse );
}

void CCDtestDlg::Display_Setting()
{
	LOG_OUT("(S)\n");

	DWORD dwData = 0;

	// ディスプレイ数を取得して作業領域確保
	m_MonitorMax = GetSystemMetrics(SM_CMONITORS);
	m_rectMonitor = new RECT[m_MonitorMax];
	LOG_OUT("m_MonitorMax = %ld\n", m_MonitorMax);

	// ディスプレイを列挙しモニター情報を取得
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)DisplayEnumProc, (LPARAM)&dwData);

	LOG_OUT("(E)\n");
}

// モニター情報取得
BOOL CCDtestDlg::DisplayEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData)
{
	LOG_OUT("(S)\n");

	char strMessage[256];
	int &iIndex = *(int *)dwData;

	CLog.WriteLog("\r\n");
	CLog.WriteLogSection( "Display Detection" );
	LOG_OUT("[Display Detection]\n"); 

	MONITORINFOEX minfo;
	minfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMon, &minfo);

	sprintf(strMessage, "DisplayEnumProc No(%d). = %s", iIndex + 1, minfo.szDevice); CLog.WriteLog(strMessage); LOG_OUT("%s\n",strMessage); CLog.WriteLog("\r\n");
	sprintf(strMessage, "Left = %d, ", lpMon->left); LOG_OUT("%s\n",strMessage); CLog.WriteLog(strMessage);
	sprintf(strMessage, "Top = %d, ", lpMon->top); LOG_OUT("%s\n",strMessage); CLog.WriteLog(strMessage);
	sprintf(strMessage, "Right = %d, ", lpMon->right); LOG_OUT("%s\n",strMessage); CLog.WriteLog(strMessage);
	sprintf(strMessage, "Bottom = %d", lpMon->bottom); LOG_OUT("%s\n",strMessage); CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");

	CHAR *p = minfo.szDevice + strlen("\\\\.\\DISPLAY");
	if (p != NULL) 
	{
		int i = -1;
		sscanf(p, "%d", &i);
		if (i < 1 || i >  m_MonitorMax) 
		{
			sprintf(strMessage, "Invalid device name."); CLog.WriteLog(strMessage); LOG_OUT("%s\n",strMessage); CLog.WriteLog("\r\n\r\n");
		}
		else 
		{
			i--;
			m_rectMonitor[i].bottom = lpMon->bottom;
			m_rectMonitor[i].left = lpMon->left;
			m_rectMonitor[i].right = lpMon->right;
			m_rectMonitor[i].top = lpMon->top;
		}
	}

	iIndex++;

	LOG_OUT("(E)(TRUE)\n");

	return TRUE;
}

bool CCDtestDlg::Image_FileCheck( char* file )
{
	LOG_OUT("(S)\n");

	bool bResult = TRUE;

	if( !::PathFileExists( file ) ) 
	{
		bResult = FALSE;
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bResult) );

	return( bResult );
}

// V3.1.2.1->
bool CCDtestDlg::CheckTDConf()
{
#define TDCONF_NAME		"C:\\TDConf.txt"

	LOG_OUT("(S)\n");

	char strMessage[256];
	bool bStatus = true;
	char chConf = ' ';

	// チェックするTDConfの項目番号を取得
	int iOffset = GetPrivateProfileInt("TDConf", "CheckColumn", -1, theApp.m_strIniFilePath);
	if (iOffset == -1) {
		bStatus = false;
		sprintf(strMessage, "CheckColumn setting error in ini file.");
		CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
		LOG_OUT("%s\n", strMessage);
	}
	else {
		// TDConfより指定された項目番号のデータを取得
		CFile tdconf;
		if (tdconf.Open(TDCONF_NAME, CFile::modeRead) != 0) {

			long lLen = (long)tdconf.GetLength();
			char *strBuffer = new char[lLen];
	
			tdconf.Read(strBuffer, lLen);
			chConf = *(strchr(strBuffer, '*') + iOffset);

			delete[] strBuffer;
			tdconf.Close();
		}
		else {
			bStatus = false;
			sprintf(strMessage, "TDConf file reading error.");
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage);
		}
	}

	if (bStatus) {
		char strCheckValue[256];
		bool bFound = false;

		// INIファイルより比較するデータ取得
		GetPrivateProfileString("TDConf", "CheckValue", "", strCheckValue, 256, theApp.m_strIniFilePath);
		if (strlen(strCheckValue) == 0) {
			bStatus = false;
			sprintf(strMessage, "CheckValue setting error ini ini file.");
			CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			LOG_OUT("%s\n", strMessage);
		}

		if (bStatus) {
			for (int i = 0; i < (int)strlen(strCheckValue); i++) {
				if (strCheckValue[i] == ',')
					continue;
				if (strCheckValue[i] == chConf) {
					bFound = true;
					break;
				}
			}
		}

		if (bFound == false) {
			bStatus = false;
			sprintf(strMessage, "Not found target setting (%c).", chConf);
		}
		else {
			sprintf(strMessage, "Found target setting (%c).", chConf);
		}
		CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
		LOG_OUT("%s\n", strMessage);
	}		

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bStatus));

	return bStatus;
}
// <-V3.1.2.1