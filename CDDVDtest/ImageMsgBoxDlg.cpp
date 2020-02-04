
// ImageMsgBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "ImageMsgBoxDlg.h"

#if 0

extern CAllFileControl CLog;
extern char strMessage[256];

const char g_szImageMsgBoxClassName[] = "ImageMsgBoxDlg";		// ウィンドウクラス名
HWND g_hImageMsgBoxDlg;					// ダイアログハンドル
int g_nExitCode = 0;					// 戻り値
HINSTANCE g_hImageMsgBoxInst;
PIMAGEMSGBOXDLG pImageMsgInfo = NULL;
Image *g_pMsgImage;
HFONT g_ButtonFont;
LOGFONT g_LogFont;
RECT g_rectDispWin;

LRESULT CALLBACK ImageMsgBoxDlgProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

int ImageMsgBoxDlg(HWND hWnd, HINSTANCE hInst, PIMAGEMSGBOXDLG pInfo, RECT *pRect)
{
	MSG msg;
	WNDCLASS wndclass;

	g_hImageMsgBoxInst = hInst;
	pImageMsgInfo = pInfo;
	g_rectDispWin = *pRect;

	// ウインドウクラスの登録
	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)ImageMsgBoxDlgProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;
	wndclass.hInstance = g_hImageMsgBoxInst;
	wndclass.hIcon = LoadIcon(hInst, (LPCTSTR)IDI_DISPLAY);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //(HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = g_szImageMsgBoxClassName;
	if (RegisterClass(&wndclass) == 0)
		return -1;
	
	// メインウインドウの生成
	if ((g_hImageMsgBoxDlg = CreateDialog(g_hImageMsgBoxInst, MAKEINTRESOURCE(IDD_IMAGEMSGBOX_DIALOG), 
											0, (DLGPROC)ImageMsgBoxDlgProc)) == NULL)
		return -1;
	
	// ウインドウの表示
	ShowWindow(g_hImageMsgBoxDlg, SW_SHOWNORMAL);
	UpdateWindow(g_hImageMsgBoxDlg);

	// ボタンアクセラレーターキーの設定
	HACCEL hAccel;
	ACCEL accel[2];

	accel[0].key = pInfo->m_Button1Caption[0];
	accel[0].cmd = IDOK;
	accel[0].fVirt = FVIRTKEY; // FALT | FCONTROL | FSHIFT

	accel[1].key = pInfo->m_Button2Caption[0];
	accel[1].cmd = IDCANCEL;
	accel[1].fVirt = FVIRTKEY; // FALT | FCONTROL | FSHIFT

	hAccel = CreateAcceleratorTable(accel, 2);

	// メッセージループ
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(g_hImageMsgBoxDlg, hAccel, &msg) &&
			!IsDialogMessage(g_hImageMsgBoxDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// アクセラレーターキー開放
	DestroyAcceleratorTable(hAccel);

	return msg.wParam;
}

