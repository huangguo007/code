// ImageMsgBoxDlg.h : header file
//
#pragma once

#include <gdiplus.h>
#include "afxwin.h"
#pragma comment(lib, "GdiPlus.lib")
using namespace Gdiplus;

#if 0

typedef struct _ImageMsgBoxDlg {
	CString m_MsgWindowTitle;	// タイトル
	CString m_Button1Caption;	// ボタン1(IDOK)キャプション
	CString m_Button2Caption;	// ボタン2(IDCANCEL)キャプション

	WCHAR *pwDataFileName;		// イメージファイル名
	LONG m_ButtonFontSize;		// ボタンフォントサイズ
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
	CString m_MsgWindowTitle;	// タイトル
	CString m_Button1Caption;	// ボタン1(IDOK)キャプション
	CString m_Button2Caption;	// ボタン2(IDCANCEL)キャプション
	CString m_Button3Caption;	// ボタン3(Retry)キャプション

	BOOL m_bButton1;
	BOOL m_bButton2;
	BOOL m_bButton3;

	WCHAR *pwDataFileName;		// イメージファイル名
	LONG m_ButtonFontSize;		// ボタンフォントサイズ
	bool m_ButtonFontBold;		// ボタンフォントBold Tyoe

	RECT *m_prectDispWin;

	int m_iButton1ReturnVal;	// ボタン1戻り値
	int m_iButton2ReturnVal;	// ボタン2戻り値
	int m_iButton3ReturnVal;	// ボタン3戻り値

	int m_iButtonCursolSetNo;	// マウスカーソルをｾｯﾄするボタンの番号(1～3)

//++>v0002
	bool m_bTimeOutSetFlag;		// TimeOut true:実施, false:未実施
	int m_iTimeOutInterval;		// TimeOut Interval時間
	int m_iTimeOutReturnValue;	// TimeOut時に返す値
//<++v0002

	bool m_bButtonMouseCursolDspChgSetFlag;		// 画面解像度変更時にマウスカーソルの追随を true:行う, false行わない	//++>v0003

//++>v0004
	bool m_bDisplayImageFlag;	// モード true:画像表示, false:メッセージ表示
	CString m_csDisplayMsg1;	// 画像無し時に表示するメッセージ1行目
	CString m_csDisplayMsg2;	// 画像無し時に表示するメッセージ2行目
	CString m_csDisplayMsg3;	// 画像無し時に表示するメッセージ3行目
	int m_iDisplayMsg1Alignment;// 画像無し時に表示するメッセージ1行目
	int m_iDisplayMsg2Alignment;// 画像無し時に表示するメッセージ2行目
	int m_iDisplayMsg3Alignment;// 画像無し時に表示するメッセージ3行目
	int m_iUseMsgCount;			// 画像無し時に表示するメッセージの行数
	int m_iDisplayMsgHeight;	// 画像無し時のスタティックボックス高さ
	int m_iDisplayMsgWidth;		// 画像無し時のスタティックボックス幅
	int m_iDisplayMsgFontSize;	// 画像無し時のスタティックボックスフォントサイズ
	int m_iDisplayMsgBold;		// 画像無し時のスタティックボックスボールド
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
