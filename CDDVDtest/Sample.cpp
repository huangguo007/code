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
	int iMsgDialogMode = 0;	//++v0004	(0:�摜�\������̃_�C�A���O�B1:���b�Z�[�W�\���̃_�C�A���O�B2:�W�����b�Z�[�W�{�b�N�X)

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
		// �摜�\���_�C�A���O���g�p���郂�[�h�B
		iMsgDialogMode = 0;
		// �摜����t���O
		str_MsgItem.m_bImageFlag = true;
	}else{
		// �W���I��
		memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
		GetPrivateProfileString( chBufMsgSectionName, "MsgBoxUsgMessageDialogFlag", "On", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
		if( !strcmpi( chBuf_Work, "On" )){
			// ���b�Z�[�W�\���_�C�A���O���g�p���郂�[�h�B
			iMsgDialogMode = 1;

			// �摜�Ȃ��t���O
			str_MsgItem.m_bImageFlag = false;
			
			// 1�s�ڕ\�����b�Z�[�W
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg1", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			str_MsgItem.m_DisplayMsg1.Format( "%s", chBuf_Work );
			
			// 2�s�ڕ\�����b�Z�[�W
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg2", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			str_MsgItem.m_DisplayMsg2.Format( "%s", chBuf_Work );
			
			// 3�s�ڕ\�����b�Z�[�W
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg3", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			str_MsgItem.m_DisplayMsg3.Format( "%s", chBuf_Work );

			// 1�s�ڕ\�����b�Z�[�W�̃A���C�����g�ݒ�(0:Center(�f�t�H���g), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg1Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "Right" )){
				str_MsgItem.m_DisplayMsg1Alignment = 2;
			} else if ( !strcmpi( chBuf_Work, "Left" )){
				str_MsgItem.m_DisplayMsg1Alignment = 1;
			} else{
				str_MsgItem.m_DisplayMsg1Alignment = 0;
			}

			// 2�s�ڕ\�����b�Z�[�W�̃A���C�����g�ݒ�(0:Center(�f�t�H���g), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg2Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "Right" )){
				str_MsgItem.m_DisplayMsg2Alignment = 2;
			} else if ( !strcmpi( chBuf_Work, "Left" )){
				str_MsgItem.m_DisplayMsg2Alignment = 1;
			} else{
				str_MsgItem.m_DisplayMsg2Alignment = 0;
			}

			// 3�s�ڕ\�����b�Z�[�W�̃A���C�����g�ݒ�(0:Center(�f�t�H���g), 1:Left, 2:Right)
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsg3Alignment", "Center", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "Right" )){
				str_MsgItem.m_DisplayMsg3Alignment = 2;
			} else if ( !strcmpi( chBuf_Work, "Left" )){
				str_MsgItem.m_DisplayMsg3Alignment = 1;
			} else{
				str_MsgItem.m_DisplayMsg3Alignment = 0;
			}
			
			// ���b�Z�[�W�\�����̃X�^�e�B�b�N�{�b�N�X�̍���
			str_MsgItem.m_iDisplayMsgHeight = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgHeight", 30, IniFilePath );

			// ���b�Z�[�W�\�����̃X�^�e�B�b�N�{�b�N�X�̕�
			str_MsgItem.m_iDisplayMsgWidth = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgWidth", 700, IniFilePath );
			
			// ���b�Z�[�W�\�����̃X�^�e�B�b�N�{�b�N�X�̃t�H���g�T�C�Y
			str_MsgItem.m_iDisplayMsgFontSize = GetPrivateProfileInt( chBufMsgSectionName, "MsgBoxDisplayMsgFontSize", 23, IniFilePath );

			// ���b�Z�[�W�\�����̃X�^�e�B�b�N�{�b�N�X�̃t�H���g�{�[���h
			memset( chBuf_Work, NULL, sizeof( chBuf_Work ));
			GetPrivateProfileString( chBufMsgSectionName, "MsgBoxDisplayMsgBold", "", chBuf_Work, sizeof( chBuf_Work ), IniFilePath );
			if( !strcmpi( chBuf_Work, "On" )){
				str_MsgItem.m_iDisplayMsgBold = true;
			}
			else{
				str_MsgItem.m_iDisplayMsgBold = false;
			}
			
		}else{
			// �W�����b�Z�[�W�{�b�N�X���g�p���郂�[�h�B
			iMsgDialogMode = 2;
		}
	}

	// ���[�h�ɉ����ď������s���B
	switch(iMsgDialogMode) {
	case 0:	// �摜�L�B
		iResult = Msg_Dlg_Display( &str_MsgItem );
		break;
	case 1:	// �摜���B���b�Z�[�W�_�C�A���O�g�p�B
		iResult = Msg_Dlg_Display( &str_MsgItem );
		break;
	case 2:	// �摜���B���b�Z�[�W�_�C�A���O���g�p�B
		// �^�C���A�E�g�ݒ肪����ꍇ�́ASleep���s���̂݁B�^�C���A�E�g�ݒ肪�����ꍇ�́A���b�Z�[�W�{�b�N�X��\������B
		if(str_MsgItem.m_bTimeOutSetFlag){
			Sleep(str_MsgItem.m_iTimeOutInterval * 1000);
			// �����OK�������ꂽ�l��ݒ肷��B�_�C�A���O�̌Ăяo�����ŁA�߂�l�ɂ�鏈���̕�����s���ꍇ�́A�v�C���B
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
		// �摜�����݂���ꍇ�́A�摜��\������B
		pMessageDlg->m_bDisplayImageFlag = true;

		int len = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,mSts->m_ImageFilePath,-1,NULL,0);
		pwDataFileName = new wchar_t[len + 1]; 
		MultiByteToWideChar( CP_ACP,				// ANSI code page
							MB_PRECOMPOSED,		// ����̕ϊ����@
							mSts->m_ImageFilePath,    // �ϊ���������
							-1,					// �ϊ���������T�C�Y(-1�w��Ŏ����v�Z)
							pwDataFileName,        // �ϊ���o�b�t�@
							len+1);				// �ϊ���o�b�t�@�T�C�Y
		pMessageDlg->pwDataFileName = pwDataFileName;
	}else{
		// �摜�����݂���ꍇ�́A���b�Z�[�W��\������B
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

	// �f�B�X�v���C�����擾���č�Ɨ̈�m��
	m_MonitorMax = GetSystemMetrics(SM_CMONITORS);
	m_rectMonitor = new RECT[m_MonitorMax];

	// �f�B�X�v���C��񋓂����j�^�[�����擾
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)DisplayEnumProc, (LPARAM)&dwData);
}

// ���j�^�[���擾
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

