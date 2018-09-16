// SnapshotManager.cpp : implementation file
//

#include "stdafx.h"
#include "VXIConfig.h"
#include "SnapshotManager.h"
#include "SnapshotView.h"
#include <fstream>
#include "MacroDef.h"
#include "MainFrm.h"

extern CVXIConfigApp theApp;
static std::locale g_loc = std::locale("", std::locale::ctype);

#define NEED_LOAD_FORCE (EXPLAB_VERSION == EXPLAB_H611_VERSION)

// CSnapshotManager
static BOOL Urt_CreatePath(CString strPath)
{
	DWORD msk = 0;
	msk = GetFileAttributes(strPath);
	if ( (msk!=0xffffffff) && (msk&FILE_ATTRIBUTE_DIRECTORY) )
		return TRUE;

	CString str;
	int iPos = 0;
	BOOL bCreate = FALSE;
	while ( true )
	{
		str = strPath.Tokenize(_T("\\"), iPos);
		if ( iPos == -1 )
			break;

		if ( str == _T("") )
			continue;
		
		bCreate = CreateDirectory(strPath.Left(iPos), NULL);
	}

	return bCreate;
}

IMPLEMENT_DYNAMIC(CSnapshotManager, CCmdTarget)

CSnapshotManager::CSnapshotManager()
: m_snapshotcount(0)
, m_nForceChan(-1)
, m_bFirstSnapshot(FALSE)
, m_snapshotState(eServiceStoped)
, m_nDisplayCol(10)
, m_nLevelSrc(0)
, m_coninueSnapshot(FALSE)
, m_coninueSnapshotFreq(0)
, m_coninueSnapshotMax(FALSE)
{
	EnableAutomation();
	m_chanList.CoCreateInstance(__uuidof(UrtStringMapCollection));

	m_snapshotParam.pSnapshotMgr = this;
	m_snapshotParam.hDoSnapshot = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_snapshotParam.hStopEvent  = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSnapshotManager::~CSnapshotManager()
{
	::CloseHandle(m_snapshotParam.hDoSnapshot);
	::CloseHandle(m_snapshotParam.hStopEvent);
}

void CSnapshotManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CSnapshotManager, CCmdTarget)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CSnapshotManager, CCmdTarget)
END_DISPATCH_MAP()

// Note: we add support for IID_ISnapshotManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {790019BA-BCD6-46E4-8660-5BD4BE19F204}
static const IID IID_ISnapshotManager =
{ 0x790019BA, 0xBCD6, 0x46E4, { 0x86, 0x60, 0x5B, 0xD4, 0xBE, 0x19, 0xF2, 0x4 } };

BEGIN_INTERFACE_MAP(CSnapshotManager, CCmdTarget)
	INTERFACE_PART(CSnapshotManager, IID_ISnapshotManager, Dispatch)
END_INTERFACE_MAP()

// CSnapshotManager message handlers
DWORD WINAPI CSnapshotManager::SnapshotThreadFunc(LPVOID lpParam )
{
	::CoInitialize(NULL);
	SNAPSHOT_PARAM* pParam = (SNAPSHOT_PARAM*)lpParam;
#if EXPLAB_VERSION == EXPLAB_H31_VERSION
	pParam->pSnapshotMgr->m_strPathname = _T("");
#endif
	while ( WAIT_TIMEOUT == ::WaitForSingleObject(pParam->hStopEvent, 50) )
	{
		// 执行一次快照
		if ( WAIT_OBJECT_0 == ::WaitForSingleObject(pParam->hDoSnapshot, 50) )
			pParam->pSnapshotMgr->ExecuteSnapshot();
	}

	// add [12/31/2014 zcl]
	ResetEvent(pParam->hStopEvent);

	::CoUninitialize();
	return 0;
}

void CSnapshotManager::PrepareSnapshotFileFor7021()
{
	ExpLabService* pService = ExpLabService::Open();
	CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
	ExpLabService::Close();
	COleVariant varPath;
	if ( S_OK != pSetting->GetValue(GLOBAL_SNAPSHOT_FILEPATH, &varPath) )
		varPath = L"C:\\temp.snap";

	m_strPathname = V_BSTR(&varPath);
	Urt_CreatePath(m_strPathname);
	m_bFirstSnapshot = TRUE;
}

