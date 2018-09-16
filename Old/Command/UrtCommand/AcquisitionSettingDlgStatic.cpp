// AcquisitionSettingDlgStatic.cpp : implementation file
//

#include "stdafx.h"
#include "UrtCommand.h"
#include "AcquisitionSettingDlgStatic.h"
const long TRIGGER_DELAY_STEP_VALUE = 1;		// 预触发点数每次的增加值
const long MAX_TRIGGER_DELAY = 4294;			// 预触发点数的最大值
const long MIN_TRIGGER_DELAY = 0;               // 预触发点数的最小值

#if EXPLAB_VERSION == EXPLAB_QD712_VERSION
	const double MAX_SAMPLE_RATE = 3.0/0.001;		// 最大采样率
#else
	const double MAX_SAMPLE_RATE = 1.0/0.001;		// 最大采样率
#endif

const double MIN_SAMPLE_RATE = 1.0/4294.0;		// 最小采样率
const double SAMPLE_RATE_STEP_VALUE = 1;		// 采样率每次的增加量
const double TRIGGER_TIMER_STEP_VALUE = 0.1;    // 采样时间每次的增加量
const long   BLOCK_SIZE_STEP_VALUE = 5;         // 块大小每次的增加量
const long   MIN_BLOCK_SIZE = 1;				// 最小的块大小 

// CAcquisitionSettingDlgStatic dialog

IMPLEMENT_DYNAMIC(CAcquisitionSettingDlgStatic, CAcquisitionSettingDlgBase)

CAcquisitionSettingDlgStatic::CAcquisitionSettingDlgStatic(CWnd* pParent /*=NULL*/)
	: CAcquisitionSettingDlgBase(CAcquisitionSettingDlgStatic::IDD, pParent)
{
	m_maxSampleRate = MAX_SAMPLE_RATE;
	m_minSampleRate = MIN_SAMPLE_RATE;
}

CAcquisitionSettingDlgStatic::CAcquisitionSettingDlgStatic(CWnd* pParent, EUrtAcquisitionType acqType)
: CAcquisitionSettingDlgBase(CAcquisitionSettingDlgStatic::IDD, pParent)
{
	m_acquisitionType = acqType;
	m_maxSampleRate = MAX_SAMPLE_RATE;
	m_minSampleRate = MIN_SAMPLE_RATE;
}

CAcquisitionSettingDlgStatic::~CAcquisitionSettingDlgStatic()
{
	m_pTreeViewProxy->DetachTreeCtrl();
	m_pTreeViewProxy->DetachTreeData();
}

BEGIN_MESSAGE_MAP(CAcquisitionSettingDlgStatic, CAcquisitionSettingDlgBase)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLERATE_STATIC, &CAcquisitionSettingDlgStatic::OnDeltaposSpinSamplerateStatic)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLOCKSIZE_STATIC, &CAcquisitionSettingDlgStatic::OnDeltaposSpinBlocksizeStatic)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLETIMER, &CAcquisitionSettingDlgStatic::OnDeltaposSpinSampletimer)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLECOUNT, &CAcquisitionSettingDlgStatic::OnDeltaposSpinSamplecount)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIGGERDELAY, &CAcquisitionSettingDlgStatic::OnDeltaposSpinTriggerdelay)
	ON_EN_CHANGE(IDC_SAMPLE_TIMER_STATIC, &CAcquisitionSettingDlgStatic::OnEnChangeSampleTimerStatic)
	ON_EN_CHANGE(IDC_SAMPLE_COUNT_STATIC, &CAcquisitionSettingDlgStatic::OnEnChangeSampleCountStatic)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CARD_ACQUIRE_STATIC, &CAcquisitionSettingDlgStatic::OnTvnSelchangedTreeCardAcquireStatic)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_CARD_ACQUIRE_STATIC, &CAcquisitionSettingDlgStatic::OnTvnSelchangingTreeCardAcquireStatic)
	ON_EN_CHANGE(IDC_EDIT_BLOCKSIZE_STATIC, &CAcquisitionSettingDlgStatic::OnEnChangeEditBlocksizeStatic)
	ON_EN_CHANGE(IDC_PRE_TRIGGER_COUNT_STATIC, &CAcquisitionSettingDlgStatic::OnEnChangePreTriggerCountStatic)
	ON_EN_KILLFOCUS(IDC_EDIT_SAMPLERATE_STATIC, &CAcquisitionSettingDlgStatic::OnEnKillfocusEditSamplerateStatic)
	ON_EN_CHANGE(IDC_EDIT_SAMPLERATE_STATIC, &CAcquisitionSettingDlgStatic::OnEnChangeEditSamplerateStatic)
