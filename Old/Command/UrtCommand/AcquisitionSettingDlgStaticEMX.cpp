// AcquisitionSettingDlgStaticEMX.cpp : implementation file
//

#include "stdafx.h"
#include "UrtCommand.h"
#include "AcquisitionSettingDlgStaticEMX.h"

const double MAX_SAMPLE_RATE = 625000.0;			// 最大采样率
const double MIN_SAMPLE_RATE = 1.0;					// 最小采样率
const double SAMPLE_RATE_STEP_VALUE = 5000.0;		// 采样率每次的增加量
const long   BLOCK_SIZE_STEP_VALUE = 1000;			// 块大小每次的增加量
const long   MIN_BLOCK_SIZE = 1;					// 最小的块大小 

const double g_SampleRateBase[] = {
	102400,
	65536,
	51200,
	32768,
	25600,
	16384,
	12800,
	8192,
	6400,
	4096,
	3200,
	2048,
	1600,
	1024,
	800,
	512,
	400,
	256,
	200,
	100,
	50
};
const int g_nSampleRateBase = sizeof(g_SampleRateBase)/sizeof(g_SampleRateBase[0]);

const double g_SampleRateRX0124[] = {
	128000,
	100000,
	78125,
};
const int g_nSampleRateRX0124 = sizeof(g_SampleRateRX0124)/sizeof(g_SampleRateRX0124[0]);

const double g_SampleRate42XX[] = {
	204800,
	131072
};
const int g_nSampleRate42XX = sizeof(g_SampleRate42XX)/sizeof(g_SampleRate42XX[0]);

const double g_SampleRate43XX[] = {
	625000,
	524288,
	409600,
	312500,
	262144,
	156250,
	78125,
};
const int g_nSampleRate43XX = sizeof(g_SampleRate43XX)/sizeof(g_SampleRate43XX[0]);

//const double g_SampleRateRX0124[] = {
//	
//	128000,
//	102400,
//	100000,
//	78125,
//	65536,
//	//64000,
//	51200,
//	//50000,
//	32768,
//	//32000,
//	25600,
//	//25000,
//	20480,
//	//20000,
//	16384,
//	//16000,
//	//15625,
//	12800,
//	//12500,
//	//10240,
//	//10000,
//	8192,
//	//8000,
//	6400,
//	//6250,
//	//5120,
//	//5000,
//	4096,
//	//4000,
//	3200,
//	//3125,
//	//2560,
//	//2500,
//	2048,
//	//2000,
//	1600,
//	//1280,
//	//1250,
//	1024,
//	//1000,
//	800,
//	//640,
//	//625,
//	512,
//	//500,
//	400,
//	//320,
//	256,
//	//250,
//	200,
//	//160,
//	//128,
//	//125,
//	100,
//	//80,
//	//64,
//	50
//	/*40,
//	32,
//	25,
//	20,
//	16,
//	10,
//	8,
//	5,
//	4,
//	2,
//	1	*/
//};

static const UINT g_acqCtrlID[] = 
{
	IDC_COMBO_SAMPLERATE_STATIC,
	IDC_EDIT_BLOCKSIZE,
	IDC_SAMPLE_TIMER_STATIC,
	IDC_SPIN_SAMPLETIMER,
	IDC_SAMPLE_COUNT_STATIC,
	IDC_SPIN_SAMPLECOUNT,
	IDC_PRE_TRIGGER_COUNT_STATIC,
	IDC_SPIN_TRIGGERDELAY
};

static const int g_nAcqCtrlID = sizeof(g_acqCtrlID) / sizeof(g_acqCtrlID[0]);

// CAcquisitionSettingDlgStaticEMX dialog

IMPLEMENT_DYNAMIC(CAcquisitionSettingDlgStaticEMX, CAcquisitionSettingDlgBase)

