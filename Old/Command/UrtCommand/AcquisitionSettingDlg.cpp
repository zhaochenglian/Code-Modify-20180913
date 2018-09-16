// AcquisitionSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UrtCommand.h"
#include "AcquisitionSettingDlg.h"
#include "AcquisitionApplyDlg.h"
#include <map>

typedef std::map<int, CString>			 SampleParamMap;
typedef SampleParamMap::iterator		 SampleParamMapIter;    
typedef SampleParamMap::reverse_iterator SampleParamMapReverseIter;

static SampleParamMap   g_SampleRateMap;
static SampleParamMap   g_BlockSizeMap;

typedef struct tagAddSampleRateMap 
{
	tagAddSampleRateMap(int type, CString strRemark)
	{
		g_SampleRateMap[type] = strRemark;
	}
}AddSampleRateMap;

static AddSampleRateMap g_initSampleRateMap[] =
{
	AddSampleRateMap(102400, _T("102.4 k")),
	AddSampleRateMap(51200,  _T("51.20 k")),
	AddSampleRateMap(25600,  _T("25.60 k")),
	AddSampleRateMap(12800,  _T("12.80 k")),
	AddSampleRateMap(10240,  _T("10.24 k")),
	AddSampleRateMap(6400,   _T("6.400 k")),
	AddSampleRateMap(5120,   _T("5.120 k")),
	AddSampleRateMap(3200,   _T("3.200 k")),
	AddSampleRateMap(2560,   _T("2.560 k")),
	AddSampleRateMap(1600,   _T("1.600 k")),
	AddSampleRateMap(1280,   _T("1.280 k")),
	AddSampleRateMap(800,    _T("800.0")),
	AddSampleRateMap(640,    _T("640.0")),
	AddSampleRateMap(400,    _T("400.0")),
	AddSampleRateMap(320,    _T("320.0")),
	AddSampleRateMap(200,    _T("200.0")),
	AddSampleRateMap(160,    _T("160.0")),
	AddSampleRateMap(100,    _T("100.0")),
	AddSampleRateMap(80,     _T("80.00")),
	AddSampleRateMap(50,     _T("50.00")),
	AddSampleRateMap(40,     _T("40.00")),
	AddSampleRateMap(25,     _T("25.00")),
	AddSampleRateMap(20,     _T("20.00"))
};

typedef struct tagAddBlockSizeMap 
{
	tagAddBlockSizeMap(int type, CString strRemark)
	{
		g_BlockSizeMap[type] = strRemark;
	}
}AddBlockSizeMap;

static AddBlockSizeMap g_initBlockSizeMap[] =
{
	AddBlockSizeMap(32768, _T("32768")),
	AddBlockSizeMap(16384, _T("16384")),
	AddBlockSizeMap(8192,  _T("8192")),
	AddBlockSizeMap(4096,  _T("4096")),
	AddBlockSizeMap(2048,  _T("2048")),
	AddBlockSizeMap(1024,  _T("1024")),
	AddBlockSizeMap(512,   _T("512")),
	AddBlockSizeMap(256,   _T("256")),
	AddBlockSizeMap(128,   _T("128")),
	AddBlockSizeMap(64,    _T("64")),
	AddBlockSizeMap(32,    _T("32"))
};

static const UINT g_acqCtrlID[] = 
{
	IDC_SAMPLE_RATE,
	IDC_BLOCK_SIZE,
	IDC_SAMPLE_TIMER,
	IDC_SAMPLE_COUNT,
	IDC_PRE_TRIGGER_COUNT,
	IDC_SPIN_SAMPLETIMER,
	IDC_SPIN_SAMPLECOUNT,
	IDC_PRE_TRIGGER_COUNT,
	IDC_APPLYTO_ALL
};

static const int g_nAcqCtrlID = sizeof(g_acqCtrlID) / sizeof(g_acqCtrlID[0]);

// CAcquisitionSettingDlg dialog

IMPLEMENT_DYNAMIC(CAcquisitionSettingDlg, CAcquisitionSettingDlgBase)

CAcquisitionSettingDlg::CAcquisitionSettingDlg(CWnd* pParent /*=NULL*/)
	: CAcquisitionSettingDlgBase(CAcquisitionSettingDlg::IDD, pParent)
{
	m_dSampleRate	= 51200;
	m_lBlockSize	= 1024;
	m_dSampleTimer	= 1.0;
	m_lSampleCount	= 1024;
	m_lTriggerDelay	= 0;
}