HRESULT CSnapshotManager::ExecuteSnapshotFor7021()
{
	ExpLabService* pService = ExpLabService::Open();
	GLOBAL_SETTING* pGlobalSetting = pService->GetGlobalSetting();
	ExpLabService::Close();

	LONG nLevel = GetPrivateProfileInt(_T("SnapshotManager"), _T("Level"), 0, ExpLabService::Open()->GetIniFilePath());
	if ( nLevel < 0 )
		nLevel = -2; // ++nLevel 也要小于0

	if ( m_bFirstSnapshot )
	{
		m_bFirstSnapshot = FALSE;
	}

	CString strTxt;
	strTxt.Format(_T("%sD%0.4d.txt"), m_strPathname, nLevel);
	std::wofstream snpFile;
	snpFile.open(strTxt, ios_base::app);
	snpFile.imbue(g_loc);
	snpFile.seekp(0, ios_base::end);
	snpFile.setf(std::ios::fixed);
	snpFile << CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")).GetString()
		<< std::endl << std::endl
		<< _T("级数 = ") << nLevel
		<< std::endl << std::endl;

	snpFile.width(VALUE_WIDTH);
	snpFile.precision(pGlobalSetting->lShowPrecision);
	CComPtr<IEnumVARIANT> spEnum;
	m_chanList->get__NewEnum((IUnknown**)&spEnum);

	ULONG nBatchSize = m_nDisplayCol;	// x个通道一个转接盒
	ULONG	nReturned = 0;
	std::vector<VARIANT> vVariant;
	vVariant.reserve(nBatchSize);
	vVariant.push_back(_variant_t(0));
	VARIANT* arrVariant = &vVariant[0];
	ZeroMemory(arrVariant, sizeof(VARIANT)*nBatchSize);
	HRESULT hr = E_UNEXPECTED;
	do
	{
		hr = spEnum->Next(nBatchSize, &arrVariant[0], &nReturned);
		if (FAILED(hr))
			break;

		for ( ULONG i = 0; i < nReturned; ++i )
		{
			CComQIPtr<IUrtTdmCurveData> pCurve(arrVariant[i].pdispVal);

			// [9/13/2013 Easy]
			::VariantClear(&arrVariant[i]);

			DOUBLE dVal;
			pCurve->GetSnapshotResult(&dVal, NULL);

			// 型如：" -82345.1234567"
			snpFile.width(15);
			snpFile.precision(6);
			snpFile << dVal;
		}

		snpFile << std::endl;

	} while ( hr != S_FALSE ); // S_FALSE indicates end of collection

	snpFile.flush();
	snpFile.close();
	// 重置快照信号
	::ResetEvent(m_snapshotParam.hDoSnapshot);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

	CString strTitle;
	strTitle.Format(_T("快照 %d"), nLevel);
	pMainFrm->SetWindowText(strTitle);

	ExpLabService::Open()->WritePrivateProfileInt(_T("SnapshotManager"), _T("Level"), ++nLevel); // 准备下级
	pMainFrm->UpdateSnapshotView(ID_EXECUTE_SNAPSHOT_ACQUIRE);
	return 0L;
}