LRESULT CALLBACK ImageMsgBoxDlgProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (uMessage) {
	//ウインドウ作成時の処理
	case WM_INITDIALOG:
		if (pImageMsgInfo != NULL) {
			// ウィンドウタイトル
			SetWindowText(hWnd, pImageMsgInfo->m_MsgWindowTitle);

			// ボタン文字列
			SetDlgItemText(hWnd, IDOK, pImageMsgInfo->m_Button1Caption);
			SetDlgItemText(hWnd, IDCANCEL, pImageMsgInfo->m_Button2Caption);

			// メッセージ画像よりウィンドウサイズ設定
			g_pMsgImage = Image::FromFile(pImageMsgInfo->pwDataFileName);

			UINT uiWidth = g_pMsgImage->GetWidth();		// 画像の幅
			UINT uiHeight = g_pMsgImage->GetHeight();	// 画像の高さ
			// 画像の高さの1.5倍 + メニュー高 + タイトル高
			UINT uiHeightx = uiHeight + uiHeight / 2 + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYMENUSIZE);

			SetWindowPos(hWnd, HWND_TOP,
							g_rectDispWin.left + (g_rectDispWin.right - g_rectDispWin.left - uiWidth) / 2,
							g_rectDispWin.top + (g_rectDispWin.bottom - g_rectDispWin.top - uiHeightx) / 2,
							uiWidth,
							uiHeightx,
							SWP_SHOWWINDOW);

			CLog.WriteLog("\r\n");
			sprintf(strMessage, "Message Window"); CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
			sprintf(strMessage, "X = %d, ", g_rectDispWin.left + (g_rectDispWin.right - g_rectDispWin.left - uiWidth) / 2); CLog.WriteLog(strMessage);
			sprintf(strMessage, "Y = %d, ", g_rectDispWin.top + (g_rectDispWin.bottom - g_rectDispWin.top - uiHeightx) / 2); CLog.WriteLog(strMessage);
			sprintf(strMessage, "Width = %d, ", uiWidth); CLog.WriteLog(strMessage);
			sprintf(strMessage, "Height = %d", uiHeightx); CLog.WriteLog(strMessage);
			CLog.WriteLog("\r\n");

			// ボタンサイズ、ボタンフォント設定
			UINT uiBtnWidth = uiWidth / 4;
			UINT uiBtnHeight = uiHeight / 4;

			HWND hButton = GetDlgItem(hWnd, IDOK);
			MoveWindow(hButton, uiWidth / 4 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 2, uiBtnWidth, uiBtnHeight, TRUE);

				// マウスカーソル移動
				RECT btnRect;
				GetWindowRect(hButton, &btnRect);
				// ボタンの中心へ移動
				SetCursorPos(btnRect.left + (btnRect.right - btnRect.left) / 2, btnRect.top + (btnRect.bottom - btnRect.top) / 2); 

			g_ButtonFont = (HFONT)SendMessage(hButton, WM_GETFONT, 0, 0);
			GetObject(g_ButtonFont, sizeof(LOGFONT), &g_LogFont);
			g_LogFont.lfHeight = pImageMsgInfo->m_ButtonFontSize;
			g_ButtonFont = CreateFontIndirect(&g_LogFont);
			SendDlgItemMessage(hWnd, IDOK, WM_SETFONT, (WPARAM)g_ButtonFont, 0);

			hButton = GetDlgItem(hWnd, IDCANCEL);
			MoveWindow(hButton, uiWidth / 4 * 3 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 2, uiBtnWidth, uiBtnHeight, TRUE);

			SendDlgItemMessage(hWnd, IDCANCEL, WM_SETFONT, (WPARAM)g_ButtonFont, 0);
		}
		break;
		
	//ユーザーメッセージ処理
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			g_nExitCode = IDOK;
			DestroyWindow(hWnd);
			break;

		case IDCANCEL:
			g_nExitCode = IDCANCEL;
			DestroyWindow(hWnd);
			break;
		
		default:
			return DefWindowProc(hWnd, uMessage, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		{
			Graphics MyGraphics(hdc);
			MyGraphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

			MyGraphics.DrawImage(g_pMsgImage, 0, 0);
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
		PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
		break;
	
	case WM_DESTROY:
		DeleteObject(g_ButtonFont);
		if (pImageMsgInfo->pwDataFileName)
			free(pImageMsgInfo->pwDataFileName);

		PostQuitMessage(g_nExitCode);
		break;

	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	} /* switch */
	
	return 0;
}

#else

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// グローバル変数
GdiplusStartupInput gdiSI;
ULONG_PTR gdiToken;

// CImageMsgBoxDlg dialog
CImageMsgBoxDlg::CImageMsgBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageMsgBoxDlg::IDD, pParent)
{
}

void CImageMsgBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC2, m_csMsg1);
	DDX_Control(pDX, IDC_STATIC3, m_csMsg2);
	DDX_Control(pDX, IDC_STATIC4, m_csMsg3);
}

BEGIN_MESSAGE_MAP(CImageMsgBoxDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CImageMsgBoxDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CImageMsgBoxDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RETRY, &CImageMsgBoxDlg::OnBnClickedRetry)

	ON_WM_TIMER()
