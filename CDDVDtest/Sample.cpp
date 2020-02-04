int m_MonitorMax = 1;
RECT *m_rectMonitor = NULL;



int CTestUSB30FWVerTool::IniRead_MessageDlg_Section( char* secname )
{
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
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxTitle", "", chMsgBoxTitle, sizeof( chMsgBoxTitle ), IniFilePath );

	// StartMsgBoxTitle Get
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxCaption", "", chMsgBoxCaption, sizeof( chMsgBoxCaption ), IniFilePath );

	// MsgBoxImagePath Get
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxImagePath", "", chMsgBoxImage, sizeof( chMsgBoxImage ), IniFilePath );

	// MsgBoxButton1Caption Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton1Caption", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	str_MsgItem.m_Button1Caption.Format( "%s", chBuf_Work );

	// MsgBoxType Get
	iMsgBoxType = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxType", 0, IniFilePath );

	// MsgBoxButton2Caption Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton2Caption", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	str_MsgItem.m_Button2Caption.Format( "%s", chBuf_Work );

	// MsgBoxButton3Caption Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton3Caption", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	str_MsgItem.m_Button3Caption.Format( "%s", chBuf_Work );

	// MsgBoxButton1Visible Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton1Visible", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	if( !strcmpi( chBuf_Work, "On" )){
		str_MsgItem.m_bButton1 = true;
	}
	else{
		str_MsgItem.m_bButton1 = false;
	}

	// MsgBoxButton2Visible Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton2Visible", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	if( !strcmpi( chBuf_Work, "On" )){
		str_MsgItem.m_bButton2 = true;
	}
	else{
		str_MsgItem.m_bButton2 = false;
	}

	// MsgBoxButton3Visible Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButton3Visible", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	if( !strcmpi( chBuf_Work, "On" )){
		str_MsgItem.m_bButton3 = true;
	}
	else{
		str_MsgItem.m_bButton3 = false;
	}

	// MsgBoxButtonBold Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButtonBold", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	if( !strcmpi( chBuf_Work, "On" )){
		str_MsgItem.m_ButtonFontBold = true;
	}
	else{
		str_MsgItem.m_ButtonFontBold = false;
	}

	// MsgBoxButton1ReturnValue Get
	str_MsgItem.m_iButton1ReturnVal = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButton1ReturnValue", 1, IniFilePath );

	// MsgBoxButton2ReturnValue Get
	str_MsgItem.m_iButton2ReturnVal = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButton2ReturnValue", 1, IniFilePath );

	// MsgBoxButton3ReturnValue Get
	str_MsgItem.m_iButton3ReturnVal = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButton3ReturnValue", 1, IniFilePath );

	// MsgBoxButtonMouseCursolSetNo Get
	str_MsgItem.m_iButtonCursolSetNo = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButtonMouseCursolSetNo", 1, IniFilePath );

	// MsgBoxButtonFontSize Get
	str_MsgItem.m_ButtonFontSize = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxButtonFontSize", 10, IniFilePath );

	// MsgBoxTimeOutSet Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxTimeOutSet", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	if( !strcmpi( chBuf_Work, "On" )){
		str_MsgItem.m_bTimeOutSetFlag = true;
	}
	else{
		str_MsgItem.m_bTimeOutSetFlag = false;
	}

	if( str_MsgItem.m_bTimeOutSetFlag ){
		// MsgBoxTimeOutIntervalTime Get
		str_MsgItem.m_iTimeOutInterval = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxTimeOutIntervalTime", 0, IniFilePath );

		// MsgBoxTimeOutReturnValue Get
		str_MsgItem.m_im_iTimeOutReturnValue = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxTimeOutReturnValue", 0, IniFilePath );
	}

	// MsgBoxButtonMouseCursolDisplayChangeSet Get
	memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
	GetPrivateProfileString( chBufMsgSectionName, "MsgBoxButtonMouseCursolDisplayChangeSet", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
	if( !strcmpi( chBuf_Work, "On" )){
		str_MsgItem.m_bButtonMouseCursolDspChgSetFlag = true;
	}
	else{
		str_MsgItem.m_bButtonMouseCursolDspChgSetFlag = false;
	}
	
	str_MsgItem.m_MsgWindowTitle.Format( "%s", chMsgBoxTitle );
	wsprintf( str_MsgItem.m_ImageFilePath, "%s", chMsgBoxImage );

	// Image File Check
	if( Image_FileCheck( chMsgBoxImage )){
		// 画像表示ダイアログを使用するモード。
		iMsgDialogMode = 0;
		// 画像ありフラグ
		str_MsgItem.m_bImageFlag = true;
	}else{
		// 標準的な
		memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
		GetPrivateProfileString( chBufMsgSectionName, "MsgBoxUsgMessageDialogFlag", "On", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
		if( !strcmpi( chBuf_Work, "On" )){
			// メッセージ表示ダイアログを使用するモード。
			iMsgDialogMode = 1;

			// 画像なしフラグ
			str_MsgItem.m_bImageFlag = false;
			
			// 1行目表示メッセージ
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg1", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			str_MsgItem.m_DisplayMsg1.Format( "%s", chBuf_Work );
			
			// 2行目表示メッセージ
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg2", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			str_MsgItem.m_DisplayMsg2.Format( "%s", chBuf_Work );
			
			// 3行目表示メッセージ
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg3", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			str_MsgItem.m_DisplayMsg3.Format( "%s", chBuf_Work );

			// 1行目表示メッセージのアライメント設定(0:Center(デフォルト), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg1Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "Right" )){
				str_MsgItem.m_DisplayMsg1Alignment = 2;
			} else if ( !strcmpi( chBuf_Work, "Left" )){
				str_MsgItem.m_DisplayMsg1Alignment = 1;
			} else{
				str_MsgItem.m_DisplayMsg1Alignment = 0;
			}

			// 2行目表示メッセージのアライメント設定(0:Center(デフォルト), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg2Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "Right" )){
				str_MsgItem.m_DisplayMsg2Alignment = 2;
			} else if ( !strcmpi( chBuf_Work, "Left" )){
				str_MsgItem.m_DisplayMsg2Alignment = 1;
			} else{
				str_MsgItem.m_DisplayMsg2Alignment = 0;
			}

			// 3行目表示メッセージのアライメント設定(0:Center(デフォルト), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg3Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "Right" )){
				str_MsgItem.m_DisplayMsg3Alignment = 2;
			} else if ( !strcmpi( chBuf_Work, "Left" )){
				str_MsgItem.m_DisplayMsg3Alignment = 1;
			} else{
				str_MsgItem.m_DisplayMsg3Alignment = 0;
			}
			
			// メッセージ表示時のスタティックボックスの高さ
			str_MsgItem.m_iDisplayMsgHeight = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgHeight", 30, IniFilePath );

			// メッセージ表示時のスタティックボックスの幅
			str_MsgItem.m_iDisplayMsgWidth = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgWidth", 700, IniFilePath );
			
			// メッセージ表示時のスタティックボックスのフォントサイズ
			str_MsgItem.m_iDisplayMsgFontSize = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgFontSize", 23, IniFilePath );

			// メッセージ表示時のスタティックボックスのフォントボールド
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsgBold", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "On" )){
				str_MsgItem.m_iDisplayMsgBold = true;
			}
			else{
				str_MsgItem.m_iDisplayMsgBold = false;
			}
			
		}else{
			// 標準メッセージボックスを使用するモード。
			iMsgDialogMode = 2;
		}
	}

	// モードに応じて処理を行う。
	switch(iMsgDialogMode) {
	case 0:	// 画像有。
		iResult = Msg_Dlg_Display( &str_MsgItem );
		break;
	case 1:	// 画像無。メッセージダイアログ使用。
		iResult = Msg_Dlg_Display( &str_MsgItem );
		break;
	case 2:	// 画像無。メッセージダイアログ未使用。
		// タイムアウト設定がある場合は、Sleepを行うのみ。タイムアウト設定が無い場合は、メッセージボックスを表示する。
		if(str_MsgItem.m_bTimeOutSetFlag){
			Sleep(str_MsgItem.m_iTimeOutInterval * 1000);
			// 現状はOKが押された値を設定する。ダイアログの呼び出し元で、戻り値による処理の分岐を行う場合は、要修正。
			iResult = IDOK;
		}else{
			iResult = ::MessageBox( theApp.GetMainWnd()->GetSafeHwnd(), chMsgBoxCaption, chMsgBoxTitle, iMsgBoxType );
		}
		break;
	default:
		break;
	}

	return( iResult );
}