CAcquisitionSettingDlg::~CAcquisitionSettingDlg()
{
	m_pTreeViewProxy->DetachTreeCtrl();
	m_pTreeViewProxy->DetachTreeData();
}

void CAcquisitionSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CAcquisitionSettingDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CARD_ACQUIRE, m_treeCard);
	DDX_Control(pDX, IDC_SAMPLE_RATE, m_cmbSampleRate);
	DDX_Control(pDX, IDC_BLOCK_SIZE, m_cmbBlockSize);
	DDX_Text(pDX, IDC_SAMPLE_TIMER, m_dSampleTimer);
	DDX_Text(pDX, IDC_PRE_TRIGGER_COUNT, m_lTriggerDelay);
	DDX_Text(pDX, IDC_SAMPLE_COUNT, m_lSampleCount);
	DDV_MinMaxDouble(pDX, m_dSampleTimer, 1e-7, 100000);
	DDV_MinMaxULongLong(pDX, m_lSampleCount, 1, 100000000);
	DDV_MinMaxLong(pDX, m_lTriggerDelay, -16777215, 16777215);
}


BEGIN_MESSAGE_MAP(CAcquisitionSettingDlg, CAcquisitionSettingDlgBase)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CARD_ACQUIRE, &CAcquisitionSettingDlg::OnTvnSelchangedTreeCardAcquire)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_CARD_ACQUIRE, &CAcquisitionSettingDlg::OnTvnSelchangingTreeCardAcquire)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLETIMER, &CAcquisitionSettingDlg::OnDeltaposSpinSampletimer)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLECOUNT, &CAcquisitionSettingDlg::OnDeltaposSpinSamplecount)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIGGERDELAY, &CAcquisitionSettingDlg::OnDeltaposSpinTriggerdelay)
	ON_CBN_SELCHANGE(IDC_SAMPLE_RATE, &CAcquisitionSettingDlg::OnCbnSelchangeSampleRate)
	ON_EN_CHANGE(IDC_SAMPLE_TIMER, &CAcquisitionSettingDlg::OnEnChangeSampleTimer)
	ON_EN_CHANGE(IDC_SAMPLE_COUNT, &CAcquisitionSettingDlg::OnEnChangeSampleCount)
	ON_CBN_SELCHANGE(IDC_BLOCK_SIZE, &CAcquisitionSettingDlg::OnCbnSelchangeBlockSize)
	ON_EN_CHANGE(IDC_PRE_TRIGGER_COUNT, &CAcquisitionSettingDlg::OnEnChangePreTriggerCount)
END_MESSAGE_MAP()

// CAcquisitionSettingDlg message handlers

BOOL CAcquisitionSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	FillObjTree();

	SampleParamMapReverseIter rateIter = g_SampleRateMap.rbegin();
	for ( ; rateIter != g_SampleRateMap.rend(); ++rateIter )
	{
		int nIndex = m_cmbSampleRate.AddString(rateIter->second);
		m_cmbSampleRate.SetItemData(nIndex, rateIter->first);
	}

	SampleParamMapReverseIter blockIter = g_BlockSizeMap.rbegin();
	for ( ; blockIter != g_BlockSizeMap.rend(); ++blockIter )
	{
		int nIndex = m_cmbBlockSize.AddString(blockIter->second);
		m_cmbBlockSize.SetItemData(nIndex, blockIter->first);
	}

	// 缺省选择第一个板卡
	HTREEITEM rootItem = m_treeCard.GetRootItem();
	if ( m_treeCard.ItemHasChildren(rootItem) )
	{
		HTREEITEM hChildItem = m_treeCard.GetChildItem(rootItem);
		m_treeCard.SelectItem(hChildItem);
	}

	UpdateControlsState();
	return TRUE;
}

