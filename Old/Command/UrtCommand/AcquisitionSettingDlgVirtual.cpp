// AcquisitionSettingDlgVirtual.cpp : implementation file
//

#include "stdafx.h"
#include "UrtCommand.h"
#include "AcquisitionSettingDlgVirtual.h"

static const long   BLOCK_SIZE_STEP_VALUE = 10;			// 块大小每次的增加量
static const long   MIN_BLOCK_SIZE = 1;					// 最小的块大小 

// CAcquisitionSettingDlgVirtual dialog

IMPLEMENT_DYNAMIC(CAcquisitionSettingDlgVirtual, CAcquisitionSettingDlgBase)

CAcquisitionSettingDlgVirtual::CAcquisitionSettingDlgVirtual(CWnd* pParent /*=NULL*/)
	: CAcquisitionSettingDlgBase(CAcquisitionSettingDlgVirtual::IDD, pParent)
{
	m_dSampleRate	= 5;
	m_lBlockSize	= 1;
	m_dSampleTimer	= 10000;
	m_lSampleCount	= 50000;
	m_lTriggerDelay	= 0;
}

CAcquisitionSettingDlgVirtual::~CAcquisitionSettingDlgVirtual()
{
	m_pTreeViewProxy->DetachTreeCtrl();
	m_pTreeViewProxy->DetachTreeData();
}

void CAcquisitionSettingDlgVirtual::DoDataExchange(CDataExchange* pDX)
{
	CAcquisitionSettingDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CARD_ACQUIRE_STATIC, m_treeCard);
	DDX_Control(pDX, IDC_EDIT_SAMPLERATE_STATIC, m_editSampleRate);
	DDX_Control(pDX, IDC_EDIT_BLOCKSIZE_STATIC, m_editBlockSize);
	DDX_Text(pDX, IDC_EDIT_SAMPLERATE_STATIC, m_dSampleRate);
	DDX_Text(pDX, IDC_SAMPLE_TIMER_STATIC, m_dSampleTimer);
	DDX_Text(pDX, IDC_PRE_TRIGGER_COUNT_STATIC, m_lTriggerDelay);
	DDX_Text(pDX, IDC_SAMPLE_COUNT_STATIC, m_lSampleCount);
	DDX_Text(pDX, IDC_EDIT_BLOCKSIZE_STATIC, m_lBlockSize);
	DDV_MinMaxDouble(pDX, m_dSampleRate, 0.0, 10000000);
}


BEGIN_MESSAGE_MAP(CAcquisitionSettingDlgVirtual, CAcquisitionSettingDlgBase)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLERATE_STATIC, &CAcquisitionSettingDlgVirtual::OnDeltaposSpinSamplerateStatic)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLOCKSIZE_STATIC, &CAcquisitionSettingDlgVirtual::OnDeltaposSpinBlocksizeStatic)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLETIMER, &CAcquisitionSettingDlgVirtual::OnDeltaposSpinSampletimer)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLECOUNT, &CAcquisitionSettingDlgVirtual::OnDeltaposSpinSamplecount)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIGGERDELAY, &CAcquisitionSettingDlgVirtual::OnDeltaposSpinTriggerdelay)
	ON_EN_CHANGE(IDC_SAMPLE_TIMER_STATIC, &CAcquisitionSettingDlgVirtual::OnEnChangeSampleTimerStatic)
	ON_EN_CHANGE(IDC_SAMPLE_COUNT_STATIC, &CAcquisitionSettingDlgVirtual::OnEnChangeSampleCountStatic)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CARD_ACQUIRE_STATIC, &CAcquisitionSettingDlgVirtual::OnTvnSelchangedTreeCardAcquireStatic)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_CARD_ACQUIRE_STATIC, &CAcquisitionSettingDlgVirtual::OnTvnSelchangingTreeCardAcquireStatic)
	ON_EN_CHANGE(IDC_PRE_TRIGGER_COUNT_STATIC, &CAcquisitionSettingDlgVirtual::OnEnChangePreTriggerCountStatic)
	ON_EN_KILLFOCUS(IDC_EDIT_SAMPLERATE_STATIC, &CAcquisitionSettingDlgVirtual::OnEnKillfocusEditSamplerateStatic)
	ON_EN_KILLFOCUS(IDC_EDIT_BLOCKSIZE_STATIC, &CAcquisitionSettingDlgVirtual::OnEnKillfocusEditBlocksizeStatic)
