// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "VXIConfig.h"
#include "MainFrm.h"
#include "GraphicDoc.h"

#include "DlgMasterCtrl.h"
#include "UrtSetProjcetIDDlg.h"
#include "DlgMasterCtrl460.h"
#include "OutputBarEx.h"
#include "SnapshotView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* 功能: 执行一次手动快照
/************************************************************************/
void CMainFrame::OnToolSnapshotOnce()
{
#if(FLEXLIC)
	if ( CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_FEAT_SNAPSHOT_FORM_VIEW,FLEX_SHOWERROR,LM_CO_LOCALTEST))
#endif
	{
		if ( GetAcquireState() != eAcquireStoped )
		 {
			OnToolSnapshot();
		 }
	}
}

/************************************************************************/
/* 功能: 执行快照采集
/************************************************************************/
void CMainFrame::OnToolSnapshot()
{
#if(FLEXLIC)
	if ( CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_FEAT_SNAPSHOT_FORM_VIEW,FLEX_SHOWERROR,LM_CO_LOCALTEST))
#endif
	{
		CDocument* pSnapshotDoc = theApp.GetActiveSnapshotDoc();
		if ( pSnapshotDoc == NULL )
			return;

		POSITION pos = pSnapshotDoc->GetFirstViewPosition();
		while ( pos )
		{
			CView* pView = pSnapshotDoc->GetNextView(pos);
			CSnapshotView* pSnapshotView = DYNAMIC_DOWNCAST(CSnapshotView, pView);
			if( pSnapshotView == NULL )
				continue;

			pSnapshotView->UpdateSnapshotBlock();
			break;
		}

		if ( m_sysType & eSysTSlaver )
		{
			// 如果系统已经处在快照采集状态,则不进行处理
			LONG recordState = GetRecordState();
			if ( !(recordState & eDataSnapshotNone) || m_pAquireMgr->GetAcquireState()==eAcquireStoped )
				return ;

			m_recordState &= 0xFFFF0FFF;
			m_recordState |= eDataSnapshotBegin;
			
			// 开始快照采集时给接收端发送命令,让接收端也进行快照
			SendSlaverMsgToReceiver(eMsgTypeStartSnapshot, 0, eSDT_DATA_GROUP);
		}
		else if ( m_pSnapshotMgr != NULL )
		{
			m_pSnapshotMgr->DoSnapshot(0);
		}
	}
}

/************************************************************************/
/* 功能: 执行快照采集置零
/************************************************************************/
void CMainFrame::OnToolSnapshotZero()
{
	if ( m_pSnapshotMgr != NULL )
		m_pSnapshotMgr->PrepareSnapshotFile();
}

EUrtAcquireState CMainFrame::GetAcquireState()
{
	if ( m_pAquireMgr != NULL )
		return m_pAquireMgr->GetAcquireState(); 

	return eAcquireStoped;
}

void    CMainFrame::SetSaveState(BOOL bSave)
{
	LONG recordState = GetRecordState();
	if ( bSave )
	{
		// 开始记录
		if ( recordState & eDataSaveNone )
		{
			m_recordState &= 0xFFFFFF0F;
			m_recordState |= eDataSaveBegin;
		}
	}
	else
	{
		// 结束记录,如果当前不处在停止记录状态
		if ( recordState & eDataSaveBegin ) 
		{
			m_recordState &= 0xFFFFFF0F;
			m_recordState |= eDataSaveNone;
		}
		else
		if ( recordState & eDataDoSave )
		{
			m_recordState &= 0xFFFFFF0F;
			m_recordState |= eDataSaveEnd;
		}
	}
}

void  CMainFrame::SetZeroState(BOOL bZero)
{
	LONG recordState = GetRecordState();
	if ( bZero )
	{
		// 开始零位采集
		if ( recordState & eDataZeroNone )
		{
			m_recordState &= ~eDataZeroAll;
			m_recordState |= eDataZeroBegin;

			ExecuteEventOperator(EVENT_SYSID_BGEIN_ZERO, eOperSetEvent);
		}
	}
	else
	{
		// 结束零位采集,如果当前不处在零位采集状态
		if ( recordState & eDataZeroBegin ) 
		{
			m_recordState &= ~eDataZeroAll;
			m_recordState |= eDataZeroNone;
		}
		else if ( recordState & eDataDoZero )
		{
			m_recordState &= ~eDataZeroAll;
			m_recordState |= eDataZeroEnd;
		}
	}
}

/************************************************************************/
/* 功能: 零点计算
/************************************************************************/
LRESULT CMainFrame::OnZeroAcquire(WPARAM wParam, LPARAM lParam)
{
	LONG recordState = GetRecordState();

	recordState &= 0xFFFFF0FF;
	m_recordState = recordState|(0x0F00&wParam);

	return 0;
}

BOOL   CMainFrame::GetAcqSlaver(LPDISPATCH* ppSlaver)
{
	if ( m_pAcqSlaver == NULL )
		return FALSE;

	*ppSlaver = m_pAcqSlaver;
	(*ppSlaver)->AddRef();

	return TRUE;
}

BOOL   CMainFrame::GetAcqMaster(LPDISPATCH* ppMaster)
{
	if ( m_pAcqMaster == NULL )
		return FALSE;

	*ppMaster = m_pAcqMaster;
	(*ppMaster)->AddRef();

	return TRUE;
}

BOOL   CMainFrame::GetAcqReceiver(LPDISPATCH* ppReceiver)
{
	if ( m_pAcqReceiver == NULL )
		return FALSE;

	*ppReceiver = m_pAcqReceiver;
	(*ppReceiver)->AddRef();

	return TRUE;
}

STDMETHODIMP CMainFrame::XUrtSelSet::GetNumOfSelectedObj(LONG* pVal)
{
	METHOD_PROLOGUE(CMainFrame, UrtSelSet)

	CComPtr<IUrtTreeNode> pTreeNode = pThis->m_wndWorkSpace.GetSelTreeNode();
	if ( pTreeNode == NULL )
	{
		*pVal = 0;
		return S_FALSE;
	}

	// 当前只允许选择一个对象
	*pVal = 1;
	return S_OK;
}

STDMETHODIMP CMainFrame::XUrtSelSet::GetSelectedObj(LONG lIndex, IUrtObjId** ppObjId)
{
	METHOD_PROLOGUE(CMainFrame, UrtSelSet)

	CComPtr<IUrtTreeNode> pTreeNode = pThis->m_wndWorkSpace.GetSelTreeNode();
	if ( pTreeNode == NULL )
		return E_POINTER;

	return pTreeNode->get_objectId(ppObjId);
}

