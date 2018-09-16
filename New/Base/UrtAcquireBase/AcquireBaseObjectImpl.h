#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <math.h>
#include "UrtAcquireBase.h"
#include "AcquireBaseMacroDef.h"
#include "../../include/NewAdd/urtcombaseimpl.h"
#include "../../include/NewAdd/compile_defaultsamplerate.h"
#include "../../include/NewAdd/compile_explabuser.h"

template <class T>
class ATL_NO_VTABLE IUrtAcquisitionSettingImpl :
	public IDispatchImpl<IUrtAcquisitionSetting, &__uuidof(IUrtAcquisitionSetting), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /*wMinor =*/ 0>
{
public:
	IUrtAcquisitionSettingImpl() 
		: m_dSampleRate(EXPLAB_DEFAULT_SAMPLERATE)
		, m_lBlockSize(EXPLAB_DEFAULT_BLOCKSIZE)
		, m_lTriggerDelay(0)
		, m_lSampleCount(LONG(EXPLAB_DEFAULT_ACQTIME*EXPLAB_DEFAULT_SAMPLERATE))
	{
	}

public:
	STDMETHOD(get_sampleRate)(double * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_dSampleRate;
		return S_OK;
	}
	STDMETHOD(put_sampleRate)(double newVal)
	{
		if ( newVal < 1e-9 || fabs(m_dSampleRate-newVal) < 1e-6 )
			return S_OK;

		m_dSampleRate = newVal;

		URT_SET_MODIFY_FLAG(this, MFLAG_ACQ_SAMPLERATE);
		return S_OK;
	}
	STDMETHOD(get_sampleCount)(long * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lSampleCount;
		return S_OK;
	}
	STDMETHOD(put_sampleCount)(long newVal)
	{
		if ( m_lSampleCount == newVal )
			return S_OK;

		m_lSampleCount = newVal;
		URT_SET_MODIFY_FLAG(this, MFLAG_ACQ_SAMPLCOUNT);
		return S_OK;
	}
	STDMETHOD(get_blockSize)(long * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lBlockSize;
		return S_OK;
	}
	STDMETHOD(put_blockSize)(long newVal)
	{
		if ( newVal < 1 || m_lBlockSize == newVal )
			return S_OK;

		m_lBlockSize = newVal;
		URT_SET_MODIFY_FLAG(this, MFLAG_ACQ_BLOCKSIZE);
		return S_OK;
	}
	STDMETHOD(get_triggerDelay)(long * pVal)
	{
		ATLASSERT(pVal != NULL);
		
		*pVal = m_lTriggerDelay;
		return S_OK;
	}
	STDMETHOD(put_triggerDelay)(long newVal)
	{
		if ( m_lTriggerDelay == newVal )
			return S_OK;

		m_lTriggerDelay = newVal;
		URT_SET_MODIFY_FLAG(this, MFLAG_ACQ_TRIGGERDELAY);
		return S_OK;
	}
	STDMETHOD(get_acquisitionType)(EUrtAcquisitionType* pVal)
	{
		*pVal = eACQST_Unknown;
		return E_NOTIMPL;
	}

public:
	DOUBLE			m_dSampleRate;
	LONG			m_lBlockSize;
	LONG			m_lTriggerDelay;	// 预触发点数
	LONG			m_lSampleCount;		// 采样点数
};

template <class T>
class ATL_NO_VTABLE IUrtAcquireSystemImpl :
	public IDispatchImpl<IUrtAcquireSystem, &__uuidof(IUrtAcquireSystem), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /*wMinor =*/ 0>
{
public:
	IUrtAcquireSystemImpl()
		: m_systemType(eSST_Unknown)
	{

	}
public:
	STDMETHOD(get_systemType)(EUrtAcquireSystemType * pVal)
	{
		if ( pVal == NULL )
		{
			ATLASSERT(0);
			return E_FAIL;
		}

		*pVal = m_systemType;
		return S_OK;
	}

	STDMETHOD(put_systemType)(EUrtAcquireSystemType pVal)
	{
		m_systemType = pVal;
		return S_OK;
	}

protected:
	EUrtAcquireSystemType	m_systemType;
};

