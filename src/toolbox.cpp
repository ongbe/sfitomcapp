///linux  版 20130507
#include "toolbox.h"
#include "define.h"
#include <sys/sysinfo.h>
using namespace std;

char  *lpCharacter=new char[CON_WIN_WIDTH];
char *lpOldCharacter=new char[CON_WIN_WIDTH];

/*函数名称：getSubstr()
函数功能：以空格为分隔符分割字段
版本号：20120201
参数1：const string originstr 	要分割的字符串
参数2：int subn	要提取第几段
参数3：string delims		指定分隔符，默认是空格
返回值：截取出来的字段*/
string getSubstr(const string originstr,int subn,string delims)
{
	try
	{
		string str=originstr;
		string::size_type pbegin=0;	//指向字符串要截取的开始位置
		string::size_type pend=0;		//指向字符串要截取的结束位置
		for (int i=1; i<=subn; i++)
		{
			pbegin=str.find_first_not_of(delims,pend);	//以上一次结束位置为起点寻找第一个不是分隔符的位置，作为新的起点
			if (str.find(delims, pend+1)==string::npos) //如果没找到分隔符则认为是最后一段了。
			{
				if (i<subn) return "";		//如果已经搜索到最后一段了，但指定要分割的段数超出了界限，则返回空值。
				pend=str.length()+1; break;	//把结束位置设置为字符串最后位置
			}
			pend=str.find(delims, pbegin);
		}
		str=str.substr(pbegin, pend-pbegin);		//截取字符串
		return str;
	}
	catch (const std::exception& error)
	{
		//LOG("getSubstr of %s error: %s", originstr.c_str(), error.what());
		cout<<"getSubstr of"<<originstr.c_str()<<" error:"<<error.what()<<endl;
		exit(-1);
	}
}
/*函数名称：getConfig()
函数功能：获取配置文件ini中相应大标题title下指定配置字段cfgname的值
参数1：string title		大标题[***]
参数2：string cfgName		大标题下的配置字段
返回值：配置文件ini中相应大标题title下指定配置字段cfgname的值
*/
string getConfig(string title,string cfgName)
{
	const char* INIFile="config.ini";
	ifstream inifile(INIFile);
	if (!inifile.is_open())
	{
		cerr<<"Could not open "<<INIFile<<endl;
		inifile.clear();
		exit(-1);
	}	
	string strtmp,strtitle,strcfgname,returnValue;
	int flag=0;
	while(getline(inifile, strtmp,'\n'))
	{
		if (strtmp.substr(0,1)=="#")	continue;	//过滤注释		
		if (flag==0)
		{
			if (strtmp.find(title)!=string::npos)
			{
				if (strtmp.substr(0,1)=="[")
				{
					if (strtmp.find("]")==string::npos) 	break;	//缺失“]”退出
					strtitle=strtmp.substr(1);
					strtitle=strtitle.erase(strtitle.find("]"));
					if (strtitle==title)		//找到大标题设置标志位为1，这样就不再找下一个大标题了
					{
						flag=1;
						continue;
					}
				}
			}
		} 
		if (flag==1)
		{
			if (strtmp.substr(0,1)=="[")	break;	//如果遇到下一个[]号说明当前大标题对应的配置字段搜索完毕，结束搜索。
			if (strtmp.find(cfgName)!=string::npos)	
			{
				if (strtmp.find("=")==string::npos)	break;	//缺失“=”退出
				strcfgname=strtmp;
				strcfgname=strcfgname.erase(strcfgname.find("="));
				if (strcfgname==cfgName)		//找到大标题相应的字段后，返回值
				{
					returnValue=strtmp.substr(strtmp.find("=")+1);
					return returnValue;
				}
				else continue;
			}	
		}
	}
	//LOG("配置文件错误：没找到%s对应配置项%s!", title.c_str(),cfgName.c_str());
	cout<<"配置文件错误：没找到"<<title.c_str()<<"对应配置项"<<cfgName.c_str()<<endl;
	return NONE;
}

