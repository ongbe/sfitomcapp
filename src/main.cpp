#include "TraderHandlerBaseCTP.h"
#include "TraderHandlerBaseFeima.h"
#include "TraderHandlerBaseSGIT.h"
#include "TraderHandlerBaseCFFEX.h"
#include "TraderHandlerBaseShfe.h"
#include <cstdlib>
#include <math.h>
#include <algorithm>
using namespace std;
vector <DWORD> exchRttList,ordcancelRttList,sysRttList;
vector <DelaylistDetail> aList,errList;
ofstream syslog;
DWORD tStart,tEnd;
int nCount=0;
string startTime;

void OrderInsertStatistics();
void OrderCanceledStatistics();
void OrderUnknownStatistics();
void genReport(int input);

int main(int argc, char *argv[])
{
	system("clear");
	cout<<  "      +############################ 免责声明 ########################+\n"
		"      | 本软件只做内部测试使用，一切因使用本软件而引致之任何意外、   |\n"
		"      | 疏忽、合约毁坏、诽谤、版权或知识产权侵犯及其所造成的损失，本 |\n"
		"      | 软件概不负责，亦不承担任何法律责任。                         |\n"
		"      +--------------------------------------------------------------+\n"
		"如果要继续使用，则表示您已阅读并同意上述声明的所有条款之约定；否则立即停止使用。\n"
		"同意并继续使用(输入jixu)，不同意(n):";
	string t;
	cin>>t;
	if (t!="jixu")
	{
		_exit(0);
	}




	if (argc==2 && strcmp(argv[1],"-v")==0)
	{ 
		cout<<"order insert tool v1.14.11.24 "<<__DATE__<<" "<<__TIME__<<endl;
		_exit(0);
	}

	syslog.open("syslog.txt");

	cout<<"1.ctp报单测试\n"
		<<"2.feima报单测试\n"
		<<"3.cffex报单测试\n"
		<<"4.shfe报单测试\n"
		<<"5.金飞鼠报单测试\n"
		<<"请输入:";
		
	int input;
	cin>>input;
	string systype;

	startTime = logtime();

	if (input == 1)
	{
		CTraderHandlerBaseCTP *ctp = new CTraderHandlerBaseCTP;
		ctp->connect();
		ctp->orderinsert();
	}
	else if (input == 2)
	{
		CTraderHandlerBaseFeima *fm = new CTraderHandlerBaseFeima;
		fm->connect();
		fm->orderinsert();
	}
	else if (input == 3)
	{
		CTraderHandlerBaseCFFEX *ffex = new CTraderHandlerBaseCFFEX;
		ffex->connect();
		ffex->orderinsert();
	}
	else if (input == 4)
	{
		CTraderHandlerBaseSHFE *shfe = new CTraderHandlerBaseSHFE;
		shfe->connect();
		shfe->orderinsert();
	}
	else if (input == 5)
	{
		CTraderHandlerBaseSGIT *sg = new CTraderHandlerBaseSGIT;
		sg->connect();
		sg->orderinsert();
	}
	

	syslog.close();
	OrderInsertStatistics();
	OrderCanceledStatistics();
	OrderUnknownStatistics();
	genReport(input);
	cout<<"EXE EXITED!"<<endl;
	return 1;
}

void OrderInsertStatistics()
{
	if (exchRttList.size()<2) return;
	DWORD totTime=0;
	vector<int>::size_type ix=0;
	for (; ix != exchRttList.size(); ++ix)
	{
		totTime+=exchRttList[ix];
	}
	sort(exchRttList.begin(), exchRttList.end());
	cout.setf(ios::fixed);
	cout<<setprecision(2)<<"------------ 交易所报单回报延时统计 ------------\n"
		<<nCount<<" packets transmitted, "
		<<exchRttList.size()<<" received, "
		<<(1-(double)exchRttList.size()/nCount)*100<<"% packet loss, "
		<<"time "<<(tEnd-tStart)/1000000<<"s\n"
		<<"rtt 最小/最大/平均 = "
		<<(double)exchRttList[0]/1000<<"/"
		<<(double)exchRttList[ix-1]/1000<<"/"
		<<(double)totTime/exchRttList.size()/1000<<"ms"
		<<endl;
}

