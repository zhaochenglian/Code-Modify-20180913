#pragma once

extern bool g_chanCfgCoreChan;
void CBaseChanCfgTool::InitBaseTitleIndexForNull()
{
}

void CBaseChanCfgTool::InitBaseTitleIndexFor702()
{
	s_titleMap[strSymbolic].LabelName = sDef_ParamSign;
	if ( m_chanType == eCHT_VIRTUAL )
	{	
		Enable(strSymbolic);
		Enable(btnExpression);
	}
	else
	{
		Enable(_referenceChan);
		Enable(strSymbolic);
		Enable(chkZeroSign);
		Enable(preGain);
		Enable(prefixOffset);
		Enable(aModulus);
		Enable(bModulus);	
	}

	Enable(alarmRangeMax);
	Enable(alarmRangeMin);
	Enable(alarmMax);
	Enable(alarmMin);
	Enable(preAlarmMax);
	Enable(preAlarmMin);
	Enable(globalChanNum);
}

void CBaseChanCfgTool::InitBaseTitleIndexFor7021()
{
	// 7025下将通道名称当贴片号
	s_titleMap[strChanname].LabelName = sDef_ChanName;

	Enable(strChanname);
	Enable(chkZeroSign);
	if ( m_chanType == eCHT_VIRTUAL )
	{	
		Enable(btnExpression);
	}
	else
	{
		Enable(aModulus);
		Enable(bModulus);	
	}

	Enable(alarmRangeMax);
	Enable(alarmRangeMin);
	Enable(alarmMax);
	Enable(alarmMin);
	Enable(preAlarmMax);
	Enable(preAlarmMin);
	Enable(globalChanNum);
}

void CBaseChanCfgTool::InitBaseTitleIndexFor31()
{
	s_titleMap[strSymbolic].LabelName = sDef_ParamSign;
	if ( m_chanType ==eCHT_VIRTUAL )
	{
		Enable(strSymbolic);
		Enable(btnExpression);
	}
	else
	{
		Enable(_referenceChan);
		Enable(strSymbolic);
		Enable(chkZeroSign);
		Enable(preGain);
		Enable(prefixOffset);
		Enable(aModulus);
		Enable(bModulus);	
	}

	Enable(alarmRangeMax);
	Enable(alarmRangeMin);
	Enable(alarmMax);
	Enable(alarmMin);
	Enable(preAlarmMax);
	Enable(preAlarmMin);
	Enable(globalChanNum);
}

void CBaseChanCfgTool::InitBaseTitleIndexForCG()
{
	s_titleMap[strSymbolic].LabelName = sDef_ParamSign;
	if ( m_chanType ==eCHT_VIRTUAL )
	{
		Enable(strSymbolic);
		Enable(strChanname);
		Enable(btnExpression);
	}
	else
	{
		Enable(strSymbolic);
		Enable(strChanname);
		Enable(chkZeroSign);
		Enable(aModulus);
		Enable(bModulus);	
	}
	Enable(globalChanNum);
}

void CBaseChanCfgTool::InitBaseTitleIndexForQD712()
{
	Enable(_chanID);
	Enable(aModulus);
	Enable(bModulus);
	Enable(arrReleaseMode);
	Enable(globalChanNum);
}

void CBaseChanCfgTool::InitBaseTitleIndexForScanTable()
{
	if ( m_chanType == eCHT_VIRTUAL )
	{
		Enable(arrType);
		Enable(arrSymbolic);
		Enable(arrName);
		Enable(_engineeringUnit);
		Enable(btnExpression);
		Enable(arrDefSensor);
		Enable(arrCheckPos);
		Enable(arrSystem);
		Enable(alarmRangeMax); 
		Enable(alarmRangeMin); 
		Enable(alarmMax); 
		Enable(alarmMin); 
		Enable(preAlarmMax); 
		Enable(preAlarmMin); 
	}

	Enable(strPipeNum);
	if ( EXPLAB_VERSION==EXPLAB_H624_VERSION )
	{
		Enable(_chanID);
		if ( m_chanType==eCHT_LXIEX && m_subType==eCHTEXDSA3217
			||
			m_chanType==eCHT_GPIB && m_subType==eCHTTQM603
			||
			eCHT_LXIEX==m_chanType && m_subType==eCHTEXRPT350 )
		{
			Enable(_active);
			Enable(_range);
			Enable(_unit);
		}
		Enable(globalChanNum);
	}
	else
	{
		Enable(globalChanNum);
		Enable(_chanID);
	}
}

void CBaseChanCfgTool::InitBaseTitleIndexFor692()
{
	Enable(aModulus);
	Enable(bModulus);
}