STDMETHODIMP CMainFrame::XCommandEventHandler::HandleEvent(EUrtTransMsgType msgType, BSTR strSenderIP, LONG wParam, LONG lParam)
{
	METHOD_PROLOGUE(CMainFrame, CommandEventHandler)

	// 必须采用发送消息的机制，它们不是在同一个线程，调用窗口函数会出问题
	UINT msgID = WM_USER_MASTER_MSG;
	switch ( wParam )
	{
	case eSysTMasterMain:
	case eSysTMasterNormal:
	{	msgID = WM_USER_MASTER_MSG;
		switch ( msgType )
		{
		case eMsgTypeSubSystemInfo:        
			URT_OUTPUT_MSG(eMsgCmdSubSystemInfo, strSenderIP);
			break;
		case eMsgTypeAcqSlaverSysInfo:     
			URT_OUTPUT_MSG(eMsgCmdAcqSlaverSysInfo, strSenderIP);
			break;
		case eMsgTypeEstablishedLink:
			URT_OUTPUT_MSG(eMsgCmdEstablishedLink, strSenderIP);
			break;
		case eMsgTypeLinkError:
			URT_OUTPUT_MSG(eMsgCmdLinkError, strSenderIP);
			break;
		case eMsgTypeCloseLink:
			URT_OUTPUT_MSG(eMsgCmdCloseLink, strSenderIP);
			break;
		case eMsgTypeStartAcquire:
			URT_OUTPUT_MSG(eMsgCmdStartAcquire, strSenderIP);
			break;
		case eMsgTypeEndAcquire:
			URT_OUTPUT_MSG(eMsgCmdEndAcquire, strSenderIP);
			break;
		case eMsgTypeStartRecord:
			URT_OUTPUT_MSG(eMsgCmdStartRecord, strSenderIP);
			break;
		case eMsgTypeEndRecord:
			URT_OUTPUT_MSG(eMsgCmdEndRecord, strSenderIP);
			break;
		case eMsgTypeStartAcquireAndRecord:
			URT_OUTPUT_MSG(eMsgCmdStartAcquireAndRecord, strSenderIP);
			break;
		case eMsgTypeMonitorEnter:
			URT_OUTPUT_MSG(eMsgCmdMonitorEnter, strSenderIP);
			break;
		case eMsgTypeMonitorLeave:
			URT_OUTPUT_MSG(eMsgCmdMonitorLeave, strSenderIP);
			break;
		case eMsgTypeAcqSlaverData:
			URT_OUTPUT_MSG(eMsgCmdAcqSlaverData, strSenderIP);
			break;
		case eMsgTypeSlaverLogMsg:
		{
			CString strMsg = CString(strSenderIP)+_T(",")+CString(BSTR(lParam));
			lParam = (LPARAM)strMsg.AllocSysString();
			SysFreeString(strSenderIP);
		}
			break;
		default:
			break;
		}

		if ( lParam == NULL )
			lParam = (LPARAM)::SysAllocString(strSenderIP);
	}
		break;
	case eSysTSlaver:
	{
		msgID = WM_USER_SLAVER_MSG;
		switch( msgType )
		{
		case eMsgTypeStartAcquire:
			URT_OUTPUT_MSG(eMsgCmdStartAcquire, strSenderIP);
			break;
		case eMsgTypeUserCmd:
			URT_OUTPUT_MSG(eMsgTypeUserCmd, strSenderIP);
			break;
		case eMsgTypeEndAcquire:
			URT_OUTPUT_MSG(eMsgCmdEndAcquire, strSenderIP);
			break;
		case eMsgTypeStartRecord:
			URT_OUTPUT_MSG(eMsgCmdStartRecord, strSenderIP);
			break;
		case eMsgTypeEndRecord:
			URT_OUTPUT_MSG(eMsgCmdEndRecord, strSenderIP);
			break;
		case eMsgTypeStartAcquireAndRecord:
			URT_OUTPUT_MSG(eMsgCmdStartAcquireAndRecord, strSenderIP);
			break;
		case eMsgTypeSubSystemInfoRequest:	
			URT_OUTPUT_MSG(eMsgCmdSubSystemInfoRequest, strSenderIP);
			break;
		case eMsgTypeMasterSubSystemInfo:	
			URT_OUTPUT_MSG(eMsgCmdMasterSubSystemInfo, strSenderIP);
			break;
		case eMsgTypeAcqSlaverInfoRequest:
			URT_OUTPUT_MSG(eMsgCmdAcqSlaverInfoRequest, strSenderIP);
			break;
		case eMsgTypeEstablishedLink:
			URT_OUTPUT_MSG(eMsgCmdEstablishedLink, strSenderIP);	
			break;
		case eMsgTypeCloseLink:
 			URT_OUTPUT_MSG(eMsgCmdCloseLink, strSenderIP);
			break;
		case eMsgTypeLinkError:
			URT_OUTPUT_MSG(eMsgCmdLinkError, strSenderIP);
			break;
		default:
			break;
		}
	}
		break;
	case eSysTRecvNormal:
	{
		msgID = WM_USER_RECEIVER_MSG;
		switch( msgType )
		{
		case eMsgTypeMasterSlaverInfo:
			URT_OUTPUT_MSG(eMsgCmdAcqMasterInfo, strSenderIP);	
			break;
		case eMsgTypeSubSystemInfo:         
			URT_OUTPUT_MSG(eMsgCmdSubSystemInfo, strSenderIP);	
			break;
		case eMsgTypeAcqSlaverSysInfo:      
			URT_OUTPUT_MSG(eMsgCmdAcqSlaverSysInfo, strSenderIP);	
			break;
		case eMsgTypeCloseLink:
			URT_OUTPUT_MSG(eMsgCmdCloseLink, strSenderIP);	
			break;
		case eMsgTypeMonitorState:
			URT_OUTPUT_MSG(eMsgCmdMonitorState, strSenderIP);	
			break;
		default:
			break;
		}
	}
		break;
	default:
		break;
	}

	::PostMessage(pThis->m_hWnd, msgID, msgType, lParam);
	return S_OK;
}

STDMETHODIMP CMainFrame::XCommandEventHandler::WaitOnEvent(EUrtTransMsgType msgType, LONG timeOut)
{
	METHOD_PROLOGUE(CMainFrame, CommandEventHandler)

	return S_OK;
}

void CMainFrame::OnToolTriggerset()
{
#if(FLEXLIC)
	if ( !CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_TOOL_SET_TRIGGER,FLEX_SHOWERROR,LM_CO_LOCALTEST))
		return ;
#endif
	CComPtr<IUrtCommand> pCommand; 
	pCommand.CoCreateInstance(__uuidof(UrtTriggerSettingCmd));

	if ( pCommand != NULL )
	{
		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		pCommand->Execute(pApplication, 0);
	}
}

void CMainFrame::OnToolChannelConfig()
{
#if(FLEXLIC)
	if ( !CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_TOOL_SET_CHANNELCONFIG,FLEX_SHOWERROR,LM_CO_LOCALTEST))
		return ;
#endif
	CComPtr<IUrtCommand> pCommand;
	pCommand.CoCreateInstance(__uuidof(UrtChannelCfgCmd));

	if ( pCommand != NULL )
	{
		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		pCommand->Execute(pApplication, 0);

		UpdateChannelView(ID_TOOL_UPDATECHANSLIST);

		// 更新左边的树控件
		CComPtr<IDispatch> pDisp;
		if (SUCCEEDED(pApplication->GetActiveProject(&pDisp)))
		{
			CComQIPtr<IVXIProject> pPrj(pDisp);
			if (pPrj != NULL)
				m_wndWorkSpace.FillClassView(pPrj, GetSystemType());

			CComPtr<IUrtCollection> pSystems;
			if (FAILED(pPrj->QueryInterface(&pSystems)))
				return ;

			CComPtr<IUrtIterator> pObjIter;
			if (SUCCEEDED(pSystems->NewEnum(&pObjIter)))
			{
				for (pObjIter->First(); S_OK != pObjIter->IsDone(NULL); pObjIter->Next() )
				{
					CComPtr<IUrtObject> pObj;
					if ( FAILED(pObjIter->GetCurObject(__uuidof(IUrtObject), (LPUNKNOWN*)&pObj)) )
						continue;

					UpdateConfigView(pObj, eModifyModified);
				}
			}
		}
	}
}

void CMainFrame::OnDummyChannelConfig()
{
	CComPtr<IUrtCommand> pCommand;
	pCommand.CoCreateInstance(__uuidof(UrtChannelCfgCmd));
	if ( pCommand != NULL )
	{
		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

		CComQIPtr<IUrtSelSet> pSelSet;
		Urt_GetApplication()->GetSelSet(&pSelSet);

		LONG lNum;
		pSelSet->GetNumOfSelectedObj(&lNum);
		if ( lNum > 0 )
		{
			CComPtr<IUrtObjId> pObjId;
			pSelSet->GetSelectedObj(0, &pObjId);

			CComPtr<IUrtObject> pObj;
			pObjId->QueryObjInterface(__uuidof(IUrtObject), (LPUNKNOWN*)&pObj);

			CComQIPtr<IUrtChannelCfgCmd> pCfgCmd(pCommand);
			pCfgCmd->AttachRootObject(pObj);
			if ( SUCCEEDED(pCommand->Execute(pApplication, 0)) )
			{
				// 更新左边的树控件
				CComPtr<IDispatch> pDisp;
				if (SUCCEEDED(pApplication->GetActiveProject(&pDisp)))
				{
					CComQIPtr<IVXIProject> pPrj(pDisp);
					if (pPrj != NULL)
						m_wndWorkSpace.FillClassView(pPrj, GetSystemType());
				}
				
				UpdateChannelView(ID_TOOL_UPDATECHANSLIST);	

				UpdateConfigView(pObj, eModifyModified);
			}
		}
	}
}

void CMainFrame::OnToolAcquireset()
{
#if(FLEXLIC)
	if ( !CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_TOOL_SET_ACQUIRESET,FLEX_SHOWERROR,LM_CO_LOCALTEST))
		return ;
#endif
	CComPtr<IUrtCommand> pCommand;
	pCommand.CoCreateInstance(__uuidof(UrtAcquisitionSettingCmd));

	if ( pCommand != NULL )
	{
		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		pCommand->Execute(pApplication, 0);
	}

	UpdateChannelView(ID_TOOL_UPDATECHANSINFO);
}

void CMainFrame::OnDummyAcquireset()
{
	CComPtr<IUrtCommand> pCommand;
	pCommand.CoCreateInstance(__uuidof(UrtAcquisitionSettingCmd));

	if ( pCommand != NULL )
	{
		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

		CComPtr<IUrtSelSet> pSelSet;
		Urt_GetApplication()->GetSelSet(&pSelSet);

		LONG lNum;
		pSelSet->GetNumOfSelectedObj(&lNum);
		if ( lNum > 0 )
		{
			CComPtr<IUrtObjId> pObjId;
			pSelSet->GetSelectedObj(0, &pObjId);

			CComPtr<IUrtObject> pObj;
			pObjId->QueryObjInterface(__uuidof(IUrtObject), (LPUNKNOWN*)&pObj);

			CComQIPtr<IUrtAcquisitionSettingCmd> pSettingCmd(pCommand);
			pSettingCmd->AttachRootObject(pObj);
			pCommand->Execute(pApplication, 0);

			UpdateChannelView(ID_TOOL_UPDATECHANSINFO);
		}
	}
}

void CMainFrame::OnOptionSetting()
{
	CComPtr<IUrtCommand> pCommand;
	pCommand.CoCreateInstance(__uuidof(UrtOptionSettingCmd));

	if ( pCommand != NULL )
	{
		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		LONG mask(-1);
		pCommand->Execute(pApplication, mask);
	}
}

void CMainFrame::OnCheckUpdate()
{
	CComPtr<IUrtCommand> pCommand;
	if (FAILED(pCommand.CoCreateInstance(__uuidof(UrtSimpleCmd))))
	{
		MessageBox(STR_MSG_NOT_REGISTER_MODULE, EXPLAB_TITLE_NAME);
		return;
	}

	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
	pCommand->Execute(pApplication, eSMPCMD_CheckUpdate);
}

