#pragma once
#include "ThostFtdcTraderApi.h"
#include "toolbox.h"
#include <vector>
#include <iomanip>
#include "define.h"

class CTraderHandlerBaseCTP : public CThostFtdcTraderSpi
{
public:
	CTraderHandlerBaseCTP();
	~CTraderHandlerBaseCTP(void){};

private:
	CThostFtdcTraderApi *m_pUserApi;
	TThostFtdcFrontIDType m_frontID;
	TThostFtdcSessionIDType	m_sessionID;
	TThostFtdcOrderSysIDType m_orderSysID;
	TThostFtdcBrokerIDType	m_brokerID;
	TThostFtdcUserIDType m_userID;
	TThostFtdcPasswordType	m_password;
	string m_address;
	HANDLE event_OnRspUserLogin;
	HANDLE event_OnRtnOrder;
	HANDLE event_UnknownOrder;
	HANDLE m_event_FrontConnected;
	bool bGetTimeBeforeInsert;
	bool bOnRspMode;
	bool bOnErrRtnOrderAction;
	bool bOnRspOrderAction;
	bool bOnRspOrderInsert;
	bool bOnRspError;
	TThostFtdcRequestIDType nRequestID;
	TThostFtdcInstrumentIDType m_instrumentid;
	TThostFtdcExchangeIDType m_exchangeid;
	TIMES m_times;
	int m_interval;
	DWORD m_duration;

private:
	virtual void OnFrontConnected();
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);
	virtual void OnErrRtnOrderAction (CThostFtdcOrderActionField *pOrderAction,CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRspOrderAction(CThostFtdcOrderActionField *pOrderAction,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

public:
	void orderinsert();
	void connect();
};