template <class T>
class ATL_NO_VTABLE IUrtAcqTimeSettingImpl :
	public IDispatchImpl<IUrtAcqTimeSetting, &__uuidof(IUrtAcqTimeSetting), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /*wMinor =*/ 0>
{
public:
	IUrtAcqTimeSettingImpl()
		: m_eTimeSrc(eATS_ComputerTime)
		, m_isMasterTime(0)
		, m_ulStartTime(0)
		, m_dTimeStart(0.0f)
	{

	}

public:
	STDMETHOD(get_timeSrc)(EUrtAcquireTimeSrc * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_eTimeSrc;
		return S_OK;
	}

	STDMETHOD(put_timeSrc)(EUrtAcquireTimeSrc newVal)
	{
		m_eTimeSrc = newVal;
		return S_OK;
	}

	STDMETHOD(get_supportTimeSrc)(long * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = eATS_ComputerTime;
		return S_OK;
	}

	STDMETHOD(get_isMasterTime)(short * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_isMasterTime;
		return S_OK;
	}

	STDMETHOD(put_isMasterTime)(short newVal)
	{
		m_isMasterTime = newVal;
		return S_OK;
	}

	STDMETHOD(get_startAcqTime)(ULONGLONG * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_ulStartTime;
		return S_OK;
	}

	STDMETHOD(put_startAcqTime)(ULONGLONG newVal)
	{
		m_ulStartTime = newVal;
		return S_OK;
	}

protected:
	EUrtAcquireTimeSrc	m_eTimeSrc;			// 时间来源
	SHORT				m_isMasterTime;		// 是否主时间
	ULONGLONG			m_ulStartTime;		// 起始采集点绝对时间
	DOUBLE				m_dTimeStart;
};

template <class T>
class ATL_NO_VTABLE IUrtCardBaseImpl :
	public IDispatchImpl<IUrtCardBase, &__uuidof(IUrtCardBase), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
protected:
	IUrtCardBaseImpl()
		: m_modeCode(0)
		, m_memorySize(0)
		, m_memorySpace(0)
		, m_cardBaseType(0)
		, m_cardSubType(0)
	{
	}

public:
	STDMETHOD(Create)( LONG cardBaseType, LONG cardSubType)
	{
		ATLASSERT(0);
		return E_NOTIMPL;
	}
	STDMETHOD(get_cardType)( LONGLONG * pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = MAKELONGLONG(m_cardSubType, m_cardBaseType);
		return S_OK;
	}
	STDMETHOD(get_cardBaseType)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_cardBaseType;
		return S_OK;
	}
	STDMETHOD(get_cardSubType)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_cardSubType;
		return S_OK;
	}
	STDMETHOD(get_memorySize)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_memorySize;
		return S_OK;
	}
	STDMETHOD(get_memorySpace)(SHORT* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_memorySpace;
		return S_OK;
	}
	STDMETHOD(put_modelCode)(LONG newVal)
	{
		m_modeCode = newVal;
		return S_OK;
	}
	STDMETHOD(get_modelCode)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_modeCode;
		return S_OK;
	}
	STDMETHOD(get_modelName)(BSTR* pVal)
	{
		ATLASSERT(pVal != NULL);
		return m_modeName.CopyTo(pVal);
	}

public:
	CComBSTR		m_modeName;
	LONG			m_modeCode;
	LONG            m_memorySize;
	SHORT           m_memorySpace;
	LONG			m_cardBaseType;
	LONG			m_cardSubType;	
};

template <class T>
class ATL_NO_VTABLE IUrtAcquireOperatorImpl :
	public IDispatchImpl<IUrtAcquireOperator, &__uuidof(IUrtAcquireOperator), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	IUrtAcquireOperatorImpl()
		: m_pDistTempBuf(NULL)
		, m_pvarTempBuf(NULL)
		, m_startAcqTime(0)
	{
	}

	~IUrtAcquireOperatorImpl()
	{
		ATLASSERT(m_pDistTempBuf == NULL);
		ATLASSERT(m_pvarTempBuf == NULL);

		if ( m_pvarTempBuf != NULL )
		{
			URT_EXPLAB_TOOLS::Urt_DeleteBuffer(m_pvarTempBuf);
			delete m_pvarTempBuf;
			m_pvarTempBuf = NULL;
		}

		if ( m_pDistTempBuf != NULL )
		{
			URT_EXPLAB_TOOLS::Urt_DeleteBuffer(m_pDistTempBuf);
			delete m_pDistTempBuf;
			m_pDistTempBuf = NULL;
		}
	}