void CSnapshotManager::PrepareSnapshotFileForTrunk()
{
#define BLOGBUFMAX (1<<17)
	WCHAR textline[BLOGBUFMAX+1];	// 64k WORDs
	CString strChanIDs, strNames, strUnits;
	strChanIDs.Format(_T("%15s"), STR_TIME);
	if ( g_bAbstimeInSnapshotFile )
		strUnits.Format(_T("%15s"), _T("Datetime"));
	else
		strUnits.Format(_T("%15s"), _T("s"));

	strUnits.Format(_T("%15s"), _T("s"));
	strNames.Format(_T("%15s"), _T("Time"));

	ExpLabService* pService = ExpLabService::Open();
	CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
	ExpLabService::Close();

	COleVariant varPath;
	pSetting->GetValue(GLOBAL_SNAPSHOT_FILEPATH, &varPath);
	CString strPath(V_BSTR(&varPath));

	m_nLevelSrc = GetPrivateProfileInt(_T("Snapshot"), _T("LevelSrc"), m_nLevelSrc, pService->GetIniFilePath());

	LONG nChanIndex = 0;

	CComPtr<IEnumVARIANT> spEnum;
	m_chanList->get__NewEnum((IUnknown**)&spEnum);

	const ULONG nBatchSize = 3;
	ULONG	nReturned = 0;
	VARIANT arrVariant[nBatchSize] = {0};
	m_nForceChan = -1;
	HRESULT hr = E_UNEXPECTED;
	do
	{
		hr = spEnum->Next(nBatchSize, &arrVariant[0], &nReturned);
		if (FAILED(hr))
			break;

		for ( ULONG i = 0; i < nReturned; ++i,++nChanIndex )
		{
			CComQIPtr<IUrtChannel> pChan(arrVariant[i].pdispVal);
			CComQIPtr<IUrtTdmCurveData> pCurve(pChan);
			CComQIPtr<IUrtParamChannel> pParam(pChan);
			::VariantClear(&arrVariant[i]);

			CComBSTR bstrChanId;
			pChan->get_chanID(&bstrChanId);

			std::map<CString, bool>::iterator iter = m_mapRecordChan.find(CString(bstrChanId));
			if (iter != m_mapRecordChan.end())
			{
				if (!iter->second)
					continue;
			}

			CComBSTR chanGroup, chanID, chanName, chanUnit;
			pChan->get_chanGroup(&chanGroup);
			pParam->get_paramName(&chanName);
			pChan->get_chanID(&chanID);
			pCurve->get_engineUnit(&chanUnit);

			EUrtConversionSign convSign;
			pCurve->get_conversionSign(&convSign);
			if ( m_nLevelSrc == 0 && eConvSStressLoad == convSign )
			{
				if ( m_nForceChan != -1 )
					pCurve->put_conversionSign(convSign);
				else
					m_nForceChan = nChanIndex;
			}

			DOUBLE dVal = 0.0;
			pCurve->GetSnapshotResult(&dVal, NULL);
			CString str;
			str.Format(_T("%15s"), chanID);
			strChanIDs += str;
			str.Format(_T("%15s"), chanUnit);
			strUnits += str;
			str.Format(_T("%15s"), chanName==_T("")?_T("　"):chanName);
			strNames += str;
		}

	} while ( hr != S_FALSE ); // S_FALSE indicates end of collection

	// [9/14/2012 Easy] 只有哦611项目需要载荷通道
	if ( NEED_LOAD_FORCE )
	{
		CString str;
		str.Format(_T("%15s"), STR_LOAD_FORCE);
		strChanIDs += str;
		str.Format(_T("%15s"), _T("N"));
		strUnits += str;
		str.Format(_T("%15s"), _T("Force"));
		strNames += str;
	}
	
	CString strDir = strPath.Mid(0, strPath.ReverseFind(_T('\\')));
	Urt_CreatePath(strDir);

	BOOL bFileExist = FALSE;
	CFileFind tempFind;
	BOOL bFound = tempFind.FindFile(strPath);
	while ( bFound )
	{
		bFound = tempFind.FindNextFile();
		if ( tempFind.IsDirectory() )
			continue;

		CFileStatus status;
		CFile::GetStatus(strPath, status);
		if (status.m_size == 0)
			break;

		streamsize readsize = status.m_size>BLOGBUFMAX ? BLOGBUFMAX : (streamsize)status.m_size;
		std::wifstream temp;
		temp.open(strPath);
		temp.imbue(g_loc);
		temp.read(textline, readsize);
		temp.close();

		textline[readsize] = _T('');

		CString strFileHead(textline);
		CString strStartTime,ChanIDs,Units,Names;
		int idx = 0;
		if ( !g_bAbstimeInSnapshotFile )
			AfxExtractSubString(strStartTime, strFileHead,	idx++, _T('\n'));

		AfxExtractSubString(ChanIDs, strFileHead,		idx++, _T('\n'));
		AfxExtractSubString(Units, strFileHead,			idx++, _T('\n'));
		AfxExtractSubString(Names, strFileHead,			idx++, _T('\n'));
		if ( ChanIDs == strChanIDs && Units == strUnits && Names == strNames )
		{
			bFileExist = TRUE;

			if ( g_bAbstimeInSnapshotFile )
				break;

			SYSTEMTIME locTime;
			ConvertStringToSysTime(strStartTime, locTime, 3);
			FILETIME locftime;
			SystemTimeToFileTime(&locTime, &locftime);
			m_startTime = 1.0e-7*(*(ULONGLONG*)&locftime);
			break;
		}

		CString strBakPath = strPath;
		GetParentLevel(strBakPath);
		strBakPath += "\\";

		SYSTEMTIME locTime;
		if ( g_bAbstimeInSnapshotFile )
		{
			CFileStatus status;
			CFile::GetStatus(strPath, status);
			status.m_ctime.GetAsSystemTime(locTime);
		}
		else
			ConvertStringToSysTime(strStartTime, locTime, 3);

		WCHAR wcsTime[TIME_STRING_LEN] = {'\0'};
		ConvertSysTimeToString(locTime, wcsTime, 0);
		strBakPath += wcsTime;
		strBakPath += _T(".") + g_snpFileExName;

		CFile::Rename(strPath, strBakPath);
		break;
	}

	tempFind.Close();

	m_strPathname = strPath;
	if ( bFileExist )
		return;

	std::wofstream snpFile;
	snpFile.open(strPath);
	snpFile.imbue(g_loc);

	if ( !g_bAbstimeInSnapshotFile )
	{
		FILETIME ftime;
		GetSystemTimeAsFileTime(&ftime);
		SYSTEMTIME locTime;
		FILETIME locftime;
		FileTimeToLocalFileTime(&ftime, &locftime);
		FileTimeToSystemTime(&locftime, &locTime);
		m_startTime = 1.0e-7*(*(ULONGLONG*)&locftime);
		WCHAR wcsTime[TIME_STRING_LEN] = {'\0'};
		ConvertSysTimeToString( locTime, wcsTime, 3 );
		_stprintf_s(textline,_MAX_PATH,_T("%s"), wcsTime);
		snpFile << textline << std::endl;
	}

	_stprintf_s(textline, BLOGBUFMAX, strChanIDs);
	snpFile << textline << std::endl;
	strUnits.Replace(_T("%"), _T("%%"));
	_stprintf_s(textline, BLOGBUFMAX, strUnits);
	snpFile << textline << std::endl;
	_stprintf_s(textline, BLOGBUFMAX, strNames);
	snpFile << textline << std::endl;
	snpFile.flush();
	snpFile.close();

	CFileStatus stat;
	CFile::GetStatus(strPath, stat);
	stat.m_ctime = CTime::GetCurrentTime();
	CFile::SetStatus(strPath, stat);
	m_bFirstSnapshot = TRUE;
}

