#include "stdafx.h"

#include "VXIConfig.h"
#include "MainFrm.h"
#include "GlobalRes.h"
extern BOOL g_addLoadLevChan;

void CMainFrame::InitIgnoreToolIDByIni()
{
#if(FLEXLIC)
	EUrtSystemType sysType = GetSystemType();
	m_arrayIgnoreToolID.RemoveAll();
	
	// ���ζ������л�
	m_arrayIgnoreToolID.Add(ID_LANG_CH_ZH);
	m_arrayIgnoreToolID.Add(ID_LANG_EN_US);
	m_arrayIgnoreToolID.Add(ID_CLOSE_PROJECT);  // �ȹر��������˵�,�ȶ����ٿ���

	//Ӣ�İ���ʱ�õ�Help, ȥ��ƥ��������
	if( EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_ENGLISH )
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_FUNCTION_DEFINE);
		// m_arrayIgnoreToolID.Add(ID_HELP);
		m_arrayIgnoreToolID.Add(ID_MASTER_IMPORT_CHANCFG);
		m_arrayIgnoreToolID.Add(ID_MASTER_EXPORT_CHANCFG);
	}

	if ( EXPLAB_H7021_VERSION==EXPLAB_VERSION)
		m_arrayIgnoreToolID.Add(ID_FILE_MRU_FILE1);


	if ( EXPLAB_1629_VERSION==EXPLAB_VERSION )
	{
		m_arrayIgnoreToolID.Add(ID_LICENSE);
		m_arrayIgnoreToolID.Add(ID_HELP_REVISONINFORMATION);
	}

	if ( sysType&eSysTMasterMain || sysType&eSysTMasterNormal )
	{
		m_bToolbarEnable[toolbarCollection] = true;//����ʾ��ToolBar ͬʱ�ڲ˵���ɾ������
		m_bToolbarEnable[toolbarControl] = true;
		m_bToolbarEnable[toolbarWebTransmit] = true;
		m_bToolbarEnable[toolbarSnapshotLoad] = true;

		m_arrayIgnoreToolID.Add(ID_FILE_NEW);
		m_arrayIgnoreToolID.Add(ID_FILE_OPEN);
		m_arrayIgnoreToolID.Add(ID_FILE_NEW_GRAPHIC);
		m_arrayIgnoreToolID.Add(ID_FILE_SAVE_ALL);
		m_arrayIgnoreToolID.Add(ID_TOOL_ACQUIREZERO);
		m_arrayIgnoreToolID.Add(ID_TOOL_EUCONVERSION);
		m_arrayIgnoreToolID.Add(ID_FILE_MRU_FILE1);
		m_arrayIgnoreToolID.Add(ID_TOOL_TRANSDUCERLIB);
		m_arrayIgnoreToolID.Add(ID_ADD_DEVICES);
		m_arrayIgnoreToolID.Add(ID_SEARCH_DEVICES);
		m_arrayIgnoreToolID.Add(ID_MASTER_CREATE_DATATABLE);
		m_arrayIgnoreToolID.Add(ID_TOOL_EVENT);

		m_arrayIgnoreToolID.Add(ID_TOOL_ZERORESTORE);
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTZEROR);
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTZERO);
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTLEADR);
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTLEADR);

		/*m_arrayIgnoreToolID.Add(ID_DOWN_CHANSCONFIG);
		m_arrayIgnoreToolID.Add(ID_MASTER_IMPORT_CHANCFG);*/

		m_arrayIgnoreToolID.Add(ID_ZERO_CALIBRATE);
		m_arrayIgnoreToolID.Add(ID_SELF_CAL);
		m_arrayIgnoreToolID.Add(ID_SELF_TEST);
		m_arrayIgnoreToolID.Add(ID_RESET_DEVICE);
		m_arrayIgnoreToolID.Add(ID_MEAS_SHUNT);
		m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
		m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
		m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
		m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);
		m_arrayIgnoreToolID.Add(ID_MEAS_EXCIT);
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE);

		m_arrayIgnoreToolID.Add(ID_TOOL_RUN);
		m_arrayIgnoreToolID.Add(ID_TOOL_ZEROACQ_START);
		m_arrayIgnoreToolID.Add(ID_TOOL_STOP);
		m_arrayIgnoreToolID.Add(ID_TOOL_PRE_RECORD);
		m_arrayIgnoreToolID.Add(ID_TOOL_STOPRECORD);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_0);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_1);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_2);
		
		m_arrayIgnoreToolID.Add(ID_TOOL_START_CYCLE_SAVE);
		m_arrayIgnoreToolID.Add(ID_TOOL_STOP_CYCLE_SAVE);
		m_arrayIgnoreToolID.Add(ID_TOOL_REVIEW);

		m_arrayIgnoreToolID.Add(ID_LATCHING_ALARM);
		m_arrayIgnoreToolID.Add(ID_LATCHING_ALARM_RESTORE);
		
	}
	else if ( sysType==eSysTRecvNormal )
	{
		m_bToolbarEnable[toolbarCollection] = false;
		m_bToolbarEnable[toolbarWebTransmit]= false;
	}
	else if (sysType & eSysTSlaver)
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_ENTER_MASTER);
	}

	m_bToolbarEnable[toolbarSnapshotLoad] = g_addLoadLevChan;
	if ( !g_addLoadLevChan )
	{
		m_arrayIgnoreToolID.Add(ID_VIEW_SNAPSHOTLOAD_TOOLBAR);
	}
	
	// ֧��TDM3000�������ݹ���ϵͳ
	if( !USE_TDM3000_MODULE )
	{
		m_arrayIgnoreToolID.Add(ID_UPLOAD_CHANSCONFIG);
		m_arrayIgnoreToolID.Add(ID_DOWN_CHANSCONFIG);
		m_arrayIgnoreToolID.Add(ID_MASTER_DOWNLOAD_TESTCONFIG);
		m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD);
		m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD_STRUCT);
		m_arrayIgnoreToolID.Add(ID_TOOL_DICT_MGR);
		m_arrayIgnoreToolID.Add(ID_CHECK_UPDATE);
	}

	if (EXPLAB_VERSION & EXPLAB_606_VERSION) 
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_DICT_MGR);
		m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD_STRUCT);
	}

	if (EXPLAB_VERSION == EXPLAB_70211_VERSION) 
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_PRE_RECORD);				//70211�رտ�ʼ��¼��ť
		m_arrayIgnoreToolID.Add(ID_TOOL_STOPRECORD);				//�رտ�ʼֹͣ��ť
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_0);					//�ر��ֶ�������¼
		m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD_STRUCT);		
		//m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD);					//�ύ����
		m_arrayIgnoreToolID.Add(ID_ADD_DEVICES);
		m_arrayIgnoreToolID.Add(ID_MASTER_DOWNLOAD_TESTCONFIG);		//�����������ñ�
		m_arrayIgnoreToolID.Add(ID_TOOL_DICT_MGR);					//�����ֵ����
		m_arrayIgnoreToolID.Add(ID_UPLOAD_CHANSCONFIG);				//�ϴ�ͨ������
		m_arrayIgnoreToolID.Add(ID_CHECK_UPDATE);					//�Զ�����
	}

	//��ӡ
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICOFFLINEPRINT)))
		m_arrayIgnoreToolID.Add(ID_TOOL_PRINT);
	
	//�ɼ�����¼,�������Ȩ,��������ʾ
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(INICONIFIG),_T(CONFRUNANDRECORD)))
		m_arrayIgnoreToolID.Add(ID_RUN_AND_RECORD);

	// ʹ��624�����ؾͲ���Ҫ��������Ի�����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICMASTER))/* || USE_MASTER_FORM_VIEW*/)
		m_arrayIgnoreToolID.Add(ID_TOOL_ENTER_MASTER);		

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICALARMWND)))
		m_arrayIgnoreToolID.Add(ID_VIEW_ALARM);

	// ���չ���
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICSNAPSHOT)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_SNAPSHOT);
		m_arrayIgnoreToolID.Add(ID_TOOL_SNAPSHOT_ONCE);
	}

	// �����Ĳ˵���				
	m_arrayIgnoreToolID.Add(ID_TOOL_RING);							//���壬Ŀǰֻ�����Ͼ����⣬����Ŀ¼��
	m_arrayIgnoreToolID.Add(ID_TOOL_WAVEEDITER);					//���α༭

	m_arrayIgnoreToolID.Add(ID_TOOL_REPORT_TEMPLATE);				//�������ɣ������Ͼ�����
	m_arrayIgnoreToolID.Add(ID_IMPORT_SCXI_1520);					//����SCXI�豸������460ʹ��
	if ( EXPLAB_H7021_VERSION != EXPLAB_VERSION )
		m_arrayIgnoreToolID.Add(ID_TOOL_GLOBALNUM_RESET);				//ȫ��ͨ�������ã�ֻ��702

	// �Զ����㷨����	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICFUNC)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_FUNCTION_DEFINE);
		m_arrayIgnoreToolID.Add(ID_TOOL_FUNCTION_CREATE);
	}

	// �¼�����	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICEVENT)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_EVENT);
	}

	// �����¼����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICRECORDGROUP)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORDGROUPS_MANAGEMENT);
		m_arrayIgnoreToolID.Add(ID_CTRL_GROUPRECORD);
	}

	// ֧�������豸����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICTCPIP))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICVXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICPXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICGPIB),_T(LICGPIB_VXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICGPIB))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICASRL))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICAMC)))
		m_arrayIgnoreToolID.Add(ID_SEARCH_DEVICES);


	// ֧������豸����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICPXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICPXI),_T(LICPCI1588))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICASRL),_T(LICRS34401A))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICUSB))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICASRL),_T(LICSERIALPORT))	//
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICASRL),_T(LICPM6306))		//
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICPXI),_T(LICSD1000))		
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTCPIP),_T(LICDSA3217)))
		m_arrayIgnoreToolID.Add(ID_ADD_DEVICES);

	// DSA217��ѹУ׼
	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTCPIP),_T(LICDSA3217)) )
		m_arrayIgnoreToolID.Add(ID_DSA3217_ABSCALC);

	// ֧�ִ������⹦��
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICTRANS)))
		m_arrayIgnoreToolID.Add(ID_TOOL_TRANSDUCERLIB);

	// ֧��ʵʱ�ж�����
	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET), _T(LICJUDGEDISPLAY)) )
		m_arrayIgnoreToolID.Add(ID_CTRL_JUDGEDISPLAY);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICJUDGE)))
	{
		m_arrayIgnoreToolID.Add(ID_IMPORT_JUDGE_INPUT);
		m_arrayIgnoreToolID.Add(ID_DOWNLOAD_JUDGE_INPUT);
		m_arrayIgnoreToolID.Add(ID_EXPORT_JUDGE_RESULT);
		m_arrayIgnoreToolID.Add(ID_UPLOAD_JUDGE_RESULT);
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICSURGEJUDGE)))
		m_arrayIgnoreToolID.Add(ID_DUMMY_SURGEJUDGE_SETTING);

	if( (EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_ENGLISH) || !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICLOOPSAVE)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_START_CYCLE_SAVE);
		m_arrayIgnoreToolID.Add(ID_TOOL_STOP_CYCLE_SAVE);
		m_arrayIgnoreToolID.Add(ID_TOOL_REVIEW);
	}

	// ������
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICDISTRIBUTE)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_ENTER_NETTEST);	
		m_arrayIgnoreToolID.Add(ID_TOOL_EXIT_NETTEST);
	}

	// ��λ�ɼ�����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICZEROAQUIRE)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_ZEROACQ_START);				//�������
		m_arrayIgnoreToolID.Add(ID_TOOL_ZERORESTORE);				//�������
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTZEROR);				//�������
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTZERO);				//�������
	}

	// �Ƿ�֧��ɨ�����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICSCANTABLE)))
	{
		m_arrayIgnoreToolID.Add(ID_DOWNLOAD_SCANCFG);		//����ɨ�������
		m_arrayIgnoreToolID.Add(ID_MASTER_IMPORT_SCANCFG);			//����ɨ�������
		m_arrayIgnoreToolID.Add(ID_MASTER_EXPORT_SCANCFG);			//����ɨ�������
		m_arrayIgnoreToolID.Add(ID_TOOL_CHANNEL_VALIDATE);			//����ɨ�������
	}
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICOPTIONLOCK)))
		m_arrayIgnoreToolID.Add(ID_OPTION_LOCK);

		
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICPROTOCOL)))	//����ϵͳ�¿���
		m_arrayIgnoreToolID.Add(ID_PROTOCAL_CONFIGE);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICASRL)) &&
		!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICCOM))  &&
		!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICRS232)))		//����ϵͳ�¿���
		m_arrayIgnoreToolID.Add(ID_PROTOCAL_707_CONFIG);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICCHCFG_EXIMPORT)))
	{
		// ͨ�����õ��뵼��
		m_arrayIgnoreToolID.Add(ID_MASTER_EXPORT_CHANCFG);
		m_arrayIgnoreToolID.Add(ID_MASTER_IMPORT_CHANCFG);
	}

	// �Ƿ�֧��1629�豸
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTCPIP),_T(LICEX1629)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTLEADR);				//���뵼�ߵ���
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTLEADR);				//�������ߵ���
		m_arrayIgnoreToolID.Add(ID_MEAS_SHUNT);						//��·У׼
		m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
		m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
		m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
		m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);				//ƥ�����
		m_arrayIgnoreToolID.Add(ID_MEAS_EXCIT);						//��������
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE);					//���߲���
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE_BY_CHAN);			//���ͨ�����߲���
	}
	else
	{
		if( EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_ENGLISH )
		{
			m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);				//ƥ�����
			m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
			m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
			//m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
		}
	}			

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_0)))		//ʱ����������
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_0);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_1)))		//��ֵ����
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_1);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_2)))		//�ٷֱ�����
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_2);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_4)))		//XY����
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_4);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_5)))		//FFT����
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_5);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_6)))		//�߽�����
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_6);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICDRAWNUM)))
		m_arrayIgnoreToolID.Add(ID_CTRL_NUMBER);						//��ֵ�ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICNUMDISPLAY)))
		m_arrayIgnoreToolID.Add(ID_CTRL_DISPLAY);						//˲ʱֵ�ؼ�

	// ���̿ؼ�
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_M);							//���̿ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER_0)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_0);							//����

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER_1)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_1);							//�Ǳ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER_2)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_2);							//����
	
	// ��״ͼ�ؼ�
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_M);	//��״ͼ�ؼ�
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM_0)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_0);							//Բͷ��״�ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM_1)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_1);							//Һ���Ϳؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM_2)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_2);							//�¶ȼ��Ϳؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_3)))
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_3);							//������״ͼ�ؼ�

	//����ؼ�
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICSLIDER)))
		m_arrayIgnoreToolID.Add(ID_CTRL_SLIDER_0);							
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICLABLE)))
		m_arrayIgnoreToolID.Add(ID_CTRL_LABLE);							//��ǩ�ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHCAVPREVIEW)))
		m_arrayIgnoreToolID.Add(ID_CTRL_VIDEO);							//��Ƶ�ؼ�

	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICRADARDISPLAY))) //[Mike_zx 2013-06-07 10:07:50ע��] ���Licenses��Ϣ
		m_arrayIgnoreToolID.Add(ID_CTRL_RADARDISPLAY);

	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICREMOTESENSING))) //[Mike_zx 2013-09-25 15:01:53ע��] �����άң��Licenses��Ϣ
		m_arrayIgnoreToolID.Add(ID_CTRL_REMOTESENSING);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICDRAWNUM)))
		m_arrayIgnoreToolID.Add(ID_CTRL_STOPWATCH);						//��ʱ�ؼ�
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICDISGRAPH)))
		m_arrayIgnoreToolID.Add(ID_CTRL_DISTRIBUTE);						//�ֲ�ͼ�ؼ�
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICREPORTTABLE)))
		m_arrayIgnoreToolID.Add(ID_CTRL_REPORT);						//����ؼ�

	// �����ƿؼ�
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMSIGN)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMSIGN_M);						//�����ƿؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMSIGN_0)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMSIGN_0);						//Բ�α����ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMSIGN_1)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMSIGN_1);						//���α����ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICPLOTCHART)))
		m_arrayIgnoreToolID.Add(ID_CTRL_PLOTCHART);						//����ͼ�ؼ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMGRID)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMGRID);						//�����ؼ�

	//����
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_MASTERMODE)))
		m_arrayIgnoreToolID.Add(ID_TOOL_MASTERMODE);						//��������

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_CHANNELCONF)))
		m_arrayIgnoreToolID.Add(ID_TOOL_CHANNEL_CONFIG);					//ͨ������

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_AQUIRE)))
		m_arrayIgnoreToolID.Add(ID_TOOL_ACQUIRESET);						//�ɼ�����

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_TRIGGER)))
		m_arrayIgnoreToolID.Add(ID_TOOL_TRIGGERSET);						//��������

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_RECORD)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORDSET);							//��������
		// m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_0);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_1);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_2);
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_DUMMY)))
		m_arrayIgnoreToolID.Add(ID_DUMMY_SURGEJUDGE_SETTING);				//�д�����

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_TIMESRC)))
		m_arrayIgnoreToolID.Add(ID_TOOL_TIME_SRC);							//ʱ��Դ����

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_DATAFILE)))
		m_arrayIgnoreToolID.Add(ID_SET_DATAFILE);							//��������
	
	//�Լ� 
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_ZEROCALIBRATE)))
		m_arrayIgnoreToolID.Add(ID_ZERO_CALIBRATE);						//��λУ׼

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_SELFCAL)))
		m_arrayIgnoreToolID.Add(ID_SELF_CAL);							//��У׼

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_SELFTEST)))
		m_arrayIgnoreToolID.Add(ID_SELF_TEST);							//�豸�Լ�

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_RESET)))
		m_arrayIgnoreToolID.Add(ID_RESET_DEVICE);						//�豸��λ

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEASSHUNT)))
	{
		m_arrayIgnoreToolID.Add(ID_MEAS_SHUNT);							//��·У׼
		m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
		m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
		m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_CALCOMPLETION_R)))
		m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);					//У׼ƥ�����

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEASEXCI)))
		m_arrayIgnoreToolID.Add(ID_MEAS_EXCIT);							//��������

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEASLEADWIRE)))
	{
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE);						//���߲���
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTINITVOLT);				//1629��ʼ��ѹ����
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTINITVOLT);				//1629��ʼ��ѹ����
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEAS_LEADWIRE_BYCHAN)))
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE_BY_CHAN);				//���ͨ�����߲���
#endif

#if (EXPLAB_VERSION & EXPLAB_606_VERSION )
	{
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE_BY_CHAN);				//���ͨ�����߲���
		m_arrayIgnoreToolID.Add(ID_MASTER_DOWNLOAD_TESTCONFIG);			//���ͨ�����߲���
	}
#endif
}

void CMainFrame::InitIgnoreToolID()
{
	InitIgnoreToolIDByIni();
}