void CMainFrame::OnToolSetacqslaver()
{
	// UpdateToolBar();

	// 把工程信息发送到控制组
	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
	CComPtr<IDispatch> pPrjDisp;
	pApplication->GetActiveProject(&pPrjDisp);

	EUrtAcquireState acquireState = GetAcquireState();
	LONG  recordState = GetRecordState();

	EUrtTransMsgType msgType;
	if ( acquireState == eAcquireStoped )
	{
		msgType = eMsgTypeEndAcquire;
	}
	else
	{
		msgType = eMsgTypeStartAcquire;
		if ( (recordState & eDataSaveBegin ) || (recordState & eDataDoSave) )
			msgType = eMsgTypeStartRecord;
	}

	if ( m_pAcqSlaver == NULL )
	{
		// 创建采集端服务对象
		m_pAcqSlaver.CoCreateInstance(__uuidof(UrtAcqSlaver));
		if ( m_pAcqSlaver != NULL )
		{
			ExpLabService* pService = ExpLabService::Open();
			CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
			ExpLabService::Close();

			_variant_t varMode;
			SHORT releaseMode = 1;  // 采集端发布模式: 0:直接发布; 1：间接发布
			if ( SUCCEEDED(pSetting->GetValue(GLOBAL_RELEASE_MODE, &varMode)) )
				releaseMode = V_I2(&varMode);

			EUrtSystemType sysType = ((releaseMode==0) ? eSysTMasterMain : eSysTSlaver);

			_variant_t varAddr, varPort, varLocalIP;
			V_BSTR(&varLocalIP) = NULL;

			if ( sysType == eSysTSlaver )
			{
				if ( FAILED(pSetting->GetValue( GLOBAL_ACQGROUP_ADDRESS, &varAddr )) )
					varAddr = GROUP_ACQUIRE_ADDRESS;

				if ( FAILED(pSetting->GetValue( GLOBAL_ACQGROUP_PORT, &varPort )) )
					varPort = SHORT(GROUP_ACQUIRE_PORT);

				if ( SUCCEEDED(pSetting->GetValue( GLOBAL_ACQG_LOCAL_IP, &varLocalIP )) )
				{
					if ( CComBSTR(V_BSTR(&varLocalIP)) == INADDR_ANY_STRING )
					{
						varLocalIP.Clear();
						V_BSTR(&varLocalIP) = NULL;
					}
				}
			}
			else
			{
				if ( FAILED(pSetting->GetValue( GLOBAL_DATAGROUP_ADDRESS, &varAddr )) )
					varAddr = GLOBAL_DATAGROUP_ADDRESS;

				if ( FAILED(pSetting->GetValue( GLOBAL_DATAGROUP_PORT, &varPort )) )
					varPort = SHORT(GLOBAL_DATAGROUP_PORT);

				if ( SUCCEEDED(pSetting->GetValue( GLOBAL_DATA_LOCAL_IP, &varLocalIP )) )
				{
					if ( CComBSTR(V_BSTR(&varLocalIP)) == INADDR_ANY_STRING )
					{
						varLocalIP.Clear();
						V_BSTR(&varLocalIP) = NULL;
					}
				}
			}

			m_pAcqSlaver->Create(V_BSTR(&varAddr), V_I2(&varPort), V_BSTR(&varLocalIP), sysType);
			
			// 注册事件接收器
			LPDISPATCH pHandler = GetIDispatch(FALSE);
			m_pAcqSlaver->InstallHandler(pHandler, &m_acqSlaverCookie);
		}
	}
	
	if ( m_pAcqSlaver != NULL )
	{
		// 重新初始化采集端信息,再发送到主控
		m_pAcqSlaver->InitSlaverProjectInfo(pPrjDisp, msgType);
		m_pAcqSlaver->SendSlaverProjectInfo(0);
	}

	URT_OUTPUT_MSG(eMsgCmdEstablishedLink, MAINFRAME_NAME);
}

void CMainFrame::OnToolSetacqmaster()
{
	ASSERT( m_pAcqSlaver == NULL && m_pAcqReceiver == NULL );

	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

	CComPtr<IDispatch> pPrjDisp;
	pApplication->GetActiveProject(&pPrjDisp);
	CComQIPtr<IVXIProject> pProject(pPrjDisp);

	ExpLabService* pService = ExpLabService::Open();
	GLOBAL_SETTING* pGlobalSetting = pService->GetGlobalSetting();
	ExpLabService::Close();

	// 如果已经设置了工程ID,则不再弹出设置ID对话框
	CComBSTR prjId;
	pProject->get_projectID(&prjId);
	if ( prjId.Length() == 0 )
	{
		if( USE_TDM3000_MODULE )
		{
			CUrtSetProjcetIDDlg prjDlg(this);
			if ( prjDlg.DoModal() == IDOK )
			{
				pProject->put_projectID(CComBSTR(prjDlg.GetProjectID()));
				pProject->put_projectNO(CComBSTR(prjDlg.GetProjectNO()));
			}
		}
	}

	if ( m_pAcqMaster == NULL )
	{
		m_pAcqMaster.CoCreateInstance(__uuidof(UrtAcqMaster));

		/* 以下代码是设置数据路径 数据路经为： 工程路径 / 时间 / */
		ExpLabService* pService = ExpLabService::Open();
		CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
		ExpLabService::Close();

		_variant_t varAcqAddr, varDataAddr, varLocalAcq, varAcqPort, varDataPort, varLocalData;
		V_BSTR(&varLocalAcq) = NULL;
		V_BSTR(&varLocalData) = NULL;

		if ( FAILED(pSetting->GetValue( GLOBAL_ACQGROUP_ADDRESS, &varAcqAddr )) )
			varAcqAddr = GROUP_ACQUIRE_ADDRESS;

		if ( FAILED(pSetting->GetValue( GLOBAL_ACQGROUP_PORT, &varAcqPort )) )
			varAcqPort = SHORT(GROUP_ACQUIRE_PORT);

		if ( FAILED(pSetting->GetValue( GLOBAL_DATAGROUP_ADDRESS, &varDataAddr )))
			varDataAddr = GROUP_DATA_ADDRESS;

		if ( FAILED(pSetting->GetValue( GLOBAL_DATAGROUP_PORT, &varDataPort )) )
			varDataPort = GROUP_DATA_PORT;
		
		pSetting->GetValue( GLOBAL_ACQG_LOCAL_IP, &varLocalAcq );
		pSetting->GetValue( GLOBAL_DATA_LOCAL_IP, &varLocalData );

		m_pAcqMaster->Create(V_BSTR(&varAcqAddr), V_I2(&varAcqPort), V_BSTR(&varLocalAcq), V_BSTR(&varDataAddr), V_I2(&varDataPort), V_BSTR(&varLocalData), 0);

		// 注册事件接收器
		LPDISPATCH pHandler = GetIDispatch(FALSE);
		m_pAcqMaster->InstallHandler(pHandler, &m_acqMasterCookie);

		CComPtr<IUrtSlaverDataReceiverMgr> pReceiverMgr;
		m_pAcqMaster->GetSlaverDataReceiverMgr(&pReceiverMgr);

		pReceiverMgr->AttachProject(pProject);

		// 704项目要求自动启动接收和发布
#if (EXPLAB_VERSION & EXPLAB_704_VERSION)
		if ( !m_bLock )
		{
			pReceiverMgr->StartReceiveService(0);
			pReceiverMgr->StartRecordService(0);
		}
#endif	
	}

	if ( m_pAcqMaster != NULL )
	{
		// 注意:一定要间隔一定时间发送命令,不然会出现发送失败情况
		// 往控制组中发送采集终端信息请求
		m_pAcqMaster->SendSlaverProjectInfoRequest(0);
	
		// 往数据发布组中发送接收端信息请求
		::Sleep(100);
		m_pAcqMaster->SendMonitorInfoRequest(0);

		::Sleep(100);
		m_pAcqMaster->SendMasterInfoToDataGroup(0);
	}

	URT_OUTPUT_MSG(eMsgCmdEstablishedLink, MAINFRAME_NAME);

	// 显示控制对话框
#if !USE_MASTER_FORM_VIEW
	PostMessage(WM_COMMAND, ID_TOOL_ENTER_MASTER, 0);
#endif
	
	UpdateMasterFormView(WM_USER_MASTER_ENTERNETTEST, 0, 0);
}

void CMainFrame::OnToolSetreceiver()
{
	if ( m_pAcqReceiver == NULL )
	{
		m_pAcqReceiver.CoCreateInstance(__uuidof(UrtAcqReceiver));

		ExpLabService* pService = ExpLabService::Open();
		CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
		ExpLabService::Close();

		_variant_t varDataAddr, varDataPort, varLocalData;
		V_BSTR(&varLocalData) = NULL;

		if ( FAILED(pSetting->GetValue( GLOBAL_DATAGROUP_ADDRESS, &varDataAddr )))
			varDataAddr = GROUP_DATA_ADDRESS;

		if ( FAILED(pSetting->GetValue( GLOBAL_DATAGROUP_PORT, &varDataPort )) )
			varDataPort = GROUP_DATA_PORT;

		if ( SUCCEEDED(pSetting->GetValue( GLOBAL_DATA_LOCAL_IP, &varLocalData )) )
		{
			if ( CComBSTR(V_BSTR(&varLocalData)) == INADDR_ANY_STRING )
			{
				varLocalData.Clear();
				V_BSTR(&varLocalData) = NULL;
			}
		}
		
		m_pAcqReceiver->Create( V_BSTR(&varDataAddr), V_I2(&varDataPort), V_BSTR(&varLocalData), 0);

		// 注册事件接收器
		LPDISPATCH pHandler = GetIDispatch(FALSE);
		m_pAcqReceiver->InstallHandler(pHandler, &m_acqReceiverCookie);
	}

	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

	CComPtr<IDispatch> pPrjDisp;
	pApplication->GetActiveProject(&pPrjDisp);
	m_pAcqReceiver->AttachProject(pPrjDisp);

	m_pAcqReceiver->SendMasterInfoRequest(0);
	::Sleep(50);

	m_pAcqReceiver->SendMonitorInfoToMaster(true, 0);

	URT_OUTPUT_MSG(eMsgCmdEstablishedLink, MAINFRAME_NAME);
}