HRESULT CSnapshotManager::ExecuteSnapshotForTrunk()
{
	ExpLabService* pService = ExpLabService::Open();
	CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
	GLOBAL_SETTING* pGlobalSetting = pService->GetGlobalSetting();
	ExpLabService::Close();

	std::wofstream snpFileTemp;
	snpFileTemp.open(m_strPathname, ios_base::_Nocreate);
	bool bOpen = snpFileTemp.is_open();
	snpFileTemp.close();
	if ( !bOpen )
	{
		PrepareSnapshotFile();
		m_bFirstSnapshot = TRUE;
	}

	LONG nLevel = GetPrivateProfileInt(_T("SnapshotManager"), _T("Level"), 0, ExpLabService::Open()->GetIniFilePath());
	std::wofstream snpFile;
	snpFile.open(m_strPathname, ios_base::_Nocreate);
	snpFile.imbue(g_loc);
	snpFile.seekp(0,ios_base::end);
	snpFile.setf(std::ios::fixed);
	snpFile.precision(pGlobalSetting->lShowPrecision);
	snpFile.setf(std::ios::adjustfield);

	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	FILETIME locftime;
	FileTimeToLocalFileTime(&time, &locftime);
	snpFile.width(VALUE_WIDTH);
	if ( g_bAbstimeInSnapshotFile )
	{
		SYSTEMTIME locTime;
		FileTimeToSystemTime(&locftime, &locTime);

		WCHAR wcsTime[TIME_STRING_LEN] = {'\0'};
		ConvertSysTimeToString(locTime, wcsTime, 3);
		snpFile << wcsTime;
	}
	else
	{
		DOUBLE dtimespan = 1.0e-7*(*(ULONGLONG*)&locftime) - m_startTime;
		snpFile << dtimespan;
	}

	LONG nChanIndex = 0;
	DOUBLE dForce = 0;
	CComPtr<IEnumVARIANT> spEnum;
	m_chanList->get__NewEnum((IUnknown**)&spEnum);

#if (EXPLAB_H31_VERSION == EXPLAB_VERSION)
	CString strTempPath = m_strPathname;
	GetParentLevel(strTempPath);


	COleVariant varFileName;
	pSetting->GetValue(GLOBAL_SAVE_FILE_NAME, &varFileName);
	CString strFileName(V_BSTR(&varFileName));

	CString strPath = strTempPath +_T("\\") + strFileName + _T(".dat");
	std::wofstream dataFile;
	dataFile.open(strPath, ios_base::app);
	dataFile.precision(pGlobalSetting->lShowPrecision);
	dataFile.setf(std::ios::fixed);
#endif

	const ULONG nBatchSize = 3;
	ULONG	nReturned = 0;
	VARIANT arrVariant[nBatchSize] = {0};
	HRESULT hr = E_UNEXPECTED;
	do
	{
		hr = spEnum->Next(nBatchSize, &arrVariant[0], &nReturned);
		if (FAILED(hr))
			break;

		for ( ULONG i = 0; i < nReturned; ++i,++nChanIndex )
		{
			CComQIPtr<IUrtChannel> pChan(arrVariant[i].pdispVal);
			CComQIPtr<IUrtTdmCurveData> pCurve(pChan);
			::VariantClear(&arrVariant[i]);

			CComBSTR bstrChanId;
			pChan->get_chanID(&bstrChanId);

			std::map<CString, bool>::iterator iter = m_mapRecordChan.find(CString(bstrChanId));
			if (iter != m_mapRecordChan.end())
			{
				if (!iter->second)
					continue;
			}

			DOUBLE dVal = 0.0;
			pCurve->GetSnapshotResult(&dVal, NULL);
			snpFile.width(VALUE_WIDTH);
			snpFile << dVal;
			
#if (EXPLAB_H31_VERSION == EXPLAB_VERSION)
			dataFile << dVal << endl;
#endif
			if ( m_nLevelSrc == 1 || m_nForceChan != nChanIndex )
				continue;

			dForce = dVal;
			if ( !m_bFirstSnapshot )
				continue;

			m_bFirstSnapshot = FALSE;
			CComQIPtr<IUrtObject> pObj(pChan);
			EUrtChannelType eChanType;
			LONGLONG llSubChanType;
			pChan->get_chanType(&eChanType);
			pChan->get_chanSubType(&llSubChanType);
			if ( eChanType == eCHT_PROTOCOL && eCHTPROTOCOLMOOG )
			{
				dForce = 0.0;
			}

			pSetting->SetValue(GLOBAL_SNAPSHOT_FORCE_CHAN_ZERO, _variant_t(dForce));

			CComQIPtr<IUrtZeroSetting> pZeroSetting(pCurve);
			pZeroSetting->put_zeroValue(dForce);

			CDocument* pSnapshotDoc = theApp.GetActiveSnapshotDoc();
			if ( pSnapshotDoc == NULL )
				continue;

			POSITION pos = pSnapshotDoc->GetFirstViewPosition();
			while ( pos )
			{
				CView* pView = pSnapshotDoc->GetNextView(pos);
				pView->PostMessage(WM_COMMAND, ID_UPDATE_SNAPSHOT_SETTING);
			}
		}

	} while ( hr != S_FALSE ); // S_FALSE indicates end of collection

	snpFile.width(VALUE_WIDTH);
	if ( m_nLevelSrc == 1 )
	{
		CString strLevelForce = ExpLabService::Open()->GetPrivateProfileString(_T("SnapshotManager"), _T("LevelForce"), g_snapshotDefautLevels);
		LONG nLevel = GetPrivateProfileInt(_T("SnapshotManager"), _T("Level"), 0, ExpLabService::Open()->GetIniFilePath());
		CString strSub;
		AfxExtractSubString(strSub, strLevelForce, nLevel, ',');
		dForce = _tstof(strSub);
	}
	snpFile << dForce;
	snpFile << endl;
	snpFile.flush();
	snpFile.close();

#if (EXPLAB_H31_VERSION == EXPLAB_VERSION)
	dataFile.flush();
	dataFile.close();
#endif

	// 重置快照信号
	::ResetEvent(m_snapshotParam.hDoSnapshot);
	CString strCurrentTime = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	pMainFrm->SetWindowText(_T("Snapshot ") + strCurrentTime);
	ExpLabService::Open()->WritePrivateProfileInt(_T("SnapshotManager"), _T("Level"), ++nLevel); // 准备下级
	pMainFrm->UpdateSnapshotView(ID_EXECUTE_SNAPSHOT_ACQUIRE);

	return 0L;
}