int CTestUSB30FWVerTool::Msg_Dlg_Display( MSGDLGSTATUS* mSts )
{
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
	if(mSts->m_bImageFlag) {
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
	}else{
		// 画像が存在する場合は、メッセージを表示する。
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

	if(pwDataFileName != NULL) {
		delete pwDataFileName;
	}
	delete pMessageDlg;

	return( (INT)nResponse );
}

void CTestUSB30FWVerTool::Display_Setting()
{
	DWORD dwData = 0;

	// ディスプレイ数を取得して作業領域確保
	m_MonitorMax = GetSystemMetrics(SM_CMONITORS);
	m_rectMonitor = new RECT[m_MonitorMax];

	// ディスプレイを列挙しモニター情報を取得
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)DisplayEnumProc, (LPARAM)&dwData);
}

// モニター情報取得
BOOL CTestUSB30FWVerTool::DisplayEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData)
{
	char strMessage[256];
	int &iIndex = *(int *)dwData;

	CLog.WriteLogSection( "Display Detection" );

	MONITORINFOEX minfo;
	minfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMon, &minfo);

	sprintf(strMessage, "DisplayEnumProc No(%d). = %s", iIndex + 1, minfo.szDevice); CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");
	sprintf(strMessage, "Left = %d, ", lpMon->left); CLog.WriteLog(strMessage);
	sprintf(strMessage, "Top = %d, ", lpMon->top); CLog.WriteLog(strMessage);
	sprintf(strMessage, "Right = %d, ", lpMon->right); CLog.WriteLog(strMessage);
	sprintf(strMessage, "Bottom = %d", lpMon->bottom); CLog.WriteLog(strMessage); CLog.WriteLog("\r\n");

	CHAR *p = minfo.szDevice + strlen("\\\\.\\DISPLAY");
	if (p != NULL) {
		int i = -1;
		sscanf(p, "%d", &i);
		if (i < 1 || i >  m_MonitorMax) {
			sprintf(strMessage, "Invalid device name."); CLog.WriteLog(strMessage); CLog.WriteLog("\r\n\r\n");
		}
		else {
			i--;
			m_rectMonitor[i].bottom = lpMon->bottom;
			m_rectMonitor[i].left = lpMon->left;
			m_rectMonitor[i].right = lpMon->right;
			m_rectMonitor[i].top = lpMon->top;
		}
	}

	iIndex++;

	return TRUE;
}

bool CTestUSB30FWVerTool::Image_FileCheck( char* file )
{
	bool bResult = TRUE;
	if( !::PathFileExists( file ) ) bResult = FALSE;
	return( bResult );
}