void CBaseChanCfgTool::InitBaseTitleIndexFor460()
{
	Enable(arrType);
	Enable(arrSymbolic);
	Enable(arrName);
	Enable(arrReleaseMode);
}

void CBaseChanCfgTool::InitBaseTitleIndexForH611()
{
	Enable(strSymbolic);
	s_titleMap[strSymbolic].LabelName = STR_LABEL_NUM;
	int nColEU = GetRealCol(_physicsAddress, TRUE);
	MoveTitleToPos(nColEU, IndexAndBelong(strSymbolic,TRUE));

	Enable(chkZeroSign);

	if ( m_chanType != eCHT_VIRTUAL )
	{
		Enable(aModulus);
		Enable(bModulus);
	}

	Enable(alarmRangeMax);
	Enable(alarmRangeMin);
}

void CBaseChanCfgTool::InitBaseTitleIndexForDefault()
{
	bool bHasScanTable = false;
#if FLEXLIC
	//[2017-2-16 guoshuai]在这个模块里直接调用CFlex会在结束时候崩溃，没能找到具体原因，改用最新的方式进行权限校验
	ExpLabService* pService = ExpLabService::Open();
	IFlexExTool* pFlexTool = pService->GetFlexTool();
	bHasScanTable = (pFlexTool->VerifyAuthorization(_T(LICFEAT), _T(LICSCANTABLE)) == S_OK);
	ExpLabService::Close();

#endif

	if ( m_chanType==eCHT_LXIEX && m_subType==eCHTEXDSA3217 )
	{
		Enable(_active);
		Enable(_range);
		Enable(_unit);
	}

	if ( m_chanType == eCHT_VIRTUAL )
	{
		if ( bHasScanTable )
		{
			Enable(arrType);
			Enable(arrSymbolic);
			Enable(arrName);
			Enable(arrSystem);
		}
		else
		{
			Enable(strSymbolic);

			if ( EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_CHINESE )
				Enable(strChanname);
		}

		Enable(btnExpression);

		Enable(chkZeroSign);
		Enable(_engineeringUnit);
		
		Enable(alarmRangeMax);
		Enable(alarmRangeMin);
		Enable(alarmMax);
		Enable(alarmMin);
		Enable(preAlarmMax);
		Enable(preAlarmMin);
	}
	else if ( !bHasScanTable )
	{
		Enable(strSymbolic);

		if ( EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_CHINESE )
			Enable(strChanname);

		Enable(chkZeroSign);
		Enable(aModulus);
		Enable(bModulus);
		Enable(_engineeringUnit);
		Enable(alarmRangeMax);
		Enable(alarmRangeMin);
		Enable(alarmMax);
		Enable(alarmMin);
		Enable(preAlarmMax);
		Enable(preAlarmMin);
	}
	
	if ( EXPLAB_VERSION_LANGUAGE == EXPLAB_VERSION_CHINESE )
	{
		if ( m_chanType != eCHT_VIRTUAL && m_chanType != eCHT_OTHERS )
			Enable(strPipeNum);

		if ( (m_chanType != eCHT_VIRTUAL) && bHasScanTable )
			Enable(_chanID);

		Enable(globalChanNum);
	}
}

void CBaseChanCfgTool::InitReferenceChanBySub()
{
	if ( m_chanType == eCHT_LXIEX )
	{
		if ((m_subType == eCHTEX1048) ||(m_subType == eCHTEX1000TC))
		{
			Enable(_referenceChan);
		}
	}

	if ( m_chanType == eCHT_VXISCP )
	{
		if ((m_subType == eCHTVTSCP1509A))
		{
			Enable(_referenceChan);
		}
	}
}

void CBaseChanCfgTool::InitTitleIndex()
{
	InitBaseTitleIndex();
}

