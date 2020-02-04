
// ImageMsgBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "ImageMsgBoxDlg.h"

#if 0

extern CAllFileControl CLog;
extern char strMessage[256];

const char g_szImageMsgBoxClassName[] = "ImageMsgBoxDlg";		// �E�B���h�E�N���X��
HWND g_hImageMsgBoxDlg;					// �_�C�A���O�n���h��
int g_nExitCode = 0;					// �߂�l
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

	// �E�C���h�E�N���X�̓o�^
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
	
	// ���C���E�C���h�E�̐���
	if ((g_hImageMsgBoxDlg = CreateDialog(g_hImageMsgBoxInst, MAKEINTRESOURCE(IDD_IMAGEMSGBOX_DIALOG), 
											0, (DLGPROC)ImageMsgBoxDlgProc)) == NULL)
		return -1;
	
	// �E�C���h�E�̕\��
	ShowWindow(g_hImageMsgBoxDlg, SW_SHOWNORMAL);
	UpdateWindow(g_hImageMsgBoxDlg);

	// �{�^���A�N�Z�����[�^�[�L�[�̐ݒ�
	HACCEL hAccel;
	ACCEL accel[2];

	accel[0].key = pInfo->m_Button1Caption[0];
	accel[0].cmd = IDOK;
	accel[0].fVirt = FVIRTKEY; // FALT | FCONTROL | FSHIFT

	accel[1].key = pInfo->m_Button2Caption[0];
	accel[1].cmd = IDCANCEL;
	accel[1].fVirt = FVIRTKEY; // FALT | FCONTROL | FSHIFT

	hAccel = CreateAcceleratorTable(accel, 2);

	// ���b�Z�[�W���[�v
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(g_hImageMsgBoxDlg, hAccel, &msg) &&
			!IsDialogMessage(g_hImageMsgBoxDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// �A�N�Z�����[�^�[�L�[�J��
	DestroyAcceleratorTable(hAccel);

	return msg.wParam;
}

