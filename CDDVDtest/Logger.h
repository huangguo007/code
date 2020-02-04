#pragma once

#include <windows.h>

#define _MAX_BUFFER 2048

#define booltoa(x) ((x)?"true":"false")
#define BOOL_TO_A(x) ((x)?"TRUE":"FALSE")

#define	LOG_START	CLogger::LogStart
#define	LOG_PREFIX	CLogger::LogPrefix
#define	LOG_END		CLogger::LogEnd
#define	LOG_OUT		CLogger(__DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__)

class CLogger
{
private:
	static TCHAR m_strLogFileName[_MAX_PATH];
	static FILE *m_pLogFile;
	static TCHAR m_pLogPrefix[_MAX_BUFFER];

	const char* date;
	const char* time;
	const char* file;
	const char* function;
	int line;

public:
	CLogger(const char* date_, const char* time_, const char* file_, const char* function_, int line_);
	void operator()(const char* format, ...);
	static void LogStart();
	static void LogStart( LPCTSTR lpLogFileName );
	static void LogPrefix( LPCTSTR lpLogPrefix );
	static void LogEnd();
	static void Sample();
};