BOOL  CMainFrame::OnRecvAcqSlaverSysInfo(LONG lParam)
{
	BSTR strAddr = (BSTR)lParam;
	if ( m_pAcqMaster != NULL && strAddr != NULL )
	{
		ExpLabService* pService = ExpLabService::Open();
		CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
		ExpLabService::Close();

		_variant_t varLocalIP;
		V_BSTR(&varLocalIP) = NULL;
		if ( SUCCEEDED(pSetting->GetValue( GLOBAL_ACQG_LOCAL_IP, &varLocalIP )) )
		{
			if ( CComBSTR(V_BSTR(&varLocalIP)) == INADDR_ANY_STRING )
			{
				varLocalIP.Clear();
				V_BSTR(&varLocalIP) = NULL;
			}
		}

		m_pAcqMaster->ConnectToSlaver(strAddr, 0, V_BSTR(&varLocalIP));
		::SysFreeString(strAddr);
	}

	return TRUE;
}

/************************************************************************/
/* 功能:接收到采集端发送的采集系统信息
/************************************************************************/
BOOL  CMainFrame::OnRecvSubSystemInfo(LONG lParam)
{
	BSTR strFileName = (BSTR)lParam;

	// 只有在停止采集时才可以处理此请求
	EUrtAcquireState acqState = GetAcquireState();
	if ( acqState != eAcquireStoped && acqState != eAcquireNone )
	{
		::SysFreeString(strFileName);
		return FALSE;
	}

	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

	CComPtr<IDispatch> pPrjDisp;
	if ( SUCCEEDED(pApplication->GetActiveProject(&pPrjDisp)) )
	{
		CComQIPtr<IVXIProject> pProject(pPrjDisp);
		CComPtr<IDispatch> pSubSystemDisp;
		HRESULT hRet = pProject->ImportSubSytemFromFile(strFileName, 0, &pSubSystemDisp);

		m_wndWorkSpace.FillClassView(pProject, GetSystemType());

		UpdateChannelView(ID_TOOL_UPDATECHANSLIST);

		EUrtModifyOperate operTag = (hRet==S_OK ? eModifyAddChild :eModifyModified);
		UpdateConfigView(CComQIPtr<IUrtObject>(pProject), operTag);
	}

	::SysFreeString(strFileName);

	return TRUE;
}

/************************************************************************/
/* 功能: 接收到主控端发送过来的实验配置表,用表中的通道配置更新当前系统中的通道
/************************************************************************/
BOOL CMainFrame::OnRecvTestConfigInfo(LONG lParam)
{
	ASSERT(lParam != NULL);

	BSTR strPath = (BSTR)lParam;
	EUrtAcquireState acqState = GetAcquireState();
	if ( acqState != eAcquireStoped )
	{
		::SysFreeString(strPath);
		return TRUE;
	}

	CComPtr<IUrtCommand> pCommand;
	pCommand.CoCreateInstance(__uuidof(UrtChanCfgExportAndImportCmd));

	if ( pCommand != NULL && ::PathFileExists(strPath) )
	{
		CComQIPtr<IUrtChanCfgExportAndImportCmd> pChanCfgCmd(pCommand);
		pChanCfgCmd->SetCmdType(eCFGT_IMPORT);

		pChanCfgCmd->put_fileName(strPath);

		// 判别导入的通道配置是否是本地产生的配置
		char szHostName[256] = "";
		gethostname(szHostName, 256);

		EUrtSystemType sysType = GetSystemType();
		pChanCfgCmd->SetBaseInfo(sysType, CComBSTR(szHostName));

		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		if ( S_OK == pCommand->Execute(pApplication, 1) )
		{
			CComPtr<IDispatch> pPrjDisp;
			pApplication->GetActiveProject(&pPrjDisp);

			CComQIPtr<IVXIProject> pActivePrj(pPrjDisp);
			m_wndWorkSpace.FillClassView(pActivePrj, GetSystemType(), eObjTProject);
			UpdateChannelView(ID_TOOL_UPDATECHANSLIST);

			UpdateConfigView(CComQIPtr<IUrtObject>(pActivePrj), eModifyAddChild);
		}
	}

	::SysFreeString(strPath);

	return TRUE;
}

BOOL  CMainFrame::OnRecvAcqSlaverInfoRequest(LONG lParam)
{
	// 只有采集端才需要处理次消息
	if ( m_pAcqSlaver == NULL )
		return FALSE;

	ASSERT(lParam != NULL);

	CComPtr<IUrtSlaverDataReleaserMgr> pReleaserMgr;
	m_pAcqSlaver->GetSlaverDataReleaserMgr(&pReleaserMgr);
	
	BSTR strAddr = (BSTR)lParam;
	pReleaserMgr->SendAcqSlaverInfo(strAddr);

	::SysFreeString(strAddr);
	return TRUE;
}

/************************************************************************/
/* 接收到子系统信息请求，由主控发送过来的信息
/* 此函数用于远程配置，
/************************************************************************/
BOOL  CMainFrame::OnRecvSubSystemInfoRequest(LONG lParam)
{
	// 只有采集端才需要处理次消息
	if ( m_pAcqSlaver == NULL )
		return FALSE;

	// 只有在停止采集时才可以处理此请求
	EUrtAcquireState acqState = GetAcquireState();
	if ( acqState != eAcquireStoped && acqState != eAcquireNone    )
		return FALSE;

	SUBSYSTEMITEM* pItem = (SUBSYSTEMITEM*)lParam;

	OLECHAR szGUID[64];
	::StringFromGUID2(pItem->objGUID, szGUID, 64);

	CComPtr<IUrtObjId> pObjId;
	pObjId.CoCreateInstance(__uuidof(UrtObjId));
	pObjId->FromString( CComBSTR(szGUID) );

	CComPtr<IUrtObject> pSubSystemObj;
	while ( SUCCEEDED(pObjId->GetObject(&pSubSystemObj)) )
	{
		EUrtObjectType objType;
		pSubSystemObj->get_objectType(&objType);

		if ( eObjTSubSystem != objType )
			break;
		
		WCHAR   wszFileName[50] = {'\0'};
		VARIANT varData;
		BYTE*   pBuf    = NULL;
		LONG    msgLen  = 0;
		LONG    fileLen = 0;
		LONG    guidLen     = sizeof(OLECHAR)*64;   // 固定长度,不能修改
		LONG	fileNameLen = sizeof(WCHAR)*50;     // 固定长度,不能修改

		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		CComPtr<IDispatch> pPrjDisp;
		pApplication->GetActiveProject(&pPrjDisp);
		CComQIPtr<IVXIProject> pProject(pPrjDisp);

		TCHAR szTempPath[MAX_PATH];
		::GetTempPath(MAX_PATH, szTempPath);

		CComBSTR strFileName(szTempPath);

		// 以当前的时间作为导出文件的名称,避免冲突
		SYSTEMTIME localTime;
		::GetLocalTime(&localTime);
		wsprintf(wszFileName, L"%d_%d_%d",localTime.wHour, localTime.wMinute, localTime.wSecond);

		strFileName += wszFileName;
		strFileName += L".tst";

		// 首先把内容序列化到文件中
		if ( FAILED(pProject->ExportSubSytemToFile(pSubSystemObj, strFileName, 0)) )
		{
			ASSERT(0);
			break;
		}

		V_VT(&varData) = VT_I1 | VT_BYREF;

		CFile file;
		if ( !file.Open( strFileName, CFile::modeRead) )
			break;

		fileLen = (ULONG)file.GetLength();

		msgLen = guidLen + fileNameLen + fileLen;
		pBuf = new BYTE[msgLen];

		file.Read( &pBuf[guidLen+fileNameLen], fileLen );
		file.Close();

		// 文件使用完后就删除
		::DeleteFile(strFileName);

		::memcpy(pBuf, szGUID, guidLen);
		::memcpy( &pBuf[guidLen], wszFileName, fileNameLen);

		CComPtr<IUrtTransMsgObj> pMsgObj;
		pMsgObj.CoCreateInstance(__uuidof(UrtTransMsgObj));

		V_I1REF(&varData) = (CHAR*)pBuf;
		pMsgObj->put_msgType(eMsgTypeSubSystemInfo);
		pMsgObj->SetMsgData(varData, msgLen);

		CComPtr<IDispatch> pLinkMgrDisp;
		m_pAcqSlaver->GetSlaverLinkMgr(&pLinkMgrDisp);

		CComQIPtr<IUrtAcqSlaverLinkMgr> pLinkMgr(pLinkMgrDisp);
		pLinkMgr->Send(pItem->address, pMsgObj);

		delete [] pBuf;

		break;
	}

	delete pItem;
	return TRUE;
}