END_MESSAGE_MAP()

// CImageMsgBoxDlg message handlers

BOOL CImageMsgBoxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// GDI+ Start
	GdiplusStartup(&gdiToken, &gdiSI, NULL);

	// ウィンドウタイトル
	SetWindowText(m_MsgWindowTitle);

	// ボタン文字列
	SetDlgItemText(IDOK, m_Button1Caption);
	SetDlgItemText(IDCANCEL, m_Button2Caption);
	SetDlgItemText(IDC_RETRY, m_Button3Caption);

//-->v0004
//	// メッセージ画像よりウィンドウサイズ設定
//	m_pImage = Image::FromFile(pwDataFileName);
//
//	uiWidth = m_pImage->GetWidth();		// 画像の幅
//	uiHeight = m_pImage->GetHeight();	// 画像の高さ
//<--v0004
//++>v0004
	// モードにより処理を分岐
	if(m_bDisplayImageFlag) {
		// メッセージ画像よりウィンドウサイズ設定
		m_pImage = Image::FromFile(pwDataFileName);

		uiWidth = m_pImage->GetWidth();		// 画像の幅
		uiHeight = m_pImage->GetHeight();	// 画像の高さ

	}else{
		// 何個のスタティックボックスを使うのかをチェック(歯抜けは禁止。1行目は空で2行目のみ出力等)
		if(m_csDisplayMsg3.GetLength() > 0){
			// 3個
			m_iUseMsgCount = 3;
		}else if(m_csDisplayMsg2.GetLength() > 0) {
			// 2個
			m_iUseMsgCount = 2;
		}else{
			// 1個
			m_iUseMsgCount = 1;
		}

		// 設定は共通して使う。
		CFont *pcFontSt1 = m_csMsg1.GetFont();
		CRect rect;
		GetClientRect(&rect);

		// 表示するスタティックボックスの設定(個数順序に設定していく)
		if( m_iUseMsgCount > 0){
			// 1行目のスタティックボックス設定
			// アライメント設定の為に、削除。
			m_csMsg1.DestroyWindow();
			// アライメント設定
			UINT AlignText1;
			if(m_iDisplayMsg1Alignment == 2) {
				AlignText1 = SS_RIGHT;
			}else if(m_iDisplayMsg1Alignment == 1){
				AlignText1 = SS_LEFT;
			}else{
				AlignText1 = SS_CENTER;
			}
			// 新規にクリエイトする
			m_csMsg1.Create(_T(""), WS_VISIBLE | AlignText1, rect, this);
			// テキスト設定
			m_csMsg1.SetWindowText(m_csDisplayMsg1);
			// フォント設定
			m_csMsg1.SetFont(pcFontSt1, TRUE);
			// コントロールサイズ、位置設定 (Right設定のとき右側が表示切れするので、5補正する。)
			m_csMsg1.MoveWindow( 0, 0, m_iDisplayMsgWidth - 5, m_iDisplayMsgHeight );
		}
		if( m_iUseMsgCount > 1){
			// 2行目のスタティックボックス設定
			// アライメント設定の為に、削除。
			m_csMsg2.DestroyWindow();
			// アライメント設定
			UINT AlignText2;
			if(m_iDisplayMsg2Alignment == 2) {
				AlignText2 = SS_RIGHT;
			}else if(m_iDisplayMsg2Alignment == 1){
				AlignText2 = SS_LEFT;
			}else{
				AlignText2 = SS_CENTER;
			}
			// 新規にクリエイトする
			m_csMsg2.Create(_T(""), WS_VISIBLE | AlignText2, rect, this);
			// テキスト設定
			m_csMsg2.SetWindowText(m_csDisplayMsg2);
			// フォント設定
			m_csMsg2.SetFont(pcFontSt1, TRUE);
			// コントロールサイズ、位置設定 (Right設定のとき右側が表示切れするので、5補正する。)
			m_csMsg2.MoveWindow( 0, m_iDisplayMsgHeight, m_iDisplayMsgWidth - 5, m_iDisplayMsgHeight );
		}
		if( m_iUseMsgCount > 2){
			// 3行目のスタティックボックス設定
			// アライメント設定の為に、削除。
			m_csMsg3.DestroyWindow();
			// アライメント設定
			UINT AlignText3;
			if(m_iDisplayMsg3Alignment == 2) {
				AlignText3 = SS_RIGHT;
			}else if(m_iDisplayMsg3Alignment == 1){
				AlignText3 = SS_LEFT;
			}else{
				AlignText3 = SS_CENTER;
			}
			// 新規にクリエイトする
			m_csMsg3.Create(_T(""), WS_VISIBLE | AlignText3, rect, this);
			// テキスト設定
			m_csMsg3.SetWindowText(m_csDisplayMsg3);
			// フォント設定
			m_csMsg3.SetFont(pcFontSt1, TRUE);
			// コントロールサイズ、位置設定 (Right設定のとき右側が表示切れするので、5補正する。)
			m_csMsg3.MoveWindow( 0, m_iDisplayMsgHeight * 2, m_iDisplayMsgWidth - 5, m_iDisplayMsgHeight );
		}

		// ダイアログサイズ設定用に、スタティックボックスの高さと横幅を取得する。
		uiHeight = m_iDisplayMsgHeight * 3;
		uiWidth = m_iDisplayMsgWidth;
	}