LRESULT CALLBACK ImageMsgBoxDlgProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (uMessage) {
	//�E�C���h�E�쐬���̏���
	case WM_INITDIALOG:
		if (pImageMsgInfo != NULL) {
			// �E�B���h�E�^�C�g��
			SetWindowText(hWnd, pImageMsgInfo->m_MsgWindowTitle);

			// �{�^��������
			SetDlgItemText(hWnd, IDOK, pImageMsgInfo->m_Button1Caption);
			SetDlgItemText(hWnd, IDCANCEL, pImageMsgInfo->m_Button2Caption);

			// ���b�Z�[�W�摜���E�B���h�E�T�C�Y�ݒ�
			g_pMsgImage = Image::FromFile(pImageMsgInfo->pwDataFileName);

			UINT uiWidth = g_pMsgImage->GetWidth();		// �摜�̕�
			UINT uiHeight = g_pMsgImage->GetHeight();	// �摜�̍���
			// �摜�̍�����1.5�{ + ���j���[�� + �^�C�g����
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

			// �{�^���T�C�Y�A�{�^���t�H���g�ݒ�
			UINT uiBtnWidth = uiWidth / 4;
			UINT uiBtnHeight = uiHeight / 4;

			HWND hButton = GetDlgItem(hWnd, IDOK);
			MoveWindow(hButton, uiWidth / 4 - uiBtnWidth / 2, uiHeight + uiBtnHeight / 2, uiBtnWidth, uiBtnHeight, TRUE);

				// �}�E�X�J�[�\���ړ�
				RECT btnRect;
				GetWindowRect(hButton, &btnRect);
				// �{�^���̒��S�ֈړ�
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
		
	//���[�U�[���b�Z�[�W����
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

// �O���[�o���ϐ�
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

	// �E�B���h�E�^�C�g��
	SetWindowText(m_MsgWindowTitle);

	// �{�^��������
	SetDlgItemText(IDOK, m_Button1Caption);
	SetDlgItemText(IDCANCEL, m_Button2Caption);
	SetDlgItemText(IDC_RETRY, m_Button3Caption);

//-->v0004
//	// ���b�Z�[�W�摜���E�B���h�E�T�C�Y�ݒ�
//	m_pImage = Image::FromFile(pwDataFileName);
//
//	uiWidth = m_pImage->GetWidth();		// �摜�̕�
//	uiHeight = m_pImage->GetHeight();	// �摜�̍���
//<--v0004
//++>v0004
	// ���[�h�ɂ�菈���𕪊�
	if(m_bDisplayImageFlag) {
		// ���b�Z�[�W�摜���E�B���h�E�T�C�Y�ݒ�
		m_pImage = Image::FromFile(pwDataFileName);

		uiWidth = m_pImage->GetWidth();		// �摜�̕�
		uiHeight = m_pImage->GetHeight();	// �摜�̍���

	}else{
		// ���̃X�^�e�B�b�N�{�b�N�X���g���̂����`�F�b�N(�������͋֎~�B1�s�ڂ͋��2�s�ڂ̂ݏo�͓�)
		if(m_csDisplayMsg3.GetLength() > 0){
			// 3��
			m_iUseMsgCount = 3;
		}else if(m_csDisplayMsg2.GetLength() > 0) {
			// 2��
			m_iUseMsgCount = 2;
		}else{
			// 1��
			m_iUseMsgCount = 1;
		}

		// �ݒ�͋��ʂ��Ďg���B
		CFont *pcFontSt1 = m_csMsg1.GetFont();
		CRect rect;
		GetClientRect(&rect);

		// �\������X�^�e�B�b�N�{�b�N�X�̐ݒ�(�������ɐݒ肵�Ă���)
		if( m_iUseMsgCount > 0){
			// 1�s�ڂ̃X�^�e�B�b�N�{�b�N�X�ݒ�
			// �A���C�����g�ݒ�ׂ̈ɁA�폜�B
			m_csMsg1.DestroyWindow();
			// �A���C�����g�ݒ�
			UINT AlignText1;
			if(m_iDisplayMsg1Alignment == 2) {
				AlignText1 = SS_RIGHT;
			}else if(m_iDisplayMsg1Alignment == 1){
				AlignText1 = SS_LEFT;
			}else{
				AlignText1 = SS_CENTER;
			}
			// �V�K�ɃN���G�C�g����
			m_csMsg1.Create(_T(""), WS_VISIBLE | AlignText1, rect, this);
			// �e�L�X�g�ݒ�
			m_csMsg1.SetWindowText(m_csDisplayMsg1);
			// �t�H���g�ݒ�
			m_csMsg1.SetFont(pcFontSt1, TRUE);
			// �R���g���[���T�C�Y�A�ʒu�ݒ� (Right�ݒ�̂Ƃ��E�����\���؂ꂷ��̂ŁA5�␳����B)
			m_csMsg1.MoveWindow( 0, 0, m_iDisplayMsgWidth - 5, m_iDisplayMsgHeight );
		}
		if( m_iUseMsgCount > 1){
			// 2�s�ڂ̃X�^�e�B�b�N�{�b�N�X�ݒ�
			// �A���C�����g�ݒ�ׂ̈ɁA�폜�B
			m_csMsg2.DestroyWindow();
			// �A���C�����g�ݒ�
			UINT AlignText2;
			if(m_iDisplayMsg2Alignment == 2) {
				AlignText2 = SS_RIGHT;
			}else if(m_iDisplayMsg2Alignment == 1){
				AlignText2 = SS_LEFT;
			}else{
				AlignText2 = SS_CENTER;
			}
			// �V�K�ɃN���G�C�g����
			m_csMsg2.Create(_T(""), WS_VISIBLE | AlignText2, rect, this);
			// �e�L�X�g�ݒ�
			m_csMsg2.SetWindowText(m_csDisplayMsg2);
			// �t�H���g�ݒ�
			m_csMsg2.SetFont(pcFontSt1, TRUE);
			// �R���g���[���T�C�Y�A�ʒu�ݒ� (Right�ݒ�̂Ƃ��E�����\���؂ꂷ��̂ŁA5�␳����B)
			m_csMsg2.MoveWindow( 0, m_iDisplayMsgHeight, m_iDisplayMsgWidth - 5, m_iDisplayMsgHeight );
		}
		if( m_iUseMsgCount > 2){
			// 3�s�ڂ̃X�^�e�B�b�N�{�b�N�X�ݒ�
			// �A���C�����g�ݒ�ׂ̈ɁA�폜�B
			m_csMsg3.DestroyWindow();
			// �A���C�����g�ݒ�
			UINT AlignText3;
			if(m_iDisplayMsg3Alignment == 2) {
				AlignText3 = SS_RIGHT;
			}else if(m_iDisplayMsg3Alignment == 1){
				AlignText3 = SS_LEFT;
			}else{
				AlignText3 = SS_CENTER;
			}
			// �V�K�ɃN���G�C�g����
			m_csMsg3.Create(_T(""), WS_VISIBLE | AlignText3, rect, this);
			// �e�L�X�g�ݒ�
			m_csMsg3.SetWindowText(m_csDisplayMsg3);
			// �t�H���g�ݒ�
			m_csMsg3.SetFont(pcFontSt1, TRUE);
			// �R���g���[���T�C�Y�A�ʒu�ݒ� (Right�ݒ�̂Ƃ��E�����\���؂ꂷ��̂ŁA5�␳����B)
			m_csMsg3.MoveWindow( 0, m_iDisplayMsgHeight * 2, m_iDisplayMsgWidth - 5, m_iDisplayMsgHeight );
		}

		// �_�C�A���O�T�C�Y�ݒ�p�ɁA�X�^�e�B�b�N�{�b�N�X�̍����Ɖ������擾����B
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

	// �摜�̍�����1.5�{ + ���j���[�� + �^�C�g����
	UINT uiHeightx = uiHeight + uiHeight / 2 + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYMENUSIZE);
	// �摜�̍����� + (���݂̍��� x 2)  + ���j���[�� + �^�C�g����
//	UINT uiHeightx = uiHeight + ((uiHeight / 5)+(((uiHeight / 5)/2))) + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYMENUSIZE);

	uiHeightx += iStatic_TimeOut_Height + (iStatic_TimeOut_Height/2);	//++>v0002

//++>v0004
	// ���b�Z�[�W�\�����́A�X�^�e�B�b�N�{�b�N�X�̌��ɂ��A�_�C�A���O�̍����ɕ␳���s��
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
	// �T�C�Y�␳
	if(!m_bDisplayImageFlag) {
		uiHeight = m_iDisplayMsgHeight * m_iUseMsgCount;
	}
//<++v0004

//++>v0002
	// TimeOut Static�̐ݒ�
	if( m_bTimeOutSetFlag ){
		CWnd *hStatic = GetDlgItem(IDC_STATIC1);
		hStatic->MoveWindow( (uiWidth / 2 - iStatic_TimeOut_Width / 2), uiHeight + (iStatic_TimeOut_Height / 2), 
							 iStatic_TimeOut_Width, iStatic_TimeOut_Height );
	}
//<++v0002

	// �{�^���T�C�Y�A�{�^���t�H���g�ݒ�
	UINT uiBtnWidth = uiWidth / 4;
//	UINT uiBtnHeight = uiHeight / 3;							//--v0004
//++>v0004
	UINT uiBtnHeight = 0;
	// ���[�h�ɂ�菈���𕪊�
	if(m_bDisplayImageFlag) {
		uiBtnHeight = uiHeight / 3;
	}else{
		// �X�^�e�B�b�N�̎g�p�s���ɂ��␳��������
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

	// �{�^���t�H���g�T�C�Y�ݒ�
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
	// �X�^�e�B�b�N�{�b�N�X�t�H���g�ݒ�
	if(!m_bDisplayImageFlag) {
		CFont *pcFontSt = m_csMsg1.GetFont();	// �ݒ�͋���
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
			// 1�s�ڂ̃X�^�e�B�b�N�{�b�N�X�ɐݒ�
			m_csMsg1.SetFont(&cFontst, TRUE);
		}
		if(m_iUseMsgCount > 1) {
			// 2�s�ڂ̃X�^�e�B�b�N�{�b�N�X�ɐݒ�
			m_csMsg2.SetFont(&cFontst, TRUE);
		}
		if(m_iUseMsgCount > 2) {
			// 3�s�ڂ̃X�^�e�B�b�N�{�b�N�X�ɐݒ�
			m_csMsg3.SetFont(&cFontst, TRUE);
		}
	}
//<++v0004

	// �{�^���}�E�X�J�[�\���ݒ�
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
	// �{�^���ւ̃t�H�[�J�X�ݒ�
	Button_FocusSet();
//<++v0004

//	return TRUE;  // return TRUE  unless you set the focus to a control	//--v0004
	return FALSE;	//++v0004 �{�^���ւ̃t�H�[�J�X�ݒ���s���Ă���̂ŁAFALSE��Ԃ��B
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
	// TODO : �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
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
	// TODO : �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	switch( message ){
		case WM_DISPLAYCHANGE:
			{
				// ��ʉ𑜓x�ύX���ɓ��삷��B
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
	// �{�^���}�E�X�J�[�\���ݒ�
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
	// �{�^���}�E�X�J�[�\���ݒ�
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