BOOL  CMainFrame::OnRecvStartAcquireAndSave(LONG lParam)
{
	if ( StartAcquire(eExecImmediate, NULL, 0) )
	{
		if ( StartSave(eExecImmediate, NULL) )
		{
#if(FLEXLIC)
			if (CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICCYCLESAVE)) && m_pAquireMgr)
#else
			if ( USE_CYCLE_SAVE_MODULE && m_pAquireMgr != NULL )
#endif
			{
				// 如果成功启动了记录,则启动循环记录，高速缓冲
				m_pAquireMgr->StartCycleSave(eExecImmediate, NULL);
			}
		}

		SendSlaverMsgToMaster(eMsgTypeStartAcquireAndRecord, NULL, 0);
	}

	return TRUE;
}
/************************************************************************/
/* 响应主控端发送过来的命令
/************************************************************************/
LRESULT  CMainFrame::OnSlaverMsg(WPARAM wParam, LPARAM lParam)
{
	EUrtTransMsgType msgType = (EUrtTransMsgType)wParam;

	switch( msgType )
	{
	case eMsgTypeStartAcquire:
		StartAcquire(eExecImmediate, NULL, 0);
		break;
	case eMsgTypeEndAcquire:
		StopAcquire(eExecImmediate, NULL);
		break;
	case eMsgTypeStartRecord:
		StartSave(eExecImmediate, NULL);
		break;
	case eMsgTypeEndRecord:
		StopSave(eExecImmediate, NULL);
		break;
	case eMsgTypeStartAcquireAndRecord:
		if ( StartAcquire(eExecImmediate, NULL, 0) == TRUE )
		{
			if ( StartSave(eExecImmediate, NULL) )
				SendSlaverMsgToMaster(eMsgTypeStartAcquireAndRecord, NULL, 0);
		}
		break;
	case eMsgTypeStartCycleSave:
		StartCycleSave(eExecImmediate, NULL);
		break;
	case eMsgTypeStopCycleSave:
		StopCycleSave(eExecImmediate, NULL);
		break;
	case eMsgTypeExecCycleSave:
		ExecCycleSave(eExecImmediate, NULL);
		break;
	case eMsgTypeSubSystemInfoRequest:				// 子系统信息请求,远程配置
		OnRecvSubSystemInfoRequest(lParam); 
		break;
	case eMsgTypeMasterSubSystemInfo:				// 主控端发送过来的子系统信息,远程配置
		OnRecvSubSystemInfo(lParam);
		break;
	case eMsgTypeAcqSlaverInfoRequest:
		OnRecvAcqSlaverInfoRequest(lParam);			// 接收到发送采集端信息命令
		break;
	case eMsgTypeTestItemTableInfoRequest:			// 测试项目配置列表请求
		OnSlaverRecvReceiverMsg(msgType, lParam);
		break;
	case eMsgTypeEstablishedLink:
	case eMsgTypeCloseLink:
	case eMsgTypeLinkError:
	{
		if ( lParam != NULL )
		{
			BSTR strAddr = (BSTR)lParam;
			::SysFreeString(strAddr);
		}
	}
		break;
	case eMsgTypeTestConfigInfo:
		OnRecvTestConfigInfo(lParam);
		break;
	case eMsgTypeRemoteShutDown:
		OnRecvRemoteShutDown(lParam);
		break;
	case eMsgTypeManualTrigger:
		OnRecvManualTrigger(lParam);
		break;
	case eMsgTypeDeviceSelfTest:
		OnSelfTestAll();
		break;
	case eMsgTypeDeviceReset:
		OnResetDeviceAll();
		break;
	case eMsgTypeSyncControl:
		OnSyncControlAll();
		break;
	case eMsgTypeZeroAcquire:
		DoZeroacqStart();
		break;
	case eMsgTypeShuntON:
		OnShuntOn();
		break;
	case eMsgTypeShuntOFF:
		OnShuntOff();
		break;
	case eMsgTypeUserCmd:
		{
			CString strCmd = (BSTR)lParam;
			if ( strCmd.GetLength()==0 )
				return 0;

			CString subStr;
			LONG lCmd, lChan, lMode;
			AfxExtractSubString(subStr, strCmd, 1, _T(';'));
			lCmd = _tstoi(subStr);
			AfxExtractSubString(subStr, strCmd, 2, _T(';'));
			lChan = _tstoi(subStr);
			AfxExtractSubString(subStr, strCmd, 3, _T(';'));
			lMode = _tstoi(subStr);

			CComQIPtr<IUrtChannel> pSelChan;
			if ( lChan > 0 )
			{
				CComPtr<IUrtChannelVisitor> pChanVisitor = m_pAquireMgr->GetChannelVisitor();
				LONG chanCount;
				pChanVisitor->get_channelCount(&chanCount);
				for ( LONG i=0; i<chanCount; i++ )
				{
					CComPtr<IUrtChannel> pChan;
					pChanVisitor->GetChannelByIndex(i, &pChan);
					LONG bSerialNumber = 0;

					pChan->get_serialNumber(&bSerialNumber);
					if ( bSerialNumber == lChan )
					{
						pSelChan = pChan;
						break;
					}
				}

				if ( !pSelChan )
				{
					strCmd.Replace(_T(")"), _T(";-1)"));
					SendSlaverMsgToMaster(eMsgTypeUserCmd, CComBSTR(strCmd), FALSE);
					return 0;
				}
			}

			HRESULT hr = S_OK;
			LONG calType = 0;
			switch ( lCmd )
			{
			case 1:
				calType = eCalMeasLeadWireAndOTZero;
				break;
			case 2:
				calType = eCalOTZero;
				break;
			case 3:
				calType = eCalOTFastZero;
				break;
			case 4:
				calType = eCalMeasLeadWire;
				break;
			case 5:
				calType = eCalCheckChan;
				break;
			case 6:
				if ( m_pAquireMgr == NULL || m_pAquireMgr->GetAcquireState() != eAcquireStoped )
				{
					hr = S_OK;
					break;
				}

				if (!lMode && !PreStartAcquire())
				{
					hr = E_FAIL;
					break;
				}

				hr = StartAcquire(eExecImmediate, NULL, 0)==TRUE?S_OK:E_FAIL;
				break;
			case 7:
				if ( m_pAquireMgr == NULL || m_pAquireMgr->GetAcquireState() == eAcquireStoped )
				{
					hr = S_OK;
					break;
				}

				if (!lMode && !PreStopAcquire())
				{
					hr = E_FAIL;
					break;
				}

				hr = StopAcquire(eExecImmediate, NULL)==TRUE?S_OK:E_FAIL;
				break;
			case 8:
				hr = DoCalOprAll(eCalMeasShuntON);
				break;
			case 9:
				hr = DoCalOprAll(eCalMeasShuntOFF);
				break;
			case 10:
				hr = DoCalOprAll(eCalMeasShunt);
				break;
			case 11:
				hr = StartSave(eExecImmediate, NULL)==TRUE?S_OK:E_FAIL;
				break;
			case 12:
				hr = StopSave(eExecImmediate, NULL)==TRUE?S_OK:E_FAIL;
				break;
			case 13:
				hr = StartCycleSave(eExecImmediate, NULL)==TRUE?S_OK:E_FAIL;
				break;
			case 14:
				hr = StopCycleSave(eExecImmediate, NULL)==TRUE?S_OK:E_FAIL;
				break;
			case 15:
				hr = ExecCycleSave(eExecImmediate, NULL)==TRUE?S_OK:E_FAIL;
				break;
			case 16:
				if ( m_pAquireMgr->GetAcquireState() == eAcquireStoped )
				{
					hr = E_FAIL;
					break;
				}

				if ( lMode )
					SetZeroState(TRUE);
				else
				{
					if ( IDYES == MessageBox(STR_SURE_EXECUTE_ZEROACQ, EXPLAB_TITLE_NAME, MB_YESNO|MB_ICONQUESTION) )
						SetZeroState(TRUE);
					else
					{
						hr = E_FAIL;
						break;
					}
				}
				hr = S_OK;
				break;
			case 17:
				{
					if ( !lMode && MessageBox(STR_MSG_SURE_RESET_ZEROVALUE, EXPLAB_TITLE_NAME, MB_YESNO|MB_ICONQUESTION) != IDYES )
						break;

					CComPtr<IUrtCommand> pCommand;
					pCommand.CoCreateInstance(__uuidof(UrtSimpleCmd));

					CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
					hr = pCommand->Execute(pApplication, eSMPCMD_ZeroRestoration);

					UpdateChannelView(ID_TOOL_UPDATECHANSLIST);
				}
				break;
			default:
				strCmd.Replace(_T(")"), _T(";-2)"));
				SendSlaverMsgToMaster(eMsgTypeUserCmd, CComBSTR(strCmd), FALSE);
				return 0;
				break;
			}

			if ( calType )
			{
				CComPtr<IUrtCommand> pCommand;
				pCommand.CoCreateInstance(__uuidof(UrtCalibrateOperatorCmd));
				CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
				CComQIPtr<IUrtCalibrateOperatorCmd> pCalOprCmd(pCommand);
				pCalOprCmd->AttachRootObject(pSelChan);
				pCalOprCmd->SetParam(GetAcquireState());
				pCalOprCmd->SetMode(lMode);
				hr = pCommand->Execute(pApplication, calType);
			}

			CString str;
			str.Format(_T(";%d)"), SUCCEEDED(hr));
			strCmd.Replace(_T(")"), str);
			SendSlaverMsgToMaster(eMsgTypeUserCmd, CComBSTR(strCmd), SUCCEEDED(hr));
		}
		break;
	default:
		break;
	}
	return 0;
}

