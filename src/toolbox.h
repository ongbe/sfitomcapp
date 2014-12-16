#pragma once
//update by wen.zh linux edition 20131013
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <iomanip>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/sem.h>
#include <ncurses.h>

#define CON_WIN_WIDTH 150
typedef unsigned long DWORD;
typedef sem_t HANDLE;
#define FALSE 0
#define TRUE 1
#define OUTPUT_FILE 2
#define OUTPUT_CONSOLE 1
#define OUTPUT_BOTH 3
#define INFINITE 0
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT 110
#define NOFILE 3
#define  NONE "None"
//#define IPCKEY 0x111

using namespace std;

struct CURRENTDATE
{
	string year;
	string month;
	string day;
	string hour;
	string minute;
	string second;
	string curdate;
};

struct FORMATTICKCOUNT
{
	string usecLevelTime;	//微秒级别的时间，格式化后的
	DWORD usecLevelCount;	//微秒级别的计时，未格式化的
};

struct TIMEDIFF
{
	DWORD begin;
	DWORD end;
};

//从配置文件中读取配置项
string getConfig(string title,string cfgName);

//按照指定字符分割字符串
string getSubstr(const string originstr,int subn,string delims=" ");

//获取当前日期
CURRENTDATE getDate();

//检查路径是否存在
void ifExistPath(string file);

//检查文件是否存在
bool ifExistFile(string file);

//获取CPU时间，精确到微秒
DWORD GetTickCount();

//获取格式化后的CPU时间
void GetFormatTickCount(FORMATTICKCOUNT &fc);

//输入密码，以*号显示
int inputPWD(string password);

//等待一定的毫秒时间
void timewait(DWORD ms);

//格式化的时间，日志专用
const string logtime();
const string filetime();

//等待单个信号
int WaitForSingleObject(HANDLE & _sem, DWORD _milisecond);

//设置信号为可用
int SetEvent(HANDLE & _sem);

//重置信号
void ResetEvent(HANDLE & _sem);

//休眠一定的毫秒时间
void Sleep(unsigned int milisecond);

//使程序变为守护进程
void init_daemon(void);

//获得当前线程的cpu序号
int getThreadCPUCore();

//获得当前系统的cpu核数
int getCPUCoreNumber();

//获得当前线程ID
int getThreadID();

//设置当前线程的cpu序号
int setThreadCPUCore(int _coreid);

//获取linux内存大小
long getMemorySize();

//获取linux操作系统版本
string getOSversion();

//获取linuxCPU信息
string getCPUInfo();

//获取网卡信息
string getEthernetInterfaceType();

//打印在屏幕正中间
void disclaimer();

//按回车继续
void pressentertocontiue();

//锁，这个锁在离开作用域的时候自动解锁，很给力。
class CSimpleMutex
{
public:
	CSimpleMutex(pthread_mutex_t & lock) : mylock(lock)
	{
		acquire();
	}
	~CSimpleMutex()
	{
		release();
	}
private:
	int acquire()
	{
		return pthread_mutex_lock(&mylock);
	}
	int release()
	{
		return pthread_mutex_unlock(&mylock);
	}
	pthread_mutex_t & mylock;
};


//日志类
class Csyslog
{
public:
	Csyslog (string filename){ Csyslog::open(filename.c_str()); }
	Csyslog () {}
	~Csyslog(){syslog.close();}

public:
	ofstream syslog;
	int nLogID;
	pthread_mutex_t lock;

public:
	void open(const char* filename, ios_base::openmode mode = ios_base::trunc)
	{
		syslog.open(filename, mode);
		if (!syslog.is_open())
		{
			cout<<"open "<<filename<<" failed!"<<endl;
			exit(-1);
		}
		nLogID=0;
		pthread_mutex_init(&lock, NULL);
	};
	template <class T>
	ostream & operator<<(T & strings)
	{
		CSimpleMutex l(lock);
#ifdef DEBUG
		syslog<<setiosflags(ios::fixed)<<setprecision(3)<<"["<<logtime()<<"]\t"<<strings;
#else
		syslog<<setiosflags(ios::fixed)<<setprecision(3)<<strings;
#endif
		return syslog;
	};
};



