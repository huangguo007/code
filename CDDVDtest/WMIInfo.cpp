#include "StdAfx.h"
#include "wmiinfo.h"

#include "wbemidl.h"
#pragma comment(lib, "wbemuuid.lib")

#include <comdef.h>

CWMIInfo::CWMIInfo(void)
{
	IsInitWMI = false;
}

CWMIInfo::~CWMIInfo(void)
{
}

bool CWMIInfo::InitWMI()
{
	bool bFuncResult = false;

	if( IsInitWMI )
	{
		goto END_InitWMI;
	}

	CoInitialize(NULL);
 
	HRESULT hResult = CoInitializeSecurity(NULL,
											-1,
											NULL,
											NULL,
											RPC_C_AUTHN_LEVEL_PKT,
											RPC_C_IMP_LEVEL_IMPERSONATE,
											NULL,
											EOAC_SECURE_REFS,
											0
										);
	if ( hResult == RPC_E_TOO_LATE )
	{
	}
	else if ( hResult != WBEM_S_NO_ERROR )
	{
		goto END_InitWMI;
	}		

	bFuncResult = true;

END_InitWMI:

	if ( bFuncResult == true )
	{
		IsInitWMI = true;
	}

	return bFuncResult;
}

bool CWMIInfo::FinalWMI()
{
	if( IsInitWMI == true )
	{
		CoUninitialize();
	}

	IsInitWMI=false;
	
	return true;
}

bool CWMIInfo::GetWMIProperty(WCHAR* query,WCHAR* propname,CComVariant* var)
{
	// TODO: Add your control notification handler code here

    IWbemLocator *pLocator = NULL;
    IWbemServices *pNamespace = NULL;
    IWbemClassObject * pClass = NULL;
	IEnumWbemClassObject *pEnum = NULL;
	HRESULT hr=S_OK;

	BSTR path = SysAllocString(L"root\\CIMV2");
//	BSTR path = SysAllocString(L"root\\wmi");
	bool rtn=true;

	try
	{
		if(	CoCreateInstance (
//							CLSID_WbemAdministrativeLocator,
							CLSID_WbemLocator,
							NULL ,
							CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER , 
							IID_IUnknown ,
							( void ** ) & pLocator
			) != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not CoCreateInstance\n");
			throw false;
		}

		if(	pLocator->ConnectServer(
									path,			// Namespace
									NULL,			// Userid
									NULL,           // PW
									NULL,           // Locale
									0,              // flags
									NULL,           // Authority
									NULL,           // Context
									&pNamespace
			) != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not connect Server\n");
			throw false;
		}

		WCHAR tstr[128];
		wsprintfW(tstr,L"Select * from %s",query);
		
		//BSTR strQuery = (L"Select * from win32_Processor");		
		BSTR strQuery = SysAllocString(tstr);
		BSTR strQL =  SysAllocString(L"WQL");
		hr = pNamespace->ExecQuery(strQL, strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnum);
		SysFreeString(strQuery);
		SysFreeString(strQL);

		if(hr != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not execute Query\n");
			throw false;
		}

		ULONG uCount = 1, uReturned;

		hr = pEnum->Reset();

		if(hr != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not Enumerate\n");
			throw false;
		}

		hr = pEnum->Next(WBEM_INFINITE,uCount, &pClass, &uReturned);
		if(hr != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not Enumerate Next\n");
			throw false;
		}

		BSTR strClassProp = SysAllocString(propname);
		CIMTYPE cimType;
		hr = pClass->Get(strClassProp, 0, var, &cimType, 0);

		if(hr != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not Get Value\n");
			throw false;
		}

		SysFreeString(strClassProp);
	}
	catch(const bool b)
	{
		rtn=b;
	}

	// Free up resources
    SysFreeString(path);

	if(pLocator)
		pLocator->Release();

	if(pNamespace)
		pNamespace->Release();

	if(pEnum)
		pEnum->Release();

	if(pClass)
		pClass->Release();

	return rtn;
}