void OrderCanceledStatistics()
{
	if (ordcancelRttList.size()<2) return;
	DWORD totTime=0;
	vector<int>::size_type ix=0;
	for (; ix != ordcancelRttList.size(); ++ix)
	{
		totTime+=ordcancelRttList[ix];
	}
	sort(ordcancelRttList.begin(), ordcancelRttList.end());
	cout.setf(ios::fixed);
	cout<<setprecision(2)<<"------------ 交易所撤单回报延时统计 ------------\n"
		<<nCount<<" packets transmitted, "
		<<ordcancelRttList.size()<<" received, "
		<<(1-(double)ordcancelRttList.size()/nCount)*100<<"% packet loss, "
		<<"time "<<(tEnd-tStart)/1000000<<"s\n"
		<<"rtt 最小/最大/平均 = "
		<<(double)ordcancelRttList[0]/1000<<"/"
		<<(double)ordcancelRttList[ix-1]/1000<<"/"
		<<(double)totTime/ordcancelRttList.size()/1000<<"ms"
		<<endl;
}

void OrderUnknownStatistics()
{
	if (sysRttList.size()<2 ) return;
	DWORD totTime=0;
	vector<int>::size_type ix=0;
	for (; ix != sysRttList.size(); ++ix)
	{
		totTime+=sysRttList[ix];
	}
	sort(sysRttList.begin(), sysRttList.end());
	cout.setf(ios::fixed);
	cout<<setprecision(2)<<"------------ CTP未知单延时统计 ------------\n"
		<<nCount<<" packets transmitted, "
		<<sysRttList.size()<<" received, "
		<<(1-(double)sysRttList.size()/nCount)*100<<"% packet loss, "
		<<"time "<<(tEnd-tStart)/1000000<<"s\n"
		<<"rtt 最小/最大/平均 = "
		<<(double)sysRttList[0]/1000<<"/"
		<<(double)sysRttList[ix-1]/1000<<"/"
		<<(double)totTime/sysRttList.size()/1000<<"ms"
		<<endl;
}