END_MESSAGE_MAP()

void CAcquisitionSettingDlgStatic::DoDataExchange(CDataExchange* pDX)
{
	CAcquisitionSettingDlgBase::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_CARD_ACQUIRE_STATIC, m_treeCard);
	DDX_Control(pDX, IDC_EDIT_SAMPLERATE_STATIC, m_editSampleRate);
	DDX_Control(pDX, IDC_EDIT_BLOCKSIZE_STATIC, m_editBlockSize);

	DDX_Text(pDX, IDC_SAMPLE_TIMER_STATIC, m_dSampleTimer);
	DDX_Text(pDX, IDC_SAMPLE_COUNT_STATIC, m_lSampleCount);

	DDX_Text(pDX, IDC_PRE_TRIGGER_COUNT_STATIC, m_lTriggerDelay);
	DDV_MinMaxLong(pDX, m_lTriggerDelay, 0, 4294);

	DDX_Text(pDX, IDC_EDIT_BLOCKSIZE_STATIC, m_lBlockSize);
	DDV_MinMaxLong(pDX, m_lBlockSize, 1, 1024000);

	DDX_Control(pDX, IDC_SAMPLE_TIMER_STATIC, m_editSampleTimer);
	DDX_Control(pDX, IDC_SAMPLE_COUNT_STATIC, m_editSampleCount);
	DDX_Control(pDX, IDC_PRE_TRIGGER_COUNT_STATIC, m_editPreTriggerCount);

	DDX_Text(pDX, IDC_EDIT_SAMPLERATE_STATIC, m_dSampleRate);
	DDV_MinMaxDouble(pDX, m_dSampleRate, m_minSampleRate, m_maxSampleRate);

	DDV_MinMaxDouble(pDX, m_dSampleTimer, 0, 10000000);
}

// CAcquisitionSettingDlgStatic message handlers
BOOL CAcquisitionSettingDlgStatic::OnInitDialog()
{
	CAcquisitionSettingDlgBase::OnInitDialog();

	// 设置采样率提示信息
	CString strSampleRateInfo;
	strSampleRateInfo.Format(_T("%.5lf < ")sDef_SampleRate _T(" < %g"), m_minSampleRate, m_maxSampleRate);
	GetDlgItem(IDD_STATIC_SAMPLE_RATE_PROMPT)->SetWindowText(strSampleRateInfo);

	UpdateControlsState();
	return TRUE;
}

void CAcquisitionSettingDlgStatic::DataExchange(BOOL bSave /* = TRUE */)
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

		if ( m_lTriggerDelay < MIN_TRIGGER_DELAY )
			m_lTriggerDelay = MIN_TRIGGER_DELAY;

		if ( m_lTriggerDelay > MAX_TRIGGER_DELAY)
			m_lTriggerDelay = MAX_TRIGGER_DELAY;
	}
}

// 更新各个控件的状态
void CAcquisitionSettingDlgStatic::UpdateControlsState()
{
	switch (m_acquisitionType)
	{
	case eACQST_VTOPC:
		GetDlgItem(IDC_EDIT_SAMPLERATE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_SAMPLERATE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_BLOCKSIZE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_BLOCKSIZE_STATIC)->EnableWindow(FALSE);
		break;
	case eACQST_DSTP:
		GetDlgItem(IDC_EDIT_SAMPLERATE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_SAMPLERATE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_BLOCKSIZE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAMPLE_TIMER_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_SAMPLETIMER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAMPLE_COUNT_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_SAMPLECOUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE_TRIGGER_COUNT_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_TRIGGERDELAY)->EnableWindow(FALSE);
		break;
	default:
		break;
	}
}

