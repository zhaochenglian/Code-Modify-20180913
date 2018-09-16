#pragma once

#include "Resource.h"
#include "AcquisitionSettingDlgBase.h"
#include "afxwin.h"


// CAcquisitionSettingDlgStatic dialog

class CAcquisitionSettingDlgStatic : public CAcquisitionSettingDlgBase
{
	DECLARE_DYNAMIC(CAcquisitionSettingDlgStatic)

public:
	CAcquisitionSettingDlgStatic(CWnd* pParent = NULL);   // standard constructor
	CAcquisitionSettingDlgStatic(CWnd* pParent, EUrtAcquisitionType acqType, DOUBLE minRate=1/4296.0, DOUBLE maxRate=1000);   // standard constructor
	virtual ~CAcquisitionSettingDlgStatic();

// Dialog Data
	enum { IDD = IDD_DLG_ACQUISITION_STATIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDeltaposSpinSamplerateStatic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBlocksizeStatic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinSampletimer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinSamplecount(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinTriggerdelay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSampleTimerStatic();
	afx_msg void OnEnChangeSampleCountStatic();
	afx_msg void OnTvnSelchangedTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTreeCardAcquireStatic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditBlocksizeStatic();
	afx_msg void OnEnChangePreTriggerCountStatic();
	afx_msg void OnEnKillfocusEditSamplerateStatic();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnChangeEditSamplerateStatic();

public:
	virtual CUrtTreeCtrl& GetTreeCtrl(){ return m_treeCard; }
	virtual void DataExchange(BOOL bSave = TRUE);
	void SetMinMaxSampleRate(double min, double max)
	{
		m_minSampleRate = min;
		m_maxSampleRate = max;
	};

protected:
	// 更新各个控件的状态
	void UpdateControlsState();
	BOOL UpdateSampleRate();

private:
	/* 该这行变量定义这些类型 只是为了控制只能输入有效的字符 */
	URT_WIDGET::CIntEdit			m_editBlockSize;
	URT_WIDGET::CIntEdit			m_editSampleCount;
	URT_WIDGET::CIntEdit			m_editPreTriggerCount;
	URT_WIDGET::CDoubleEdit			m_editSampleRate;
	URT_WIDGET::CDoubleEdit			m_editSampleTimer;
	double m_maxSampleRate, m_minSampleRate;
};
