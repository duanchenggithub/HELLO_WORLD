#ifndef __LOG_H
#define __LOG_H
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>


#define LOGFILE "/tmp/testlog"

#define INTERVAL_TIME 10

enum level{ERROR, WARNING, DEBUG};

int GetStrTime(char* strtime);
int CreateLogFile();
time_t StrTimeUnixTime(char* p_time);
void freshen(int sig);
int GetNowStrTime(char* strtime);  

#endif