void CAcquisitionSettingDlgStatic::OnDeltaposSpinSamplerateStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		if ((m_dSampleRate - SAMPLE_RATE_STEP_VALUE) > m_minSampleRate)
			m_dSampleRate -= SAMPLE_RATE_STEP_VALUE;
		else
			m_dSampleRate = m_minSampleRate;
	}
	else
	{
		if ((m_dSampleRate + SAMPLE_RATE_STEP_VALUE) < m_maxSampleRate)
			m_dSampleRate += SAMPLE_RATE_STEP_VALUE;
		else
			m_dSampleRate = m_maxSampleRate;
	}
	
	UpdateBlocksize();
	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStatic::OnDeltaposSpinBlocksizeStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	if ( pNMUpDown->iDelta > 0)
	{
		if ((m_lBlockSize - BLOCK_SIZE_STEP_VALUE) > MIN_BLOCK_SIZE)
			m_lBlockSize -= BLOCK_SIZE_STEP_VALUE;
	}
	else
	{
		m_lBlockSize += BLOCK_SIZE_STEP_VALUE;
	}

	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStatic::OnDeltaposSpinSampletimer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return ;

	m_dSampleTimer += (-1) * pNMUpDown->iDelta * TRIGGER_TIMER_STEP_VALUE;
	if ( m_dSampleTimer < 1e-7)
		m_dSampleTimer = 1e-7;

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);
	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStatic::OnDeltaposSpinSamplecount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	
	if ( !UpdateData(TRUE) )
		return ;

	m_lSampleCount += (-1) * pNMUpDown->iDelta * (long)m_dSampleRate;
	if ( m_lSampleCount < 1 )
		m_lSampleCount = 1;

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	UpdateData(FALSE);
	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStatic::OnDeltaposSpinTriggerdelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if ( !UpdateData(TRUE) )
		return;

	if ( pNMUpDown->iDelta > 0 )
	{
		if ((m_lTriggerDelay - TRIGGER_DELAY_STEP_VALUE) >= MIN_TRIGGER_DELAY)
			m_lTriggerDelay -= TRIGGER_DELAY_STEP_VALUE;
	}
	else
	{
		if ((m_lTriggerDelay + TRIGGER_DELAY_STEP_VALUE) <= MAX_TRIGGER_DELAY)
			m_lTriggerDelay += TRIGGER_DELAY_STEP_VALUE;
	}

	UpdateData(FALSE);
}

void CAcquisitionSettingDlgStatic::OnEnChangeSampleTimerStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);
	UpdateData(FALSE);
}

void CAcquisitionSettingDlgStatic::OnEnChangeSampleCountStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	m_dSampleTimer = m_lSampleCount / m_dSampleRate;
	if ( m_dSampleTimer < 1e-7 )
		m_dSampleTimer = 1e-7;

	UpdateData(FALSE);
}

void CAcquisitionSettingDlgStatic::OnTvnSelchangedTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult)
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

void CAcquisitionSettingDlgStatic::OnTvnSelchangingTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	*pResult = 0;

	// 在当前板卡切换之前把设置保存
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStatic::OnEnChangeEditBlocksizeStatic()
{
// 	if ( !UpdateData(TRUE) )
// 		return ;
// 
// 	DataExchange(TRUE);
}

void CAcquisitionSettingDlgStatic::OnEnChangePreTriggerCountStatic()
{
	if ( !UpdateData(TRUE) )
		return ;

	DataExchange(TRUE);
}

BOOL CAcquisitionSettingDlgStatic::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD nID = LOWORD(wParam);
	if (nID == IDYES)
	{
		if ( !UpdateData(TRUE) )
			return FALSE;

		DataExchange(TRUE);
	}

	return CAcquisitionSettingDlgBase::OnCommand(wParam, lParam);
}

BOOL CAcquisitionSettingDlgStatic::UpdateSampleRate()
{
	double dOldSampleRate = m_dSampleRate;
	UpdateData(TRUE);
	
	BOOL bValid = (m_dSampleRate >= m_minSampleRate) && (m_dSampleRate <= m_maxSampleRate);
	if ( !bValid )
	{
		::MessageBox(m_hWnd, STR_SAMPLERATE_NOINRANGE ,EXPLAB_TITLE_NAME, MB_OK|MB_ICONERROR);
		m_dSampleRate = dOldSampleRate;
	}
	else
	{
		m_lSampleCount = (long)(m_dSampleRate * m_dSampleTimer);
		UpdateBlocksize();
	}

	UpdateData(FALSE);
	if ( !bValid )
	{
		m_editSampleRate.SetFocus();
		m_editSampleRate.SetSel(0, -1);
	}
	return bValid;
}

void CAcquisitionSettingDlgStatic::OnEnKillfocusEditSamplerateStatic()
{
	// UpdateSampleRate();
}

void CAcquisitionSettingDlgStatic::OnEnChangeEditSamplerateStatic()
{
	// [12/11/2014 Easy] 会出现重复验证问题
	// UpdateSampleRate();
}
