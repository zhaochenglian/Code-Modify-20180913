// AcquisitionSettingSheet.cpp : implementation file
//

#include "stdafx.h"
#include "UrtCommand.h"

#include "AcquisitionSettingDlgBase.h"
#include "AcquisitionSettingSheet.h"
#include "AcquisitionSettingDlg.h"
#include "AcquisitionSettingDlgStatic.h"
#include "AcquisitionSettingDlgStaticEx1629.h"
#include "AcquisitionSettingDlgStaticUSB14503.h"
#include "AcquisitionSettingDlgStaticNeff470.h"
#include "AcquisitionSettingDlgStaticPSI8400.h"
#include "AcquisitionSettingDlgStaticPSI9816.h"
#include "AcquisitionSettingDlgStaticZZX_8.h"
#include "AcquisitionSettingDlgStaticTQM603.h"
#include "AcquisitionSettingDlgStaticTQM613.h"
#include "AcquisitionSettingDlgStaticDH593X.h"
#include "AcquisitionSettingDlgVT1413C.h"
#include "AcquisitionSettingDlgDSA3217.h"
#include "AcquisitionSettingDlgStaticPI6000.h"
#include "AcquisitionSettingDlgStaticPXI.h"
#include "AcquisitionSettingDlgStaticAMC2322.h"
#include "AcquisitionSettingDlgStaticAMC2343.h"
#include "AcquisitionSettingDlgStaticVM2603.h"
#include "AcquisitionSettingDlgStaticVM3616.h"
#include "AcquisitionSettingDlg16015S.h"
#include "AcquisitionSettingDlgVXI0043.h"
#include "AcquisitionSettingDlgVirtual.h"
#include "AcquisitionSettingDlgFile.h"
#include "AcquisitionSettingDlgStaticSerialPort.h"
#include "AcquisitionSettingDlgStaticOpc.h"
#include "AcquisitionSettingDlgProtocol.h"
#include "AcquisitionSettingDlgStaticEMX.h"

extern CardTypeMap g_cardTypeMap;

// CAcquisitionSettingSheet dialog

IMPLEMENT_DYNAMIC(CAcquisitionSettingSheet, CDialog)

CAcquisitionSettingSheet::CAcquisitionSettingSheet(IUrtObject* pRootObj, CWnd* pParent /*=NULL*/)
	: CDialog(CAcquisitionSettingSheet::IDD, pParent)
	, m_pActiveSheet(NULL)
	, m_pRootObj(pRootObj)
	, m_strTitle(_T("UNKNOWN"))
	, m_curSelType(EUrtAcquisitionType(-1))
{

}

CAcquisitionSettingSheet::~CAcquisitionSettingSheet()
{
	if ( m_pActiveSheet != NULL )
	{
		m_pActiveSheet->DestroyWindow();
		delete m_pActiveSheet;
	}
}

void CAcquisitionSettingSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ACQUISITION, m_strTitle);
	DDX_Control(pDX, IDC_CMB_TESTCARD_TYPE, m_cmbCardType);
}


BEGIN_MESSAGE_MAP(CAcquisitionSettingSheet, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_TESTCARD_TYPE, &CAcquisitionSettingSheet::OnCbnSelchangeCmbTestcardType)
	ON_BN_CLICKED(IDC_APPLYTO_ALL, &CAcquisitionSettingSheet::OnBnClickedApplytoAll)
	ON_BN_CLICKED(IDOK, &CAcquisitionSettingSheet::OnBnClickedOk)
END_MESSAGE_MAP()


// CAcquisitionSettingSheet message handlers