public:
	STDMETHOD(get_acqModeType)(EUrtAcquireModeType * pVal)
	{
		if ( pVal != NULL )
			*pVal = eAcqModeContinue;

		return S_OK;
	}

	STDMETHOD(GetAcquirePriority)(EUrtAcqPriority* pVal)
	{
		if ( pVal != NULL )
			*pVal = eACQ_PRIORITY_NORMAL;
		
		return S_OK;
	}


	STDMETHOD(CanAcquire)(VARIANT_BOOL * pVal)
	{
		ATLASSERT(0);
		return E_NOTIMPL;
	}

	STDMETHOD(PrepareAcquire)(long lParam)
	{
		ATLASSERT(0);
		return E_NOTIMPL;
	}

	STDMETHOD(ExecuteAcquire)(long lParam)
	{
		ATLASSERT(0);
		return E_NOTIMPL;
	}

	STDMETHOD(ReadFifoBlock)(VARIANT * pBuffer, long bufferCnt, long * pReadCnt)
	{
		ATLASSERT(0);
		return E_NOTIMPL;
	}

	STDMETHOD(ExecuteDistribute)(long lParam)
	{
		ATLASSERT(0);
		return E_NOTIMPL;
	}

	STDMETHOD(ExecuteRecord)(long lParam)
	{
		if ( m_vActiveChans.empty() )
			return sokRecordCompleted;

		BOOL bRecordFinished = TRUE;
		EUrtRecordState saveSatae  = (EUrtRecordState)(lParam & eDataSaveAll);
		EUrtRecordState recordState= (EUrtRecordState)(lParam & eDataRecordAll);

		ChannelVcetorIter iter = m_vActiveChans.begin();
		for (; iter != m_vActiveChans.end(); ++iter )
		{
			CComQIPtr<IUrtChanAcqOperator> pAcqOper(*iter);
			HRESULT hr = pAcqOper->DoRecord(lParam);

			// 如果所有通道的数据都存储完毕则结束记录
			if ( saveSatae == eDataDoSave && hr != sokRecordCompleted )
			{
				bRecordFinished = FALSE;
			}
		}

		if ( recordState == eDataRecordEnd )
		{
			//　采集完成后清空缓冲区
			if ( m_pDataBuffer != NULL )
				m_pDataBuffer->Clear();

			if ( m_pvarTempBuf != NULL )
				URT_EXPLAB_TOOLS::Urt_DeleteBuffer(m_pvarTempBuf);

			//　采集完成后清空缓冲区
			if ( m_pDistTempBuf != NULL )
				URT_EXPLAB_TOOLS::Urt_DeleteBuffer(m_pDistTempBuf);
		}

		if ( saveSatae == eDataDoSave  )
			return bRecordFinished ? sokRecordCompleted : S_OK;

		return S_OK;
	}

	STDMETHOD(CanRelease)(VARIANT_BOOL * pVal)
	{
		return CanAcquire(pVal);
	}

	STDMETHOD(ExecuteRelease)(long lParam)
	{
		if ( m_vActiveChans.empty() || m_pDistTempBuf == NULL )
			return S_FALSE;

		CComQIPtr<IUrtObject> pThisObj(this);
		if ( pThisObj == NULL )
			return S_FALSE;

		CComQIPtr<IConnectionPointContainer> pPointContainer(this);
		if ( pPointContainer == NULL )
			return S_FALSE;

		CComPtr<IConnectionPoint> pCP;
		if ( FAILED(pPointContainer->FindConnectionPoint(__uuidof(IUrtNetDataSink), &pCP )) )
			return S_FALSE;

		CComPtr<IEnumConnections> pEnum;
		if ( FAILED(pCP->EnumConnections(&pEnum)) )
			return S_FALSE;

		// 发布模式,
		EUrtReleaseMode releaseMode = (EUrtReleaseMode)lParam;
		LONG blockSize = 0;

		// 每个通道一块数据大小
		LONG    nCurPos = 0;
		ULONG   chanSize = 0;
		ULONG   realChanSize = 0;
		bool    bHasTimeChan = false;
		DOUBLE* pDataBuffer = V_R8REF(m_pDistTempBuf);
		DOUBLE  fStartX, fPeriodX, timeChanStartX;

		ChannelVcetorIter iter = m_vActiveChans.begin();
		for (; iter != m_vActiveChans.end(); ++iter )
		{
			CComPtr<IUrtChannel> pChan = *iter;
			CComQIPtr<IUrtChanAcqOperator> pAcqOper(pChan);
			CComQIPtr<IUrtTdmCurveData> pCurve(pAcqOper);
			pCurve->get_blockSize(&blockSize);

			// 判断通道是否需要发布,现在的策略是不是所有的通道都发布
			// 由用户决定需要发布的通道和发布模式,挑点发布模式下,全发布通道不再这里发布
			EUrtReleaseMode chanMode;
			pCurve->get_releaseMode(&chanMode);

			if ( (chanMode == eRelMNone)
			    || (releaseMode == eRelMPick && chanMode == eRelMFull) )
				continue;

			realChanSize++;

			VARIANT varBuff;
			LONG    nLen;
			if ( S_OK == pAcqOper->GetData(&fStartX, &fPeriodX, &varBuff, &nLen) )
			{
				DOUBLE dStartTime = 0.0;
				if ( releaseMode == eRelMPick )
				{
					// 挑点发布模式,通道也是挑点发布模式
					pDataBuffer[nCurPos] = V_R8REF(&varBuff)[nLen-1];
					nCurPos += 1;  // 一包数据取最后一个点进行发布

					dStartTime = pDataBuffer[nCurPos];
				}
				else
				{
					if ( blockSize != nLen )//  [7/16/2012 zcl]
						return errDeviceNotMatching;

					// 全发布模式,不管通道选择的是挑点发布还是全发布,都进行全数据发布
					if ( pDataBuffer != NULL )
						memcpy(&pDataBuffer[nCurPos], V_R8REF(&varBuff), nLen*sizeof(DOUBLE)) ;
					ASSERT(blockSize == nLen);

					if ( pDataBuffer != NULL )
						dStartTime = pDataBuffer[nCurPos]; // 时间为点一个点的相对时间
	
					nCurPos += nLen;
				}

				chanSize++;

				EUrtChannelType chanType;
				LONGLONG chanSubType;
				pChan->get_chanType(&chanType);
				pChan->get_chanSubType(&chanSubType);

				if ( chanType == eCHT_OTHERS && chanSubType == eCHTRELATIVETIME)
				{
					bHasTimeChan = true;
					timeChanStartX = dStartTime;
				}
			}
		}

		// 没有通道需要发布则退出
		if ( chanSize == 0 )
			return S_FALSE;

		if ( realChanSize != chanSize )
			return E_FAIL;

		// 如果有时间通道,则使用时间通道的值来替换掉逻辑计算出来的相对时间
		if ( bHasTimeChan )
			fStartX = timeChanStartX;

		LONG dataCouont = blockSize*chanSize;
		if ( releaseMode == eRelMPick )
		{
			fPeriodX   = fPeriodX*blockSize;
			dataCouont = chanSize;
		}

		CComPtr<IUrtObjId> pObjId;
		pThisObj->GetObjId(&pObjId);

		CComBSTR bstrTag;
		pObjId->ToString(&bstrTag);

		CONNECTDATA rgelt[1];
		ULONG celtFetched;
		while ( S_OK == pEnum->Next(1, rgelt, &celtFetched) )
		{
			CComQIPtr<IUrtNetDataSink> pDataSink(rgelt[0].pUnk);
			pDataSink->PutRealTimeData( fStartX, fPeriodX, dataCouont, *m_pDistTempBuf, bstrTag, m_startAcqTime);

			rgelt[0].pUnk->Release();
		}

		return S_OK;
	}

	STDMETHOD(CanSave)(VARIANT_BOOL * pVal)
	{
		return CanAcquire(pVal);
	}

	STDMETHOD(DoSaveBegin)(long lParam, LPDISPATCH pStorageDisp, BSTR dataPath, BSTR prefix)
	{
		ChannelVcetorIter iter = m_vActiveChans.begin();
		for (; iter != m_vActiveChans.end(); ++iter )
		{
			CComQIPtr<IUrtChanAcqOperator> pAcqOper(*iter);
			pAcqOper->DoSaveBegin(lParam, pStorageDisp, dataPath, prefix);
		}

		return S_OK;
	}

	STDMETHOD(DoSave)(long lParam)
	{
		ChannelVcetorIter iter = m_vActiveChans.begin();
		for (; iter != m_vActiveChans.end(); ++iter )
		{
			CComQIPtr<IUrtChanAcqOperator> pAcqOper(*iter);
			pAcqOper->DoSave(lParam);
		}

		return S_OK;
	}

	STDMETHOD(DoSaveEnd)(long lParam)
	{
		ChannelVcetorIter iter = m_vActiveChans.begin();
		for (; iter != m_vActiveChans.end(); ++iter )
		{
			CComQIPtr<IUrtChanAcqOperator> pAcqOper(*iter);
			pAcqOper->DoSaveEnd(lParam);
		}

		return S_OK;
	}

