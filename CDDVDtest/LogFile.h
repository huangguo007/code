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
	//CBaseFileControl(CString	csCallerFileName);	//コンストラクタ
	virtual ~CBaseFileControl(void);

public:
	// functions
	virtual void	InitLog(void);									  //Init関数
	virtual long	LogOpen(CString csCallerFileName)			= 0;  //LogFileオープン関数
	virtual	long	LogClose(void)								= 0;  //LogFileクローズ関数
	virtual void	LogStart(void);									  //START時間取得関数
	virtual void	LogFinish(void);								  //FINISH時間取得関数
	virtual long	WriteLogHeader(void)						= 0;  //ヘッダー情報書き込み関数
	virtual long	WriteLogHeader(nameversion nv)				= 0;  //ヘッダー情報書き込み関数
	virtual long	WriteLogHeader(CString process)				= 0;  //ヘッダー情報書き込み関数	//++ver1.0.2
	virtual long	WriteLogHeader(CString sn, CString process) = 0;  //ヘッダー情報書き込み関数
	virtual long	WriteLogHeader(nameversion nv, 
								CString process)				= 0;  //ヘッダー情報書き込み関数	//++ver1.0.2
	virtual long	WriteLogHeader(nameversion nv, 
								CString sn, CString process)	= 0;  //ヘッダー情報書き込み関数
	virtual long	WriteLogResult(long	lState)					= 0;  //結果書き込み関数
	virtual long	WriteLogResult(long	lState, 
								CString errcode[5])				= 0;  //結果書き込み関数
	virtual long	WriteLogSection(CString csSectionStr)		= 0;  //Section書き込み関数
	virtual long	WriteLog(CString csStr)						= 0;  //書き込み関数
	virtual long	WritePass(void);								  //PASS 書き込み関数
	virtual long	WriteFail(void);								  //FAIL 書き込み関数
	virtual long	WriteSkip(void);								  //SKIP 書き込み関数
	virtual long	WriteLogOnly(CString csStrOnly);					  //LOG形式専用書き込み関数
	virtual long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);	  //XML形式専用書き込み関数

public:	// for XML
	virtual msxml::IXMLDOMDocumentPtr	IXMLDocPtr(void);

protected:
	long		lPassCnt;		//Pass Count
	long		lSkipCnt;		//Skip Count
	long		lFailCnt;		//Fail Count
	CString		csStart;		// 開始時刻
	CString		csFinish;		// 終了時刻
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
	//CLogFileControl(CString	csCallerFileName);	//コンストラクタ
	~CLogFileControl(void);

public:
	//functions
	void	InitLog(void);									  //Init関数
	long	LogOpen(CString csCallerFileName);				  //LogFileオープン関数
	long	LogClose(void);									  //LogFileクローズ関数
	// void	LogStart(void);									  //START時間取得関数
	// void	LogFinish(void);								  //FINISH時間取得関数
	long	WriteLogHeader(void);							  //ヘッダー情報書き込み関数
	long	WriteLogHeader(nameversion nv);					  //ヘッダー情報書き込み関数
	long	WriteLogHeader(CString process);				  //ヘッダー情報書き込み関数	//++ver1.0.2
	long	WriteLogHeader(CString sn, CString process);	  //ヘッダー情報書き込み関数
	long	WriteLogHeader(nameversion nv, 
						CString process);					  //ヘッダー情報書き込み関数	//++ver1.0.2
	long	WriteLogHeader(nameversion nv, 
						CString sn, CString process);		  //ヘッダー情報書き込み関数
	long	WriteLogResult(long	lState);					  //結果書き込み関数
	long	WriteLogResult(long	lState, 
						CString errcode[5]);				  //結果書き込み関数
	long	WriteLogSection(CString csSectionStr);			  //Section書き込み関数
	long	WriteLog(CString csStr);						  //書き込み関数
	long	WritePass(void);								  //PASS 書き込み関数
	long	WriteFail(void);								  //FAIL 書き込み関数
	long	WriteSkip(void);								  //SKIP 書き込み関数
	long	WriteLogOnly(CString csStrOnly);					  //LOG形式専用書き込み関数
	//long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);	  //XML形式専用書き込み関数

protected:
	CString		csFileName;		//ファイル名
	CFile		cfFileObj;		//ファイルOBJ
};

// end of LOG