/*
//共享内存，封装了一些常用功能，有待完善
typedef struct 
{ 
	char buf[1024]; 
}memory;

union semun 
{ 
	int val; 
	struct semid_ds *buf; 
	unsigned short *array; 
}; 

class CShareMemory
{
public:
	CShareMemory()
	{
		p = NULL; 
	}
	~CShareMemory()
	{
		Delete();
	}


private:
	key_t key;
	int shmid;
	int create_flag;
	int sem_id;
	int ssize;
	memory *p;
	int sem_num;	//要创建的信号集的个数

public:
	void Init(int _size, int _SemNum = 1)
	{
		ssize = _size;
		sem_num = _SemNum;
		Create();
	}


	//往共享内存中写数据，需要自己在外部加锁
	void Write(char *source)
	{
		//strncpy(p->buf, source, ssize);
		memcpy(p->buf, source, ssize);
	}


	//从共享内存中读数据，需要自己再外部释放锁
	void* Read()
	{
		return p->buf;
	}


private:
	//创建共享内存和共享信号
	void Create()
	{
		if ((key = ftok("/tmp", IPCKEY)) < 0) 
		{ 
			cout<<"failed to get key"<<endl; 
			exit(-1); 
		} 
		if ((sem_id = semget(key, sem_num, 0666 | IPC_CREAT | IPC_EXCL)) < 0) 
		{ 
			if (errno == EEXIST) 
			{ 
				if ((sem_id = semget(key, sem_num, 0666)) < 0) 
				{ 
					cout<<"failed to semget"<<endl; 
					exit(-1); 
				} 
			} 
		} 

		init_sem(sem_id, 0);

		if ((shmid = shmget(key, sizeof(memory), 0666 | IPC_CREAT | IPC_EXCL)) < 0) 
		{ 
			if (errno == EEXIST) 
			{ 
				if ((shmid = shmget(key, sizeof(memory), 0666)) < 0) 
				{ 
					cout<<"failed to shmget memory"<<endl; 
					exit(-1); 
				} 
			} 
			else 
			{ 
				cout<<"failed to shmget"<<endl; 
				exit(-1); 
			} 
		} 
		else 
			create_flag = 1; 


		if ((p = (memory *)shmat(shmid, NULL, 0)) == (void *)(-1))
		{ 
			cout<<"failed to shmat memory"<<endl; 
			exit(-1); 
		}
	}

private:
	//删除共享内存和共享信号
	void Delete()
	{
		if (create_flag == 1) 
		{ 
			if (shmdt(p) < 0) 
			{ 
				cout<<"failed to shmdt memory"<<endl; 
				exit(-1); 
			} 

			if (shmctl(shmid, IPC_RMID, NULL) == -1) 
			{ 
				cout<<"failed to delete share memory"<<endl; 
				exit(-1); 
			} 

			delete_sem(sem_id); 
		} 
	}

private:
	//初始化共享信号
	void init_sem(int sem_id, int init_value) 
	{ 
		union semun sem_union; 

		sem_union.val = init_value; 

		if (semctl(sem_id, 0, SETVAL, sem_union) < 0) 
		{ 
			cout<<"failed to init_sem"<<endl; 
			exit(-1); 
		} 

		return ; 
	} 

private:
	//删除共享信号
	void delete_sem(int sem_id) 
	{ 
		union semun sem_union; 

		if (semctl(sem_id, 0, IPC_RMID, sem_union) < 0) 
		{ 
			cout<<"failed to delete_sem"<<endl; 
			exit(-1); 
		} 

		return ; 
	} 


public:
	//对指定编号的共享信号做V操作，即+1。编号从0开始
	void SetSem(int sem_num = 0, short op = 1) 
	{ 
		struct sembuf sem_b; 

		sem_b.sem_num = sem_num; 
		sem_b.sem_op = op; 
		sem_b.sem_flg = SEM_UNDO; 

		if (semop(sem_id, &sem_b, 1) < 0) 
		{ 
			cout<<"failed to sem_v"<<endl; 
			exit(-1); 
		} 
		return ; 
	}


	//对指定编号的共享信号做P操作，即-1。编号从0开始
	int WaitSem(int sem_num = 0, short op = -1, short flag = SEM_UNDO) 
	{ 
		struct sembuf sem_b; 

		sem_b.sem_num = sem_num; 
		sem_b.sem_op = op; 
		sem_b.sem_flg = flag; 

		if (semop(sem_id, &sem_b, 1) < 0) 
		{ 
			if (errno == EAGAIN && flag == IPC_NOWAIT) return 1;	//如果设置为不阻塞，那么错误为EAGAIN是正常的
			cout<<"failed to sem_p"<<endl; 
			exit(-1); 
		} 

		return 0; 
	}
};
*/