CAcquisitionSettingDlgStaticEMX::CAcquisitionSettingDlgStaticEMX(CWnd* pParent /*=NULL*/)
	: CAcquisitionSettingDlgBase(CAcquisitionSettingDlgStaticEMX::IDD, pParent)
	, m_lBlockSize(0)
{
	m_dSampleRate	= 12800;
	m_lBlockSize	= 2560;
	m_dSampleTimer	= 200;
	m_lSampleCount	= ULONGLONG(m_dSampleRate*m_dSampleTimer);
	m_lTriggerDelay	= 0;
}

CAcquisitionSettingDlgStaticEMX::~CAcquisitionSettingDlgStaticEMX()
{
	m_pTreeViewProxy->DetachTreeCtrl();
	m_pTreeViewProxy->DetachTreeData();
}

void CAcquisitionSettingDlgStaticEMX::DoDataExchange(CDataExchange* pDX)
{
	CAcquisitionSettingDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CARD_ACQUIRE_STATIC, m_treeCard);
	DDX_Text(pDX, IDC_SAMPLE_TIMER_STATIC, m_dSampleTimer);
	DDX_Text(pDX, IDC_PRE_TRIGGER_COUNT_STATIC, m_lTriggerDelay);
	DDX_Text(pDX, IDC_SAMPLE_COUNT_STATIC, m_lSampleCount);
	DDV_MinMaxDouble(pDX, m_dSampleRate, MIN_SAMPLE_RATE, MAX_SAMPLE_RATE);
	DDX_Control(pDX, IDC_COMBO_SAMPLERATE_STATIC, m_comboSampleRate);
	DDX_Text(pDX, IDC_EDIT_BLOCKSIZE, m_lBlockSize);
}

BEGIN_MESSAGE_MAP(CAcquisitionSettingDlgStaticEMX, CAcquisitionSettingDlgBase)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLETIMER, &CAcquisitionSettingDlgStaticEMX::OnDeltaposSpinSampletimer)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLECOUNT, &CAcquisitionSettingDlgStaticEMX::OnDeltaposSpinSamplecount)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIGGERDELAY, &CAcquisitionSettingDlgStaticEMX::OnDeltaposSpinTriggerdelay)
	ON_EN_CHANGE(IDC_SAMPLE_TIMER_STATIC, &CAcquisitionSettingDlgStaticEMX::OnEnChangeSampleTimerStatic)
	ON_EN_CHANGE(IDC_SAMPLE_COUNT_STATIC, &CAcquisitionSettingDlgStaticEMX::OnEnChangeSampleCountStatic)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CARD_ACQUIRE_STATIC, &CAcquisitionSettingDlgStaticEMX::OnTvnSelchangedTreeCardAcquireStatic)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_CARD_ACQUIRE_STATIC, &CAcquisitionSettingDlgStaticEMX::OnTvnSelchangingTreeCardAcquireStatic)
	ON_EN_CHANGE(IDC_PRE_TRIGGER_COUNT_STATIC, &CAcquisitionSettingDlgStaticEMX::OnEnChangePreTriggerCountStatic)
	ON_CBN_SELCHANGE(IDC_COMBO_SAMPLERATE_STATIC, &CAcquisitionSettingDlgStaticEMX::OnCbnSelchangeComboSamplerateStatic)
END_MESSAGE_MAP()


// CAcquisitionSettingDlgStaticEMX message handlers
BOOL CAcquisitionSettingDlgStaticEMX::OnInitDialog()
{
	CAcquisitionSettingDlgBase::OnInitDialog();
	
	InitValidElem();
	
	UpdateData(FALSE);
	DataExchange(FALSE);
	UpdateControlsState();	
	return TRUE;
}

