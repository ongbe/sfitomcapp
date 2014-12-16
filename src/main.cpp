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
vector<USERINFO> g_userlist;
ofstream syslog;
DWORD tStart,tEnd;
int nCount=0;
string startTime;

void OrderInsertStatistics();
void OrderCanceledStatistics();
void OrderUnknownStatistics();
void genReport(int input);
int test_delay();
int test_press();
void getuserlist();
void *pressthread(void *arg);
void balance(int &_userCount, DWORD &_freq, int _pressMount);

int main(int argc, char *argv[])
{
	if (argc==2 && strcmp(argv[1],"-v")==0)
	{ 
		cout<<"order insert tool v1.14.12.16 "<<__DATE__<<" "<<__TIME__<<endl;
		_exit(0);
	}

	disclaimer();

	getuserlist();

	int mainchoose,t;
	syslog.open("syslog.txt");

	cout<<"********************\n"
		<<"       ���˵�       \n"
		<<"********************\n";
	cout<<"1.ѹ������\n"
		<<"2.������ʱ��Ӧ����\n"
		<<"������:";
	cin>>mainchoose;
	switch(mainchoose)
	{
	case 1:
		t=test_press();
		break;
	case 2:
		t=test_delay();
		break;
	default:
		break;
	}

	syslog.close();
	OrderInsertStatistics();
	OrderCanceledStatistics();
	OrderUnknownStatistics();
	genReport(t);
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
	cout<<setprecision(2)<<"------------ �����������ر���ʱͳ�� ------------\n"
		<<nCount<<" packets transmitted, "
		<<exchRttList.size()<<" received, "
		<<(1-(double)exchRttList.size()/nCount)*100<<"% packet loss, "
		<<"time "<<(tEnd-tStart)/1000000<<"s\n"
		<<"rtt ��С/���/ƽ�� = "
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
	cout<<setprecision(2)<<"------------ �����������ر���ʱͳ�� ------------\n"
		<<nCount<<" packets transmitted, "
		<<ordcancelRttList.size()<<" received, "
		<<(1-(double)ordcancelRttList.size()/nCount)*100<<"% packet loss, "
		<<"time "<<(tEnd-tStart)/1000000<<"s\n"
		<<"rtt ��С/���/ƽ�� = "
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
	cout<<setprecision(2)<<"------------ CTPδ֪����ʱͳ�� ------------\n"
		<<nCount<<" packets transmitted, "
		<<sysRttList.size()<<" received, "
		<<(1-(double)sysRttList.size()/nCount)*100<<"% packet loss, "
		<<"time "<<(tEnd-tStart)/1000000<<"s\n"
		<<"rtt ��С/���/ƽ�� = "
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
		systype = "����";
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
		systype = "�����";
		exchange = getConfig("SGIT","ExchangeID");
		instrumentid = getConfig("SGIT","InstrumentID");
		break;
	default:
		break;
	}

	string file = "report_"+filetime()+".txt";
	ofstream report(file.c_str());
	report.setf(ios::fixed);
	//����
	report<<"                                            ������ʱ���Ա���                                              \n"
		<<"                                                                       �Ʊ�ʱ�䣺"<<logtime()<<"\n"
		<<"----------------------------------------------------------------------------------------------------------\n\n";
	//������Ϣ
	report<<"ϵͳ���ͣ�"<<systype<<"\t���Խ�������"<<exchange<<"\t���Ժ�Լ��"<<instrumentid<<"\n"
		<<"����ʱ�䣺"<<startTime<<"\n\n";
	//����������
	report<<"                                            ��������������                                           \n"
		<<"----------------------------------------------------------------------------------------------------------\n"
		<<"������"<<getEthernetInterfaceType()
		<<"CPU��"<<getCPUInfo()<<"\n"
		<<"�ڴ棺"<<setprecision(2)<<getMemorySize()/1024/1024/1024.00<<"G\n"
		<<"����ϵͳ��"<<getOSversion()<<"\n\n";
	
	int da=0;
	if (aList.size()>2)
	{
	//δ֪����Ӧ��ʱ��ϸ
	report<<"                                           δ֪����Ӧ��ʱ��ϸ                                        \n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n"
		<<"|  ���  | ������ | ��Լ | ���� |  �۸�  | ��ƽ |  ��ʱ  |   ��������   |               ����״̬         |\n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n";
	
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
	report<<setprecision(3)<<"|��"<<setw(4)<<aList.size()<<"��|"
		<<" ƽ����"<<(double)da/sysRttList.size()/1000<<"ms"
		<<" ��С��"<<(double)sysRttList[0]/1000<<"ms"
		<<" ���"<<(double)sysRttList.back()/1000<<"ms"
		<<setw(51)<<"|"<<endl;
	report<<"+--------+-----------------------------------------------------------------------------------------------+\n\n";
	}
	int de=0;
	if (errList.size()>2)
	{
	//��������Ӧ��ʱ��ϸ
	report<<"                                           ��������Ӧ��ʱ��ϸ                                        \n"
		<<"+--------+--------+------+------+--------+------+--------+--------------+--------------------------------+\n"
		<<"|  ���  | ������ | ��Լ | ���� |  �۸�  | ��ƽ |  ��ʱ  |   ��������   |               ����״̬         |\n"
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
	report<<setprecision(3)<<"|��"<<setw(4)<<errList.size()<<"��|"
		<<" ƽ����"<<(double)de/exchRttList.size()/1000<<"ms"
		<<" ��С��"<<(double)exchRttList[0]/1000<<"ms"
		<<" ���"<<(double)exchRttList.back()/1000<<"ms"
		<<setw(51)<<"|"<<endl;
	report<<"+--------+-----------------------------------------------------------------------------------------------+\n\n";
	}

	//���Ի���
	report<<"                                                ���Ի���                                             \n"
		<<"----------------------------------------------------------------------------------------------------------\n";
	if (da!=0)
	report<<"δ֪���ر�ƽ����ʱ��"<<(double)da/sysRttList.size()/1000<<"ms\n";
	if (de!=0)
	report<<"�������ر�ƽ����ʱ��"<<(double)de/exchRttList.size()/1000<<"ms\n";
	report<<"----------------------------------------------------------------------------------------------------------\n";
	report<<"                                                                                                     END\n";

	report.close();
	_exit(0);
}