/*函数名称：genDate()
版本：20130217
函数功能：生成日期，放入结构体CURRENTDATE中
参数：
%a 星期几的简写 
%A 星期几的全称 
%b 月份的简写 
%B 月份的全称 
%c 标准的日期的时间串 
%C 年份的后两位数字 
%d 十进制表示的每月的第几天 
%D 月/天/年 
%e 在两字符域中，十进制表示的每月的第几天 
%F 年-月-日 
%g 年份的后两位数字，使用基于周的年 
%G 年份，使用基于周的年 
%h 简写的月份名 
%H 24小时制的小时 
%I 12小时制的小时
%j 十进制表示的每年的第几天 
%m 十进制表示的月份 
%M 十时制表示的分钟数 
%n 新行符 
%p 本地的AM或PM的等价显示 
%r 12小时的时间 
%R 显示小时和分钟：hh:mm 
%S 十进制的秒数 
%t 水平制表符 
%T 显示时分秒：hh:mm:ss 
%u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
%U 第年的第几周，把星期日作为第一天（值从0到53）
%V 每年的第几周，使用基于周的年 
%w 十进制表示的星期几（值从0到6，星期天为0）
%W 每年的第几周，把星期一做为第一天（值从0到53） 
%x 标准的日期串 
%X 标准的时间串 
%y 不带世纪的十进制年份（值从0到99）
%Y 带世纪部分的十制年份 
%z，%Z 时区名称，如果不能得到时区名称则返回空字符。
%% 百分号
返回值：返回结构体，放入CURRENTDATE中。
*/
CURRENTDATE getDate()
{
	char tp[21];
	//输出文件命名时用到的时间
	string strst,stryear,strmonth,strday,strhour,strmin,strsec;
	time_t t=time(NULL);
	struct tm* local=localtime(&t);
	struct CURRENTDATE date;
	//年
	strftime(tp, 20, "%Y", local);
	date.year=tp;
	//月
	strftime(tp, 20, "%m", local);
	date.month=tp;
	//日
	strftime(tp, 20, "%d", local);
	date.day=tp;
	//小时
	strftime(tp, 20, "%H", local);
	date.hour=tp;
	//分
	strftime(tp, 20, "%M", local);
	date.minute=tp;
	//秒
	strftime(tp, 20, "%S", local);
	date.second=tp;
	strftime(tp, 20, "%Y%m%d", local);
	date.curdate=tp;
	return date;
}


/*函数名称：ifExistFile()
函数功能：判断文件夹是否存在
参数：string file	要判断的文件
返回值：无
*/
bool ifExistFile(string file)
{
	ifstream fin(file.c_str());	
	if (!fin.is_open())
	{
		cout<<"无法打开"<<file;
		//::AfxMessageBox(msg.c_str());
		return FALSE;
	}
	fin.close();
	return TRUE;
}

DWORD GetTickCount()
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0)
        return 0;
	return tv.tv_sec*1000000+tv.tv_usec;
	//return 0;
}

void  GetFormatTickCount( FORMATTICKCOUNT &fc)
{
	char temp[22];
	time_t now;
	time(&now);  
	strftime(temp, 21, "%H:%M:%S:", localtime(&now));
	string ltime(temp);
	struct timeval tv;
	if(gettimeofday(&tv, NULL) != 0) return;
	string lusec;
	stringstream ss;
	ss<<setw(9)<<tv.tv_usec;
	ss>>lusec;
	fc.usecLevelTime=ltime+lusec;
	fc.usecLevelCount=tv.tv_sec*1000000+tv.tv_usec;
}

