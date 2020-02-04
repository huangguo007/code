// CDtestDlg.h : ヘッダー ファイル
//

#if !defined(AFX_CDTESTDLG_H__FAA262C6_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
#define AFX_CDTESTDLG_H__FAA262C6_6413_11D5_AE4C_0010A405E9A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include "Atapi.h"

#include <Shlwapi.h>
#include "ImageMsgBoxDlg.h"

#pragma warning(disable:4996)

#define TIMER_ID					1		// タイマーID
#define TEST_LOOP_TIME				250		// テストループ間隔(ms)

#define TEST_NAME(TEST_TYPE)			((TEST_TYPE)?"DVD Test":"CD Test")
#define TEST_DRIVE_NAME(TEST_TYPE)		((TEST_TYPE)?"DVD Drive":"CD Drive")
#define TEST_MEDIA_NAME(TEST_TYPE)		((TEST_TYPE)?"DVD Media":"CD Media")

#define TEST_UNIT_READY_MAX_RETRY_COUNT				400			// TEST UNIT READYリトライ回数

typedef enum
{
	TEST_CD = 0,
	TEST_DVD
} TEST_TYPE;

typedef struct 
{
	CString m_MsgWindowTitle;
	CString m_Button1Caption;
	CString m_Button2Caption;
	CString m_Button3Caption;
	bool m_bButton1;
	bool m_bButton2;
	bool m_bButton3;
	bool m_ButtonFontBold;
	int m_iButton1ReturnVal;
	int m_iButton2ReturnVal;
	int m_iButton3ReturnVal;
	int m_iButtonCursolSetNo;
	char m_ImageFilePath[MAX_PATH];
	LONG m_ButtonFontSize;
	bool m_bTimeOutSetFlag;
	int m_iTimeOutInterval;
	int m_im_iTimeOutReturnValue;
	bool m_bButtonMouseCursolDspChgSetFlag;
	bool m_bImageFlag;
	CString m_DisplayMsg1;
	CString m_DisplayMsg2;
	CString m_DisplayMsg3;
	int m_DisplayMsg1Alignment;
	int m_DisplayMsg2Alignment;
	int m_DisplayMsg3Alignment;
	int m_iDisplayMsgHeight;
	int m_iDisplayMsgWidth;
	int m_iDisplayMsgFontSize;
	int m_iDisplayMsgBold;
} MSGDLGSTATUS, *lpMSGDLGSTATUS;

/////////////////////////////////////////////////////////////////////////////
// CCDtestDlg ダイアログ

class CCDtestDlg : public CDialog
{
// 構築
public:
	CCDtestDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CCDtestDlg)
	enum { IDD = IDD_CDTEST_DIALOG };
		// メモ: この位置に ClassWizard によってデータ メンバが追加されます。
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CCDtestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	HICON m_hIcon;

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CCDtestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void StrUpper(char * pszString);
	int GetModuleFileDirectory(char * pszPath, int nSize);
	int GetDriveMediaType(const char * pDrive);
	void SendWaitPCBTester(void);