int test_delay()
{
	int input;
	cout<<"********************\n"
		<<"   ������ʱ��Ӧ����  \n"
		<<"********************\n";
	cout<<"1.ctp��������\n"
		<<"2.feima��������\n"
		<<"3.cffex��������\n"
		<<"4.shfe��������\n"
		<<"5.����󱨵�����\n"
		<<"������:";

	cin>>input;
	string systype;

	startTime = logtime();

	if (input == 1)
	{
		CTraderHandlerBaseCTP *ctp = new CTraderHandlerBaseCTP;
		ctp->showdelaytestconfirmsg();
		ctp->connect();
		ctp->delaytest();
	}
	else if (input == 2)
	{
		CTraderHandlerBaseFeima *fm = new CTraderHandlerBaseFeima;
		fm->showdelaytestconfirmsg();
		fm->connect();
		fm->delaytest();
	}
	else if (input == 3)
	{
		CTraderHandlerBaseCFFEX *ffex = new CTraderHandlerBaseCFFEX;
		ffex->showdelaytestconfirmsg();
		ffex->connect();
		ffex->delaytest();
	}
	else if (input == 4)
	{
		CTraderHandlerBaseSHFE *shfe = new CTraderHandlerBaseSHFE;
		shfe->showdelaytestconfirmsg();
		shfe->connect();
		shfe->delaytest();
	}
	else if (input == 5)
	{
		CTraderHandlerBaseSGIT *sg = new CTraderHandlerBaseSGIT;
		sg->showdelaytestconfirmsg();
		sg->connect();
		sg->delaytest();
	}
	return input;
}

int test_press()
{
	cout<<"********************\n"
		<<"      ѹ������       \n"
		<<"********************\n";
	cout<<"1.ctpѹ������\n"
		<<"������:";
	int input;
	cin>>input;
	string systype;

	startTime = logtime();

	if (input == 1)
	{	
		int c;			//Ͷ��������
		DWORD itvl;		//����Ƶ��
		balance(c, itvl, atoi(getConfig("CTP","PressMount").c_str()));
		for (int i=0; i<c ; i++)
		{
			THREADPARAM *t = new THREADPARAM;
			t->uid = i;
			t->interval = itvl;
			t->type = CTPTRADE;
			pthread_t p;
			pthread_create(&p, 0, pressthread, t);
		}
		Sleep(INFINITE);
	}
	else
	{
		_exit(0);
	}
	return input;
}

void getuserlist()
{
	string path=getConfig("CTP","Userlist");
	ifstream file;
	file.open(path.c_str());
	if (!file)
	{
		cout<<"Can not open file "<<path<<endl;
		_exit(-1);
	}
	string line;
	while(getline(file, line, '\n'))
	{
		if (line.find("BrokerID")!=string::npos || line.find("#")!=string::npos || line.length()<2)
			continue;
		USERINFO u;
		u.BrokerID = getSubstr(line, 1, ",");
		u.UserID = getSubstr(line, 2, ",");
		u.Password = getSubstr(line, 3, ",");
		u.InvestorID = getSubstr(line, 4, ",");
		u.ParticipantID = getSubstr(line, 5, ",");
		u.ClientID = getSubstr(line, 6, ",");
		g_userlist.push_back(u);
	}
	if (g_userlist.size()<1)
	{
		cout<<path<<" û��Ͷ���߻��߸�ʽ����"<<endl;
		_exit(0);
	}
}

void *pressthread(void *arg)
{
	THREADPARAM t = *(THREADPARAM *)arg;
	if (t.type == CTPTRADE)
	{
		CTraderHandlerBaseCTP *ctp = new CTraderHandlerBaseCTP;
		ctp->connect(t.uid);
		ctp->presstest(t.interval);
	}
	else
	{
		_exit(0);
	}
}

//���䱨��Ƶ�ʺ�Ͷ��������
void balance(int &_userCount, DWORD &_freq, int _pressMount)
{
	_userCount = 1;
	while (true)
	{
		if (1000%(_pressMount/_userCount)!=0)				//ÿ��Ͷ����ѹ���ٱ�,��������Ҫ����Ͷ����
		{
			_userCount++;
		}
		else
		{
			if (_userCount>g_userlist.size())
			{
				cout<<"Ͷ����������������Ҫ"<<_userCount<<"��"<<endl;
				_exit(0);
			}
			
			_freq = 1000/(_pressMount/_userCount);
			return;
		}
	}
}