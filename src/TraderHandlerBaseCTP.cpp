#include "TraderHandlerBaseCTP.h"

CTraderHandlerBaseCTP::CTraderHandlerBaseCTP()
{
	m_pUserApi = NULL;
	m_frontID = 0;
	m_sessionID = 0;
	memset(&m_orderSysID, 0, sizeof(TThostFtdcOrderSysIDType));
	sem_init(&event_OnRspUserLogin, 0, 0);
	sem_init(&event_OnRtnOrder, 0, 0);
	sem_init(&event_UnknownOrder, 0, 0);
	sem_init(&m_event_FrontConnected, 0, 0);
	bGetTimeBeforeInsert=true;
	bOnRspMode=false;
	bOnErrRtnOrderAction=atoi(getConfig("LOG","OnErrRtnOrderAction").c_str());
	bOnRspOrderAction=atoi(getConfig("LOG","OnRspOrderAction").c_str());
	bOnRspOrderInsert=atoi(getConfig("LOG","OnRspOrderInsert").c_str());
	bOnRspError=atoi(getConfig("LOG","OnRspError").c_str());
	bOnRtnOrder=true;
	nRequestID = 0;
	strcpy(m_instrumentid, getConfig("CTP","InstrumentID").c_str());
	strcpy(m_exchangeid, getConfig("CTP","ExchangeID").c_str());
	m_interval=atoi(getConfig("CTP","Interval").c_str());
	memset(&m_times, 0, sizeof(TIMES));
}

void CTraderHandlerBaseCTP::OnFrontConnected()
{
	cout<<"front connected!"<<endl;
	CThostFtdcReqUserLoginField reqUserLoginField={0};
	strcpy(reqUserLoginField.BrokerID, m_brokerID);
	strcpy(reqUserLoginField.UserID, m_userID);
	strcpy(reqUserLoginField.Password, m_password);
	while(m_pUserApi->ReqUserLogin(&reqUserLoginField, nRequestID++)!=0)
		continue;
	SetEvent(m_event_FrontConnected);
}

void CTraderHandlerBaseCTP::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo->ErrorID != 0) 
	{
		cout<<"["<<pRspUserLogin->BrokerID<<"]";
		cout<<"["<<pRspUserLogin->UserID<<"]";
		cout<<"Failed to syslogin:";
		cout<<"["<<pRspInfo->ErrorID<<"]";
		cout<<"["<<pRspInfo->ErrorMsg<<"]"<<endl;
		_exit(0);
	} 
	else
	{
		m_frontID = pRspUserLogin->FrontID;
		m_sessionID = pRspUserLogin->SessionID;
		cout<<"UserLogin >>> UserID=["<<pRspUserLogin->UserID<<"] "
		<<"FrontID=["<<pRspUserLogin->FrontID<<"] "
		<<"SessionID=["<<pRspUserLogin->SessionID<<"] "
		<<endl;	
	}
	CThostFtdcSettlementInfoConfirmField settlementInfoConfirm;
	strcpy(settlementInfoConfirm.BrokerID, m_brokerID);
	strcpy(settlementInfoConfirm.InvestorID, m_userID);
	m_pUserApi->ReqSettlementInfoConfirm(&settlementInfoConfirm, nRequestID++);
	SetEvent(event_OnRspUserLogin);
}

void CTraderHandlerBaseCTP::OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast)
{
	if (!bOnRspError) return;
	if (pRspInfo->ErrorID!=0)
	{
		cout<<"OnRspError="<<pRspInfo->ErrorMsg<<endl;;
	}
}

void CTraderHandlerBaseCTP::OnErrRtnOrderAction (CThostFtdcOrderActionField *pOrderAction,CThostFtdcRspInfoField *pRspInfo)
{
	if (!bOnErrRtnOrderAction) return;
	printf("CTraderHandlerBase::<OnErrRtnOrderAction>: ");
	printf("Error=[%d] ", pRspInfo->ErrorID);
	printf("ErrorMsg=[%s]\n", pRspInfo->ErrorMsg);
}

