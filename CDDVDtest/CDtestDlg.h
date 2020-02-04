// CDtestDlg.h : �w�b�_�[ �t�@�C��
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

#define TIMER_ID					1		// �^�C�}�[ID
#define TEST_LOOP_TIME				250		// �e�X�g���[�v�Ԋu(ms)

#define TEST_NAME(TEST_TYPE)			((TEST_TYPE)?"DVD Test":"CD Test")
#define TEST_DRIVE_NAME(TEST_TYPE)		((TEST_TYPE)?"DVD Drive":"CD Drive")
#define TEST_MEDIA_NAME(TEST_TYPE)		((TEST_TYPE)?"DVD Media":"CD Media")

#define TEST_UNIT_READY_MAX_RETRY_COUNT				400			// TEST UNIT READY���g���C��

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
// CCDtestDlg �_�C�A���O

class CCDtestDlg : public CDialog
{
// �\�z
public:
	CCDtestDlg(CWnd* pParent = NULL);	// �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CCDtestDlg)
	enum { IDD = IDD_CDTEST_DIALOG };
		// ����: ���̈ʒu�� ClassWizard �ɂ���ăf�[�^ �����o���ǉ�����܂��B
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CCDtestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	HICON m_hIcon;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
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

	TEST_TYPE			m_TestType;									// �e�X�g���( CD or DVD )

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
	TEST_STEP			m_CurrentStep;								// �e�X�g�̏�ԑJ��

	HTDODDDRIVE			m_TDODDHandle;								// ODD�h���C�u�n���h��
	UCHAR				m_ucBuffer[65536];							// �f�[�^�o�b�t�@
	char				m_szOddDrive[_MAX_DRIVE+1];					// ODD�h���C�u������
	CString				m_strVolumeLabel;							// ODD ���f�B�A�̃��H�����[�����x��
	UINT				m_uDebugOption;								// �f�o�b�O�I�v�V����
	UINT				m_timerID;									// �^�C�}�[ID
	UINT				m_uProfileCheckOption;						// �����郁�f�B�A�̃`�F�b�N���s����		// 3.1.2.0

	BOOL				m_bRetryFlag;								// �t���O�F���g���C�i�S�Ẵe�X�g�X�e�b�v�j�����邩�ۂ�
	BOOL				m_bTrayOpenFlag_TestEnd;					// �e�X�g�i�v���O�����j�I�����Ƀg���C���I�[�v�����邩�ۂ�
	BOOL				m_bMainLogOpenFlag;							// ���C�����O���I�[�v������Ă��邩�ۂ�
	BOOL				m_bTestDataSaveFlag;						// �e�X�g���f�B�A����e�X�g�Ŏg�p����f�[�^��ǂݎ��A��r�i����l�j�t�@�C���ɃZ�[�u���邩�ۂ�
	BOOL				m_bIniFileExist;							// INI�t�@�C�������݂��邩�ۂ�

	BOOL				m_bVolumeLock;								// �t���O�F�{�����[�����b�N
	BOOL				m_bTrayLock;								// �t���O�F�g���C���b�N

	CString				m_strCompareTargetFile;						// �e�X�g�Ŏg�p����f�[�^�t�@�C���̃p�X
    ULONG				m_ulSectorOffset;							// �e�X�g�Ŏg�p����DVD�f�[�^�̈ʒu���i�擪����̃I�t�Z�b�g�j
	ULONG				m_ulSectorNum;								// �e�X�g�Ŏg�p����DVD�f�[�^�̃Z�N�^�[��

	int					m_iTestUnitReadyCount;						// TEST UNIT READY �̃��g���C��

	int					m_iMaxRetryCount;							// �S�̏����F�ő僊�g���C��
	int					m_iCurrentRetryCount;						// �S�̏����F���݂̃��g���C��

	int					m_iDeviceOpen_MaxRetryCount;				// �f�o�C�X�I�[�v�������F�ő僊�g���C��
	int					m_iDeviceOpen_WaitTime;						// �f�o�C�X�I�[�v�������F�ҋ@����
	int					m_iDeviceOpen_CurrentRetryCount;			// �f�o�C�X�I�[�v�������F���݂̃��g���C��

	int					m_iVolumeLock_MaxRetryCount;				// �{�����[�����b�N�����F�ő僊�g���C��
	int					m_iVolumeLock_WaitTime;						// �{�����[�����b�N�����F�ҋ@����
	int					m_iVolumeLock_CurrentRetryCount;			// �{�����[�����b�N�����F���݂̃��g���C��

	int					m_iReadDataCompare_MaxRetryCount;			// �f�[�^�ǂݎ�聕��r�����F�ő僊�g���C��
	int					m_iReadDataCompare_WaitTime;				// �f�[�^�ǂݎ�聕��r�����F�ҋ@����
	int					m_iReadDataCompare_CurrentRetryCount;		// �f�[�^�ǂݎ�聕��r�����F���݂̃��g���C��

	int					m_iReadDataSave_MaxRetryCount;				// �f�[�^�ǂݎ�聕��r�i����l�j�f�[�^�t�@�C���쐬�����F�ő僊�g���C��
	int					m_iReadDataSave_WaitTime;					// �f�[�^�ǂݎ�聕��r�i����l�j�f�[�^�t�@�C���쐬�����F�ҋ@����
	int					m_iReadDataSave_CurrentRetryCount;			// �f�[�^�ǂݎ�聕��r�i����l�j�f�[�^�t�@�C���쐬�����F���݂̃��g���C��

	CString		GetTestStepString( TEST_STEP step );				// �w�肳�ꂽ�e�X�g�X�e�b�v�ɑΉ�����������̎擾
	CString		GetTestTypeString( TEST_TYPE type );				// �w�肳�ꂽ�e�X�g�^�C�v�ɑΉ�����������̎擾
	BOOL		GetTestType();										// ���݂̃e�X�g�^�C�v�𔻒肷��

	BOOL		InitMainLog();										// ���C���E���O�̏��������s��
	BOOL		UnInitMainLog();									// ���C���E���O�̌�n�����s��
	BOOL		AnalizeAppParam();									// �A�v���P�[�V�����E�p�����[�^�̉��
	BOOL		GetIniFileInfo();									// INI�t�@�C�����̎擾

	BOOL		DriveDetect(PDWORD pdwStatus);						// ODD�h���C�u�̌�������
	BOOL		TrayLock(VOID);										// �g���C�̃��b�N����
	BOOL		MediaCheck(VOID);									// ���f�B�A�̃`�F�b�N����
	BOOL		ProfilesCheck(VOID);								// �����郁�f�B�A�̃`�F�b�N����		// 3.1.2.0
	BOOL		ReadDataCompare(VOID);								// �e�X�g�f�[�^�̓ǂݎ�聕��r����
	BOOL		ReadDataSave(VOID);									// �e�X�g�f�[�^�ǂݎ�聕��r�i����l�j�t�@�C���쐬����
	void		Epilogue();											// �A�v���P�[�V�����̏I������
	CString		GetVolumeLabel( char *pszDrive );					// �w�肳�ꂽ�h���C�u�̃��H�����[�����x�����擾���܂�
	CString		GetProfileString( USHORT usProfile );				// �w�肳�ꂽ�v���t�@�C���̕�������擾���܂��B

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
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_CDTESTDLG_H__FAA262C6_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