/************************************************************************/
/* 功能: 更新主控视图
/************************************************************************/
void   CMainFrame::UpdateMasterFormView(WPARAM wParam, LPARAM lParam, LPARAM lParam1)
{
	int a = 0;
	CVXIConfigApp *pApp = (CVXIConfigApp*)AfxGetApp();

	CDocument* pDoc = pApp->GetActiveMasterDoc();
	while ( pDoc != NULL )
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		if ( pos == NULL )
			break;

		CView* pView = pDoc->GetNextView(pos);
		if ( pView == NULL )
			break;

		pView->PostMessage(wParam, lParam, lParam1);
		break;
	}
		
	return ;
}

BOOL  CMainFrame::OnSlaverRecvReceiverMsg(EUrtTransMsgType msgType, LONG lParam)
{
	BSTR strIPAddr = (BSTR)lParam;

	CComPtr<IUrtSlaverDataReleaserMgr> pReleaseMgr;
	m_pAcqSlaver->GetSlaverDataReleaserMgr(&pReleaseMgr);
	switch ( msgType )
	{
	case eMsgTypeTestItemTableInfoRequest:
		pReleaseMgr->SendTestItemTableToDataGroup(strIPAddr);
		break;
	default:
		break;
	}

	::SysFreeString(strIPAddr);

	return TRUE;
}

BOOL  CMainFrame::OnMasterRecvReceiverMsg(EUrtTransMsgType msgType, LONG lParam)
{	
	BSTR strIPAddr = (BSTR)lParam;

	CComPtr<IUrtSlaverDataReceiverMgr> pReceiverMgr;
	m_pAcqMaster->GetSlaverDataReceiverMgr(&pReceiverMgr);

	if ( msgType == eMsgTypeMasterSlaverInfoRequest )
		pReceiverMgr->SendMasterSlaverInfoToReceiver(strIPAddr);

	if ( msgType == eMsgTypeTestItemTableInfoRequest )
		pReceiverMgr->SendTestItemTableToDataGroup(strIPAddr);

	::SysFreeString(strIPAddr);

	return TRUE;
}

// 响应主控端消息
LRESULT  CMainFrame::OnMasterMsg(WPARAM wParam, LPARAM lParam)
{
	EUrtTransMsgType msgType = (EUrtTransMsgType)wParam;
	BSTR strSenderIP = (BSTR)lParam;
	switch( msgType )
	{
	case eMsgTypeSubSystemInfo:         // 接收到子系统信息
		OnRecvSubSystemInfo(lParam);
		break;
	case eMsgTypeAcqSlaverSysInfo:      // 接收到采集系统信息,用于建立连接
		OnRecvAcqSlaverSysInfo(lParam);
		break;
	case eMsgTypeAcqSlaverInfo:			// 采集端采集系统配置信息
		break;
	case eMsgTypeEstablishedLink:
		if (m_pOutput->IsKindOf(RUNTIME_CLASS(COutputBarEx)))
		{
			COutputBarEx *pBar = DYNAMIC_DOWNCAST(COutputBarEx,m_pOutput);
			pBar->GetLogWnd().AddNewLogWnd(CString(strSenderIP));
			UpdateMasterFormView(WM_USER_MASTER_UPDATESLAVER, wParam, lParam);
		}
		break;

	case eMsgTypeCloseLink:
		if (m_pOutput->IsKindOf(RUNTIME_CLASS(COutputBarEx)))
		{
			COutputBarEx *pBar = DYNAMIC_DOWNCAST(COutputBarEx,m_pOutput);
			pBar->GetLogWnd().RemoveLogWnd(CString(strSenderIP));
			UpdateMasterFormView(WM_USER_MASTER_UPDATESLAVER, wParam, lParam);
		}
		break;
	case eMsgTypeLinkError:
	case eMsgTypeStartAcquire:
	case eMsgTypeEndAcquire:
	case eMsgTypeStartRecord:
	case eMsgTypeEndRecord:
	case eMsgTypeStartAcquireAndRecord:
	{
		// 必须采用发送消息的机制，它们不是在同一个线程，调用窗口函数会出问题
		UpdateMasterFormView(WM_USER_MASTER_UPDATESLAVER, wParam, lParam);
	}
		break;
	case eMsgTypeMonitorEnter:
	case eMsgTypeMonitorLeave:
	{
		// 必须采用发送消息的机制，它们不是在同一个线程，调用窗口函数会出问题
		UpdateMasterFormView(WM_USER_MASTER_UPDATERECEIVER, wParam, lParam);
	}
		break;

	case eMsgTypeMasterSlaverInfoRequest:			// 采集端数据
	case eMsgTypeTestItemTableInfoRequest:			// 测试项目配置列表请求
	case eMsgTypeReceiverEstablishedLink:
		OnMasterRecvReceiverMsg(msgType, lParam);
		break;
	case eMsgTypeSlaverLogMsg:
		OnMasterRecvSlaverLogMsg(msgType, lParam);
		break;
	case eMsgTypeStartRecordEx:
	case eMsgTypeEndRecordEx:
	{
		// 必须采用发送消息的机制，它们不是在同一个线程，调用窗口函数会出问题
		UpdateMasterFormView(WM_USER_MASTER_UPDATESERVICE, wParam, lParam);
	}
		break;
	default:
		break;
	}

	return 0;
}

BOOL CMainFrame::OnMasterRecvSlaverLogMsg(EUrtTransMsgType msgType, LONG lParam)
{
	if ( lParam == 0 )
		return FALSE;

	BSTR strSenderIP = (BSTR)lParam;
	CString str = CString(strSenderIP)+_T(",")+ CString(strSenderIP);
	CString resToken;
	int curPos= 0;
	CString strIP =str.Tokenize(_T(","), curPos);

	LogMsg* pLogMsg = new LogMsg;
	WPARAM wParam = (WPARAM)pLogMsg;
	lParam = (LPARAM)strIP.AllocSysString();

	ClogMsg(GetSafeHwnd(), &wParam, &lParam);

	resToken= str.Tokenize(_T(","), curPos);
	while ( curPos != -1 )
	{
		pLogMsg->msgType = resToken;
		resToken= str.Tokenize(_T(","), curPos);
		if ( curPos == -1 )
			break;

		pLogMsg->msgSrc =/* _T("[") +strIP +_T("]") + */resToken ;
		resToken= str.Tokenize(_T(","), curPos);
		if ( curPos == -1 )
			break;

		pLogMsg->msgInfo = resToken;
		resToken= str.Tokenize(_T(","),curPos);
		if ( curPos == -1 )
			break;

		pLogMsg->msgCode = resToken;
		break;
	}

	SysFreeString(strSenderIP);

	return TRUE;
}

// 响应接收端消息处理
LRESULT  CMainFrame::OnReceiverMsg(WPARAM wParam, LPARAM lParam)
{
	if ( m_pAcqReceiver == NULL )
		return 0L;

	// 非接收端，不处理此消息
	EUrtTransMsgType msgType = (EUrtTransMsgType)wParam;
	switch( msgType )
	{
	case eMsgTypeAcqMasterData:
	{
		// 数据分发到各个通道中，同时发送到监视控件
		m_pAcqReceiver->UpdateContent(lParam);

		// 如果采集端和接收端同时存在时,且采集正在进行,则采用采集的刷新速度进行刷新
		if ( GetAcquireState() != eAcquireStarting )
		{
			OnUpdateContent(0, lParam);
		}
	}
		break;
	case eMsgTypeMasterSlaverInfo:
	case eMsgTypeTestItemTableInfo:
	{
		CDocTemplate* pDocTemp = theApp.GetGraphicDocTemplate();
		POSITION pos = pDocTemp->GetFirstDocPosition();
		while ( pos )
		{
			CGraphicDoc* pGItem = dynamic_cast<CGraphicDoc*>(pDocTemp->GetNextDoc(pos));
			if ( pGItem != NULL )
			{
				pGItem->UpdateNetDataLink();
			}
		}

		CVXIConfigApp *pApp = DYNAMIC_DOWNCAST(CVXIConfigApp,AfxGetApp());
		CGraphicPrjDoc *pDoc = DYNAMIC_DOWNCAST(CGraphicPrjDoc,pApp->GetActiveNewGraphicDoc());
		if ( pDoc != NULL )
		{
			pDoc->UpdateNetDataLink();
		}

		CComPtr<IUrtApplication> pApplication = Urt_GetApplication();
		
		CComPtr<IDispatch> pPrjDisp;
		pApplication->GetActiveProject(&pPrjDisp);

		CComQIPtr<IVXIProject> pActivePrj(pPrjDisp);
		m_wndWorkSpace.FillClassView(pActivePrj, GetSystemType(), eObjTProject);

		// 如果采集端和接收端同时存在时,则不更新,因为视图显示的是采集端的配置
		EUrtSystemType sysType = GetSystemType();
		if ( !(sysType & eSysTSlaver) )
		{
			UpdateChannelView(ID_TOOL_UPDATECHANSLIST);
		}
		
		// 更新扫描表时,只要进行视图更新就可以了
		if ( msgType == eMsgTypeTestItemTableInfo )
			UpdateConfigView(CComQIPtr<IUrtObject>(pActivePrj), eModifyModified);
		else
			UpdateConfigView(CComQIPtr<IUrtObject>(pActivePrj), eModifyAddChild);

		// 先停止更新视图
		StopUpdateView();

		// 初始化后再开始更新视图
		InitManagerService(pActivePrj);
		StartUpdateView();
	}
		break;
	case eMsgTypeMonitorState:   // 收到允许接收数据命令,则刷新连接信息
	{
		if ( lParam == 1 )
		{
			CDocTemplate* pDocTemp = theApp.GetGraphicDocTemplate();
			POSITION pos = pDocTemp->GetFirstDocPosition();
			while ( pos )
			{
				CGraphicDoc* pGItem = dynamic_cast<CGraphicDoc*>(pDocTemp->GetNextDoc(pos));
				if ( pGItem != NULL )
					pGItem->UpdateNetDataLink();
			}
		}
	}	
		break;
	case eMsgTypeMasterInfo:  // 接收到主控信息,则与主控建立TCP/IP连接
	{
		BSTR strSenderIP = (BSTR)lParam;

		ExpLabService* pService = ExpLabService::Open();
		CComPtr<IUrtGlobalSetting> pSetting = pService->GetGlobalSettingService();
		ExpLabService::Close();

		_variant_t varLocalIP;
		V_BSTR(&varLocalIP) = NULL;
		if ( SUCCEEDED(pSetting->GetValue( GLOBAL_DATA_LOCAL_IP, &varLocalIP )) )
		{
			if ( CComBSTR(V_BSTR(&varLocalIP)) == INADDR_ANY_STRING )
			{
				varLocalIP.Clear();
				V_BSTR(&varLocalIP) = NULL;
			}
		}

		// 如果连接失败则发出一条日志
		if ( S_OK != m_pAcqReceiver->ConnectToMaster(strSenderIP, 0, V_BSTR(&varLocalIP)) )
			URT_OUTPUT_CMDINFO(eMsgCmdLinkError, strSenderIP);
		
		::SysFreeString(strSenderIP);
	}
		break;
	case eMsgTypeReceiverEstablishedLink:
	{
		BSTR strSenderIP = (BSTR)lParam;
		m_pAcqReceiver->SendMasterSlaverInfoRequest(0);

		::Sleep(200);

		m_pAcqReceiver->SendTestItemTableRequest(0);
		::SysFreeString(strSenderIP);

		URT_OUTPUT_CMDINFO(eMsgCmdEstablishedLink, strSenderIP);
	}
		break;
	case eMsgTypeStopSnapshot:
	{
		if ( m_pSnapshotMgr != NULL )
			m_pSnapshotMgr->DoSnapshot(0);
	}
	break;
	case eMsgTypeContinueSnapshot:
		{
			if ( m_pSnapshotMgr != NULL )
				m_pSnapshotMgr->DoSnapshotContinue(lParam);
		}
		break;
	default:
		break;
	}

	return 0L;
}

