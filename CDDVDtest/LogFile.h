#ifndef __LOG_FILE_CONTROL_HEADER__
#define  __LOG_FILE_CONTROL_HEADER__
#pragma once

//#import "msxml.dll" rename_namespace("msxml")
#import "msxml6.dll" rename_namespace("msxml")
#include <Shlwapi.h>

#define	STATE_PASS		0
#define	STATE_FAIL		1
#define	STATE_SKIP		2

struct	nameversion{
	CString name;
	CString version;
	nameversion(): name(""), version(""){}
};

class AFX_EXT_CLASS CBaseFileControl
{
public:
	CBaseFileControl(long flg = 0);
	//CBaseFileControl(CString	csCallerFileName);	//�R���X�g���N�^
	virtual ~CBaseFileControl(void);

public:
	// functions
	virtual void	InitLog(void);									  //Init�֐�
	virtual long	LogOpen(CString csCallerFileName)			= 0;  //LogFile�I�[�v���֐�
	virtual	long	LogClose(void)								= 0;  //LogFile�N���[�Y�֐�
	virtual void	LogStart(void);									  //START���Ԏ擾�֐�
	virtual void	LogFinish(void);								  //FINISH���Ԏ擾�֐�
	virtual long	WriteLogHeader(void)						= 0;  //�w�b�_�[��񏑂����݊֐�
	virtual long	WriteLogHeader(nameversion nv)				= 0;  //�w�b�_�[��񏑂����݊֐�
	virtual long	WriteLogHeader(CString process)				= 0;  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	virtual long	WriteLogHeader(CString sn, CString process) = 0;  //�w�b�_�[��񏑂����݊֐�
	virtual long	WriteLogHeader(nameversion nv, 
								CString process)				= 0;  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	virtual long	WriteLogHeader(nameversion nv, 
								CString sn, CString process)	= 0;  //�w�b�_�[��񏑂����݊֐�
	virtual long	WriteLogResult(long	lState)					= 0;  //���ʏ������݊֐�
	virtual long	WriteLogResult(long	lState, 
								CString errcode[5])				= 0;  //���ʏ������݊֐�
	virtual long	WriteLogSection(CString csSectionStr)		= 0;  //Section�������݊֐�
	virtual long	WriteLog(CString csStr)						= 0;  //�������݊֐�
	virtual long	WritePass(void);								  //PASS �������݊֐�
	virtual long	WriteFail(void);								  //FAIL �������݊֐�
	virtual long	WriteSkip(void);								  //SKIP �������݊֐�
	virtual long	WriteLogOnly(CString csStrOnly);					  //LOG�`����p�������݊֐�
	virtual long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);	  //XML�`����p�������݊֐�

public:	// for XML
	virtual msxml::IXMLDOMDocumentPtr	IXMLDocPtr(void);

protected:
	long		lPassCnt;		//Pass Count
	long		lSkipCnt;		//Skip Count
	long		lFailCnt;		//Fail Count
	CString		csStart;		// �J�n����
	CString		csFinish;		// �I������
	SYSTEMTIME	stStart;		//Start Time
	SYSTEMTIME	stFinish;		//Finish Time
	// CString		csTestName;
	//CString		csVerStr;
	long		ctrl_type;		// 

protected:	// get test_name & test_version
	char	FileDescription[128];
	char	ProductVersion[128];
	DWORD*	dwTrans;
	void*	pVersionInfo;
	nameversion	createNameVersion();
	void*		GetVersionInfo(char* ModuleName, DWORD** dwTrans);
	BOOL		GetVersionValue(char* Buffer, void* pVersionInfo, DWORD dwTrans, const char* KeyWord );
};

// end of Base

class AFX_EXT_CLASS CLogFileControl :
	virtual public CBaseFileControl
{
public:
	CLogFileControl(void);
	//CLogFileControl(CString	csCallerFileName);	//�R���X�g���N�^
	~CLogFileControl(void);

public:
	//functions
	void	InitLog(void);									  //Init�֐�
	long	LogOpen(CString csCallerFileName);				  //LogFile�I�[�v���֐�
	long	LogClose(void);									  //LogFile�N���[�Y�֐�
	// void	LogStart(void);									  //START���Ԏ擾�֐�
	// void	LogFinish(void);								  //FINISH���Ԏ擾�֐�
	long	WriteLogHeader(void);							  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(nameversion nv);					  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(CString process);				  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	long	WriteLogHeader(CString sn, CString process);	  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(nameversion nv, 
						CString process);					  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	long	WriteLogHeader(nameversion nv, 
						CString sn, CString process);		  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogResult(long	lState);					  //���ʏ������݊֐�
	long	WriteLogResult(long	lState, 
						CString errcode[5]);				  //���ʏ������݊֐�
	long	WriteLogSection(CString csSectionStr);			  //Section�������݊֐�
	long	WriteLog(CString csStr);						  //�������݊֐�
	long	WritePass(void);								  //PASS �������݊֐�
	long	WriteFail(void);								  //FAIL �������݊֐�
	long	WriteSkip(void);								  //SKIP �������݊֐�
	long	WriteLogOnly(CString csStrOnly);					  //LOG�`����p�������݊֐�
	//long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);	  //XML�`����p�������݊֐�

protected:
	CString		csFileName;		//�t�@�C����
	CFile		cfFileObj;		//�t�@�C��OBJ
};

