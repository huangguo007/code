// CDtest.h : CDTEST �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_CDTEST_H__FAA262C4_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
#define AFX_CDTEST_H__FAA262C4_6413_11D5_AE4C_0010A405E9A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��

#include <stdlib.h>
#include "Logger.h"
//#include "LogFile.h"

#define T_XML_LOG_PATH	"C:\\Logs\\"

// �߂�l
#define ERROR_NORMAL_END		0	// PASS�I��
#define ERROR_FAIL_END			1	// FAIL�I��
#define ERROR_PARAMETER			2	// �w�肳�ꂽ�p�����[�^�Ɉُ킪����ꍇ
#define ERROR_INIFILE			3	// INI�t�@�C���֘A�ُ̈킪����ꍇ
#define ERROR_LOGFILE			4	// LOG�t�@�C���֘A�ُ̈킪����ꍇ

/////////////////////////////////////////////////////////////////////////////
// CCDtestApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� CDtest.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CCDtestApp : public CWinApp
{
public:
	CCDtestApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CCDtestApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����

	//{{AFX_MSG(CCDtestApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	int					m_iReturnValue;								// �A�v���P�[�V�����I���R�[�h�i�G���[�R�[�h�j
	BOOL				m_bProgramStatus;							// �ŏI�e�X�g����
	CString				m_strErrorCode[5];							// �G���[�R�[�h

	CString				m_strIniFilePath;							// INI�t�@�C���p�X
	CString				m_strLogFilePath;							// ���O�t�@�C���p�X
	CString				m_strLogFileName;							// ���O�t�@�C����

	CString				m_strFileVersion;							// �t�@�C���o�[�W����
	TCHAR				m_szModuleFileName_FullPath[_MAX_PATH];		// ���W���[���t�@�C���F�t�@�C�����i�t���p�X�j
	TCHAR				m_szModuleFileName_Drive[_MAX_DRIVE];		// ���W���[���t�@�C���F�h���C�u��
	TCHAR				m_szModuleFileName_Dir[_MAX_DIR];			// ���W���[���t�@�C���F�f�B���N�g��
	TCHAR				m_szModuleFileName_FileName[_MAX_FNAME];	// ���W���[���t�@�C���F�t�@�C����
	TCHAR				m_szModuleFileName_EXt[_MAX_EXT];			// ���W���[���t�@�C���F�g���q


	virtual int ExitInstance();
	BOOL		GetAppInfo();										// �A�v���P�[�V�����̏����擾����
	BOOL		GetFileVersion();									// �t�@�C���o�[�W�������擾����
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_CDTEST_H__FAA262C4_6413_11D5_AE4C_0010A405E9A7__INCLUDED_)