void CBaseChanCfgTool::InitBaseTitleIndex()
{
	if ( g_chanCfgCoreChan )
		Enable(_coreChan);

	if ( m_chanType == eCHT_LXIEX )
	{
		if ((m_subType == eCHTEX1000TCCJC)
			||(m_subType == eCHTEX1000TCDIO)
			||(m_subType == eCHTEX1000TCVTB)
			||(m_subType == eCHTEX1048CJC)
			||(m_subType == eCHTEX1048DIO)
			||(m_subType == eCHTEX1048VTB)
			||(m_subType == eCHTEX1629DIO))
			return;
	}

	// 由所加板卡决定是否加入参考通道 [4/10/2012 Dan]
	InitReferenceChanBySub();

	CString strAppName = _T("ChanTitle")+GetViewName();
	// 初始化需要自动加数值的列
	std::set<LONG> setAutoNum;
	setAutoNum.insert(globalChanNum);
	std::set<LONG> setAutoNumText;
	setAutoNumText.insert(_chanID);
	setAutoNumText.insert(strPipeNum);

	// 为EXPLAB_EXPRESS_VERSION作的特殊处理 [12/7/2010 Dan]
	if ( EXPLAB_VERSION == EXPLAB_EXPRESS_VERSION || EXPLAB_VERSION == EXPLAB_W702_VERSION )
		setAutoNumText.insert(strSymbolic);

	// 有用户要求 [1/29/2016 easy]
	m_setAutoNumText.insert(strChanname);

	ExpLabService* pService = ExpLabService::Open();
	TCHAR* strIniPath = pService->GetIniFilePath();
	ExpLabService::Close();
	for (LONG i=_baseNo; i<=globalChanNum; i++)
	{
		BOOL bAutoNum = GetPrivateProfileInt(strAppName, g_strBaseCellTitle[i]+ _T("_AutoNum"),
			setAutoNum.find(i)!=setAutoNum.end(), strIniPath);

		if ( bAutoNum )
			m_setAutoNum.insert(i);

		BOOL bAutoNumText = GetPrivateProfileInt(strAppName, g_strBaseCellTitle[i]+ _T("_AutoNumText"),
			setAutoNumText.find(i)!=setAutoNumText.end(), strIniPath);

		if ( bAutoNumText )
			m_setAutoNumText.insert(i);
	}

	if ( (EXPLAB_VERSION == EXPLAB_812_VERSION) || (EXPLAB_VERSION == EXPLAB_502_VERSION)
		|| (EXPLAB_VERSION == EXPLAB_H624_VERSION) )
		return InitBaseTitleIndexForScanTable();

	if ( EXPLAB_VERSION == EXPLAB_H702_VERSION )
		return InitBaseTitleIndexFor702();

	if ( EXPLAB_VERSION ==  EXPLAB_H7021_VERSION )
		return InitBaseTitleIndexFor7021();

	if (EXPLAB_VERSION == EXPLAB_707_VERSION)
		return InitBaseTitleIndexForNull();

	if (EXPLAB_VERSION == EXPLAB_H31_VERSION)
		return InitBaseTitleIndexFor31();

	if (EXPLAB_VERSION == EXPLAB_CG_VERSION)
		return InitBaseTitleIndexForCG();

	if ( EXPLAB_VERSION == EXPLAB_692_VERSION )
		return InitBaseTitleIndexFor692();

	if (EXPLAB_VERSION == EXPLAB_QD712_VERSION)
		return InitBaseTitleIndexForQD712();

	if (EXPLAB_VERSION == EXPLAB_460_VERSION)
		return InitBaseTitleIndexFor460();

	if (EXPLAB_VERSION == EXPLAB_H611_VERSION)
		return InitBaseTitleIndexForH611();

	InitBaseTitleIndexForDefault();

	bool hasIniModifyDefault = false;
	for (LONG i=_baseNo; i<=globalChanNum; i++)
	{
		IterTitleIndex iterIndex = find(m_vectTitleIndex.begin(), m_vectTitleIndex.end(), IndexAndBelong(i, TRUE));
		bool bFindIndex = (iterIndex != m_vectTitleIndex.end());
		UINT active = GetPrivateProfileInt(strAppName, g_strBaseCellTitle[i]+ _T("_active"), 0, strIniPath);
		if ( active == 0 )
			continue;

		if ( active == -1 )
		{
			if ( bFindIndex )
				m_vectTitleIndex.erase(iterIndex);

			continue;
		}

		hasIniModifyDefault = true;
		if ( !bFindIndex )
			Enable(i);// 添加显示列

		TitleMapIter iter = s_titleMap.find(i);
		ASSERT(iter!=s_titleMap.end());
		CString& strLabelName = iter->second.LabelName;
		strLabelName = pService->GetPrivateProfileString(strAppName, g_strBaseCellTitle[i], strLabelName);
	}

	// [9/4/2013 Easy] 所有的通道名称改为参数名称,所有通道符号改为参数符号
	//bool strSymbolicModify = !(EXPLAB_VERSION&EXPLAB_TRUNK_VERSION && EXPLAB_VERSION!=EXPLAB_W702_VERSION)
	//						&& (EXPLAB_VERSION & EXPLAB_H611_VERSION);

	//if ( strSymbolicModify && !hasIniModifyDefault )
	//{
	//	// ini文件没修改,特殊处理strSymbolic LabelName
	//	if ( STR_CHAN_SIGN == s_titleMap[strSymbolic].LabelName )
	//	{
	//		s_titleMap[strSymbolic].LabelName = sDef_ParamSign;
	//	}
	//}
}