private:

	TEST_TYPE			m_TestType;									// テスト種別( CD or DVD )

	typedef enum
	{
		Step_Drive_Detect = 0,
		Step_Drive_Ready,
		Step_Tray_Lock,
		Step_Volume_Lock,
		Step_Media_Information,
		Step_ReadDataCompare,
		Step_ReadDataSave,
		Step_OK,
		Step_NG,
		Step_Retry
	} TEST_STEP;
	TEST_STEP			m_CurrentStep;								// テストの状態遷移

	HTDODDDRIVE			m_TDODDHandle;								// ODDドライブハンドル
	UCHAR				m_ucBuffer[65536];							// データバッファ
	char				m_szOddDrive[_MAX_DRIVE+1];					// ODDドライブ文字列
	CString				m_strVolumeLabel;							// ODD メディアのヴォリュームラベル
	UINT				m_uDebugOption;								// デバッグオプション
	UINT				m_timerID;									// タイマーID
	UINT				m_uProfileCheckOption;						// 扱えるメディアのチェックを行うか		// 3.1.2.0

	BOOL				m_bRetryFlag;								// フラグ：リトライ（全てのテストステップ）ンするか否か
	BOOL				m_bTrayOpenFlag_TestEnd;					// テスト（プログラム）終了時にトレイをオープンするか否か
	BOOL				m_bMainLogOpenFlag;							// メインログがオープンされているか否か
	BOOL				m_bTestDataSaveFlag;						// テストメディアからテストで使用するデータを読み取り、比較（正解値）ファイルにセーブするか否か
	BOOL				m_bIniFileExist;							// INIファイルが存在するか否か

	BOOL				m_bVolumeLock;								// フラグ：ボリュームロック
	BOOL				m_bTrayLock;								// フラグ：トレイロック

	CString				m_strCompareTargetFile;						// テストで使用するデータファイルのパス
    ULONG				m_ulSectorOffset;							// テストで使用するDVDデータの位置情報（先頭からのオフセット）
	ULONG				m_ulSectorNum;								// テストで使用するDVDデータのセクター数

	int					m_iTestUnitReadyCount;						// TEST UNIT READY のリトライ数

	int					m_iMaxRetryCount;							// 全体処理：最大リトライ数
	int					m_iCurrentRetryCount;						// 全体処理：現在のリトライ数

	int					m_iDeviceOpen_MaxRetryCount;				// デバイスオープン処理：最大リトライ数
	int					m_iDeviceOpen_WaitTime;						// デバイスオープン処理：待機時間
	int					m_iDeviceOpen_CurrentRetryCount;			// デバイスオープン処理：現在のリトライ数

	int					m_iVolumeLock_MaxRetryCount;				// ボリュームロック処理：最大リトライ数
	int					m_iVolumeLock_WaitTime;						// ボリュームロック処理：待機時間
	int					m_iVolumeLock_CurrentRetryCount;			// ボリュームロック処理：現在のリトライ数

	int					m_iReadDataCompare_MaxRetryCount;			// データ読み取り＆比較処理：最大リトライ数
	int					m_iReadDataCompare_WaitTime;				// データ読み取り＆比較処理：待機時間
	int					m_iReadDataCompare_CurrentRetryCount;		// データ読み取り＆比較処理：現在のリトライ数

	int					m_iReadDataSave_MaxRetryCount;				// データ読み取り＆比較（正解値）データファイル作成処理：最大リトライ数
	int					m_iReadDataSave_WaitTime;					// データ読み取り＆比較（正解値）データファイル作成処理：待機時間
	int					m_iReadDataSave_CurrentRetryCount;			// データ読み取り＆比較（正解値）データファイル作成処理：現在のリトライ数

	CString		GetTestStepString( TEST_STEP step );				// 指定されたテストステップに対応した文字列の取得
	CString		GetTestTypeString( TEST_TYPE type );				// 指定されたテストタイプに対応した文字列の取得
	BOOL		GetTestType();										// 現在のテストタイプを判定する

	BOOL		InitMainLog();										// メイン・ログの初期化を行う
	BOOL		UnInitMainLog();									// メイン・ログの後始末を行う
	BOOL		AnalizeAppParam();									// アプリケーション・パラメータの解析
	BOOL		GetIniFileInfo();									// INIファイル情報の取得

	BOOL		DriveDetect(PDWORD pdwStatus);						// ODDドライブの検索処理
	BOOL		TrayLock(VOID);										// トレイのロック処理
	BOOL		MediaCheck(VOID);									// メディアのチェック処理
	BOOL		ProfilesCheck(VOID);								// 扱えるメディアのチェック処理		// 3.1.2.0
	BOOL		ReadDataCompare(VOID);								// テストデータの読み取り＆比較処理
	BOOL		ReadDataSave(VOID);									// テストデータ読み取り＆比較（正解値）ファイル作成処理
	void		Epilogue();											// アプリケーションの終了処理
	CString		GetVolumeLabel( char *pszDrive );					// 指定されたドライブのヴォリュームラベルを取得します
	CString		GetProfileString( USHORT usProfile );				// 指定されたプロファイルの文字列を取得します。

	CDBERRCODE	CdbErrorSave;		// 3.1.2.0

	int			m_SelectMonitor;
	int			IniRead_MessageDlg_Section( char* secname );
	int			Msg_Dlg_Display( MSGDLGSTATUS* mSts );
	void		Display_Setting();
	static BOOL DisplayEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData);
	bool		Image_FileCheck( char* file );

	bool CheckTDConf();		// V3.1.2.1

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CDTESTDLG_H__FAA262C6_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