public:
	VARIANT*		m_pvarTempBuf;
	VARIANT*		m_pDistTempBuf;
	ChannelVector	m_vActiveChans;			// 存放所选择的通道对象
	ULONGLONG		m_startAcqTime;			// 开始采集时间,实际是第一遍数据采集到的时间

	CComPtr<IUrtDataBuffer>	m_pDataBuffer;
};

template <class T>
class ATL_NO_VTABLE IUrtSCPSettingImpl :
	public IDispatchImpl<IUrtSCPSetting, &__uuidof(IUrtSCPSetting), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
protected:
	IUrtSCPSettingImpl()
	{
		::ZeroMemory(m_VSelectScp, sizeof(LONG)*8);
	}

public:
	STDMETHOD(PassScpType)(LONG* pScpSel, LONG nCnt)
	{
		ATLASSERT(pScpSel != NULL && nCnt <= 8 );
		if ( pScpSel != NULL )
		{
			if ( nCnt > 8 )
				nCnt = 8;

			::memcpy(m_VSelectScp, pScpSel, sizeof(LONG)*nCnt);
		}
	
		return S_OK;
	}

	STDMETHOD(GetScpType)(LONG* pScpSel, LONG* nCnt)
	{
		ATLASSERT(pScpSel != NULL);

		ZeroMemory(pScpSel, (*nCnt)*sizeof(LONG));

		if ( (*nCnt) > 8 )
			(*nCnt) = 8;

		::memcpy(pScpSel, m_VSelectScp, sizeof(LONG)*(*nCnt));

		return S_OK;
	}