//<++v0004

//++>v0002
	int iStatic_TimeOut_Height = 0;
	int iStatic_TimeOut_Width = 0;
	if( m_bTimeOutSetFlag ){
		CRect rect_st;
		((CStatic *)GetDlgItem(IDC_STATIC1))->GetWindowRect( rect_st );
		iStatic_TimeOut_Height = rect_st.Height();
		iStatic_TimeOut_Width = rect_st.Width();
		//*
		CString CSTimeOutIngerval = "";
		CSTimeOutIngerval.Format( "%d", m_iTimeOutInterval );
		SetDlgItemText( IDC_STATIC1, CSTimeOutIngerval );
	}
	else{
		((CStatic *)GetDlgItem(IDC_STATIC1))->ModifyStyle( WS_VISIBLE, NULL, 0 );
	}
//<++v0002

	// 画像の高さの1.5倍 + メニュー高 + タイトル高
	UINT uiHeightx = uiHeight + uiHeight / 2 + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYMENUSIZE);
	// 画像の高さの + (ﾎﾞﾀﾝの高さ x 2)  + メニュー高 + タイトル高
//	UINT uiHeightx = uiHeight + ((uiHeight / 5)+(((uiHeight / 5)/2))) + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYMENUSIZE);

	uiHeightx += iStatic_TimeOut_Height + (iStatic_TimeOut_Height/2);	//++>v0002

//++>v0004
	// メッセージ表示時は、スタティックボックスの個数により、ダイアログの高さに補正を行う
	if(!m_bDisplayImageFlag) {
		uiHeightx -= m_iDisplayMsgHeight * (3 - m_iUseMsgCount);
	}
//<++v0004

	SetWindowPos(&CWnd::wndTop,
					m_prectDispWin->left + (m_prectDispWin->right - m_prectDispWin->left - uiWidth) / 2,
					m_prectDispWin->top + (m_prectDispWin->bottom - m_prectDispWin->top - uiHeightx) / 2,
					uiWidth,
					uiHeightx,
					SWP_SHOWWINDOW);

//++>v0004
	// サイズ補正
	if(!m_bDisplayImageFlag) {
		uiHeight = m_iDisplayMsgHeight * m_iUseMsgCount;
	}
//<++v0004

//++>v0002
	// TimeOut Staticの設定
	if( m_bTimeOutSetFlag ){
		CWnd *hStatic = GetDlgItem(IDC_STATIC1);
		hStatic->MoveWindow( (uiWidth / 2 - iStatic_TimeOut_Width / 2), uiHeight + (iStatic_TimeOut_Height / 2), 
							 iStatic_TimeOut_Width, iStatic_TimeOut_Height );
	}
