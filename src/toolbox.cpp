///linux  �� 20130507
#include "toolbox.h"
#include "define.h"
#include <sys/sysinfo.h>
using namespace std;

char  *lpCharacter=new char[CON_WIN_WIDTH];
char *lpOldCharacter=new char[CON_WIN_WIDTH];

/*�������ƣ�getSubstr()
�������ܣ��Կո�Ϊ�ָ����ָ��ֶ�
�汾�ţ�20120201
����1��const string originstr 	Ҫ�ָ���ַ���
����2��int subn	Ҫ��ȡ�ڼ���
����3��string delims		ָ���ָ�����Ĭ���ǿո�
����ֵ����ȡ�������ֶ�*/
string getSubstr(const string originstr,int subn,string delims)
{
	try
	{
		string str=originstr;
		string::size_type pbegin=0;	//ָ���ַ���Ҫ��ȡ�Ŀ�ʼλ��
		string::size_type pend=0;		//ָ���ַ���Ҫ��ȡ�Ľ���λ��
		for (int i=1; i<=subn; i++)
		{
			pbegin=str.find_first_not_of(delims,pend);	//����һ�ν���λ��Ϊ���Ѱ�ҵ�һ�����Ƿָ�����λ�ã���Ϊ�µ����
			if (str.find(delims, pend+1)==string::npos) //���û�ҵ��ָ�������Ϊ�����һ���ˡ�
			{
				if (i<subn) return "";		//����Ѿ����������һ���ˣ���ָ��Ҫ�ָ�Ķ��������˽��ޣ��򷵻ؿ�ֵ��
				pend=str.length()+1; break;	//�ѽ���λ������Ϊ�ַ������λ��
			}
			pend=str.find(delims, pbegin);
		}
		str=str.substr(pbegin, pend-pbegin);		//��ȡ�ַ���
		return str;
	}
	catch (const std::exception& error)
	{
		//LOG("getSubstr of %s error: %s", originstr.c_str(), error.what());
		cout<<"getSubstr of"<<originstr.c_str()<<" error:"<<error.what()<<endl;
		exit(-1);
	}
}
/*�������ƣ�getConfig()
�������ܣ���ȡ�����ļ�ini����Ӧ�����title��ָ�������ֶ�cfgname��ֵ
����1��string title		�����[***]
����2��string cfgName		������µ������ֶ�
����ֵ�������ļ�ini����Ӧ�����title��ָ�������ֶ�cfgname��ֵ
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
		if (strtmp.substr(0,1)=="#")	continue;	//����ע��		
		if (flag==0)
		{
			if (strtmp.find(title)!=string::npos)
			{
				if (strtmp.substr(0,1)=="[")
				{
					if (strtmp.find("]")==string::npos) 	break;	//ȱʧ��]���˳�
					strtitle=strtmp.substr(1);
					strtitle=strtitle.erase(strtitle.find("]"));
					if (strtitle==title)		//�ҵ���������ñ�־λΪ1�������Ͳ�������һ���������
					{
						flag=1;
						continue;
					}
				}
			}
		} 
		if (flag==1)
		{
			if (strtmp.substr(0,1)=="[")	break;	//���������һ��[]��˵����ǰ������Ӧ�������ֶ�������ϣ�����������
			if (strtmp.find(cfgName)!=string::npos)	
			{
				if (strtmp.find("=")==string::npos)	break;	//ȱʧ��=���˳�
				strcfgname=strtmp;
				strcfgname=strcfgname.erase(strcfgname.find("="));
				if (strcfgname==cfgName)		//�ҵ��������Ӧ���ֶκ󣬷���ֵ
				{
					returnValue=strtmp.substr(strtmp.find("=")+1);
					return returnValue;
				}
				else continue;
			}	
		}
	}
	//LOG("�����ļ�����û�ҵ�%s��Ӧ������%s!", title.c_str(),cfgName.c_str());
	cout<<"�����ļ�����û�ҵ�"<<title.c_str()<<"��Ӧ������"<<cfgName.c_str()<<endl;
	return NONE;
}

/*�������ƣ�genDate()
�汾��20130217
�������ܣ��������ڣ�����ṹ��CURRENTDATE��
������
%a ���ڼ��ļ�д 
%A ���ڼ���ȫ�� 
%b �·ݵļ�д 
%B �·ݵ�ȫ�� 
%c ��׼�����ڵ�ʱ�䴮 
%C ��ݵĺ���λ���� 
%d ʮ���Ʊ�ʾ��ÿ�µĵڼ��� 
%D ��/��/�� 
%e �����ַ����У�ʮ���Ʊ�ʾ��ÿ�µĵڼ��� 
%F ��-��-�� 
%g ��ݵĺ���λ���֣�ʹ�û����ܵ��� 
%G ��ݣ�ʹ�û����ܵ��� 
%h ��д���·��� 
%H 24Сʱ�Ƶ�Сʱ 
%I 12Сʱ�Ƶ�Сʱ
%j ʮ���Ʊ�ʾ��ÿ��ĵڼ��� 
%m ʮ���Ʊ�ʾ���·� 
%M ʮʱ�Ʊ�ʾ�ķ����� 
%n ���з� 
%p ���ص�AM��PM�ĵȼ���ʾ 
%r 12Сʱ��ʱ�� 
%R ��ʾСʱ�ͷ��ӣ�hh:mm 
%S ʮ���Ƶ����� 
%t ˮƽ�Ʊ�� 
%T ��ʾʱ���룺hh:mm:ss 
%u ÿ�ܵĵڼ��죬����һΪ��һ�� ��ֵ��0��6������һΪ0��
%U ����ĵڼ��ܣ�����������Ϊ��һ�죨ֵ��0��53��
%V ÿ��ĵڼ��ܣ�ʹ�û����ܵ��� 
%w ʮ���Ʊ�ʾ�����ڼ���ֵ��0��6��������Ϊ0��
%W ÿ��ĵڼ��ܣ�������һ��Ϊ��һ�죨ֵ��0��53�� 
%x ��׼�����ڴ� 
%X ��׼��ʱ�䴮 
%y �������͵�ʮ������ݣ�ֵ��0��99��
%Y �����Ͳ��ֵ�ʮ����� 
%z��%Z ʱ�����ƣ�������ܵõ�ʱ�������򷵻ؿ��ַ���
%% �ٷֺ�
����ֵ�����ؽṹ�壬����CURRENTDATE�С�
*/
CURRENTDATE getDate()
{
	char tp[21];
	//����ļ�����ʱ�õ���ʱ��
	string strst,stryear,strmonth,strday,strhour,strmin,strsec;
	time_t t=time(NULL);
	struct tm* local=localtime(&t);
	struct CURRENTDATE date;
	//��
	strftime(tp, 20, "%Y", local);
	date.year=tp;
	//��
	strftime(tp, 20, "%m", local);
	date.month=tp;
	//��
	strftime(tp, 20, "%d", local);
	date.day=tp;
	//Сʱ
	strftime(tp, 20, "%H", local);
	date.hour=tp;
	//��
	strftime(tp, 20, "%M", local);
	date.minute=tp;
	//��
	strftime(tp, 20, "%S", local);
	date.second=tp;
	strftime(tp, 20, "%Y%m%d", local);
	date.curdate=tp;
	return date;
}