BOOL CAcquisitionSettingSheet::OnInitDialog()
{
	CDialog::OnInitDialog();
	//zcl add
	//确定显示位置
	POINT cursor;
	::GetCursorPos(&cursor);
	CRect rect;
	GetWindowRect(&rect);

	int width = rect.Width();
	int height = rect.Height();
	int MaxX  = GetSystemMetrics(SM_CXSCREEN);
	int MaxY  = GetSystemMetrics(SM_CYSCREEN);
	int DeltX = cursor.x;(MaxX - rect.Width() - 10);
	int DeltY = cursor.y;(MaxY-rect.Height() - 40);
	if ( DeltX > MaxX - width - 10 )
		DeltX = MaxX - width - 10;
	
	if ( DeltY > MaxY - height - 40 )
		DeltY = MaxY - height - 40;

	MoveWindow (DeltX ,DeltY, width, height);

	CardTypeMap cardTypeMap;

	CComPtr<IUrtAcquisitionSettingVisitor> pSettingVisitor;
	if ( SUCCEEDED(pSettingVisitor.CoCreateInstance(__uuidof(UrtAcquisitionSettingVisitor))) )
	{
		// 遍历所有对象,找出支持采集的对象
		CComQIPtr<IUrtVisitor> pVisitor(pSettingVisitor);
		CComQIPtr<IUrtVisitServer> pVisitServer = m_pRootObj;
		if ( pVisitServer != NULL )
		{
			pVisitServer->Visit(pVisitor);

			LONG objCount = 0;
			EUrtAcquisitionType* pAcqTypes = NULL;
			if ( SUCCEEDED(pSettingVisitor->GetAllAcquisitionType(&objCount, &pAcqTypes)) )
			{
				for ( LONG i=0; i<objCount; i++ )
				{
					EUrtAcquisitionType curType = pAcqTypes[i];
					cardTypeMap.insert( CardTypePair(curType, g_cardTypeMap[curType]) );
				}

				delete [] pAcqTypes;
				pAcqTypes = NULL;
			}
		}
	}

	// [6/4/2013 easy] 初始定位到选择的采集对象上
	LONG curSelIndex = 0;
	EUrtAcquisitionType curSelType = eACQST_Unknown;
	CComQIPtr<IUrtAcquisitionSetting> pAcqSetting(m_pRootObj);
	if ( pAcqSetting != NULL )
		pAcqSetting->get_acquisitionType(&curSelType);
	
	CardTypeMapIter iter = cardTypeMap.begin();
	for ( ; iter != cardTypeMap.end(); ++iter )
	{
		int nIndex = m_cmbCardType.AddString(iter->second);
		if ( nIndex != CB_ERR )
			m_cmbCardType.SetItemData(nIndex, iter->first);

		if ( iter->first == curSelType )
			curSelIndex = nIndex;
	}
	
	if ( cardTypeMap.size() > 0 )
	{
		m_cmbCardType.SetCurSel(curSelIndex);
		m_cmbCardType.GetLBText(curSelIndex, m_strTitle);
		EUrtAcquisitionType eCardType = (EUrtAcquisitionType)m_cmbCardType.GetItemData(curSelIndex);
		SetActiveSheet(eCardType);
	}
	
	UpdateData(FALSE);

	return TRUE;  
}