/*函数名称：inputPWD()
函数功能：输入密码，显示星号，能处理退格键
参数：无
返回值：返回输入的密码
备注：需要在link中加入-lcurses
*/
/*
int inputPWD(string password)
{
	string pwd;
	initscr();
	cbreak();
	nl();
	noecho();
	intrflush(stdscr,FALSE);
	keypad(stdscr,TRUE);
	int c;
	int n = 0;
	printw("Password:\n");
	do
	{
		c = getch();
		if (c==263)
		{
			echochar('\b');
			echochar(' ');
			echochar('\b');
			if (!pwd.empty()) pwd=pwd.erase(pwd.size()-1,1);
		}
		else if (c != '\n')
		{
			echochar('*');
			pwd+=c;
		}
	}while(c != '\n');
	endwin();
	if (pwd!=password) 
	{
		cout<<"Password Error!"<<endl;
		_exit(-1);
	}
}
*/

/*函数名称：timewait()
函数功能：
参数：
返回值：无
*/
void timewait( DWORD ms)	///interval is ms
{
	if (ms == INFINITE)
	{
		select(0, NULL, NULL, NULL, NULL);
		return;
	}
	struct timeval tval;
	tval.tv_sec = 0;
	tval.tv_usec = ms*1000-90;
	select(0, NULL, NULL, NULL, &tval);
}

/*函数名称：logtime()
函数功能：格式化内容到log.txt中
参数：none
返回值：无
*/
const string logtime()
{
	char temp[22];
	time_t now;
	time(&now);  
	strftime(temp, 21, "%Y-%m-%d %H:%M:%S", localtime(&now));
	string strtemp(temp);
	return strtemp;
}
const string filetime()
{
        char temp[22];
        time_t now;
        time(&now);
        strftime(temp, 21, "%H%M%S", localtime(&now));
        string strtemp(temp);
        return strtemp;
}
/*函数名称：WaitForSingleObject()
函数功能：linux下伪装成windows的WaitForSingleObject()程序
参数：懒得说了
返回值：errno错误代码，查看内核代码中的/usr/include/asm/errno.h
*/
int WaitForSingleObject(HANDLE & _sem, DWORD _milisecond)
{
	int rt = 0;

	if (_milisecond == INFINITE)
	{
		while ( rt=sem_wait(&_sem) == -1 && errno == EINTR )
		{
			//LOG_FATAL("errno=%d",errno);
			continue;
		}
		return WAIT_OBJECT_0;
	}

	struct timespec ts;
	struct timeval tt;
	gettimeofday(&tt,NULL);

	ts.tv_sec = tt.tv_sec;
	ts.tv_nsec = tt.tv_usec*1000 + _milisecond * 1000 * 1000;//这里可能造成纳秒>1000 000 000
	ts.tv_sec += ts.tv_nsec/(1000 * 1000 *1000);
	ts.tv_nsec %= (1000 * 1000 *1000);

	while (rt=sem_timedwait(&_sem,&ts) == -1 && errno == EINTR )
	{
		//LOG("errno=%d",errno);
		continue;
	}
	if (errno == 110)
	{
		return WAIT_TIMEOUT;
	}
	
	return WAIT_OBJECT_0;
}

/*函数名称：WaitForMultipleObjects()
函数功能：linux下伪装成windows的WaitForMultipleObjects()程序
参数：懒得说了,先搞两个信号量。
返回值：
*/
struct thread_param
{
	int tid;
	sem_t * sem;
	sem_t * semmain;
	bool rt;
};

void* multiObject_thread_fun(void* param)
{
	thread_param *th_parm=(thread_param*)param;
	while ( sem_wait(th_parm->sem) == -1 && errno == EINTR ) continue;
	th_parm->rt=true;
	sem_post(th_parm->semmain);
}


/*函数名称：SetEvent()
函数功能：linux下伪装成windows的SetEvent()程序
参数：懒得说了
返回值：同上
*/
int SetEvent(HANDLE & _sem)
{
	return sem_post(&_sem);
}