void genReport(int input)
{
	string systype;
	string exchange;
	string instrumentid;

	switch(input)
	{
	case 1:
		systype = "CTP";
		exchange = getConfig("CTP","ExchangeID");
		instrumentid = getConfig("CTP","InstrumentID");
		break;
	case 2:
		systype = "飞马";
		exchange = getConfig("FEIMA","ExchangeID");
		instrumentid = getConfig("FEIMA","InstrumentID");
		break;
	case 3:
		systype = "CFFEX";
		exchange = getConfig("CFFEX","ExchangeID");
		instrumentid = getConfig("CFFEX","InstrumentID");
		break;
	case 4:
		systype = "SHFE";
		exchange = getConfig("SHFE","ExchangeID");
		instrumentid = getConfig("SHFE","InstrumentID");
		break;
	case 5:
		systype = "金飞鼠";
		exchange = getConfig("SGIT","ExchangeID");
		instrumentid = getConfig("SGIT","InstrumentID");
		break;
	default:
		break;
	}

	string file = "report_"+filetime()+".txt";
	ofstream report(file.c_str());
	//标题
	report<<"                                            报单延时测试报告                                              \n"
		<<"                                                                       制表时间："<<logtime()<<"\n"
		<<"----------------------------------------------------------------------------------------------------------\n\n";
	//基本信息
	report<<"系统类型："<<systype<<"\t测试交易所："<<exchange<<"\t测试合约："<<instrumentid<<"\n"
		<<"测试时间："<<startTime<<"\n\n";
	//服务器配置
	report<<"                                            报单服务器配置                                           \n"
		<<"----------------------------------------------------------------------------------------------------------\n"
		<<"网卡："<<getEthernetInterfaceType()
		<<"CPU："<<getCPUInfo()<<"\n"
		<<"内存："<<setprecision(2)<<getMemorySize()/1024/1024/1024.00<<"G\n"
		<<"操作系统："<<getOSversion()<<"\n\n";
	
	int da=0;
	if (aList.size()>2)
	{
	//未知单响应延时明细
	report<<"                                           未知单响应延时明细                                        \n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n"
		<<"|  序号  | 交易所 | 合约 | 方向 |  价格  | 开平 |  延时  |   报单引用   |               报单状态         |\n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n";
	
	report.setf(ios::fixed);
	for (int i=0; i<aList.size(); i++)
	{
		report<<setprecision(2)
			<<"|"<<setw(7)<<i
			<<" |"<<setw(7)<<aList[i].exchangeid
			<<" |"<<setw(5)<<aList[i].instrumentid
			<<"|"<<aList[i].direction
			<<"|"<<setw(7)<<aList[i].price
			<<" |"<<aList[i].offsetflag
			<<"|"<<setw(7)<<aList[i].delay
			<<" |"<<setw(13)<<aList[i].orderid
			<<" | "<<setw(30)<<aList[i].orderstatusmsg
			<<" |"<<endl;
		da += aList[i].delay;
	}
	report<<"+--------+-----------------------------------------------------------------------------------------------+\n";
	report<<setprecision(3)<<"|共"<<setw(4)<<aList.size()<<"条|"
		<<" 平均："<<(double)da/sysRttList.size()/1000<<"ms"
		<<" 最小："<<(double)sysRttList[0]/1000<<"ms"
		<<" 最大："<<(double)sysRttList.back()/1000<<"ms"
		<<setw(51)<<"|"<<endl;
	report<<"+--------+-----------------------------------------------------------------------------------------------+\n\n";
	}
	int de=0;
	if (errList.size()>2)
	{
	//交易所响应延时明细
	report<<"                                           交易所响应延时明细                                        \n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n"
		<<"|  序号  | 交易所 | 合约 | 方向 |  价格  | 开平 |  延时  |   报单引用   |               报单状态         |\n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n";

	for (int i=0; i<errList.size(); i++)
	{
		report<<setprecision(2)
			<<"|"<<setw(7)<<i
			<<" |"<<setw(7)<<errList[i].exchangeid
			<<" |"<<setw(5)<<errList[i].instrumentid
			<<"|"<<errList[i].direction
			<<"|"<<setw(7)<<errList[i].price
			<<" |"<<errList[i].offsetflag
			<<"|"<<setw(7)<<errList[i].delay
			<<" |"<<setw(13)<<errList[i].orderid
		    	<<" | "<<setw(30)<<errList[i].orderstatusmsg
			<<" |"<<endl;
		de += errList[i].delay;
	}
	report<<"+--------+-----------------------------------------------------------------------------------------------+\n";
	report<<setprecision(3)<<"|共"<<setw(4)<<errList.size()<<"条|"
		<<" 平均："<<(double)de/exchRttList.size()/1000<<"ms"
		<<" 最小："<<(double)exchRttList[0]/1000<<"ms"
		<<" 最大："<<(double)exchRttList.back()/1000<<"ms"
		<<setw(51)<<"|"<<endl;
	report<<"+--------+-----------------------------------------------------------------------------------------------+\n\n";
	}

	//测试汇总
	report<<"                                                测试汇总                                             \n"
		<<"----------------------------------------------------------------------------------------------------------\n";
	if (da!=0)
	report<<"未知单回报平均延时："<<(double)da/sysRttList.size()/1000<<"ms\n";
	if (de!=0)
	report<<"交易所回报平均延时："<<(double)de/exchRttList.size()/1000<<"ms\n";
	report<<"----------------------------------------------------------------------------------------------------------\n";
	report<<"                                                                                                     END\n";

	report.close();
	_exit(0);
}