void CSnapshotManager::PrepareSnapshotFileForTrunkCsv()
{
#define BLOGBUFMAX (1<<17)
	WCHAR textline[BLOGBUFMAX+1] = {0};	// 64k WORDs
	CString strChanIDs, strUnits, strNames;

	strChanIDs.Format(_T("%s,"), _T("ChanID"));
	strUnits.Format(_T("%s,"), _T("Unit"));
	if ( g_bAbstimeInSnapshotFile )
	{
		strChanIDs += _T(",");
		strUnits += _T(",");
	}

	if ( g_bBlockInSnapshotFile )
	{
		strChanIDs += _T(",");
		strUnits += _T(",");
	}

	strNames = _T("");
	if ( g_bAbstimeInSnapshotFile )
		strNames.AppendFormat(_T("%s,"), _T("Datetime"));

	strNames.AppendFormat(_T("%s,"), _T("Second"));
	if ( g_bBlockInSnapshotFile )
		strNames.AppendFormat(_T("%s,"), _T("Block"));

	ExpLabService* pService = ExpLabService::Open();
	CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
	ExpLabService::Close();

	COleVariant varPath;
	pSetting->GetValue(GLOBAL_SNAPSHOT_FILEPATH, &varPath);
	CString strPath(V_BSTR(&varPath));

	m_nLevelSrc = GetPrivateProfileInt(_T("Snapshot"), _T("LevelSrc"), m_nLevelSrc, pService->GetIniFilePath());

	LONG nChanIndex = 0;

	CComPtr<IEnumVARIANT> spEnum;
	m_chanList->get__NewEnum((IUnknown**)&spEnum);

	const ULONG nBatchSize = 3;
	ULONG	nReturned = 0;
	VARIANT arrVariant[nBatchSize] = {0};
	m_nForceChan = -1;
	HRESULT hr = E_UNEXPECTED;
	do
	{
		hr = spEnum->Next(nBatchSize, &arrVariant[0], &nReturned);
		if (FAILED(hr))
			break;

		for ( ULONG i = 0; i < nReturned; ++i,++nChanIndex )
		{
			CComQIPtr<IUrtChannel> pChan(arrVariant[i].pdispVal);
			CComQIPtr<IUrtTdmCurveData> pCurve(pChan);
			::VariantClear(&arrVariant[i]);

			CComBSTR bstrChanId;
			pChan->get_chanID(&bstrChanId);

			std::map<CString, bool>::iterator iter = m_mapRecordChan.find(CString(bstrChanId));
			if (iter != m_mapRecordChan.end())
			{
				if (!iter->second)
					continue;
			}

			::VariantClear(&arrVariant[i]);
			CComBSTR chanGroup, chanID, chanName, chanUnit;
			pChan->get_chanGroup(&chanGroup);
			pChan->get_chanName(&chanName);
			if ( chanName.m_str == NULL )
			{
				CComQIPtr<IUrtParamChannel> pParam(pChan);
				pParam->get_paramName(&chanName);
			}

			pChan->get_chanID(&chanID);
			pCurve->get_engineUnit(&chanUnit);

			EUrtConversionSign convSign;
			pCurve->get_conversionSign(&convSign);
			if ( m_nLevelSrc == 0 && eConvSStressLoad == convSign )
			{
				if ( m_nForceChan != -1 )
					pCurve->put_conversionSign(convSign);
				else
					m_nForceChan = nChanIndex;
			}

			DOUBLE dVal = 0.0;
			pCurve->GetSnapshotResult(&dVal, NULL);
			CString str;
			str.Format(_T("%s,"), chanID);
			strChanIDs += str;
			str.Format(_T("%s,"), chanUnit);
			strUnits += str;
			str.Format(_T("%s,"), CString(chanName));
			strNames += str;
		}

	} while ( hr != S_FALSE ); // S_FALSE indicates end of collection

	if ( NEED_LOAD_FORCE )
	{
		CString str;
		str.Format(_T("%s"), STR_LOAD_FORCE);
		strChanIDs += str;
		str.Format(_T("%s"), _T("N"));
		strUnits += str;
		str.Format(_T("%s"), _T("Force"));
		strNames += str;
	}

	CString strDir = strPath.Mid(0, strPath.ReverseFind(_T('\\')));
	Urt_CreatePath(strDir);

	BOOL bFileExist = FALSE;
	CFileFind tempFind;
	BOOL bFound = tempFind.FindFile(strPath);
	while ( bFound )
	{
		bFound = tempFind.FindNextFile();
		if ( tempFind.IsDirectory() )
			continue;

		CFileStatus status;
		CFile::GetStatus(strPath, status);
		if (status.m_size == 0)
			break;

		streamsize readsize = status.m_size>BLOGBUFMAX ? BLOGBUFMAX : (streamsize)status.m_size;
		std::wifstream temp;
		temp.open(strPath);
		temp.imbue(g_loc);
		temp.read(textline, readsize);
		temp.close();

		textline[readsize] = _T('');

		CString strFileHead(textline);
		CString strStartTime,ChanIDs,Units,Names;
		int idx = 0;
		if ( !g_bAbstimeInSnapshotFile )
			AfxExtractSubString(strStartTime, strFileHead,	idx++, _T('\n'));

		AfxExtractSubString(ChanIDs, strFileHead,		idx++, _T('\n'));
		AfxExtractSubString(Units, strFileHead,			idx++, _T('\n'));
		AfxExtractSubString(Names, strFileHead,			idx++, _T('\n'));
		if ( ChanIDs == strChanIDs && Units == strUnits && Names == strNames )
		{
			bFileExist = TRUE;

			if ( g_bAbstimeInSnapshotFile )
				break;

			SYSTEMTIME locTime;
			ConvertStringToSysTime(strStartTime, locTime, 3);
			FILETIME locftime;
			SystemTimeToFileTime(&locTime, &locftime);
			m_startTime = 1.0e-7*(*(ULONGLONG*)&locftime);
			break;
		}

		CString strBakPath = strPath;
		GetParentLevel(strBakPath);
		strBakPath += "\\";

		SYSTEMTIME locTime;
		if ( g_bAbstimeInSnapshotFile )
		{
			CFileStatus status;
			CFile::GetStatus(strPath, status);
			status.m_ctime.GetAsSystemTime(locTime);
		}
		else
			ConvertStringToSysTime(strStartTime, locTime, 3);

		WCHAR wcsTime[TIME_STRING_LEN] = {'\0'};
		ConvertSysTimeToString(locTime, wcsTime, 0);
		strBakPath += wcsTime;
		strBakPath += _T(".") + g_snpFileExName;

		CFile::Rename(strPath, strBakPath);
		break;
	}

	tempFind.Close();

	m_strPathname = strPath;
	if ( bFileExist )
		return;

	std::wofstream snpFile;
	snpFile.open(strPath);
	snpFile.imbue(g_loc);

	if ( !g_bAbstimeInSnapshotFile )
	{
		FILETIME ftime;
		GetSystemTimeAsFileTime(&ftime);
		SYSTEMTIME locTime;
		FILETIME locftime;
		FileTimeToLocalFileTime(&ftime, &locftime);
		FileTimeToSystemTime(&locftime, &locTime);
		m_startTime = 1.0e-7*(*(ULONGLONG*)&locftime);
		WCHAR wcsTime[TIME_STRING_LEN] = {'\0'};
		ConvertSysTimeToString( locTime, wcsTime, 3 );
		_stprintf_s(textline,_MAX_PATH,_T("%s"), wcsTime);
		snpFile << textline << std::endl;
	}

	int len = strChanIDs.GetLength();
	if ( len > 1<<13 )
	{
		for ( int pos=0; pos<len; pos+=1<<13 )
		{
			CString str = strChanIDs.Mid(pos, 1<<13);
			_stprintf_s(textline, BLOGBUFMAX, str);
			snpFile << textline;
		}
		snpFile << std::endl;
	}
	else
	{
		_stprintf_s(textline, BLOGBUFMAX, strChanIDs);
		snpFile << textline << std::endl;
	}

	strUnits.Replace(_T("%"), _T("%%"));
	_stprintf_s(textline, BLOGBUFMAX, strUnits);
	snpFile << textline << std::endl;
	_stprintf_s(textline, BLOGBUFMAX, strNames);
	snpFile << textline << std::endl;
	snpFile.flush();
	snpFile.close();

	CFileStatus stat;
	CFile::GetStatus(strPath, stat);
	stat.m_ctime = CTime::GetCurrentTime();
	CFile::SetStatus(strPath, stat);
	m_bFirstSnapshot = TRUE;
}