void CAcquisitionSettingDlg::DataExchange(BOOL bSave /* = TRUE */)
{
	if ( m_pCurSelSetting == NULL )
		return ;

	if ( bSave )
	{
		m_pCurSelSetting->put_triggerDelay(m_lTriggerDelay);

		int nIndex = m_cmbSampleRate.GetCurSel();
		if ( nIndex != CB_ERR )
		{
			m_dSampleRate = (DOUBLE)m_cmbSampleRate.GetItemData(nIndex);
			m_pCurSelSetting->put_sampleRate(m_dSampleRate);
		}

		// 重新计算记录点数,采集时间不变
		m_lSampleCount = (ULONGLONG)(m_dSampleTimer * m_dSampleRate);

		m_pCurSelSetting->put_sampleCount((LONG)m_lSampleCount);

		nIndex = m_cmbBlockSize.GetCurSel();
		if ( nIndex != CB_ERR )
		{
			m_lBlockSize = (LONG)m_cmbBlockSize.GetItemData(nIndex);
			m_pCurSelSetting->put_blockSize(m_lBlockSize);
		}
	}
	else
	{
		m_pCurSelSetting->get_sampleRate(&m_dSampleRate);
		m_pCurSelSetting->get_blockSize(&m_lBlockSize);

		int i;
		for ( i=0; i<m_cmbSampleRate.GetCount(); i++ )
		{
			DWORD_PTR dwSampleRate = m_cmbSampleRate.GetItemData(i);
			if ( dwSampleRate == (DWORD_PTR)m_dSampleRate)
			{
				m_cmbSampleRate.SetCurSel(i);
				break;
			}	
		}

		for ( i=0; i<m_cmbBlockSize.GetCount(); i++ )
		{
			DWORD_PTR dwBlockSize = m_cmbBlockSize.GetItemData(i);
			if ( dwBlockSize == (DWORD_PTR)m_lBlockSize)
			{
				m_cmbBlockSize.SetCurSel(i);
				break;
			}
		}

		m_pCurSelSetting->get_sampleCount((LONG*)&m_lSampleCount);

		m_pCurSelSetting->get_triggerDelay(&m_lTriggerDelay);

		m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	}
}

// 更新各个控件的状态
void CAcquisitionSettingDlg::UpdateControlsState()
{
	BOOL bEnable = TRUE;;
	if ( m_pCurSelSetting == NULL || m_pCurSelNode == NULL )
		bEnable = FALSE;

	for (int i=0; i<g_nAcqCtrlID; i++)
	{
		CWnd* pWnd = GetDlgItem(g_acqCtrlID[i]);
		if ( pWnd != NULL )
			pWnd->EnableWindow(bEnable);
	}

	CComQIPtr<IUrtTrigger>  pTrigger(m_pCurSelSetting);
	if ( pTrigger != NULL )
	{
		EUrtTriggerType type;
		pTrigger->get_triggerType(&type);
		if ( type == eTRIG_TYPE_IMM)
		{
			GetDlgItem(IDC_PRE_TRIGGER_COUNT)->EnableWindow(FALSE);
		}
	}
}

void CAcquisitionSettingDlg::OnTvnSelchangingTreeCardAcquire(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	// 在当前板卡切换之前把设置保存
	if ( !UpdateData(TRUE) )
		return ;
	
	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnTvnSelchangedTreeCardAcquire(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	TVITEM itemNew = pNMTreeView->itemNew;

	m_pCurSelSetting.Release();
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

	DataExchange(FALSE);

	UpdateData(FALSE);

	UpdateControlsState();
}

void CAcquisitionSettingDlg::OnDeltaposSpinSampletimer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		m_dSampleTimer -= 1.0;
	}
	else
	{
		m_dSampleTimer += 1.0;
	}
	if ( m_dSampleTimer < 1e-7)
	{
		m_dSampleTimer = 1e-7;
	}

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnDeltaposSpinSamplecount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0 )
	{
		m_lSampleCount -= (long)m_dSampleRate;
	}
	else
	{
		m_lSampleCount += (long)m_dSampleRate;
	}

	if ( m_lSampleCount < 1 )
	{
		m_lSampleCount = 1;
	}

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	UpdateData(FALSE);

	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnDeltaposSpinTriggerdelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0 )
	{
		m_lTriggerDelay -= m_lBlockSize/2;
		if ( m_lTriggerDelay < -16777215)
			m_lTriggerDelay = -16777215;
	}
	else
	{
		m_lTriggerDelay += m_lBlockSize/2;
		if ( m_lTriggerDelay > 16777215)
			m_lTriggerDelay = 16777215;
	}

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnCbnSelchangeSampleRate()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);

	UpdateData(FALSE);
}

void CAcquisitionSettingDlg::OnEnChangeSampleTimer()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnEnChangeSampleCount()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	if ( m_dSampleTimer < 1e-7 )
		m_dSampleTimer = 1e-7;

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnCbnSelchangeBlockSize()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}

void CAcquisitionSettingDlg::OnEnChangePreTriggerCount()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}