class AFX_EXT_CLASS CXmlFileControl :
	virtual public CBaseFileControl
{
public:
	CXmlFileControl(void);
	//CXmlFileControl(CString	csCallerFileName);	//コンストラクタ
	~CXmlFileControl(void);

	void	InitLog(void);									  //Init関数
	long	LogOpen(CString csCallerFileName);				  //LogFileオープン関数
	long	LogClose(void);									  //LogFileクローズ関数
	//void	LogStart(void);									  //START時間取得関数
	//void	LogFinish(void);								  //FINISH時間取得関数
	long	WriteLogHeader(void);							  //ヘッダー情報書き込み関数
	long	WriteLogHeader(nameversion nv);					  //ヘッダー情報書き込み関数
	long	WriteLogHeader(CString process);				  //ヘッダー情報書き込み関数	//++ver1.0.2
	long	WriteLogHeader(CString sn, CString process);	  //ヘッダー情報書き込み関数
	long	WriteLogHeader(nameversion nv, 
						CString process);					  //ヘッダー情報書き込み関数	//++ver1.0.2
	long	WriteLogHeader(nameversion nv, 
						CString sn, CString process);		  //ヘッダー情報書き込み関数
	long	WriteLogResult(long	lState);					  //結果書き込み関数
	long	WriteLogResult(long	lState, 
						CString errcode[5]);				  //結果書き込み関数
	long	WriteLogSection(CString csSectionStr);			  //Section書き込み関数
	long	WriteLog(CString csStr);						  //書き込み関数
	long	WritePass(void);								  //PASS 書き込み関数
	long	WriteFail(void);								  //FAIL 書き込み関数
	long	WriteSkip(void);								  //SKIP 書き込み関数
	//long	WriteLogOnly(CString csStrOnly);				  //LOG形式専用書き込み関数
	long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);  //XML形式専用書き込み関数

protected:
	CString						csFileName;		// ファイル名
	msxml::IXMLDOMDocumentPtr	xmldoc;			// IXMLドキュメント
	msxml::IXMLDOMElementPtr	rootElem;		// ルートエレメント
	msxml::IXMLDOMElementPtr	curElem;		// カレントIXMLエレメント
	bool	enter_section;						// セクションフラグ
	long	section_no;							// セクション番号
	long	detail_count;						// Detailエレメントのカウント属性の連番
	long	comment_count;						// COMMENTの数

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
	//CAllFileControl(CString	csCallerFileName);	//コンストラクタ
	~CAllFileControl(void);

	void	InitLog(void);									  //Init関数
	long	LogOpen(CString csCallerFileName);				  //LogFileオープン関数
	long	LogClose(void);									  //LogFileクローズ関数
	//void	LogStart(void);									  //START時間取得関数
	//void	LogFinish(void);								  //FINISH時間取得関数
	long	WriteLogHeader(void);							  //ヘッダー情報書き込み関数
	long	WriteLogHeader(nameversion nv);					  //ヘッダー情報書き込み関数
	long	WriteLogHeader(CString process);				  //ヘッダー情報書き込み関数	//++ver1.0.2
	long	WriteLogHeader(CString sn, CString process);	  //ヘッダー情報書き込み関数
	long	WriteLogHeader(nameversion nv, 
							CString process);				  //ヘッダー情報書き込み関数	//++ver1.0.2
	long	WriteLogHeader(nameversion nv, 
							CString sn, CString process);	  //ヘッダー情報書き込み関数
	long	WriteLogResult(long	lState);					  //結果書き込み関数
	long	WriteLogResult(long	lState, 
								CString errcode[5]);		  //結果書き込み関数
	long	WriteLogSection(CString csSectionStr);			  //Section書き込み関数
	long	WriteLog(CString csStr);						  //書き込み関数
	long	WritePass(void);								  //PASS 書き込み関数
	long	WriteFail(void);								  //FAIL 書き込み関数
	long	WriteSkip(void);								  //SKIP 書き込み関数
	long	WriteLogOnly(CString csStrOnly);					  //LOG形式専用書き込み関数
	long	WriteXmlOnly(msxml::IXMLDOMElementPtr nodeOnly);	  //XML形式専用書き込み関数

public:
	msxml::IXMLDOMDocumentPtr	IXMLDocPtr(void);	// 

};

// end of ALL

#undef AFX_DATA
#define AFX_DATA

#endif // __LOG_FILE_CONTROL_HEADER__