bool CWMIInfo::GetWMIProperty2( WCHAR* query, WCHAR* propname, CStringArray& strList)
{
	// TODO: Add your control notification handler code here

    IWbemLocator *pLocator = NULL;
    IWbemServices *pNamespace = NULL;
    IWbemClassObject * pClass = NULL;
	IEnumWbemClassObject *pEnum = NULL;
	HRESULT hr=S_OK;

	BSTR path = SysAllocString(L"root\\CIMV2");
//	BSTR path = SysAllocString(L"root\\wmi");
	bool rtn=true;

	try
	{
		if(	CoCreateInstance (
//							CLSID_WbemAdministrativeLocator,
							CLSID_WbemLocator,
							NULL ,
							CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER , 
							IID_IUnknown ,
							( void ** ) & pLocator
			) != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not CoCreateInstance\n");
			throw false;
		}

		if(	pLocator->ConnectServer(
									path,			// Namespace
									NULL,			// Userid
									NULL,           // PW
									NULL,           // Locale
									0,              // flags
									NULL,           // Authority
									NULL,           // Context
									&pNamespace
			) != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not connect Server\n");
			throw false;
		}

		WCHAR tstr[128];
		wsprintfW(tstr,L"Select * from %s",query);
		
		//BSTR strQuery = (L"Select * from win32_Processor");		
		BSTR strQuery = SysAllocString(tstr);
		BSTR strQL =  SysAllocString(L"WQL");
		hr = pNamespace->ExecQuery(strQL, strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnum);
		SysFreeString(strQuery);
		SysFreeString(strQL);

		if(hr != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not execute Query\n");
			throw false;
		}

		ULONG uCount = 1, uReturned;

		hr = pEnum->Reset();

		if(hr != WBEM_S_NO_ERROR)
		{
			LOG_OUT("WMI:Could not Enumerate\n");
			throw false;
		}

		while( hr == WBEM_S_NO_ERROR )
		{
			hr = pEnum->Next(WBEM_INFINITE,uCount, &pClass, &uReturned);
			if(hr == WBEM_S_NO_ERROR)
			{
				CComVariant var;
				BSTR strClassProp = SysAllocString(propname);
				CIMTYPE cimType;
				hr = pClass->Get(strClassProp, 0, &var, &cimType, 0);

				if(hr != WBEM_S_NO_ERROR)
				{
					LOG_OUT("WMI:Could not Get Value\n");
					throw false;
				}
			
				SysFreeString(strClassProp);
				strList.Add( CString(var.bstrVal) );
			}
		}
	}
	catch(const bool b)
	{
		rtn=b;
	}

	// Free up resources
    SysFreeString(path);

	if(pLocator)
		pLocator->Release();

	if(pNamespace)
		pNamespace->Release();

	if(pEnum)
		pEnum->Release();

	if(pClass)
		pClass->Release();

	return rtn;
}

LONG CWMIInfo::GetNumberOfPhysicalCPUCores()
{
//	LOG_OUT("(S)\n");

	LONG lFuncResult = -1;
	CWMIInfo wmiInfo;
	CComVariant var;


	if ( wmiInfo.InitWMI() == false )
	{
		LOG_OUT("InitWMI() failed.\n");
		goto END_GetNumberOfPhysicalCPUCores;
	}
	
	if ( wmiInfo.GetWMIProperty(L"Win32_Processor", L"NumberOfCores", &var) == false )
	{
		LOG_OUT("GetWMIProperty() failed.\n");
		goto END_GetNumberOfPhysicalCPUCores;
	}
	
	if ( wmiInfo.FinalWMI() == false )
	{
		LOG_OUT("FinalWMI() failed.\n");
		goto END_GetNumberOfPhysicalCPUCores;
	}

	lFuncResult = var.iVal;
//	LOG_OUT("NumberOfCores = %ld\n", lFuncResult );

END_GetNumberOfPhysicalCPUCores:

//	LOG_OUT("(E)(%ld)\n", lFuncResult );

	return lFuncResult;
}

CString CWMIInfo::GetSMBiosBiosVersion()
{
//	LOG_OUT("(S)\n");

	CString strFuncResult = "";
	CWMIInfo wmiInfo;
	CComVariant var;


	if ( wmiInfo.InitWMI() == false )
	{
		LOG_OUT("InitWMI() failed.\n");
		goto END_GetSMBiosBiosVersion;
	}
	
	if ( wmiInfo.GetWMIProperty(L"Win32_BIOS", L"SMBIOSBIOSVersion", &var) == false )
	{
		LOG_OUT("GetWMIProperty() failed.\n");
		goto END_GetSMBiosBiosVersion;
	}
	
	if ( wmiInfo.FinalWMI() == false )
	{
		LOG_OUT("FinalWMI() failed.\n");
		goto END_GetSMBiosBiosVersion;
	}

	strFuncResult = var.bstrVal;

//	LOG_OUT("SMBIOSBIOSVersion = %s\n", strFuncResult );

END_GetSMBiosBiosVersion:

//	LOG_OUT("(E)(%s)\n", strFuncResult );

	return strFuncResult;
}

BOOL CWMIInfo::GetInstalledDisplayDrivers( CStringArray& strListInstalledDisplayDrivers )
{
//	LOG_OUT("(S)\n");

	BOOL bFuncResult = FALSE;
	CWMIInfo wmiInfo;

	if ( wmiInfo.InitWMI() == false )
	{
		LOG_OUT("InitWMI() failed.\n");
		goto END_GetInstalledDisplayDrivers;
	}
	
	if ( wmiInfo.GetWMIProperty2(L"Win32_VideoController", L"InstalledDisplayDrivers", strListInstalledDisplayDrivers) == false )
	{
		LOG_OUT("GetWMIProperty() failed.\n");
		goto END_GetInstalledDisplayDrivers;
	}
	
	if ( wmiInfo.FinalWMI() == false )
	{
		LOG_OUT("FinalWMI() failed.\n");
		goto END_GetInstalledDisplayDrivers;
	}

	bFuncResult = TRUE;

END_GetInstalledDisplayDrivers:

//	LOG_OUT("(E)(%s)\n", strFuncResult );

	return bFuncResult;
}