void CTraderHandlerBaseCTP::OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast)
{
	if (!bOnRspOrderAction) return;
	printf("CTraderHandlerBase::<OnRspOrderAction>: ");
	printf("OrderSysID=[%s] ",pOrderAction->OrderSysID);
	printf("OrderActionStatus=[%s] ",pOrderAction->OrderActionStatus);
	printf("StatusMsg=[%s] ",pOrderAction->StatusMsg);	
	printf("ErrorMsg=[%s]\n",pRspInfo->ErrorMsg);
}

void CTraderHandlerBaseCTP::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast)
{
	DWORD rt=GetTickCount()-m_times.inserttime;
	sysRttList.push_back(rt);
	cout<<"rev,";
	cout<<"[Buy]["<<pInputOrder->LimitPrice<<"￥],"
		<<"[NO."<<setw(4)<<pInputOrder->RequestID<<"],"
		<<"RTime=["<<setw(8)<<rt<<"],"
		<<"ErrorMsg=["<<pRspInfo->ErrorMsg<<"]"
		<<endl;
	syslog<<"rsp,"<<rt<<endl;
	bOnRspMode = true;
	SetEvent(event_UnknownOrder);
}

void CTraderHandlerBaseCTP::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder->FrontID!=m_frontID || pOrder->SessionID!=m_sessionID || !bOnRtnOrder) return;
	if (pOrder->OrderStatus=='5')
	{
		DWORD rt = GetTickCount()-m_times.inserttime;
		exchRttList.push_back(rt);
		cout<<"send-rev,";
		cout<<"[Buy]["<<pOrder->LimitPrice<<"￥],"
			<<"[NO."<<setw(4)<<pOrder->RequestID<<"],"
			<<"RTime=["<<setw(8)<<rt<<"],"
			<<"OrderStatus=[5]["<<pOrder->StatusMsg<<"]"
			<<endl;
		syslog<<"5,"<<rt<<endl;

		DelaylistDetail d;
		d.delay = rt;
		d.exchangeid = pOrder->ExchangeID;
		pOrder->Direction == '0' ? d.direction = "买    " : d.direction = "    卖";
		strcmp(pOrder->CombOffsetFlag,"0")==0 ? d.offsetflag = "开    " :  d.offsetflag = "    平";
		d.orderid = pOrder->OrderRef;
		d.orderstatusmsg = pOrder->StatusMsg;
		d.price = pOrder->LimitPrice;
		d.instrumentid = pOrder->InstrumentID;
		errList.push_back(d);

		SetEvent(event_OnRtnOrder);
	}
	if (pOrder->OrderStatus=='a')
	{
		DWORD rt = GetTickCount()-m_times.inserttime;
		sysRttList.push_back(rt);
		cout<<"rev,";
		cout<<"[Buy]["<<pOrder->LimitPrice<<"￥],"
			<<"[NO."<<setw(4)<<pOrder->RequestID<<"],"
			<<"RTime=["<<setw(8)<<rt<<"],"
			<<"OrderStatus=[a]["<<pOrder->StatusMsg<<"]"
			<<endl;
		syslog<<"a,"<<rt<<endl;

		DelaylistDetail d;
		d.delay = rt;
		d.exchangeid = pOrder->ExchangeID;
		pOrder->Direction == '0' ? d.direction = "买    " : d.direction = "    卖";
		strcmp(pOrder->CombOffsetFlag,"0")==0 ? d.offsetflag = "开    " :  d.offsetflag = "    平";
		d.orderid = pOrder->OrderRef;
		d.orderstatusmsg = pOrder->StatusMsg;
		d.instrumentid = pOrder->InstrumentID;
		d.price = pOrder->LimitPrice;
		aList.push_back(d);

		SetEvent(event_UnknownOrder);
	}
}