//<++v0002

	// ボタンサイズ、ボタンフォント設定
	UINT uiBtnWidth = uiWidth / 4;
//	UINT uiBtnHeight = uiHeight / 3;							//--v0004
//++>v0004
	UINT uiBtnHeight = 0;
	// モードにより処理を分岐
	if(m_bDisplayImageFlag) {
		uiBtnHeight = uiHeight / 3;
	}else{
		// スタティックの使用行数により補正をかける
		uiBtnHeight = uiHeight / m_iUseMsgCount;
	}
//<++v0004

	if( m_bButton1 && m_bButton2 && m_bButton3 ){
		if( m_bTimeOutSetFlag ){
			CWnd *hButton = GetDlgItem(IDOK);
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDCANCEL);
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2, 
								(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
								uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDC_RETRY);
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);
		}
		else{
			CWnd *hButton = GetDlgItem(IDOK);
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDCANCEL);
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDC_RETRY);
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}
	}
	else if( m_bButton1 && !m_bButton2 && !m_bButton3 ){
		CWnd *hButton = GetDlgItem(IDOK);
		if( m_bTimeOutSetFlag ){
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2,
								(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
								uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}

		hButton = GetDlgItem(IDCANCEL);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );

		hButton = GetDlgItem(IDC_RETRY);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );
	}
	else if( !m_bButton1 && m_bButton2 && m_bButton3 ){
		CWnd *hButton = GetDlgItem(IDOK);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );

		if( m_bTimeOutSetFlag ){
			hButton = GetDlgItem(IDCANCEL);
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDC_RETRY);
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton = GetDlgItem(IDCANCEL);
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDC_RETRY);
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}
	}
	else if( !m_bButton1 && m_bButton2 && !m_bButton3 ){
		CWnd *hButton = GetDlgItem(IDOK);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );

		hButton = GetDlgItem(IDCANCEL);
		if( m_bTimeOutSetFlag ){
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2, 
								(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
								uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}

		hButton = GetDlgItem(IDC_RETRY);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );
	}
	else if( !m_bButton1 && !m_bButton2 && m_bButton3 ){
		CWnd *hButton = GetDlgItem(IDOK);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );

		hButton = GetDlgItem(IDCANCEL);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );

		hButton = GetDlgItem(IDC_RETRY);
		if( m_bTimeOutSetFlag ){
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2,
								(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
								uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton->MoveWindow(uiWidth / 2 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}
	}
	else if( m_bButton1 && m_bButton2 && !m_bButton3 ){
		CWnd *hButton = GetDlgItem(IDOK);
		if( m_bTimeOutSetFlag ){
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDCANCEL);
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);

			hButton = GetDlgItem(IDCANCEL);
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}

		hButton = GetDlgItem(IDC_RETRY);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );
	}
	else if( m_bButton1 && !m_bButton2 && m_bButton3 ){			//++>v0002
		CWnd *hButton = GetDlgItem(IDOK);
		if( m_bTimeOutSetFlag ){
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton->MoveWindow((uiWidth / 2 - uiBtnWidth / 2) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}

		hButton = GetDlgItem(IDCANCEL);
		hButton->ModifyStyle( WS_VISIBLE, NULL, 0 );

		hButton = GetDlgItem(IDC_RETRY);
		if( m_bTimeOutSetFlag ){
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									(uiHeight + uiBtnHeight / 3) + iStatic_TimeOut_Height + ( iStatic_TimeOut_Height / 2 ),
									uiBtnWidth, uiBtnHeight);
		}
		else{
			hButton->MoveWindow((uiWidth / 2 + uiBtnWidth / 2) + (uiWidth - (uiWidth / 2 + uiBtnWidth / 2)) / 2 - uiBtnWidth / 2,
									uiHeight + uiBtnHeight / 3, uiBtnWidth, uiBtnHeight);
		}
	}

	// ボタンフォントサイズ設定
	CFont *pcFont = ((CButton *)GetDlgItem(IDOK))->GetFont();
	LOGFONT lf;
	pcFont->GetLogFont(&lf);
	lf.lfHeight = m_ButtonFontSize;
	if( m_ButtonFontBold ){
		lf.lfWeight = FW_BOLD;
	}
	else{
		lf.lfWeight = FW_NORMAL;
	}

	cFont.CreateFontIndirect(&lf);
	((CButton *)GetDlgItem(IDOK))->SetFont(&cFont, TRUE);
	((CButton *)GetDlgItem(IDCANCEL))->SetFont(&cFont, TRUE);
	((CButton *)GetDlgItem(IDC_RETRY))->SetFont(&cFont, TRUE);