// end of LOG

class AFX_EXT_CLASS CXmlFileControl :
	virtual public CBaseFileControl
{
public:
	CXmlFileControl(void);
	//CXmlFileControl(CString	csCallerFileName);	//�R���X�g���N�^
	~CXmlFileControl(void);

	void	InitLog(void);									  //Init�֐�
	long	LogOpen(CString csCallerFileName);				  //LogFile�I�[�v���֐�
	long	LogClose(void);									  //LogFile�N���[�Y�֐�
	//void	LogStart(void);									  //START���Ԏ擾�֐�
	//void	LogFinish(void);								  //FINISH���Ԏ擾�֐�
	long	WriteLogHeader(void);							  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(nameversion nv);					  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(CString process);				  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	long	WriteLogHeader(CString sn, CString process);	  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(nameversion nv, 
						CString process);					  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	long	WriteLogHeader(nameversion nv, 
						CString sn, CString process);		  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogResult(long	lState);					  //���ʏ������݊֐�
	long	WriteLogResult(long	lState, 
						CString errcode[5]);				  //���ʏ������݊֐�
	long	WriteLogSection(CString csSectionStr);			  //Section�������݊֐�
	long	WriteLog(CString csStr);						  //�������݊֐�
	long	WritePass(void);								  //PASS �������݊֐�
	long	WriteFail(void);								  //FAIL �������݊֐�
	long	WriteSkip(void);								  //SKIP �������݊֐�
	//long	WriteLogOnly(CString csStrOnly);				  //LOG�`����p�������݊֐�
	long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);  //XML�`����p�������݊֐�

protected:
	CString						csFileName;		// �t�@�C����
	msxml::IXMLDOMDocumentPtr	xmldoc;			// IXML�h�L�������g
	msxml::IXMLDOMElementPtr	rootElem;		// ���[�g�G�������g
	msxml::IXMLDOMElementPtr	curElem;		// �J�����gIXML�G�������g
	bool	enter_section;						// �Z�N�V�����t���O
	long	section_no;							// �Z�N�V�����ԍ�
	long	detail_count;						// Detail�G�������g�̃J�E���g�����̘A��
	long	comment_count;						// COMMENT�̐�

public:
	msxml::IXMLDOMDocumentPtr	IXMLDocPtr(void);	// 

protected:	// get SN & PROCESS
	CString		get_SN(void);
	CString		get_Process(void);
};

// end of XML

class AFX_EXT_CLASS CAllFileControl :
	public CLogFileControl,
	public CXmlFileControl
{
public:
	CAllFileControl(void);
	//CAllFileControl(CString	csCallerFileName);	//�R���X�g���N�^
	~CAllFileControl(void);

	void	InitLog(void);									  //Init�֐�
	long	LogOpen(CString csCallerFileName);				  //LogFile�I�[�v���֐�
	long	LogClose(void);									  //LogFile�N���[�Y�֐�
	//void	LogStart(void);									  //START���Ԏ擾�֐�
	//void	LogFinish(void);								  //FINISH���Ԏ擾�֐�
	long	WriteLogHeader(void);							  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(nameversion nv);					  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(CString process);				  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	long	WriteLogHeader(CString sn, CString process);	  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogHeader(nameversion nv, 
							CString process);				  //�w�b�_�[��񏑂����݊֐�	//++ver1.0.2
	long	WriteLogHeader(nameversion nv, 
							CString sn, CString process);	  //�w�b�_�[��񏑂����݊֐�
	long	WriteLogResult(long	lState);					  //���ʏ������݊֐�
	long	WriteLogResult(long	lState, 
								CString errcode[5]);		  //���ʏ������݊֐�
	long	WriteLogSection(CString csSectionStr);			  //Section�������݊֐�
	long	WriteLog(CString csStr);						  //�������݊֐�
	long	WritePass(void);								  //PASS �������݊֐�
	long	WriteFail(void);								  //FAIL �������݊֐�
	long	WriteSkip(void);								  //SKIP �������݊֐�
	long	WriteLogOnly(CString csStrOnly);					  //LOG�`����p�������݊֐�
	long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);	  //XML�`����p�������݊֐�

public:
	msxml::IXMLDOMDocumentPtr	IXMLDocPtr(void);	// 

};

// end of ALL

#undef AFX_DATA
#define AFX_DATA

#endif // __LOG_FILE_CONTROL_HEADER__