HRESULT CSnapshotManager::ExecuteSnapshotForTrunkCSV()
{
	ExpLabService* pService = ExpLabService::Open();
	CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
	GLOBAL_SETTING* pGlobalSetting = pService->GetGlobalSetting();
	ExpLabService::Close();

	std::wofstream snpFileTemp;
	snpFileTemp.open(m_strPathname, ios_base::_Nocreate);
	bool bOpen = snpFileTemp.is_open();
	snpFileTemp.close();
	if ( !bOpen )
	{
		PrepareSnapshotFile();
		m_bFirstSnapshot = TRUE;
	}

	LONG nLevel = GetPrivateProfileInt(_T("SnapshotManager"), _T("Level"), 0, ExpLabService::Open()->GetIniFilePath());

	std::wofstream snpFileTry;
	snpFileTry.open(m_strPathname, ios_base::_Nocreate);
	if ( snpFileTry )
	{
		snpFileTry.close();
	}
	else
	{
		std::wifstream fi;
		fi.open(m_strPathname, std::ios::in);
		CComPtr<ITDMLogOutput> pLogOutput = pService->GetLogOutputService();
		pLogOutput->OutputInfo(eInfoError, EXPLAB_TITLE_NAME, CComBSTR(_T("Access denied. ")+m_strPathname), ERROR_ACCESS_DENIED);
		CString newDataPath = CUtilityFile::AddNumberToFileName(m_strPathname).c_str();
		pService->GetGlobalSettingService()->SetValue(GLOBAL_SNAPSHOT_FILEPATH, _variant_t(newDataPath));

		CDocument* pSnapshotDoc = theApp.GetActiveSnapshotDoc();
		if ( pSnapshotDoc != NULL )
		{
			POSITION pos = pSnapshotDoc->GetFirstViewPosition();
			while ( pos )
			{
				CView* pView = pSnapshotDoc->GetNextView(pos);
				CSnapshotView* pSnapshotView = DYNAMIC_DOWNCAST(CSnapshotView, pView);
				if( pSnapshotView == NULL )
					continue;

				pSnapshotView->SetDlgItemText(IDC_EDT_PATH, newDataPath);
				pSnapshotView->m_datatPath = newDataPath;
				break;
			}
		}

		pLogOutput->OutputInfo(eInfoInfo, EXPLAB_TITLE_NAME, CComBSTR(_T("Change to ")+newDataPath), 0);
		std::wofstream snpFileTemp;
		snpFileTemp.open(newDataPath);
		snpFileTemp.imbue(g_loc);

		fi.imbue(g_loc);
		const int MaxBufSize = 1024*16;
		TCHAR strChar[MaxBufSize];
		ULONGLONG nFilePos = 0;
		fi.getline(strChar, MaxBufSize);
		do 
		{
			fi.getline(strChar, MaxBufSize);
			if ( strChar == _T('') )
				break;

			snpFileTemp << strChar << '\n';
		} while (true);

		fi.close();
		snpFileTemp.flush();
		snpFileTemp.close();
		CFile::Rename(m_strPathname, newDataPath);
		m_strPathname = newDataPath;
	}

	std::wofstream snpFile;
	snpFile.open(m_strPathname, ios_base::_Nocreate);
	snpFile.imbue(g_loc);
	snpFile.seekp(0,ios_base::end);
	snpFile.setf(std::ios::fixed);
	snpFile.precision(pGlobalSetting->lShowPrecision);
	snpFile.setf(std::ios::adjustfield);

	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	FILETIME locftime;
	FileTimeToLocalFileTime(&time, &locftime);
	if ( g_bAbstimeInSnapshotFile )
	{
		SYSTEMTIME locTime;
		FileTimeToSystemTime(&locftime, &locTime);

		WCHAR wcsTime[TIME_STRING_LEN] = {'\0'};
		ConvertSysTimeToString(locTime, wcsTime, 7);
		snpFile << wcsTime << _T(',');
	}
	else
	{
		DOUBLE dtimespan = 1.0e-7*(*(ULONGLONG*)&locftime) - m_startTime;
		snpFile << dtimespan << _T(',');
	}

	LONG nChanIndex = 0;
	DOUBLE dForce = 0;
	CComPtr<IEnumVARIANT> spEnum;
	m_chanList->get__NewEnum((IUnknown**)&spEnum);

#if (EXPLAB_H31_VERSION == EXPLAB_VERSION)
	CString strTempPath = m_strPathname;
	GetParentLevel(strTempPath);

	COleVariant varFileName;
	pSetting->GetValue(GLOBAL_SAVE_FILE_NAME, &varFileName);
	CString strFileName(V_BSTR(&varFileName));

	CString strPath = strTempPath +_T("\\") + strFileName + _T(".dat");
	std::wofstream dataFile;
	dataFile.open(strPath, ios_base::app);
	dataFile.precision(pGlobalSetting->lShowPrecision);
	dataFile.setf(std::ios::fixed);
#endif

	if ( g_bBlockInSnapshotFile )
	{
		snpFile << m_snapshotParam.curSnapshotBlock;
		snpFile << _T(',');
	}

	const ULONG nBatchSize = 3;
	ULONG	nReturned = 0;
	VARIANT arrVariant[nBatchSize] = {0};
	HRESULT hr = E_UNEXPECTED;

	int iSnapshotValueType = GetPrivateProfileInt(_T("Snapshot"), _T("valueType"), 0, ExpLabService::Open()->GetIniFilePath());
	ExpLabService::Close();
	do
	{
		hr = spEnum->Next(nBatchSize, &arrVariant[0], &nReturned);
		if (FAILED(hr))
			break;

		for ( ULONG i = 0; i < nReturned; ++i,++nChanIndex )
		{
			CComQIPtr<IUrtChannel> pChan(arrVariant[i].pdispVal);
			CComQIPtr<IUrtTdmCurveData> pCurve(pChan);
			::VariantClear(&arrVariant[i]);

			CComBSTR bstrChanId;
			pChan->get_chanID(&bstrChanId);

			std::map<CString, bool>::iterator iter = m_mapRecordChan.find(CString(bstrChanId));
			if (iter != m_mapRecordChan.end())
			{
				if (!iter->second)
					continue;
			}

			DOUBLE dVal = 0.0;
			switch ( iSnapshotValueType )
			{
			case eSnpData_TOTAL:
				pCurve->GetSnapshotResult(&dVal, NULL);
				break;
			case eSnpData_CUR:
				pCurve->get_curPhysValue(&dVal);
				break;
			case eSnpData_MAX:
				pCurve->get_curMaxPhysValue(&dVal);
				break;
			case eSnpData_MIN:
				pCurve->get_curMinPhysValue(&dVal);
				break;
			}
			
			snpFile << dVal << _T(',');
			
#if (EXPLAB_H31_VERSION == EXPLAB_VERSION)
			dataFile << dVal << endl;
#endif
			if ( m_nLevelSrc == 1 || m_nForceChan != nChanIndex )
				continue;

			dForce = dVal;
			if ( !m_bFirstSnapshot )
				continue;

			m_bFirstSnapshot = FALSE;
			
			pSetting->SetValue(GLOBAL_SNAPSHOT_FORCE_CHAN_ZERO, _variant_t(dForce));

			CComQIPtr<IUrtZeroSetting> pZeroSetting(pCurve);
			pZeroSetting->put_zeroValue(dForce);

			CDocument* pSnapshotDoc = theApp.GetActiveSnapshotDoc();
			if ( pSnapshotDoc == NULL )
				continue;

			POSITION pos = pSnapshotDoc->GetFirstViewPosition();
			while ( pos )
			{
				CView* pView = pSnapshotDoc->GetNextView(pos);
				pView->PostMessage(WM_COMMAND, ID_UPDATE_SNAPSHOT_SETTING);
			}
		}

	} while ( hr != S_FALSE ); // S_FALSE indicates end of collection

	// [9/14/2012 Easy] 只有611项目需要载荷通道
	if ( NEED_LOAD_FORCE )
	{
		if ( m_nLevelSrc == 1 )
		{
			CString strLevelForce = ExpLabService::Open()->GetPrivateProfileString(_T("SnapshotManager"), _T("LevelForce"), g_snapshotDefautLevels);
			LONG nLevel = GetPrivateProfileInt(_T("SnapshotManager"), _T("Level"), 0, ExpLabService::Open()->GetIniFilePath());
			CString strSub;
			AfxExtractSubString(strSub, strLevelForce, nLevel, ',');
			dForce = _tstof(strSub);
		}
		snpFile << dForce << _T(',');
	}
	
	snpFile << endl;
	snpFile.flush();
	snpFile.close();

#if (EXPLAB_H31_VERSION == EXPLAB_VERSION)
	dataFile.flush();
	dataFile.close();
#endif

	// 重置快照信号
	::ResetEvent(m_snapshotParam.hDoSnapshot);
	CString strCurrentTime = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

	if ( NEED_LOAD_FORCE )
		ExpLabService::Open()->WritePrivateProfileInt(_T("SnapshotManager"), _T("Level"), ++nLevel); // 准备下级

	pMainFrm->SetWindowText(_T("Snapshot ") + strCurrentTime);
	pMainFrm->UpdateSnapshotView(ID_EXECUTE_SNAPSHOT_ACQUIRE);

	return 0L;
}