//++>v0004
	// スタティックボックスフォント設定
	if(!m_bDisplayImageFlag) {
		CFont *pcFontSt = m_csMsg1.GetFont();	// 設定は共通
		LOGFONT lfst;
		pcFontSt->GetLogFont(&lfst);
		lfst.lfHeight = m_iDisplayMsgFontSize;
		if( m_iDisplayMsgBold ){
			lfst.lfWeight = FW_BOLD;
		}
		else{
			lfst.lfWeight = FW_NORMAL;
		}

		cFontst.CreateFontIndirect(&lfst);
		if(m_iUseMsgCount > 0) {
			// 1行目のスタティックボックスに設定
			m_csMsg1.SetFont(&cFontst, TRUE);
		}
		if(m_iUseMsgCount > 1) {
			// 2行目のスタティックボックスに設定
			m_csMsg2.SetFont(&cFontst, TRUE);
		}
		if(m_iUseMsgCount > 2) {
			// 3行目のスタティックボックスに設定
			m_csMsg3.SetFont(&cFontst, TRUE);
		}
	}
//<++v0004

	// ボタンマウスカーソル設定
	Button_MouseCorsulSet();		//++>v0003
/*
-->v0003
	CRect rect;
	if( m_iButtonCursolSetNo == 1 ){
		((CButton *)GetDlgItem(IDOK))->GetWindowRect( rect );
	}
	else if( m_iButtonCursolSetNo == 2 ){
		((CButton *)GetDlgItem(IDCANCEL))->GetWindowRect( rect );
	}
	else if( m_iButtonCursolSetNo == 3 ){
		((CButton *)GetDlgItem(IDC_RETRY))->GetWindowRect( rect );
	}
	else{
		((CButton *)GetDlgItem(IDOK))->GetWindowRect( rect );
	}

	int XX = (INT)rect.right - (INT)rect.left;
	int X = XX / 2;
	int YY = (INT)rect.bottom - (INT)rect.top;
	int Y = YY / 2;
	//*
	X += (INT)rect.left;
	Y += (INT)rect.top;

	SetCursorPos( X, Y );
<--v0003
*/
	Invalidate();	
	UpdateData( TRUE );

	if( m_bTimeOutSetFlag ) SetTimer( T_TIMEOUT_SET, 0, NULL );			//++>v0002
/*
	if (m_bButton1 == FALSE)
		((CButton *)GetDlgItem(IDOK))->EnableWindow(FALSE);
	if (m_bButton2 == FALSE)
		((CButton *)GetDlgItem(IDCANCEL))->EnableWindow(FALSE);
	if (m_bButton3 == FALSE)
		((CButton *)GetDlgItem(IDC_RETRY))->EnableWindow(FALSE);
*/

//++>v0004
	// ボタンへのフォーカス設定
	Button_FocusSet();
//<++v0004

//	return TRUE;  // return TRUE  unless you set the focus to a control	//--v0004
	return FALSE;	//++v0004 ボタンへのフォーカス設定を行っているので、FALSEを返す。
}

void CImageMsgBoxDlg::OnPaint()
{
	PAINTSTRUCT ps;
	CDC *pDC = BeginPaint(&ps);

	Graphics MyGraphics(this->m_hWnd);
	MyGraphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

//	MyGraphics.DrawImage(m_pImage, 0, 0);//--v0004
//++>v0004
	if(m_bDisplayImageFlag){
		MyGraphics.DrawImage(m_pImage, 0, 0);
	}
//<++v0004

	EndPaint(&ps);
}

void CImageMsgBoxDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// GDI+ End
	GdiplusShutdown(gdiToken);

//	if (pwDataFileName)
//		free(pwDataFileName);

	cFont.DeleteObject();
	cFontst.DeleteObject();					//++>v0004
}

void CImageMsgBoxDlg::OnBnClickedOk()
{
//	EndDialog(IDOK);
	KillTimer( T_TIMEOUT_SET );				//++>v0002
	EndDialog(m_iButton1ReturnVal);
}

void CImageMsgBoxDlg::OnBnClickedCancel()
{
//	EndDialog(IDCANCEL);
	KillTimer( T_TIMEOUT_SET );				//++>v0002
	EndDialog(m_iButton2ReturnVal);
}

void CImageMsgBoxDlg::OnBnClickedRetry()
{
//	EndDialog(IDC_RETRY);
	KillTimer( T_TIMEOUT_SET );				//++>v0002
	EndDialog(m_iButton3ReturnVal);
}
#endif

//++>v0002
void CImageMsgBoxDlg::OnTimer(UINT nIDEvent)
{
	// TODO : ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	switch( nIDEvent ){
		case T_TIMEOUT_SET:
			{
				KillTimer( nIDEvent );
				if( m_iTimeOutInterval > 0 ){
					CString CSTimeOutInterval = "";
					CSTimeOutInterval.Format( "%d", m_iTimeOutInterval );
					SetDlgItemText( IDC_STATIC1, CSTimeOutInterval );
					m_iTimeOutInterval--;
					SetTimer( T_TIMEOUT_SET, 1000, NULL );
				}
				else{
					EndDialog(m_iTimeOutReturnValue);
				}
				break;
			}
		case T_MOUSE_CORSOL_SET:			//++>v0003
			{
				KillTimer( nIDEvent );
				Button_MouseCorsulSet();
				break;
			}
	}

	CDialog::OnTimer(nIDEvent);
}
//<++v0002

//++>v0003
LRESULT CImageMsgBoxDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO : ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	switch( message ){
		case WM_DISPLAYCHANGE:
			{
				// 画面解像度変更時に動作する。
				if( m_bButtonMouseCursolDspChgSetFlag ){
					SetTimer( T_MOUSE_CORSOL_SET, 10, NULL );
				}
				break;
			}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CImageMsgBoxDlg::Button_MouseCorsulSet()
{
	// ボタンマウスカーソル設定
	CRect rect;
	if( m_iButtonCursolSetNo == 1 ){
		((CButton *)GetDlgItem(IDOK))->GetWindowRect( rect );
	}
	else if( m_iButtonCursolSetNo == 2 ){
		((CButton *)GetDlgItem(IDCANCEL))->GetWindowRect( rect );
	}
	else if( m_iButtonCursolSetNo == 3 ){
		((CButton *)GetDlgItem(IDC_RETRY))->GetWindowRect( rect );
	}
	else{
		((CButton *)GetDlgItem(IDOK))->GetWindowRect( rect );
	}

	int XX = (INT)rect.right - (INT)rect.left;
	int X = XX / 2;
	int YY = (INT)rect.bottom - (INT)rect.top;
	int Y = YY / 2;
	//*
	X += (INT)rect.left;
	Y += (INT)rect.top;

	SetCursorPos( X, Y );
}
//<++v0003

//++>v0004
void CImageMsgBoxDlg::Button_FocusSet()
{
	// ボタンマウスカーソル設定
	if( m_iButtonCursolSetNo == 1 ){
		GotoDlgCtrl(GetDlgItem(IDOK));
	}
	else if( m_iButtonCursolSetNo == 2 ){
		GotoDlgCtrl(GetDlgItem(IDCANCEL));
	}
	else if( m_iButtonCursolSetNo == 3 ){
		GotoDlgCtrl(GetDlgItem(IDC_RETRY));
	}
	else{
		GotoDlgCtrl(GetDlgItem(IDOK));
	}
}
//<++v0004