void CTraderHandlerBaseCTP::delaytest()
{
	WaitForSingleObject(event_OnRspUserLogin, INFINITE);

	CThostFtdcInputOrderField inputOrder={0};
	strcpy(inputOrder.BrokerID, m_brokerID);
	strcpy(inputOrder.UserID, m_userID);
	strcpy(inputOrder.InvestorID, m_userID);
	strcpy(inputOrder.InstrumentID, m_instrumentid);
	inputOrder.OrderPriceType=THOST_FTDC_OPT_LimitPrice;
	strcpy(inputOrder.CombOffsetFlag, "0");
	strcpy(inputOrder.CombHedgeFlag, "1");
	inputOrder.LimitPrice=-1;		///price
	inputOrder.VolumeTotalOriginal=1;
	inputOrder.TimeCondition= THOST_FTDC_TC_GFD;
	strcpy(inputOrder.GTDDate, "");
	inputOrder.VolumeCondition = THOST_FTDC_VC_AV;
	inputOrder.MinVolume = 0;
	inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
	inputOrder.StopPrice = 0;
	inputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	inputOrder.IsAutoSuspend = 0;
	inputOrder.Direction=THOST_FTDC_D_Buy;		///direction


	CThostFtdcInputOrderActionField orderAction={0};
	strcpy(orderAction.BrokerID, m_brokerID);
	strcpy(orderAction.InvestorID, m_userID);
	strcpy(orderAction.UserID, m_userID);
	strcpy(orderAction.ExchangeID, m_exchangeid);
	strcpy(orderAction.InstrumentID, m_instrumentid);
	orderAction.ActionFlag=THOST_FTDC_AF_Delete;

	m_delaytest_duration = atoi(getConfig("CTP","Duration").c_str());
	
	tStart = tEnd = GetTickCount();

	while(1)
	{
		printf("send-");
		inputOrder.RequestID=nRequestID++;
		if (bGetTimeBeforeInsert)
		{
			m_times.inserttime=GetTickCount();
			m_pUserApi->ReqOrderInsert(&inputOrder, 0);
		}
		else
		{
			m_pUserApi->ReqOrderInsert(&inputOrder, 0);
			m_times.inserttime=GetTickCount();
		}
		WaitForSingleObject(event_UnknownOrder, INFINITE);
		if (!bOnRspMode)
		{
			WaitForSingleObject(event_OnRtnOrder, INFINITE);	
		}
		usleep(m_interval);			
		tEnd=GetTickCount();
		nCount++;
		if (tEnd-tStart>=m_delaytest_duration*1000000) return;
	}
}

void CTraderHandlerBaseCTP::connect(int _uid)
{
	uid = _uid;
	cout<<"connecting to front..."<<endl;

	strcpy(m_brokerID, g_userlist[uid].BrokerID.c_str());
	strcpy(m_userID, g_userlist[uid].UserID.c_str());
	strcpy(m_password, g_userlist[uid].Password.c_str());

	m_pUserApi=CThostFtdcTraderApi::CreateFtdcTraderApi();
	m_pUserApi->RegisterSpi(this);
	m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK); 
	m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);
	m_pUserApi->RegisterFront(const_cast<char *>(getConfig("CTP","FrontAddr").c_str()));
	m_pUserApi->Init();		//初始化后才会去向前置连接，才会触发onfrontconnected()//
	while (WaitForSingleObject(m_event_FrontConnected, 5000) == WAIT_TIMEOUT)
	{
		cout<<"connect time out!"<<endl;
	}
}

