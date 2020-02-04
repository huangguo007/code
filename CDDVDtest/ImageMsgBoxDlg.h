// ImageMsgBoxDlg.h : header file
//
#pragma once

#include <gdiplus.h>
#include "afxwin.h"
#pragma comment(lib, "GdiPlus.lib")
using namespace Gdiplus;

#if 0

typedef struct _ImageMsgBoxDlg {
	CString m_MsgWindowTitle;	// �^�C�g��
	CString m_Button1Caption;	// �{�^��1(IDOK)�L���v�V����
	CString m_Button2Caption;	// �{�^��2(IDCANCEL)�L���v�V����

	WCHAR *pwDataFileName;		// �C���[�W�t�@�C����
	LONG m_ButtonFontSize;		// �{�^���t�H���g�T�C�Y
} IMAGEMSGBOXDLG, *PIMAGEMSGBOXDLG;

int ImageMsgBoxDlg(HWND hWnd, HINSTANCE hInst, PIMAGEMSGBOXDLG pInfo, RECT *pRect);

#else

#define T_TIMEOUT_SET			0			//++>v0002
#define T_MOUSE_CORSOL_SET		1			//++>v0003

// CImageMsgBoxDlg dialog
class CImageMsgBoxDlg : public CDialog
{
// Construction
public:
	CImageMsgBoxDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMAGEMSGBOX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRetry();
	afx_msg void OnTimer(UINT nIDEvent);		//++>v0002

	UINT uiWidth;
	UINT uiHeight;

	CFont cFont;
	CFont cFontst;								//++>v0004

	Image *m_pImage;

	void Button_MouseCorsulSet();				//++>v0003
	void Button_FocusSet();						//++>v0004

public:
	CString m_MsgWindowTitle;	// �^�C�g��
	CString m_Button1Caption;	// �{�^��1(IDOK)�L���v�V����
	CString m_Button2Caption;	// �{�^��2(IDCANCEL)�L���v�V����
	CString m_Button3Caption;	// �{�^��3(Retry)�L���v�V����

	BOOL m_bButton1;
	BOOL m_bButton2;
	BOOL m_bButton3;

	WCHAR *pwDataFileName;		// �C���[�W�t�@�C����
	LONG m_ButtonFontSize;		// �{�^���t�H���g�T�C�Y
	bool m_ButtonFontBold;		// �{�^���t�H���gBold Tyoe

	RECT *m_prectDispWin;

	int m_iButton1ReturnVal;	// �{�^��1�߂�l
	int m_iButton2ReturnVal;	// �{�^��2�߂�l
	int m_iButton3ReturnVal;	// �{�^��3�߂�l

	int m_iButtonCursolSetNo;	// �}�E�X�J�[�\����Ă���{�^���̔ԍ�(1�`3)

//++>v0002
	bool m_bTimeOutSetFlag;		// TimeOut true:���{, false:�����{
	int m_iTimeOutInterval;		// TimeOut Interval����
	int m_iTimeOutReturnValue;	// TimeOut���ɕԂ��l
//<++v0002

	bool m_bButtonMouseCursolDspChgSetFlag;		// ��ʉ𑜓x�ύX���Ƀ}�E�X�J�[�\���̒ǐ��� true:�s��, false�s��Ȃ�	//++>v0003

//++>v0004
	bool m_bDisplayImageFlag;	// ���[�h true:�摜�\��, false:���b�Z�[�W�\��
	CString m_csDisplayMsg1;	// �摜�������ɕ\�����郁�b�Z�[�W1�s��
	CString m_csDisplayMsg2;	// �摜�������ɕ\�����郁�b�Z�[�W2�s��
	CString m_csDisplayMsg3;	// �摜�������ɕ\�����郁�b�Z�[�W3�s��
	int m_iDisplayMsg1Alignment;// �摜�������ɕ\�����郁�b�Z�[�W1�s��
	int m_iDisplayMsg2Alignment;// �摜�������ɕ\�����郁�b�Z�[�W2�s��
	int m_iDisplayMsg3Alignment;// �摜�������ɕ\�����郁�b�Z�[�W3�s��
	int m_iUseMsgCount;			// �摜�������ɕ\�����郁�b�Z�[�W�̍s��
	int m_iDisplayMsgHeight;	// �摜�������̃X�^�e�B�b�N�{�b�N�X����
	int m_iDisplayMsgWidth;		// �摜�������̃X�^�e�B�b�N�{�b�N�X��
	int m_iDisplayMsgFontSize;	// �摜�������̃X�^�e�B�b�N�{�b�N�X�t�H���g�T�C�Y
	int m_iDisplayMsgBold;		// �摜�������̃X�^�e�B�b�N�{�b�N�X�{�[���h
//<++v0004

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);		//++>v0003
//++>v0004
public:
	CStatic m_csMsg1;
	CStatic m_csMsg2;
	CStatic m_csMsg3;
//<++v0004
};
#endif
