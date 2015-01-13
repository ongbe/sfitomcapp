#pragma once
#include <fstream>
#include <vector>
using namespace std;

enum E_TRADETYPE
{
	CTPTRADE,
	CFFEXTRADE,
	SHFETRADE,
	FEIMATRADE,
	SGITTRADE,
};

struct TIMES
{
	DWORD inserttime;
	DWORD systemtime;
	DWORD exchangetime;
};

struct DelaylistDetail
{
	string id;
	string exchangeid;
	string instrumentid;
	string direction;
	double price;
	string offsetflag;
	int delay;
	string orderid;
	string orderstatusmsg;
};

struct USERINFO
{
	string BrokerID;
	string UserID;
	string InvestorID;
	string ParticipantID;
	string ClientID;
	string Password;
};

struct THREADPARAM
{
	int uid;
	DWORD interval;
	E_TRADETYPE type;
};

extern ofstream syslog;
extern vector <DWORD> exchRttList,ordcancelRttList,sysRttList;
extern vector <DelaylistDetail> aList,errList;
extern vector<USERINFO> g_userlist;
extern DWORD tStart,tEnd;
extern int nCount;