void CAcquisitionSettingDlgStaticEMX::DataExchange(BOOL bSave /* = TRUE */)
{
	if ( m_pCurSelSetting == NULL )
		return ;

	if ( bSave )
	{
		m_pCurSelSetting->put_sampleRate(m_dSampleRate);

		// 包大小不能通过界面进行设置,因为设备有相关要求
		m_pCurSelSetting->get_blockSize(&m_lBlockSize);
		m_pCurSelSetting->put_sampleCount((LONG)m_lSampleCount);
		m_pCurSelSetting->put_triggerDelay(m_lTriggerDelay);
		
	}
	else
	{
		m_pCurSelSetting->get_sampleRate(&m_dSampleRate);
		m_pCurSelSetting->get_blockSize(&m_lBlockSize);
		m_pCurSelSetting->get_sampleCount((LONG*)&m_lSampleCount);
		m_pCurSelSetting->get_triggerDelay(&m_lTriggerDelay);	
		m_dSampleTimer = m_lSampleCount / m_dSampleRate;
		
		CString strSampleRate;
		strSampleRate.Format(_T("%0.0f"), m_dSampleRate);
		int nIndex = m_comboSampleRate.FindStringExact(-1, strSampleRate);
		if ( nIndex != CB_ERR )
			m_comboSampleRate.SetCurSel(nIndex);
	}

	UpdateData(FALSE);
}

// 更新各个控件的状态
void CAcquisitionSettingDlgStaticEMX::UpdateControlsState()
{
	/*
	if ( m_pCurSelSetting != NULL )
	{
		EUrtAcquisitionType acqType;
		m_pCurSelSetting->get_acquisitionType(&acqType);
		if ( acqType != eACQST_EMX_System )
		{
			for (int i=0; i<g_nAcqCtrlID; i++)
			{
				CWnd* pWnd = GetDlgItem(g_acqCtrlID[i]);
				if ( pWnd != NULL )
					pWnd->EnableWindow(FALSE);
			}
		}
	}*/
}

void CAcquisitionSettingDlgStaticEMX::OnDeltaposSpinSampletimer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		m_dSampleTimer -= SAMPLE_RATE_STEP_VALUE;
	}
	else
	{
		if (m_lSampleCount == 1 && m_dSampleTimer < 1)
		{
			m_dSampleTimer = SAMPLE_RATE_STEP_VALUE;
		}
		else
		{
			m_dSampleTimer += SAMPLE_RATE_STEP_VALUE;
		}
	}
	if ( m_dSampleTimer < 1e-7)
	{
		m_dSampleTimer = 1/m_dSampleRate;
	}

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer + 0.1);

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnDeltaposSpinSamplecount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;
	
	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0 )
	{
		m_lSampleCount -= long(m_dSampleRate - 0.1);
	}
	else
	{
		if (m_lSampleCount == 1 && m_dSampleTimer < 1)
		{
			m_lSampleCount = long(m_dSampleRate + 0.1);
		}
		else
		{
			m_lSampleCount += long(m_dSampleRate + 0.1);
		}
	}

	if ( m_lSampleCount < 1 )
	{
		m_lSampleCount = 1;
	}

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	UpdateData(FALSE);

	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnDeltaposSpinTriggerdelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		m_lTriggerDelay -= BLOCK_SIZE_STEP_VALUE;
	}
	else
	{
		m_lTriggerDelay += BLOCK_SIZE_STEP_VALUE;
	}

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnEnChangeSampleTimerStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnEnChangeSampleCountStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	if ( m_dSampleTimer < 1e-7 )
		m_dSampleTimer = 1e-7;

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnTvnSelchangedTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	*pResult = 0;

	TVITEM itemNew = pNMTreeView->itemNew;

	m_pCurSelNode.Release();

	// 不用AddRef　也不用调Release
	IUrtTreeNode *pTreeNode = (IUrtTreeNode *)m_treeCard.GetItemData(itemNew.hItem);
	if ( pTreeNode == NULL ) 
		return;

	// 新选择的节点和当前显示的sheet对应的节点一样则返回
	CComPtr<IUrtObjId> pOdjId;
	if ( FAILED(pTreeNode->get_objectId(&pOdjId)) )
		return;

	m_pCurSelSetting.Release();
	pOdjId->QueryObjInterface(__uuidof(IUrtAcquisitionSetting), (LPUNKNOWN*)&m_pCurSelSetting);
	m_pCurSelNode = pTreeNode;

	InitValidElem();
	
	DataExchange(FALSE);

	CString strSampleRate;
	strSampleRate.Format(_T("%0.0f"),  m_dSampleRate );
	int nIndex = m_comboSampleRate.FindStringExact(-1, strSampleRate);
	if ( nIndex != CB_ERR )
		m_comboSampleRate.SetCurSel(nIndex);

	UpdateData(FALSE);

	UpdateControlsState();
}

