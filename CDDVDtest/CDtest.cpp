// CDtest.cpp : �A�v���P�[�V�����p�N���X�̒�`���s���܂��B
//

#include "stdafx.h"
#include "CDtest.h"
#include "CDtestDlg.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp

BEGIN_MESSAGE_MAP(CCDtestApp, CWinApp)
	//{{AFX_MSG_MAP(CCDtestApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp �N���X�̍\�z

CCDtestApp::CCDtestApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
	m_bProgramStatus = FALSE;
	m_iReturnValue = ERROR_FAIL_END;
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CCDtestApp �I�u�W�F�N�g

CCDtestApp theApp;

CAllFileControl CLog;

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp �N���X�̏�����

BOOL CCDtestApp::InitInstance()
{
	AfxEnableControlContainer();

	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������������
	//  ��Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ��̂��폜����
	//  ���������B
/******************
#ifdef _AFXDLL
	Enable3dControls();			// ���L DLL ���� MFC ���g���ꍇ�͂������R�[�����Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N����ꍇ�͂������R�[�����Ă��������B
#endif
*******************/

	CCDtestDlg		dlg;
	BOOL			bResult;

	//-----------------------------------------
	// �J���p�E���O�̏�����
	//-----------------------------------------
	LOG_START();
	LOG_OUT("(S)\n");

	//-----------------------------------------
	// �A�v���P�[�V�������̎擾
	//-----------------------------------------
	bResult = GetAppInfo();
	if ( bResult == FALSE )
	{
		LOG_OUT("GetAppInfo() failed.\n");
		LOG_OUT("Failure, Acquisition of application informationd.\n");

		goto END_InitInstance;
	}

	//-----------------------------------------
	// ���C���E�_�C�A���O�̋N��
	//-----------------------------------------
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//       �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <��ݾ�> �ŏ����ꂽ���̃R�[�h��
		//       �L�q���Ă��������B
	}

END_InitInstance:

	LOG_OUT("(E)\n");

	// �_�C�A���O�������Ă���A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n������́A
	// �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

int CCDtestApp::ExitInstance()
{
	LOG_OUT("(S)\n");

	LOG_OUT("m_iReturnValue = %ld\n", m_iReturnValue);

	LOG_OUT("(E)\n");

	//-----------------------------------------
	// �J���p�E���O�̌�n��
	//-----------------------------------------
	LOG_END();

	CWinApp::ExitInstance();

	return m_iReturnValue;
}

////////////////////////////////////////////////////////////////////////
// �A�v���P�[�V�����̏����擾����
////////////////////////////////////////////////////////////////////////
BOOL CCDtestApp::GetAppInfo()
{
	LOG_OUT("(S)\n");

	BOOL			bFuncResult = FALSE;
	BOOL			bResult;
	DWORD			dwResult;

	// ���݂̎��s���W���[�����܂� EXE �t�@�C���̃t���E�p�X���擾
	dwResult = ::GetModuleFileName( NULL, m_szModuleFileName_FullPath, _MAX_PATH );
	if ( dwResult == 0 )
	{
		LOG_OUT("GetModuleFileName() failed.\n");
		goto END_GetAppInfo;
	}
	else
	{
		LOG_OUT("GetModuleFileName() successed.\n");
		LOG_OUT("szModuleFileName = %s\n", m_szModuleFileName_FullPath );
	}

	// ���݂̎��s���W���[�����܂� EXE �t�@�C���̃t���E�p�X��v�f���Ƃɕ���
	::_tsplitpath( m_szModuleFileName_FullPath, m_szModuleFileName_Drive, m_szModuleFileName_Dir, m_szModuleFileName_FileName, m_szModuleFileName_EXt );
	LOG_OUT("m_szModuleFileName_Drive    = %s\n", m_szModuleFileName_Drive);
	LOG_OUT("m_szModuleFileName_Dir      = %s\n", m_szModuleFileName_Dir);
	LOG_OUT("m_szModuleFileName_FileName = %s\n", m_szModuleFileName_FileName);
	LOG_OUT("m_szModuleFileName_EXt      = %s\n", m_szModuleFileName_EXt);

	// INI�t�@�C���p�X�̏���
	m_strIniFilePath.Format(".\\%s.ini", m_szModuleFileName_FileName);
	LOG_OUT("m_strIniFilePath            = %s\n", m_strIniFilePath);

	// ���O�t�@�C���p�X�̏���
	m_strLogFilePath = T_XML_LOG_PATH;
	LOG_OUT("m_strLogFilePath            = %s\n", m_strLogFilePath);

	// ���O�t�@�C�����̏���
	if ( stricmp( m_szModuleFileName_FileName, "cdtest" ) == 0 )
	{
		m_strLogFileName = "CDtest";
	}
	else if ( stricmp( m_szModuleFileName_FileName, "dvdtest" ) == 0 )
	{
		m_strLogFileName = "DVDtest";
	}
	LOG_OUT("m_strLogFileName            = %s\n", m_strLogFileName);

	// �t�@�C���o�[�W�����̎擾
	bResult = GetFileVersion();
	if ( bResult == FALSE )
	{
		LOG_OUT("GetFileVersion() failed.\n");
		goto END_GetAppInfo;
	}

	bFuncResult = TRUE;

END_GetAppInfo:

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}