END_MESSAGE_MAP()


// CAcquisitionSettingDlgVirtual message handlers
BOOL CAcquisitionSettingDlgVirtual::OnInitDialog()
{
	CAcquisitionSettingDlgBase::OnInitDialog();

	UpdateControlsState();

	return TRUE;
}

void CAcquisitionSettingDlgVirtual::DataExchange(BOOL bSave /* = TRUE */)
{
	if ( m_pCurSelSetting == NULL )
		return ;

	if ( bSave )
	{
		m_pCurSelSetting->put_sampleRate(m_dSampleRate);
		m_pCurSelSetting->put_blockSize(m_lBlockSize);
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
	}

}

// 更新各个控件的状态
void CAcquisitionSettingDlgVirtual::UpdateControlsState()
{
	// [12/16/2014 Easy] 不需要合回主版本,因为主版本已经完整实现
	if ( m_pCurSelSetting == NULL )
		return ;

	static const UINT s_IDs[] = {
		IDC_EDIT_SAMPLERATE_STATIC,
		IDC_SPIN_SAMPLERATE_STATIC,
		IDC_EDIT_BLOCKSIZE_STATIC,
		IDC_SPIN_BLOCKSIZE_STATIC,
		IDC_SAMPLE_TIMER_STATIC,
		IDC_SPIN_SAMPLETIMER
	};
	static const int s_nIDs = sizeof(s_IDs)/sizeof(s_IDs[0]);

	EUrtAcquisitionType acqType;
	m_pCurSelSetting->get_acquisitionType(&acqType);

	for ( int i=0; i<s_nIDs; i++ )
	{
		BOOL bEnable = (acqType == eACQST_EX1200_Switch 
						|| acqType == eACQST_EX1200_MultiFunction 
						|| acqType == eACQST_EX1200_Fgen 
						|| acqType == eACQST_EX1200_Dio
						|| acqType == eACQST_EX1200_Scanner);

		CWnd* pWnd = GetDlgItem(s_IDs[i]);
		if ( pWnd != NULL )
			pWnd->EnableWindow(!bEnable); 
	}
}

void CAcquisitionSettingDlgVirtual::OnDeltaposSpinSamplerateStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	m_dSampleRate -= BLOCK_SIZE_STEP_VALUE*pNMUpDown->iDelta;

	if ( m_dSampleRate < 0)
	{
		m_dSampleRate = MIN_BLOCK_SIZE;
	}

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer + 0.5);
	if (m_lSampleCount == 0)
	{
		m_lSampleCount = BLOCK_SIZE_STEP_VALUE;
		m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	}

	UpdateBlocksize();

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgVirtual::OnDeltaposSpinBlocksizeStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	m_lBlockSize -= pNMUpDown->iDelta;

	if ( m_lBlockSize < 1)
	{
		m_lBlockSize = 1;
	}

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgVirtual::OnDeltaposSpinSampletimer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		m_dSampleTimer -= 100.0;
	}
	else
	{
		if (m_lSampleCount == 1 && m_dSampleTimer < 1)
		{
			m_dSampleTimer = 100.0;
		}
		else
		{
			m_dSampleTimer += 100.0;
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

void CAcquisitionSettingDlgVirtual::OnDeltaposSpinSamplecount(NMHDR *pNMHDR, LRESULT *pResult)
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

void CAcquisitionSettingDlgVirtual::OnDeltaposSpinTriggerdelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		m_lTriggerDelay -= 5;
	}
	else
	{
		m_lTriggerDelay += 5;
	}

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgVirtual::OnEnChangeSampleTimerStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgVirtual::OnEnChangeSampleCountStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	if ( m_dSampleTimer < 1e-7 )
		m_dSampleTimer = 1e-7;

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgVirtual::OnTvnSelchangedTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult)
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

void CAcquisitionSettingDlgVirtual::OnTvnSelchangingTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	*pResult = 0;

	// 在当前板卡切换之前把设置保存
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}


void CAcquisitionSettingDlgVirtual::OnEnKillfocusEditBlocksizeStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}


void CAcquisitionSettingDlgVirtual::OnEnChangePreTriggerCountStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}

void CAcquisitionSettingDlgVirtual::OnEnKillfocusEditSamplerateStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);
	UpdateData(FALSE);

	DataExchange(TRUE);
}