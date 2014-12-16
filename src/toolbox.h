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
	string usecLevelTime;	//΢�뼶���ʱ�䣬��ʽ�����
	DWORD usecLevelCount;	//΢�뼶��ļ�ʱ��δ��ʽ����
};

struct TIMEDIFF
{
	DWORD begin;
	DWORD end;
};

//�������ļ��ж�ȡ������
string getConfig(string title,string cfgName);

//����ָ���ַ��ָ��ַ���
string getSubstr(const string originstr,int subn,string delims=" ");

//��ȡ��ǰ����
CURRENTDATE getDate();

//���·���Ƿ����
void ifExistPath(string file);

//����ļ��Ƿ����
bool ifExistFile(string file);

//��ȡCPUʱ�䣬��ȷ��΢��
DWORD GetTickCount();

//��ȡ��ʽ�����CPUʱ��
void GetFormatTickCount(FORMATTICKCOUNT &fc);

//�������룬��*����ʾ
int inputPWD(string password);

//�ȴ�һ���ĺ���ʱ��
void timewait(DWORD ms);

//��ʽ����ʱ�䣬��־ר��
const string logtime();
const string filetime();

//�ȴ������ź�
int WaitForSingleObject(HANDLE & _sem, DWORD _milisecond);

//�����ź�Ϊ����
int SetEvent(HANDLE & _sem);

//�����ź�
void ResetEvent(HANDLE & _sem);

//����һ���ĺ���ʱ��
void Sleep(unsigned int milisecond);

//ʹ�����Ϊ�ػ�����
void init_daemon(void);

//��õ�ǰ�̵߳�cpu���
int getThreadCPUCore();

//��õ�ǰϵͳ��cpu����
int getCPUCoreNumber();

//��õ�ǰ�߳�ID
int getThreadID();

//���õ�ǰ�̵߳�cpu���
int setThreadCPUCore(int _coreid);

//��ȡlinux�ڴ��С
long getMemorySize();

//��ȡlinux����ϵͳ�汾
string getOSversion();

//��ȡlinuxCPU��Ϣ
string getCPUInfo();

//��ȡ������Ϣ
string getEthernetInterfaceType();

//��ӡ����Ļ���м�
void disclaimer();

//���س�����
void pressentertocontiue();

//������������뿪�������ʱ���Զ��������ܸ�����
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


//��־��
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
//�����ڴ棬��װ��һЩ���ù��ܣ��д�����
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
	int sem_num;	//Ҫ�������źż��ĸ���

public:
	void Init(int _size, int _SemNum = 1)
	{
		ssize = _size;
		sem_num = _SemNum;
		Create();
	}


	//�������ڴ���д���ݣ���Ҫ�Լ����ⲿ����
	void Write(char *source)
	{
		//strncpy(p->buf, source, ssize);
		memcpy(p->buf, source, ssize);
	}


	//�ӹ����ڴ��ж����ݣ���Ҫ�Լ����ⲿ�ͷ���
	void* Read()
	{
		return p->buf;
	}


private:
	//���������ڴ�͹����ź�
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
	//ɾ�������ڴ�͹����ź�
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
	//��ʼ�������ź�
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
	//ɾ�������ź�
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
	//��ָ����ŵĹ����ź���V��������+1����Ŵ�0��ʼ
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


	//��ָ����ŵĹ����ź���P��������-1����Ŵ�0��ʼ
	int WaitSem(int sem_num = 0, short op = -1, short flag = SEM_UNDO) 
	{ 
		struct sembuf sem_b; 

		sem_b.sem_num = sem_num; 
		sem_b.sem_op = op; 
		sem_b.sem_flg = flag; 

		if (semop(sem_id, &sem_b, 1) < 0) 
		{ 
			if (errno == EAGAIN && flag == IPC_NOWAIT) return 1;	//�������Ϊ����������ô����ΪEAGAIN��������
			cout<<"failed to sem_p"<<endl; 
			exit(-1); 
		} 

		return 0; 
	}
};
*/