void CAcquisitionSettingDlgStaticEMX::OnTvnSelchangingTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	*pResult = 0;

	// 在当前板卡切换之前把设置保存
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnEnChangePreTriggerCountStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStaticEMX::OnCbnSelchangeComboSamplerateStatic()
{
	int index = m_comboSampleRate.GetCurSel();
	CString strData;
	m_comboSampleRate.GetLBText(index,strData);
	m_dSampleRate = _tstof(strData);	

	if ( m_dSampleRate < MIN_SAMPLE_RATE)
	{
		m_dSampleRate = MIN_SAMPLE_RATE;
	}
	else if ( m_dSampleRate > MAX_SAMPLE_RATE )
	{
		m_dSampleRate = MAX_SAMPLE_RATE;
	}

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer + 0.1);
	if (m_lSampleCount == 0)
	{
		m_lSampleCount = 1;
		m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	}

	UpdateData(FALSE);
	DataExchange(TRUE);
}

int CAcquisitionSettingDlgStaticEMX::InitValidElem()
{
	if ( m_acquisitionType == eACQST_EMX2500 )
	{
		m_setElement.insert(1);
		m_setElement.insert(5);
		m_setElement.insert(10);
		m_setElement.insert(20);
	}
	else
	{
		// 根据系统所加载的EMX板卡确定可以选择什么样的采样率
		while ( m_pCurSelSetting != NULL )
		{
			CComQIPtr<IUrtObject> pObj(m_pCurSelSetting);
			if ( pObj == NULL )
				break;
		
			CComQIPtr<IUrtEMXSystem> pEmxSys(pObj);
			if ( pEmxSys == NULL )
			{
				CComPtr<IUrtObject> pParentObj;
				if ( FAILED(pObj->get_objParent(&pParentObj)) )
					break;

				pEmxSys = pParentObj;
			}
			
			if ( pEmxSys == NULL )
				break;

			for( int i=0; i<g_nSampleRateBase; ++i)
			{
				m_setElement.insert(g_SampleRateBase[i]);
			}

			LONG lAllSubType = 0;
			pEmxSys->GetAllCardSubType(&lAllSubType);

			bool bHas42XX = (lAllSubType & eACT_EMX2500_4250 || lAllSubType & eACT_EMX2500_4251);
			bool bHas43XX = (lAllSubType & eACT_EMX2500_4350) || (lAllSubType & eACT_EMX2500_4380);
			bool bHasRXXX = (lAllSubType & eACT_EMX2500I_RX0124) || (lAllSubType & eACT_EMX2500I_RX0124);

			if ( bHas42XX && !bHas43XX && !bHasRXXX )
			{
				for( int i=0; i<g_nSampleRate42XX; ++i)
				{
					m_setElement.insert(g_SampleRate42XX[i]);
				}
			}
			else if ( !bHas42XX && bHas43XX && !bHasRXXX )
			{
				for( int i=0; i<g_nSampleRate43XX; ++i)
				{
					m_setElement.insert(g_SampleRate43XX[i]);
				}
			}
			else if ( !bHas42XX && !bHas43XX && bHasRXXX )
			{
				for( int i=0; i<g_nSampleRateRX0124; ++i)
				{
					m_setElement.insert(g_SampleRateRX0124[i]);
				}
			}
			else if (!bHas42XX && bHas43XX && bHasRXXX )
			{
				// 78125是43XX与RXXX共有的采样率	
				m_setElement.insert(78125.00);
			}
			break;
		}
	}

	m_comboSampleRate.ResetContent();

	CString strElement;
	for(std::set<double>::iterator it = m_setElement.begin() ; it != m_setElement.end() ; ++it)
	{
		strElement.Format(_T("%0.0f"),*it);
		m_comboSampleRate.AddString(strElement);
	}

	return 0;
}