void  CAcquisitionSettingSheet::SetActiveSheet(EUrtAcquisitionType type)
{
	if ( m_curSelType == type )
		return ;

	CAcquisitionSettingDlgBase* pDlgBase = m_pActiveSheet;
	if ( pDlgBase != NULL )
	{
		WORD   wLow   = IDYES;
		WPARAM wParam = MAKEWPARAM(wLow, 0);
		pDlgBase->SendMessage(WM_COMMAND, wParam, 0);
	}

	m_curSelType = type;
	m_pActiveSheet = NULL;

	// 对话框模板的资源ID
	UINT nTempID = 0;
	switch( m_curSelType )
	{
	case eACQST_Unknown: // [6/4/2013 easy] 未知采集类型,不进行设置
	case eACQST_VIRTUAL:
	case eACQST_EX1200_Switch:
	case eACQST_EX1200_MultiFunction:
	case eACQST_EX1200_Fgen:
	case eACQST_EX1200_Dio:
	case eACQST_EX1200_Scanner:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgVirtual(this);
		nTempID = IDD_DLG_ACQUISITION_VIRTUAL;
	}
		break;
	case eACQST_VT1432A:
	case eACQST_VT1432B:
	case eACQST_VT1433B:
	case eACQST_VT1435B:
	case eACQST_VT1436B:
	{
		m_pActiveSheet = new CAcquisitionSettingDlg;
		nTempID = IDD_DLG_ACQUISITION;
	}
		break;
	case eACQST_VT1413C:
	case eACQST_VT1415A:
	case eACQST_VT1419A:
	case eACQST_VT1422A:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgVT1413C(this);
		nTempID = IDD_DLG_ACQUISITION_VT1413C;
	}
		break;
	case eACQST_EX1000:
	case eACQST_EX1000TC:
	case eACQST_EX1048:
	case eACQST_DSTP:
	case eACQST_VTOPC:
	case eACQST_FOLDER:
	case eACQST_FCS:
	case eACAST_RPT350:
	case eACQST_SA:
	case eACQST_AGILENTSSA:
	case eACQST_AGILENTNA:
    case eACAST_PCI7256:
	case eACQST_MTS:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStatic(this, m_curSelType);
		nTempID = IDD_DLG_ACQUISITION_STATIC;
	}
		break;
	case eACQST_EX1401:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStatic(this, m_curSelType, 0.000277772545814514, 20000.0);
		nTempID = IDD_DLG_ACQUISITION_STATIC;
	}
		break;
	case eACQST_EX1629:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticEx1629(this);
		nTempID = IDD_DLG_ACQUISITION_STATIC1629;
	}
		break;
	case eACQST_USB14503:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticUSB14503(this);
		nTempID = IDD_DLG_ACQUISITION_STATIC14503;
	}
		break;
	case eACQST_NEFF470:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticNeff470(this);
		nTempID = IDD_DLG_ACQUISITION_STATICNEFF470;
	}
		break;
	case eACQST_PSI8400:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticPSI8400(this);
		nTempID = IDD_DLG_ACQUISITION_STATICPSI8400;
	}
		break;
	case eACQST_PSI9816:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticPSI9816(this);
		nTempID = IDD_DLG_ACQUISITION_STATICPSI9816;
	}
		break;
	case eACQST_ZZX_8:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticZZX_8(this);
		nTempID = IDD_DLG_ACQUISITION_STATICZZX_8;
	}
		break;
	case eACQST_TQM603:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticTQM603(this);
		nTempID = IDD_DLG_ACQUISITION_STATICTQM603;
	}
		break;
	case eACQST_TQM613:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticTQM613(this);
		nTempID = IDD_DLG_ACQUISITION_STATICTQM613;
	}
		break;
	case eACQST_DH593X:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticDH593X(this);
		nTempID = IDD_DLG_ACQUISITION_STATIC593X;
	}
		break;
	case eACQST_DSA3217:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgDSA3217(this);
		nTempID = IDD_DLG_ACQUISITION_DSA3217;
	}
		break;
	case eACQST_PI6000:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticPI6000(this);
		nTempID = IDD_DLG_ACQUISITION_STATICPI6000;
	}
		break;
	case eACQST_PXI6120:
	case eACQST_PXI6123:
	case eACQST_PXI6124:
	case eACQST_PXI6143:
	case eACQST_PXI4498:
	case eACQST_PCI4472:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticPXI(m_curSelType);
		nTempID = IDD_DLG_ACQUISITION_PXI_STATIC;
	}
		break;
	case eACQST_AMC2322:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticAMC2322(this);
		nTempID = IDD_DLG_ACQUISITION_STATICAMC2322;
	}
		break;
	case eACQST_AMC2343:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticAMC2343(this);
		nTempID = IDD_DLG_ACQUISITION_STATICAMC2343;
	}
		break;
	case eACQST_VM2603:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticVM2603(this);
		nTempID = IDD_DLG_ACQUISITION_STATICVM2603;
	}
	break;
	case eACQST_VX16015S:
	{
		m_pActiveSheet = new CAcquisitionSettingDlg16015S(this);
		nTempID = IDD_DLG_ACQUISITION_16015S;
	}
		break;
	case eACQST_VXI0043:
	case eACQST_AEFT624:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgVXI0043(this);
		nTempID = IDD_DLG_ACQUISITION_VXI0043;
	}
		break;
	case eACQST_SerialPort:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticSerialPort(this);
		nTempID = IDD_DLG_ACQUISITION_STATICSERIALPORT;
	}
		break;
	case eACQST_PROTOCOL:
		{
			m_pActiveSheet = new CAcquisitionSettingDlgProtocol(this);
			nTempID = IDD_DLG_ACQUISITION_PROTOCOL;
		}
		break;
	case eACQST_FILE:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgFile(this);
		nTempID = IDD_DLG_ACQUISITION_FILE;
	}
		break;
	case eACQST_VM3608:
	case eACQST_VM3616:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticVM3616(m_curSelType);
		nTempID = IDD_DLG_ACQUISITION_STATICVM3616;
	}
		break;
	case eACQST_EMX4250:
	case eACQST_EMX4251:
	case eACQST_EMX4350:
	case eACQST_EMX4380:
	case eACQST_EMX2500:
	case eACQST_RX0124:
	case eACQST_RX0224:
	{
		m_pActiveSheet = new CAcquisitionSettingDlgStaticEMX(this);
		nTempID = IDD_DLG_ACQUISITION_STATICEMX;
	}
		break;
	//case eACQST_VTOPC:
	//{
	//	m_pActiveSheet = new CAcquisitionSettingDlgStaticOpc(this);
	//	nTempID = IDD_DLG_ACQUISITION_STATICOPC;
	//}
	//	break;
	default:
		break;
	}

	if ( m_pActiveSheet != NULL )
	{
		m_pActiveSheet->SetCardType(m_curSelType);
		m_pActiveSheet->AttachRootObj(m_pRootObj);
		m_pActiveSheet->Create(nTempID, this);

		m_pActiveSheet->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		m_pActiveSheet->ModifyStyle(0, WS_TABSTOP);
		CRect rect;
		CWnd* pWnd = GetDlgItem(IDC_STATIC_ACQUISITION);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		rect.DeflateRect(2, 16, 2, 2);

		m_pActiveSheet->MoveWindow(&rect);

		if ( !m_pActiveSheet->IsWindowVisible() ) 
			m_pActiveSheet->ShowWindow(SW_SHOW);
	}

	if ( pDlgBase != NULL )
	{
		pDlgBase->ShowWindow(SW_HIDE);
		pDlgBase->DestroyWindow();
		delete pDlgBase;
	}
}

