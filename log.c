#include "log.h"

FILE* errorfp = NULL;
FILE* warningfp = NULL;
FILE* debugfp = NULL;

char errlog[256]={0};
char warnlog[256]={0};
char debuglog[256]={0};

time_t TimeNow = 0;

//应该单独的进程来处理日志，现在先这样吧
int InitLogModle()
{
    signal(SIGALRM, freshen);
    return 0;
}

int CreateLogFile(int level) 
{
    char filedate[64]={0};

    if(!GetNowStrTime(filedate))
    {
        printf("GetStrTime error\n");
        exit(0);
    }
    switch (level)
    { 
    case ERROR:
        sprintf(errlog, "%s/%s%s", LOGFILE, filedate, ".ERROR"); 
        errorfp = fopen(errlog,"a+");
        if(errorfp == NULL)
        {
            printf("open %s error --> %s\n", errlog, strerror(errno));
            return -1;
        }
        break;
    case WARNING:
        sprintf(warnlog, "%s/%s%s", LOGFILE, filedate, "WARNING"); 
        warningfp = fopen(warnlog,"a+");
        if(warningfp == NULL)
        {
            printf("open %s error --> %s\n", warnlog, strerror(errno));
            return -1;
        }
        break;
    case DEBUG:
        sprintf(debuglog, "%s/%s%s", LOGFILE, filedate, "DEBUG"); 
        debugfp = fopen(debuglog,"a+");
        if(debugfp == NULL)
        {
            printf("open %s error --> %s\n", debuglog, strerror(errno));
            return -1;
        }
        break;
    default:
        printf("logfile level set error default debug\n");
        sprintf(debuglog, "%s/%s%s", LOGFILE, filedate, "DEBUG"); 
        debugfp = fopen(debuglog,"a+");
        if(debugfp == NULL)
        {
            printf("open %s error --> %s\n", debuglog, strerror(errno));
            return -1;
        }
        break;
    }
    return 0;
}

int OverTime()
{
    time_t timenew = 0;
    time(&timenew); 
    if(timenew - TimeNow > INTERVAL_TIME * 60)
    {
        TimeNow = timenew;
        return 1;
    }
    return 0;
}

int WriteLog(int level, char* logstr)
{
    int len = 0 ;
    len =strlen(logstr);
    if(level < 0 || len == 0)
    {
        return -1;
    }
    if(OverTime() == 1)
    {
        if( CreateLogFile(level) == -1)
            return -1;
    }
    len = strlen(logstr);
    if(level <= ERROR)
    {
       fwrite(logstr, 1, len, errorfp);
       //error
    }
    if(level <= WARNING)
    {
       fwrite(logstr, 1, len, warningfp);
       //error
    }
    if(level <= DEBUG)
    {
       fwrite(logstr, 1, len, debugfp);
       //error
    }
    return 0;
}

void freshen(int sig)
{
    if(sig != SIGALRM)
    {
        return ; 
    }
    if(errorfp != NULL)
    {
        fflush(errorfp);
    }
    if(warningfp != NULL)
    {
        fflush(warningfp);
    }
    if(debugfp != NULL)
    {
        fflush(debugfp);
    }
    alarm(2);
}

int GetNowStrTime(char* strtime)
{
    struct tm *ptime;
    time(&TimeNow);
    ptime=localtime(&TimeNow);
    return strftime(strtime,63,"%Y-%m-%d-%H:%M:%S",ptime);
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