/*函数名称：ResetEvent()
函数功能：linux下伪装成windows的ResetEvent()程序
参数：懒得说了
返回值：同上
*/
void ResetEvent(HANDLE & _sem)
{
	int rt = 0;

	do 
	{
		sem_trywait(&_sem); 
		sem_getvalue(&_sem, &rt);
	} while ( rt > 0 );
}

/*函数名称：Sleep()
函数功能：linux下伪装成windows的Sleep()程序
参数：懒得说了
返回值：同上
*/
void Sleep(unsigned int milisecond)
{
	if (milisecond == INFINITE)
	{
		sem_t sem;
		memset(&sem, 0, sizeof(sem_t));
		if (0!=sem_init(&sem, 0, 0)); //LOG("sem_init sleep() failed!");
		sem_wait(&sem);
		return;
	}
	
	usleep(milisecond*1000);
}

/*函数名称：init_daemon()
函数功能：linux下创建守护进程
参数：懒得说了
返回值：同上
*/
void init_daemon(void)
{
	int pid;
	int i;

	if(pid=fork())
		_exit(0);//是父进程，结束父进程
	else if(pid< 0)
		exit(1);//fork失败，退出
	//是第一子进程，后台继续执行

	setsid();//第一子进程成为新的会话组长和进程组长
	//并与控制终端分离
	if(pid=fork())
		_exit(0);//是第一子进程，结束第一子进程
	else if(pid< 0)
		exit(1);//fork失败，退出
	//是第二子进程，继续
	//第二子进程不再是会话组长

	for(i=0;i< NOFILE;++i)//关闭打开的文件描述符
		close(i);
	//chdir("/tmp");//改变工作目录到/tmp
	umask(0);//重设文件创建掩模
	return;
}


//获得当前线程的cpu序号
int getThreadCPUCore()
{ 
	cpu_set_t get;  
	CPU_ZERO(&get); 
	int num = getCPUCoreNumber();
	if (pthread_getaffinity_np(pthread_self(), sizeof(get), &get) < 0) 
	{  
		//LOG_FATAL("get thread affinity failed\n");  
		return -1;
	}
	for (int i=0; i<num; i++)
	{
		if (CPU_ISSET(i, &get))
		{
			return i; 
		}
	}
	//LOG_FATAL("can not get cpu core numer");
	return -1;
}

//获得当前系统的cpu核数
int getCPUCoreNumber()
{
	return sysconf(_SC_NPROCESSORS_CONF);
}

//获得当前线程ID
int getThreadID()
{
	return (int)pthread_self();
}


//设置当前线程的cpu序号
int setThreadCPUCore(int _coreid)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(_coreid, &mask);
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
	{  
		//LOG_FATAL("set thread affinity failed");
		return -1;
	}
	return 1;
}

long getMemorySize()
{
	struct sysinfo s_info;
	sysinfo(&s_info);
	return s_info.totalram;
}


string getOSversion()
{
	ifstream f("/etc/redhat-release");
	string line;
	while(getline(f, line,'\n'))
	{
		return line;
	}
}


string getCPUInfo()
{
	ifstream f("/proc/cpuinfo");
	string line;
	while(getline(f, line,'\n'))
	{
		if (line.find("model name")!=string::npos)
		{
			return getSubstr(line, 2, ":");
		}
	}
}

string getEthernetInterfaceType()
{
	system("lspci | grep Ethernet > /tmp/getEthernetInterfaceType.txt");
	string line;
	string rt;
	bool bMore=false;
	ifstream f("/tmp/getEthernetInterfaceType.txt");
	while(getline(f, line,'\n'))
	{
		if (bMore)		//之后几行增加空格以对齐
		{
			rt += "      " + getSubstr(line, 3, ":") + '\n';
		}
		else		//第一行没有空格
		{
			rt += getSubstr(line, 3, ":") + '\n';
			bMore=true;
		}
	}
	return rt;
}


//////////////////////////////////////////////////////////////////////////

