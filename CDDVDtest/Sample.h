

typedef struct {
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


class CTestUSB30FWVerTool : public CTestTools{

private:
	int IniRead_MessageDlg_Section( char* secname );
	int Msg_Dlg_Display( MSGDLGSTATUS* mSts );
	void Display_Setting();
	static BOOL DisplayEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lpMon, LPARAM dwData);
	bool Image_FileCheck( char* file );

private:
	int m_SelectMonitor;
	//*
	char IniFilePath[MAX_PATH];

};