/*
2014.12.01
1.����cffexֱ������
2.����shfeֱ������
*/
#pragma once
#include <fstream>
#include <vector>
using namespace std;

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

extern ofstream syslog;
extern vector <DWORD> exchRttList,ordcancelRttList,sysRttList;
extern vector <DelaylistDetail> aList,errList;
extern DWORD tStart,tEnd;
extern int nCount;