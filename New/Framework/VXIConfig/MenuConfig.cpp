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
	
	// 屏蔽多语言切换
	m_arrayIgnoreToolID.Add(ID_LANG_CH_ZH);
	m_arrayIgnoreToolID.Add(ID_LANG_EN_US);
	m_arrayIgnoreToolID.Add(ID_CLOSE_PROJECT);  // 先关闭这两个菜单,稳定后再开放

	//英文版暂时拿掉Help, 去掉匹配电阻测量
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
		m_bToolbarEnable[toolbarCollection] = true;//不显示该ToolBar 同时在菜单中删除该项
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
	
	// 支持TDM3000试验数据管理系统
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
		m_arrayIgnoreToolID.Add(ID_TOOL_PRE_RECORD);				//70211关闭开始记录按钮
		m_arrayIgnoreToolID.Add(ID_TOOL_STOPRECORD);				//关闭开始停止按钮
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_0);					//关闭手动触发记录
		m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD_STRUCT);		
		//m_arrayIgnoreToolID.Add(ID_TOOL_UPLOAD);					//提交数据
		m_arrayIgnoreToolID.Add(ID_ADD_DEVICES);
		m_arrayIgnoreToolID.Add(ID_MASTER_DOWNLOAD_TESTCONFIG);		//下载试验配置表
		m_arrayIgnoreToolID.Add(ID_TOOL_DICT_MGR);					//数据字典管理
		m_arrayIgnoreToolID.Add(ID_UPLOAD_CHANSCONFIG);				//上传通道配置
		m_arrayIgnoreToolID.Add(ID_CHECK_UPDATE);					//自动更新
	}

	//打印
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICOFFLINEPRINT)))
		m_arrayIgnoreToolID.Add(ID_TOOL_PRINT);
	
	//采集并记录,无相关授权,仅配置显示
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(INICONIFIG),_T(CONFRUNANDRECORD)))
		m_arrayIgnoreToolID.Add(ID_RUN_AND_RECORD);

	// 使用624的主控就不需要弹出进入对话框了
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICMASTER))/* || USE_MASTER_FORM_VIEW*/)
		m_arrayIgnoreToolID.Add(ID_TOOL_ENTER_MASTER);		

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICALARMWND)))
		m_arrayIgnoreToolID.Add(ID_VIEW_ALARM);

	// 快照功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICSNAPSHOT)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_SNAPSHOT);
		m_arrayIgnoreToolID.Add(ID_TOOL_SNAPSHOT_ONCE);
	}

	// 独立的菜单项				
	m_arrayIgnoreToolID.Add(ID_TOOL_RING);							//打铃，目前只用于南京晨光，控制目录下
	m_arrayIgnoreToolID.Add(ID_TOOL_WAVEEDITER);					//波形编辑

	m_arrayIgnoreToolID.Add(ID_TOOL_REPORT_TEMPLATE);				//报告生成，仅在南京晨光
	m_arrayIgnoreToolID.Add(ID_IMPORT_SCXI_1520);					//导入SCXI设备，仅在460使用
	if ( EXPLAB_H7021_VERSION != EXPLAB_VERSION )
		m_arrayIgnoreToolID.Add(ID_TOOL_GLOBALNUM_RESET);				//全局通道号重置，只在702

	// 自定义算法功能	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICFUNC)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_FUNCTION_DEFINE);
		m_arrayIgnoreToolID.Add(ID_TOOL_FUNCTION_CREATE);
	}

	// 事件功能	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICEVENT)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_EVENT);
	}

	// 分组记录功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICRECORDGROUP)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORDGROUPS_MANAGEMENT);
		m_arrayIgnoreToolID.Add(ID_CTRL_GROUPRECORD);
	}

	// 支持搜索设备功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICTCPIP))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICVXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICPXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICGPIB),_T(LICGPIB_VXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICGPIB))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICASRL))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICAMC)))
		m_arrayIgnoreToolID.Add(ID_SEARCH_DEVICES);


	// 支持添加设备功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICPXI))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICPXI),_T(LICPCI1588))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICASRL),_T(LICRS34401A))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICUSB))
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICASRL),_T(LICSERIALPORT))	//
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICASRL),_T(LICPM6306))		//
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICPXI),_T(LICSD1000))		
		&& !CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTCPIP),_T(LICDSA3217)))
		m_arrayIgnoreToolID.Add(ID_ADD_DEVICES);

	// DSA217绝压校准
	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTCPIP),_T(LICDSA3217)) )
		m_arrayIgnoreToolID.Add(ID_DSA3217_ABSCALC);

	// 支持传感器库功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICTRANS)))
		m_arrayIgnoreToolID.Add(ID_TOOL_TRANSDUCERLIB);

	// 支持实时判读功能
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

	// 网络测控
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICDISTRIBUTE)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_ENTER_NETTEST);	
		m_arrayIgnoreToolID.Add(ID_TOOL_EXIT_NETTEST);
	}

	// 零位采集功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICZEROAQUIRE)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_ZEROACQ_START);				//导入零点
		m_arrayIgnoreToolID.Add(ID_TOOL_ZERORESTORE);				//导入零点
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTZEROR);				//导入零点
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTZERO);				//导出零点
	}

	// 是否支持扫描表功能
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICSCANTABLE)))
	{
		m_arrayIgnoreToolID.Add(ID_DOWNLOAD_SCANCFG);		//下载扫描表配置
		m_arrayIgnoreToolID.Add(ID_MASTER_IMPORT_SCANCFG);			//导入扫描表配置
		m_arrayIgnoreToolID.Add(ID_MASTER_EXPORT_SCANCFG);			//导出扫描表配置
		m_arrayIgnoreToolID.Add(ID_TOOL_CHANNEL_VALIDATE);			//导出扫描表配置
	}
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICOPTIONLOCK)))
		m_arrayIgnoreToolID.Add(ID_OPTION_LOCK);

		
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICPROTOCOL)))	//由子系统下控制
		m_arrayIgnoreToolID.Add(ID_PROTOCAL_CONFIGE);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICASRL)) &&
		!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICCOM))  &&
		!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSUBS),_T(LICRS232)))		//由子系统下控制
		m_arrayIgnoreToolID.Add(ID_PROTOCAL_707_CONFIG);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICFEAT),_T(LICCHCFG_EXIMPORT)))
	{
		// 通道配置导入导出
		m_arrayIgnoreToolID.Add(ID_MASTER_EXPORT_CHANCFG);
		m_arrayIgnoreToolID.Add(ID_MASTER_IMPORT_CHANCFG);
	}

	// 是否支持1629设备
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTCPIP),_T(LICEX1629)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTLEADR);				//导入导线电阻
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTLEADR);				//导出导线电阻
		m_arrayIgnoreToolID.Add(ID_MEAS_SHUNT);						//旁路校准
		m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
		m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
		m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
		m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);				//匹配电阻
		m_arrayIgnoreToolID.Add(ID_MEAS_EXCIT);						//激励测量
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE);					//导线测量
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE_BY_CHAN);			//逐个通道导线测量
	}
	else
	{
		if( EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_ENGLISH )
		{
			m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);				//匹配电阻
			m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
			m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
			//m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
		}
	}			

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_0)))		//时间历程曲线
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_0);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_1)))		//数值曲线
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_1);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_2)))		//百分比曲线
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_2);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_4)))		//XY曲线
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_4);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_5)))		//FFT曲线
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_5);
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_6)))		//边界曲线
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_6);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICDRAWNUM)))
		m_arrayIgnoreToolID.Add(ID_CTRL_NUMBER);						//数值控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICNUMDISPLAY)))
		m_arrayIgnoreToolID.Add(ID_CTRL_DISPLAY);						//瞬时值控件

	// 表盘控件
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_M);							//表盘控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER_0)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_0);							//表盘

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER_1)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_1);							//仪表

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICMETER_2)))
		m_arrayIgnoreToolID.Add(ID_CTRL_METER_2);							//量表
	
	// 柱状图控件
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_M);	//柱状图控件
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM_0)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_0);							//圆头柱状控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM_1)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_1);							//液罐型控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHISTOGRAM_2)))
		m_arrayIgnoreToolID.Add(ID_CTRL_HISTOGRAM_2);							//温度计型控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICCURVE_3)))
		m_arrayIgnoreToolID.Add(ID_CTRL_CURVE_3);							//多联柱状图控件

	//滑块控件
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICSLIDER)))
		m_arrayIgnoreToolID.Add(ID_CTRL_SLIDER_0);							
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICLABLE)))
		m_arrayIgnoreToolID.Add(ID_CTRL_LABLE);							//标签控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICHCAVPREVIEW)))
		m_arrayIgnoreToolID.Add(ID_CTRL_VIDEO);							//视频控件

	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICRADARDISPLAY))) //[Mike_zx 2013-06-07 10:07:50注释] 添加Licenses信息
		m_arrayIgnoreToolID.Add(ID_CTRL_RADARDISPLAY);

	if (!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICREMOTESENSING))) //[Mike_zx 2013-09-25 15:01:53注释] 添加三维遥感Licenses信息
		m_arrayIgnoreToolID.Add(ID_CTRL_REMOTESENSING);

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICDRAWNUM)))
		m_arrayIgnoreToolID.Add(ID_CTRL_STOPWATCH);						//计时控件
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICDISGRAPH)))
		m_arrayIgnoreToolID.Add(ID_CTRL_DISTRIBUTE);						//分布图控件
	
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICREPORTTABLE)))
		m_arrayIgnoreToolID.Add(ID_CTRL_REPORT);						//报表控件

	// 报警灯控件
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMSIGN)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMSIGN_M);						//报警灯控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMSIGN_0)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMSIGN_0);						//圆形报警控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMSIGN_1)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMSIGN_1);						//方形报警控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICPLOTCHART)))
		m_arrayIgnoreToolID.Add(ID_CTRL_PLOTCHART);						//特性图控件

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICWIDGET),_T(LICALARMGRID)))
		m_arrayIgnoreToolID.Add(ID_CTRL_ALARMGRID);						//报警控件

	//设置
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_MASTERMODE)))
		m_arrayIgnoreToolID.Add(ID_TOOL_MASTERMODE);						//主从设置

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_CHANNELCONF)))
		m_arrayIgnoreToolID.Add(ID_TOOL_CHANNEL_CONFIG);					//通道配置

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_AQUIRE)))
		m_arrayIgnoreToolID.Add(ID_TOOL_ACQUIRESET);						//采集设置

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_TRIGGER)))
		m_arrayIgnoreToolID.Add(ID_TOOL_TRIGGERSET);						//触发设置

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_RECORD)))
	{
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORDSET);							//存盘设置
		// m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_0);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_1);
		m_arrayIgnoreToolID.Add(ID_TOOL_RECORD_2);
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_DUMMY)))
		m_arrayIgnoreToolID.Add(ID_DUMMY_SURGEJUDGE_SETTING);				//判喘设置

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_TIMESRC)))
		m_arrayIgnoreToolID.Add(ID_TOOL_TIME_SRC);							//时间源设置

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICTOOL_SET),_T(LICTOOL_SET_DATAFILE)))
		m_arrayIgnoreToolID.Add(ID_SET_DATAFILE);							//工程设置
	
	//自检 
	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_ZEROCALIBRATE)))
		m_arrayIgnoreToolID.Add(ID_ZERO_CALIBRATE);						//零位校准

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_SELFCAL)))
		m_arrayIgnoreToolID.Add(ID_SELF_CAL);							//自校准

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_SELFTEST)))
		m_arrayIgnoreToolID.Add(ID_SELF_TEST);							//设备自检

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_RESET)))
		m_arrayIgnoreToolID.Add(ID_RESET_DEVICE);						//设备复位

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEASSHUNT)))
	{
		m_arrayIgnoreToolID.Add(ID_MEAS_SHUNT);							//旁路校准
		m_arrayIgnoreToolID.Add(ID_SHUNT_ON);
		m_arrayIgnoreToolID.Add(ID_SHUNT_OFF);
		m_arrayIgnoreToolID.Add(ID_CHECK_CHAN);
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_CALCOMPLETION_R)))
		m_arrayIgnoreToolID.Add(ID_CAL_COMPLETION_R);					//校准匹配电阻

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEASEXCI)))
		m_arrayIgnoreToolID.Add(ID_MEAS_EXCIT);							//激励测量

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEASLEADWIRE)))
	{
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE);						//导线测量
		m_arrayIgnoreToolID.Add(ID_TOOL_EXPORTINITVOLT);				//1629初始电压导出
		m_arrayIgnoreToolID.Add(ID_TOOL_IMPORTINITVOLT);				//1629初始电压导入
	}

	if(!CFlexLic::GetFlexLic().VerifyAuthorization(_T(LICSELF_DEVICECTL),_T(LICSELF_DEVICECTL_MEAS_LEADWIRE_BYCHAN)))
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE_BY_CHAN);				//逐个通道导线测量
#endif

#if (EXPLAB_VERSION & EXPLAB_606_VERSION )
	{
		m_arrayIgnoreToolID.Add(ID_MEAS_LEADWIRE_BY_CHAN);				//逐个通道导线测量
		m_arrayIgnoreToolID.Add(ID_MASTER_DOWNLOAD_TESTCONFIG);			//逐个通道导线测量
	}
#endif
}

void CMainFrame::InitIgnoreToolID()
{
	InitIgnoreToolIDByIni();
}