LRESULT  CMainFrame::OnSendMsgToMaster(WPARAM wParam, LPARAM lParam)
{
	EUrtTransMsgType msgType = (EUrtTransMsgType)wParam;

	SendSlaverMsgToMaster(msgType, NULL, lParam);
	return 0;
}

// 采集端程序会调用此函数，发送消息到主控端
BOOL    CMainFrame::SendSlaverMsgToMaster(EUrtTransMsgType msgType, BSTR strMsg, LONG lParam)
{
	CComPtr<IDispatch> pLinkMgrDisp;
	if ( m_pAcqSlaver == NULL || FAILED(m_pAcqSlaver->GetSlaverLinkMgr(&pLinkMgrDisp)) )
		return FALSE;

	CComQIPtr<IUrtAcqSlaverLinkMgr> pLinkMgr(pLinkMgrDisp);

	CComPtr<IUrtTransMsgObj> pMsgObj;
	pMsgObj.CoCreateInstance(__uuidof(UrtTransMsgObj));
	pMsgObj->put_msgType(msgType);

	switch( msgType )
	{
	case eMsgTypeStartAcquire:
	case eMsgTypeEndAcquire:
	case eMsgTypeStartRecord:
	case eMsgTypeEndRecord:
	case eMsgTypeStartAcquireAndRecord:
		m_pAcqSlaver->InitSlaverProjectInfo(NULL, msgType);
	case eMsgTypeStartCycleSave:
	case eMsgTypeStopCycleSave:
	case eMsgTypeExecCycleSave:
	case eMsgTypeZeroAcquire:
	case eMsgTypeShuntON:
	case eMsgTypeShuntOFF:
	{
		VARIANT varData;
		V_VT(&varData) = VT_I4 | VT_BYREF;
		LONG* pData = new LONG[1];
		V_I4REF(&varData) = pData;
		pData[0] = lParam;

		pMsgObj->SetMsgData(varData, 1);	
		pLinkMgr->Send(L"", pMsgObj);

		delete [] pData;
	}
		break;
	case eMsgTypeUserCmd:
	default:
	{
		if ( strMsg != NULL )
		{
			VARIANT varData;
			V_VT(&varData)	  = VT_I1 | VT_BYREF;
			V_I1REF(&varData) =_com_util::ConvertBSTRToString(strMsg);	
			
			pMsgObj->SetMsgData(varData,1+strlen(V_I1REF(&varData)));	
			pLinkMgr->Send(L"", pMsgObj);

			SysFreeString(strMsg);
			delete [] V_I1REF(&varData);
		}
	}
		break;
	}

	return TRUE;
}

// 主控程序会调用此函数，发送消息到采集端
// 采集端程序会调用此函数,发送消息到接收端
BOOL    CMainFrame::SendMasterMsgToSlaver(EUrtTransMsgType msgType, LONG lParam)
{
	CComPtr<IDispatch> pLinkMgrDisp;
	if ( m_pAcqMaster != NULL &&
		SUCCEEDED(m_pAcqMaster->GetMasterLinkMgr(&pLinkMgrDisp)) )
	{
		CComPtr<IUrtTransMsgObj> pMsgObj;
		pMsgObj.CoCreateInstance(__uuidof(UrtTransMsgObj));

		pMsgObj->put_msgType(msgType);

		CComQIPtr<IUrtAcqMasterLinkMgr> pLinkMgr(pLinkMgrDisp);
		pLinkMgr->Send(L"", pMsgObj);
	}

	return TRUE;
}

BOOL CMainFrame::SendSlaverMsgToReceiver(EUrtTransMsgType msgType, LONG lParam, EUrtSendDestType destType)
{
	CComPtr<IDispatch> pLinkMgrDisp;
	if ( m_pAcqSlaver != NULL && SUCCEEDED(m_pAcqSlaver->GetSlaverLinkMgr(&pLinkMgrDisp)) )
	{
		CComPtr<IUrtTransMsgObj> pMsgObj;
		pMsgObj.CoCreateInstance(__uuidof(UrtTransMsgObj));

		pMsgObj->put_msgType(msgType);
		switch ( msgType )
		{
		case eMsgTypeContinueSnapshot:
			{
				VARIANT	varBuff;
				V_VT(&varBuff) = VT_BYREF|VT_I4;
				V_I4REF(&varBuff) = &lParam;
				pMsgObj->SetMsgData(varBuff, sizeof(LONG));
			}
			break;
		default:
			break;
		}

		CComQIPtr<IUrtAcqSlaverLinkMgr> pLinkMgr(pLinkMgrDisp);

		switch ( destType )
		{
		default:
		case eSDT_TCPIP:
			pLinkMgr->SendToReceiver(L"", pMsgObj);
			break;
		case eSDT_DATA_GROUP:
			pLinkMgr->SendToDataGroup(pMsgObj);
			break;
		case eSDT_DATA_GROUPEX:
			pLinkMgr->SendToDataGroupEx(pMsgObj);
			break;
		}
	}

	return TRUE;
}

