#pragma once

#include "Logger.h"

class CWMIInfo
{
public:
	CWMIInfo(void);
	~CWMIInfo(void);

	bool IsInitWMI;

	bool InitWMI();
	bool GetWMIProperty(WCHAR* query,WCHAR* propname,CComVariant* var);
	bool GetWMIProperty2(WCHAR* query,WCHAR* propname,CStringArray& strList);
	bool FinalWMI();

	static LONG GetNumberOfPhysicalCPUCores();
	static CString GetSMBiosBiosVersion();
	static BOOL GetInstalledDisplayDrivers( CStringArray& strListInstalledDisplayDrivers );

};