protected:
	LONG      m_VSelectScp[8];
};

template <class T>
class ATL_NO_VTABLE IUrtPXITriggerImpl :
	public IDispatchImpl<IUrtPXITrigger, &__uuidof(IUrtPXITrigger), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	STDMETHOD(get_RefTriggerMode)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lRefTriggerMode;
		return S_OK;
	}

	STDMETHOD(put_RefTriggerMode)(LONG newVal)
	{
		m_lRefTriggerMode = newVal;
		return S_OK;
	}

	STDMETHOD(get_RefTriggerEdge)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lRefTriggerEdge;
		return S_OK;
	}

	STDMETHOD(put_RefTriggerEdge)(LONG newVal)
	{
		m_lRefTriggerMode = newVal;
		return S_OK;
	}

	STDMETHOD(get_RefTriggerSource)(BSTR* pVal)
	{
		ASSERT(pVal != NULL);
		return m_strRefTiggerSource.CopyTo(pVal);
	}

	STDMETHOD(put_RefTriggerSource)(BSTR newVal)
	{
		m_strRefTiggerSource = newVal;
		return S_OK;
	}
	
	STDMETHOD(get_TriggerMode)(LONG* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_lTriggerMode;
		return S_OK;
	}

	STDMETHOD(put_TriggerMode)(LONG newVal)
	{
		m_lTriggerMode = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerEdge)(LONG* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_lTriggerEdge;
		return S_OK;
	}

	STDMETHOD(put_TriggerEdge)(LONG newVal)
	{
		m_lTriggerEdge = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerSource)(BSTR* pVal)
	{
		ASSERT(pVal != NULL);
		return m_strTiggerSource.CopyTo(pVal);
	}

	STDMETHOD(put_TriggerSource)(BSTR newVal)
	{
		m_strTiggerSource = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerSlope)(LONG* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_lTriggerSlope;
		return S_OK;
	}

	STDMETHOD(put_TriggerSlope)(LONG newVal)
	{
		m_lTriggerSlope = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerWindows)(LONG* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_lTriggerWindows;
		return S_OK;
	}

	STDMETHOD(put_TriggerWindows)(LONG newVal)
	{
		m_lTriggerWindows = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerLevel)(double* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_dTriggerLevel;
		return S_OK;
	}

	STDMETHOD(put_TriggerLevel)(double newVal)
	{
		m_dTriggerLevel = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerWinTop)(double* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_dTriggerWinTop;
		return S_OK;
	}

	STDMETHOD(put_TriggerWinTop)(double newVal)
	{
		m_dTriggerWinTop = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerWinFoot)(double* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_dTriggerWinFoot;
		return S_OK;
	}

	STDMETHOD(put_TriggerWinFoot)(double newVal)
	{
		m_dTriggerWinFoot = newVal;
		return S_OK;
	}

	STDMETHOD(get_TriggerOverTime)(double* pVal)
	{
		ASSERT(pVal != NULL);
		*pVal = m_dTriggerOverTime;
		return S_OK;
	}

	STDMETHOD(put_TriggerOverTime)(double newVal)
	{
		m_dTriggerOverTime = newVal;
		return S_OK;
	}