void CTraderHandlerBaseCTP::showdelaytestconfirmsg()
{
	cout<<"----------------------------报单信息确认----------------------------\n"
		<<"交易前置地址="<<getConfig("CTP","FrontAddr")<<"\n"
		<<"BrokerID="<<g_userlist[uid].BrokerID<<"\n"
		<<"UserID="<<g_userlist[uid].UserID<<"\n"
		<<"合约="<<getConfig("CTP","InstrumentID")<<"\n"
		<<"交易所="<<getConfig("CTP","ExchangeID")<<"\n"
		<<"方向=买\n"
		<<"开平=开\n"
		<<"价格=-1.0\n"
		<<"手数=1\n"
		<<"报单间隔="<<atoi(getConfig("CTP","Interval").c_str())/1000.0<<"ms\n"
		<<"报单持续时间="<<atoi(getConfig("CTP","Duration").c_str())<<"s\n"
		<<"[按回车开始报单测试]"
		<<endl;

	pressentertocontiue();
}

void CTraderHandlerBaseCTP::presstest(DWORD _interval)
{
	WaitForSingleObject(event_OnRspUserLogin, INFINITE);

	CThostFtdcInputOrderField inputOrder={0};
	strcpy(inputOrder.BrokerID, m_brokerID);
	strcpy(inputOrder.UserID, m_userID);
	strcpy(inputOrder.InvestorID, m_userID);
	strcpy(inputOrder.InstrumentID, m_instrumentid);
	inputOrder.OrderPriceType=THOST_FTDC_OPT_LimitPrice;
	strcpy(inputOrder.CombOffsetFlag, "0");
	strcpy(inputOrder.CombHedgeFlag, "1");
	inputOrder.LimitPrice=atof(getConfig("CTP","PressPrice").c_str());		///price
	inputOrder.VolumeTotalOriginal=1;
	inputOrder.TimeCondition= THOST_FTDC_TC_GFD;
	strcpy(inputOrder.GTDDate, "");
	inputOrder.VolumeCondition = THOST_FTDC_VC_AV;
	inputOrder.MinVolume = 0;
	inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
	inputOrder.StopPrice = 0;
	inputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	inputOrder.IsAutoSuspend = 0;
	inputOrder.Direction=THOST_FTDC_D_Buy;		///direction

	bOnRtnOrder = false;

	cout<<"压力测试中..."<<endl;

	while (1)
	{
		strcpy(inputOrder.CombOffsetFlag, "0");
		inputOrder.Direction=THOST_FTDC_D_Buy;
		inputOrder.RequestID=nRequestID++;
		m_pUserApi->ReqOrderInsert(&inputOrder, 0);
		Sleep(_interval);

		strcpy(inputOrder.CombOffsetFlag, "0");
		inputOrder.Direction=THOST_FTDC_D_Sell;
		inputOrder.RequestID=nRequestID++;
		m_pUserApi->ReqOrderInsert(&inputOrder, 0);
		Sleep(_interval);

		strcpy(inputOrder.CombOffsetFlag, "3");
		inputOrder.Direction=THOST_FTDC_D_Buy;
		inputOrder.RequestID=nRequestID++;
		m_pUserApi->ReqOrderInsert(&inputOrder, 0);
		Sleep(_interval);

		strcpy(inputOrder.CombOffsetFlag, "3");
		inputOrder.Direction=THOST_FTDC_D_Sell;
		inputOrder.RequestID=nRequestID++;
		m_pUserApi->ReqOrderInsert(&inputOrder, 0);
		Sleep(_interval);
	}
}

void CTraderHandlerBaseCTP::showpresstestconfirmsg()
{
	cout<<"----------------------------报单信息确认----------------------------\n"
		<<"交易前置地址="<<getConfig("CTP","FrontAddr")<<"\n"
		<<"合约="<<getConfig("CTP","InstrumentID")<<"\n"
		<<"交易所="<<getConfig("CTP","ExchangeID")<<"\n"
		<<"价格="<<atof(getConfig("CTP","PressPrice").c_str())<<"\n"
		<<"每秒压力笔数="<<atoi(getConfig("CTP","PressAmount").c_str())<<"\n"
		<<"[按回车开始报单测试]"
		<<endl;

	pressentertocontiue();
}