HRESULT CSnapshotManager::ExecuteSnapshot()
{
	HRESULT hRet = S_FALSE;

	switch ( g_snapshotType )
	{
	default:
	case eSNAPSHOTTYPE_CSV:
		hRet = ExecuteSnapshotForTrunkCSV();
		break;
	case eSNAPSHOTTYPE_SNP:
		hRet = ExecuteSnapshotForTrunk();
		break;
	case eSNAPSHOTTYPE_TEXT:
		hRet = ExecuteSnapshotFor7021();
		break;
	case eSNAPSHOTTYPE_VIEWONLY:
		// 不在这里写快照文件
		dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd())->UpdateSnapshotView(ID_EXECUTE_SNAPSHOT_ACQUIRE);
		::ResetEvent(m_snapshotParam.hDoSnapshot);
		return S_OK;
	}

	// 执行完快照后激活快照事件
	ExecuteEventOperator(EVENT_SYSID_AFTER_SNAPSHOT, eOperSetEvent);

	return hRet;
}

void CSnapshotManager::PrepareSnapshotFile()
{
	switch ( g_snapshotType )
	{
	case eSNAPSHOTTYPE_SNP:
		PrepareSnapshotFileForTrunk();
		break;
	case eSNAPSHOTTYPE_CSV:
		PrepareSnapshotFileForTrunkCsv();
		break;
	case eSNAPSHOTTYPE_TEXT:
		PrepareSnapshotFileFor7021();
		break;
	case eSNAPSHOTTYPE_VIEWONLY:
		// 不在这里写快照文件
		break;
	default:
		PrepareSnapshotFileForTrunkCsv();
		break;
	}
}