void CAcquisitionSettingSheet::OnCbnSelchangeCmbTestcardType()
{
	UpdateData(TRUE);
	int nIndex = m_cmbCardType.GetCurSel();
	if ( nIndex == CB_ERR )
		return ;

	DWORD_PTR dwType = m_cmbCardType.GetItemData(nIndex);
	SetActiveSheet((EUrtAcquisitionType)dwType);

	m_cmbCardType.GetLBText(nIndex, m_strTitle);
	UpdateData(FALSE);
}

void CAcquisitionSettingSheet::OnBnClickedApplytoAll()
{
	if ( m_pActiveSheet == NULL )
		return ;

	CAcquisitionSettingDlgBase* pDlgBase = m_pActiveSheet;
	pDlgBase->ApplytoAll();
}

void CAcquisitionSettingSheet::OnBnClickedOk()
{
	//if ( m_pActiveSheet != NULL  )
	//{
	//	WORD   wLow   = 0;
	//	WPARAM wParam = 0; 
	//	wLow   = IDYES;
	//	wParam = MAKEWPARAM(wLow, 0);
	//	m_pActiveSheet->SendMessage(WM_COMMAND, wParam, 0);
	//}

	if ( m_pActiveSheet != NULL  )
	{
		if ( !m_pActiveSheet->UpdateData(TRUE) )
			return;

		m_pActiveSheet->DataExchange(TRUE);
		CString str;
		if ( FAILED(CAcquisitionSettingDlgBase::GetDiskSpaceInfo(m_pRootObj, str, TRUE)) )
			return;
	}

	OnOK();
}
