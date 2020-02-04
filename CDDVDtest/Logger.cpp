//#include <afxwin.h>
#include <stdafx.h>
#include "Logger.h"

#define _FLAG_OUTPUT

TCHAR  CLogger::m_strLogFileName[_MAX_PATH] = {0};
TCHAR  CLogger::m_pLogPrefix[_MAX_BUFFER] = {0};
FILE*  CLogger::m_pLogFile = NULL;

CLogger::CLogger(const char* date_, const char* time_, const char* file_, const char* function_, int line_) 
	:	date(date_), 
		time(time_), 
		file(file_), 
		function(function_), 
		line(line_)
{
}

void CLogger::operator()(const char* format, ...)
{
#ifdef _FLAG_OUTPUT

	if ( m_pLogFile == NULL )
	{
		return ;
	}

	TCHAR strBuffer[_MAX_BUFFER];
	va_list ap;
	va_start(ap, format);

	_vstprintf( strBuffer, format, ap );

	va_end(ap);


	__time64_t t1;
	_time64( &t1 );

	struct tm t2;
	t2 = *_localtime64( &t1 );

	TCHAR strDateTime[_MAX_PATH];					
	_stprintf(strDateTime,"%d/%02d/%02d %02d:%02d:%02d",t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday, t2.tm_hour, t2.tm_min, t2.tm_sec );


//		fprintf( m_pLogFile, "[%s %s][%s][%s(%d)] %s", date, time, file, function, line, strBuffer );
//		fprintf( m_pLogFile, "[%s %s][%s(%d)] %s%s", date, time, function, line, m_pLogPrefix, strBuffer );
	fprintf( m_pLogFile, "[%s][%s(%d)] %s%s", strDateTime, function, line, m_pLogPrefix, strBuffer );

	fflush(m_pLogFile);

#endif
}

void CLogger::LogStart( LPCTSTR lpLogFileName )
{
#ifdef _FLAG_OUTPUT

	if ( m_pLogFile == NULL )
	{
		_stprintf( m_strLogFileName, "%s", lpLogFileName ); 
		m_pLogFile = _tfopen( m_strLogFileName, "w+" );
	}
#endif
}

void CLogger::LogStart()
{
#ifdef _FLAG_OUTPUT

	TCHAR szModuleFileName[_MAX_PATH];
	DWORD dwResult = ::GetModuleFileName( NULL, szModuleFileName, _MAX_PATH );

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFileName[_MAX_FNAME];
	TCHAR szEXt[_MAX_EXT];
	::_tsplitpath( szModuleFileName, szDrive, szDir, szFileName, szEXt );

	CString strLogFileName;
	strLogFileName.Format(".\\%s_dbg.log", szFileName);

	if ( m_pLogFile == NULL )
	{
		_stprintf( m_strLogFileName, "%s", strLogFileName ); 
		m_pLogFile = _tfopen( m_strLogFileName, "w+" );
	}
#endif
}

void CLogger::LogPrefix( LPCTSTR lpLogPrefix )
{
#ifdef _FLAG_OUTPUT

	_stprintf( m_pLogPrefix, "%s", lpLogPrefix ); 

#endif
}

void CLogger::LogEnd()
{
#ifdef _FLAG_OUTPUT

	if ( m_pLogFile != NULL )
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
#endif
}

void CLogger::Sample()
{
#ifdef _FLAG_OUTPUT

	LOG_START("test.log");
	LOG_PREFIX("CPU STRESS 2:");

	LOG_OUT("%s %s","Hello","World.");

	LOG_END();

#endif
}