HRESULT CSnapshotManager::StartSnapshot(LONG lParam)
{
	if ( m_snapshotState == eServiceStarting)
		return S_OK;

	m_snapshotState = eServiceStarting;
	m_snapshotcount = 0;

	m_hSnapshotThread = ::CreateThread( NULL, 0, 
										(LPTHREAD_START_ROUTINE)CSnapshotManager::SnapshotThreadFunc, 
										(LPVOID)&m_snapshotParam, 
										0,
										NULL );

	if ( m_hSnapshotThread == NULL )
	{
		ASSERT(0);
		return E_FAIL;
	}

	if ( eSNAPSHOTTYPE_TEXT == g_snapshotType )
	{
		ExpLabService* pService = ExpLabService::Open();
		CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
		ExpLabService::Close();
		COleVariant varPath;
		pSetting->GetValue(GLOBAL_SNAPSHOT_FILEPATH, &varPath);
		m_strPathname = varPath.bstrVal;
		COleVariant varDisplayCol;
		if ( S_OK != pSetting->GetValue(GLOBAL_DISPLAY_COL_COUNT, &varDisplayCol) )
			varDisplayCol = LONG(10);

		m_nDisplayCol = varDisplayCol.lVal;
	}

	// [7/15/2013 Easy]
	// 启动快照服务后启动快照事件
	//ExecuteEventOperator(EVENT_SYSID_AFTER_SNAPSHOT, eOperStart);

	return S_OK;
}

HRESULT CSnapshotManager::StopSnapshot(LONG lParam)
{
	if ( m_snapshotState == eServiceStoped )
		return E_FAIL;

	// 结束快照服务后结束快照事件
	//ExecuteEventOperator(EVENT_SYSID_AFTER_SNAPSHOT, eOperStop);

	m_snapshotState = eServiceStoped;

	// 设置结束信号
	::SetEvent(m_snapshotParam.hStopEvent);

	int loopCnt = 0;
	while ( WAIT_TIMEOUT == ::WaitForSingleObject(m_hSnapshotThread, 50) && ++loopCnt<30)
	{
		::Sleep(50);
	}

	ASSERT(loopCnt<30);
	// del [12/31/2014 zcl]
	//// 恢复结束信号
	//::ResetEvent(m_snapshotParam.hStopEvent);

	::CloseHandle(m_hSnapshotThread);
	m_hSnapshotThread = NULL;

	return S_OK;
}

HRESULT CSnapshotManager::DoSnapshot(LONG lParam)
{
	if ( m_snapshotState != eServiceStarting )
		return E_FAIL;

	m_snapshotcount++;
	::SetEvent(m_snapshotParam.hDoSnapshot);

	return S_OK;
}

HRESULT CSnapshotManager::DoSnapshotContinue(DOUBLE snapFreq)
{
	CDocument* pSnapshotDoc = theApp.GetActiveSnapshotDoc();
	if ( pSnapshotDoc == NULL )
		return E_FAIL;

	POSITION pos = pSnapshotDoc->GetFirstViewPosition();
	while ( pos )
	{
		CView* pView = pSnapshotDoc->GetNextView(pos);
		pView->PostMessage(WM_COMMAND, snapFreq!=0
			? ID_SNAPSHOTVIEW_CONTINUE : ID_SNAPSHOTVIEW_CONTINUE_STOP);

		m_coninueSnapshotFreq = snapFreq==0
			? m_coninueSnapshotFreq : snapFreq;
	}

	return S_OK;
}