/************************************************************************/
/* 配置应用到采集端
/************************************************************************/
void CMainFrame::OnApplytoSlaver()
{
	// 只有主控端才可以调用此函数
	if ( m_pAcqMaster == NULL )
		return ;

	// 只有在停止采集时才可以处理此请求
	EUrtAcquireState acqState = GetAcquireState();
	if ( acqState != eAcquireStoped && acqState != eAcquireNone    )
		return ;

	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

	CComPtr<IDispatch> pPrjDisp;
	pApplication->GetActiveProject(&pPrjDisp);
	CComQIPtr<IVXIProject> pProject(pPrjDisp);

	CComPtr<IUrtSelSet> pSelSet;
	pApplication->GetSelSet(&pSelSet);

	LONG lNum;
	pSelSet->GetNumOfSelectedObj(&lNum);
	if ( lNum <= 0 )
		return ;

	CComPtr<IUrtObjId> pObjId;
	pSelSet->GetSelectedObj(0, &pObjId);

	CComBSTR SystemId, strIPAddr;
	pObjId->ToString(&SystemId);

	if ( FAILED(m_pAcqMaster->GetSubSystemInfo(SystemId, &strIPAddr, NULL, NULL)) )
		return ;

	CComPtr<IUrtObject> pSubSystemObj;
	while ( SUCCEEDED(pObjId->GetObject(&pSubSystemObj)) )
	{
		EUrtObjectType objType;
		pSubSystemObj->get_objectType(&objType);

		if ( eObjTSubSystem != objType )
			break;

		WCHAR   wszFileName[50] = {'\0'};
		VARIANT varData;
		BYTE*   pBuf    = NULL;
		LONG    msgLen  = 0;
		LONG    fileLen = 0;
		LONG    guidLen     = sizeof(OLECHAR)*64;  // 固定长度,不允许修改
		LONG	fileNameLen = sizeof(WCHAR)*50;    // 固定长度,不允许修改

		::ZeroMemory(wszFileName, fileNameLen);

		TCHAR szTempPath[MAX_PATH];
		::GetTempPath(MAX_PATH, szTempPath);

		CComBSTR strFileName(szTempPath);

		// 以当前的时间作为导出文件的名称,避免冲突
		SYSTEMTIME localTime;
		::GetLocalTime(&localTime);
		wsprintf(wszFileName, L"%d_%d_%d",localTime.wHour, localTime.wMinute, localTime.wSecond);

		strFileName += wszFileName;
		strFileName += L".tst";

		// 首先把内容序列化到文件中
		if ( FAILED(pProject->ExportSubSytemToFile(pSubSystemObj, strFileName, 0)) )
		{
			ASSERT(0);
			break;
		}
		
		V_VT(&varData) = VT_I1 | VT_BYREF;

		CFile file;
		if ( !file.Open( strFileName, CFile::modeRead) )
			break;

		fileLen = (ULONG)file.GetLength();

		msgLen = guidLen + fileNameLen + fileLen;
		pBuf = new BYTE[msgLen];

		file.Read( &pBuf[guidLen+fileNameLen], fileLen );
		file.Close();

		// 文件使用完后就删除
		::DeleteFile(strFileName);

		::memcpy(pBuf, SystemId, guidLen);
		::memcpy( &pBuf[guidLen], wszFileName, fileNameLen);

		CComPtr<IUrtTransMsgObj> pMsgObj;
		pMsgObj.CoCreateInstance(__uuidof(UrtTransMsgObj));

		V_I1REF(&varData) = (CHAR*)pBuf;
		pMsgObj->put_msgType(eMsgTypeMasterSubSystemInfo);
		pMsgObj->SetMsgData(varData, msgLen);

		CComPtr<IDispatch> pLinkMgrDisp;
		m_pAcqMaster->GetMasterLinkMgr(&pLinkMgrDisp);

		CComQIPtr<IUrtAcqMasterLinkMgr> pLinkMgr(pLinkMgrDisp);
		pLinkMgr->Send(strIPAddr, pMsgObj);

		delete [] pBuf;

		break;
	}
}

/************************************************************************/
/* 从采集端同步子系统信息
/************************************************************************/
void CMainFrame::OnSyncfromMaster()
{
	if ( m_pAcqMaster == NULL )
		return ;

	CComPtr<IUrtApplication> pApplication = Urt_GetApplication();

	CComPtr<IDispatch> pPrjDisp;
	pApplication->GetActiveProject(&pPrjDisp);
	CComQIPtr<IVXIProject> pProject(pPrjDisp);

	CComPtr<IUrtSelSet> pSelSet;
	pApplication->GetSelSet(&pSelSet);

	LONG lNum;
	pSelSet->GetNumOfSelectedObj(&lNum);
	if ( lNum <= 0 )
		return ;

	CComPtr<IUrtObjId> pObjId;
	pSelSet->GetSelectedObj(0, &pObjId);
	CComPtr<IUrtObject> pSelObj;

	if ( FAILED(pObjId->GetObject(&pSelObj)) )
		return ;
	
	EUrtObjectType objType;
	pSelObj->get_objectType(&objType);

	if ( eObjTSubSystem == objType )
	{
		CComBSTR strId;
		pObjId->ToString(&strId);

		m_pAcqMaster->SendSubSystemInfoRequest(strId, 0);
	}
}

void CMainFrame::OnToolEnterMaster()
{
#if(FLEXLIC)
	if ( CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_FEAT_MASTER_FORM_VIEW,FLEX_SHOWERROR,LM_CO_LOCALTEST))
#endif
	{
		//if ( USE_MASTER_FORM_VIEW )
		//	return ;

		ASSERT( m_pAcqMaster != NULL );

		BOOL bWindowVisible = FALSE;
		if ( m_pAlarmMgr != NULL )
		{
			bWindowVisible = IsAlarmWindowVisible();
			if ( bWindowVisible && m_pAlarmMgr != NULL )
				m_pAlarmMgr->ShowAlarmWindow(SW_HIDE);
		}

		ShowWindow(SW_HIDE);

		// 进入主控对话框后,主窗口不再接收消息

#if (EXPLAB_VERSION == EXPLAB_460_VERSION)	
		//m_pAcqMaster->UnInstallHandler(m_acqMasterCookie);
		//m_acqMasterCookie = -1;
		CDlgMasterCtrl460 dlg(m_pAcqMaster, this);
#else
		CDlgMasterCtrl dlg(m_pAcqMaster, this);
#endif

		CComPtr<ITDMLogOutput> pTdmLogOutput = ExpLabService::Open()->GetLogOutputService();
		ExpLabService::Close();

		// 把输出区设置成主控对话框
		LPDISPATCH pLogDisp = dlg.GetIDispatch(FALSE);
		CComQIPtr<IDualVXILogOutput> pLogOutput(pLogDisp);

		pTdmLogOutput->AttachVXILogOutput(pLogOutput);

		dlg.DoModal();

		// 窗口关闭后恢复到主窗口
		pLogDisp = GetIDispatch(FALSE);
		pLogOutput = pLogDisp;

		pTdmLogOutput->AttachVXILogOutput(pLogOutput);

#if (EXPLAB_VERSION == EXPLAB_460_VERSION)	
		// 注册事件接收器,主窗口恢复接收消息
		// m_pAcqMaster->InstallHandler(pLogDisp, &m_acqMasterCookie);
#endif

		ShowWindow(SW_SHOW);

		if ( m_pAlarmMgr != NULL && bWindowVisible )
		{
			m_pAlarmMgr->ShowAlarmWindow(SW_SHOW);
		}

#if ( EXPLAB_VERSION & EXPLAB_704_VERSION )
		UpdateChannelView(ID_TOOL_UPDATECHANSLIST);
#endif
	}
}

void CMainFrame::OnToolExitNettest()
{
#if(FLEXLIC)
	if ( CFlexLic::GetFlexLic().CheckOutFeature(eFCEXPLAB_FEAT_DISTRIBUTE_TEST,FLEX_SHOWERROR,LM_CO_LOCALTEST))	
#endif
	{
		if ( m_pAcqMaster != NULL )
		{
			m_pAcqMaster->UnInstallHandler(m_acqMasterCookie);
			m_acqMasterCookie = -1;

			CComPtr<IUrtSlaverDataReceiverMgr> pReceiverMgr;
			m_pAcqMaster->GetSlaverDataReceiverMgr(&pReceiverMgr);
			pReceiverMgr->DetachProject();

			m_pAcqMaster->Destory();
			m_pAcqMaster.Release();

#if (EXPLAB_VERSION & EXPLAB_704_VERSION)
			UpdateChannelView(ID_TOOL_UPDATECHANSLIST);
#endif
			// 必须在这里调用一下,不然退出不保存会崩溃,应该是killtimer造成
			StopUpdateView();

			// 更新主控视图
			UpdateMasterFormView(WM_USER_MASTER_EXITNETTEST, 0, 0);

			URT_OUTPUT_MSG(eMsgCmdCloseLink, MAINFRAME_NAME);
		}

		// 接收端
		if ( m_pAcqReceiver != NULL )
		{
			m_pAcqReceiver->UnInstallHandler(m_acqReceiverCookie);
			m_acqReceiverCookie = -1;

			m_pAcqReceiver->Destroy();
			m_pAcqReceiver.Release();

			URT_OUTPUT_MSG(eMsgCmdCloseLink, MAINFRAME_NAME);

			CComPtr<IUrtApplication> pApp = Urt_GetApplication();
			CComPtr<IDispatch> pDisp;
			while ( SUCCEEDED(pApp->GetActiveProject(&pDisp)) )
			{
				CComPtr<IVXIProject> pPrj;
				if ( FAILED(pDisp->QueryInterface(&pPrj)) )
					break;

				CComQIPtr<IUrtCollection> pCollection(pPrj);

				CComPtr<IUrtIterator> pIter;
				pCollection->NewEnum(&pIter);
				for ( pIter->First(); S_OK != pIter->IsDone(NULL); pIter->Next() )
				{
					CComPtr<IUrtAcquireSystem> pAcqSys;
					if ( FAILED(pIter->GetCurObject(__uuidof(IUrtAcquireSystem), (LPUNKNOWN*)&pAcqSys)) )
						continue;

					EUrtAcquireSystemType sysType;
					pAcqSys->get_systemType(&sysType);

					CComQIPtr<IUrtObject> pAcqSysObj(pAcqSys);
					if ( sysType != eSST_PHYSICS )
						continue;

					CComPtr<IDispatch> pPhysAcqSysDisp;
					pAcqSysObj->QueryInterface(&pPhysAcqSysDisp);
					pCollection->Remove(_variant_t(pPhysAcqSysDisp));

					// 刷新配置视图
					UpdateConfigView(pAcqSysObj, eModifyDeleted);

					CComQIPtr<IUrtObjRegister> pRegister(pPhysAcqSysDisp);
					pRegister->UnRegister(0);

					break;
				}

				break;
			}

			StopUpdateView();

			UpdateClassView(0);
		}

		if ( m_pAcqSlaver != NULL )
		{
			m_pAcqSlaver->UnInstallHandler(m_acqSlaverCookie);
			m_acqSlaverCookie = -1;

			m_pAcqSlaver->Destory();
			m_pAcqSlaver.Release();

			URT_OUTPUT_MSG(eMsgCmdCloseLink, MAINFRAME_NAME);
		}
	}
}