public:
	long			m_lRefTriggerEdge;					//参考触发沿
	long			m_lRefTriggerMode;					//参考触发模式
	long			m_lTriggerEdge;						//数字触发沿
	long			m_lTriggerMode;						//开始触发模式
	long			m_lTriggerSlope;					//模拟触发斜率
	long			m_lTriggerWindows;					//模拟触发窗方式
	double			m_dTriggerLevel;					//模拟触发电平
	double			m_dTriggerWinTop;					//模拟触发窗顶
	double			m_dTriggerWinFoot;					//模拟触发窗底
	double			m_dTriggerOverTime;					//函数执行延迟
	CComBSTR		m_strRefTiggerSource;				//参考触发源
	CComBSTR		m_strTiggerSource;					//开始触发源
};

template <class T>
class ATL_NO_VTABLE IUrtMasterSlaverSettingImpl :
	public IDispatchImpl<IUrtMasterSlaverSetting, &__uuidof(IUrtMasterSlaverSetting), &LIBID_UrtAcquireBaseLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	IUrtMasterSlaverSettingImpl() 
		: m_lTriggerMsMode(eTrigger_Master)
		, m_lClockMaster(0)
		, m_lenableMaster(0)
	{
	}

public:
	STDMETHOD(get_triggerMaster)(EUrtTriggerDeviceMode* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lTriggerMsMode;
		return S_OK;
	}

	STDMETHOD(put_triggerMaster)(EUrtTriggerDeviceMode newVal)
	{
		m_lTriggerMsMode = newVal;
		return S_OK;
	}

	STDMETHOD(get_clockMaster)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lClockMaster;
		return S_OK;
	}

	STDMETHOD(put_clockMaster)(LONG newVal)
	{
		m_lClockMaster = newVal;
		return S_OK;
	}

	STDMETHOD(get_enableMaster)(LONG* pVal)
	{
		ATLASSERT(pVal != NULL);
		*pVal = m_lenableMaster;
		return S_OK;
	}

	STDMETHOD(put_enableMaster)(LONG newVal)
	{
		m_lenableMaster = newVal;
		return S_OK;
	}

	STDMETHOD(ApplySetting)(LONG level)
	{
		ATLASSERT(0);
		return S_OK;
	}

protected:
	long					m_lenableMaster;					//是否使用主从
	long					m_lClockMaster;						//是否是提供时钟的主设备
	EUrtTriggerDeviceMode	m_lTriggerMsMode;					//是否是提供触发的主设备
};
