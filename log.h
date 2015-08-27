#ifndef __LOG_H
#define __LOG_H
#include <stdio.h>
#include <limits.h> 
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>


#define LOGDIR "/tmp/logdir"

#define INTERVAL_TIME 10
#define LOGLEVEL 1

enum level{ERROR, WARNING, DEBUG};

int GetStrTime(char* strtime);
int CreateLogFile();
time_t StrTimeUnixTime(char* p_time);
void freshen(int sig);
int GetNowStrTime(char* strtime);  
int InitLogModel();
int WriteLog(int level, char* logstr);
int InitLog();
int ReadFromPipe();
int PackageLog(int level, char* file, int linenum, char* logstr);
int Errlog( char* file, int linenum, char* logstr);
int Warnlog( char* file, int linenum, char* logstr);
int Debuglog( char* file, int linenum, char* logstr);
extern int logpipefd[2];
extern int pipesize;


/*
typedef struct packlog
{
    int packsize;
    pid_t pid;
    time_t time;
    int level;
    int line;
    int filesiz;
    char filebuff[256];
    char logbuff[4096];
}PackLog;
*/
#endif
