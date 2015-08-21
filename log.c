#ifndef __LOG_H
#define __LOG_H
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
int GetStrTime(char* strtime);
int CreateLogFile();
time_t StrTimeUnixTime(char* p_time);
#endif

FILE* fp;
//应该单独的进程来处理日志，现在先这样吧
//标准io和定时fflush,作为一个头文件使用
int CreateLogFile()
{
    char filename[32]={0};
    if(!GetStrTime(filename))
    {
        printf("GetStrTime error\n");
        exit(0);
    }
    //fopen(

    



    return 0;
}
int GetStrTime(char* strtime)
{
    struct tm *ptime;
    time_t timenow = 0;
    time(&timenow);
    ptime=localtime(&timenow);
    return strftime(strtime,31,"%Y-%m-%d-%H:%M:%S",ptime);
}

time_t StrTimeUnixTime(char* p_time)
{
    struct tm filetime;
    char fmt[] = "%Y-%m-%d-%H:%M:%S";
    if(strptime(p_time, fmt, &filetime) == NULL)
    {
        return -1;
    }
    return mktime(&filetime);
}