BOOL CCDtestApp::GetFileVersion()
{
	LOG_OUT("(S)\n");

	BOOL			bFuncResult = FALSE;
	UINT			uLen;
	LPWORD			lpWord;
	LPTSTR			lpStr;
	DWORD			dwLangAndCharSet;
	char			szKey[256];
	LPVOID			lpBuff = NULL;
	BOOL			bResult;

// �o�[�W�������擾�ɕK�v�ȃ��C�u�����̓ǂݍ���
#pragma comment(lib, "version.lib")

	// ���O�̃o�[�W�������T�C�Y�̎擾
	DWORD dwZero = 0;
	DWORD dwReserved;
	DWORD dwVerInfoSize = ::GetFileVersionInfoSize( theApp.m_szModuleFileName_FullPath, &dwReserved );
	if ( dwVerInfoSize <= 0 )
	{
		LOG_OUT("GetFileVersionInfoSize() failed.\n");
		goto END_GetFileVersion;
	}

	// ���O�̃o�[�W�������̃o�b�t�@�̏���
	lpBuff = malloc( dwVerInfoSize );
	if ( !lpBuff )
	{
		LOG_OUT("malloc() failed.\n");
		goto END_GetFileVersion;
	}

	// ���O�̃o�[�W�������̎擾
	bResult = GetFileVersionInfo( theApp.m_szModuleFileName_FullPath, 0, dwVerInfoSize, lpBuff );
	if ( bResult == FALSE )
	{
		LOG_OUT("GetFileVersionInfo() failed.\n");
		goto END_GetFileVersion;
	}

	// ����E�L�����N�^�Z�b�g�ϊ��e�[�u�����擾
	bResult = VerQueryValue( lpBuff, "\\VarFileInfo\\Translation", (void**)&lpWord, &uLen );
	if ( bResult == FALSE )
	{
		LOG_OUT("VerQueryValue() failed.\n");
		goto END_GetFileVersion;
	}

	// lpWord[0]�͌���Z�b�g�AlpWord[1]�̓L�����N�^�Z�b�g
	dwLangAndCharSet = MAKELONG( lpWord[1], lpWord[0] );

	// �t�@�C���o�[�W�����̎擾
	sprintf( szKey, "\\StringFileInfo\\%08x\\%s", dwLangAndCharSet, "FileVersion" );
	bResult = VerQueryValue( lpBuff, szKey, (void **)&lpStr, &uLen );
	if ( bResult == FALSE )
	{
		LOG_OUT("VerQueryValue() failed.\n");
		goto END_GetFileVersion;
	}
	else
	{
		m_strFileVersion = lpStr;
		LOG_OUT("m_strFileVersion = %s\n", m_strFileVersion);
	}

	bFuncResult = TRUE;

END_GetFileVersion:

	if ( lpBuff )
	{
		free( lpBuff );
		lpBuff = NULL;
	}

	LOG_OUT("(E)(%s)\n", BOOL_TO_A(bFuncResult) );

	return bFuncResult;
}