/*�������ƣ�ifExistFile()
�������ܣ��ж��ļ����Ƿ����
������string file	Ҫ�жϵ��ļ�
����ֵ����
*/
bool ifExistFile(string file)
{
	ifstream fin(file.c_str());	
	if (!fin.is_open())
	{
		cout<<"�޷���"<<file;
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

/*�������ƣ�inputPWD()
�������ܣ��������룬��ʾ�Ǻţ��ܴ����˸��
��������
����ֵ���������������
��ע����Ҫ��link�м���-lcurses
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

/*�������ƣ�timewait()
�������ܣ�
������
����ֵ����
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

/*�������ƣ�logtime()
�������ܣ���ʽ�����ݵ�log.txt��
������none
����ֵ����
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
/*�������ƣ�WaitForSingleObject()
�������ܣ�linux��αװ��windows��WaitForSingleObject()����
����������˵��
����ֵ��errno������룬�鿴�ں˴����е�/usr/include/asm/errno.h
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
	ts.tv_nsec = tt.tv_usec*1000 + _milisecond * 1000 * 1000;//��������������>1000 000 000
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

/*�������ƣ�WaitForMultipleObjects()
�������ܣ�linux��αװ��windows��WaitForMultipleObjects()����
����������˵��,�ȸ������ź�����
����ֵ��
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


/*�������ƣ�SetEvent()
�������ܣ�linux��αװ��windows��SetEvent()����
����������˵��
����ֵ��ͬ��
*/
int SetEvent(HANDLE & _sem)
{
	return sem_post(&_sem);
}

/*�������ƣ�ResetEvent()
�������ܣ�linux��αװ��windows��ResetEvent()����
����������˵��
����ֵ��ͬ��
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

/*�������ƣ�Sleep()
�������ܣ�linux��αװ��windows��Sleep()����
����������˵��
����ֵ��ͬ��
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

/*�������ƣ�init_daemon()
�������ܣ�linux�´����ػ�����
����������˵��
����ֵ��ͬ��
*/
void init_daemon(void)
{
	int pid;
	int i;

	if(pid=fork())
		_exit(0);//�Ǹ����̣�����������
	else if(pid< 0)
		exit(1);//forkʧ�ܣ��˳�
	//�ǵ�һ�ӽ��̣���̨����ִ��

	setsid();//��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤
	//��������ն˷���
	if(pid=fork())
		_exit(0);//�ǵ�һ�ӽ��̣�������һ�ӽ���
	else if(pid< 0)
		exit(1);//forkʧ�ܣ��˳�
	//�ǵڶ��ӽ��̣�����
	//�ڶ��ӽ��̲����ǻỰ�鳤

	for(i=0;i< NOFILE;++i)//�رմ򿪵��ļ�������
		close(i);
	//chdir("/tmp");//�ı乤��Ŀ¼��/tmp
	umask(0);//�����ļ�������ģ
	return;
}


//��õ�ǰ�̵߳�cpu���
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

//��õ�ǰϵͳ��cpu����
int getCPUCoreNumber()
{
	return sysconf(_SC_NPROCESSORS_CONF);
}

//��õ�ǰ�߳�ID
int getThreadID()
{
	return (int)pthread_self();
}


//���õ�ǰ�̵߳�cpu���
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
		if (bMore)		//֮�������ӿո��Զ���
		{
			rt += "      " + getSubstr(line, 3, ":") + '\n';
		}
		else		//��һ��û�пո�
		{
			rt += getSubstr(line, 3, ":") + '\n';
			bMore=true;
		}
	}
	return rt;
}


//////////////////////////////////////////////////////////////////////////

