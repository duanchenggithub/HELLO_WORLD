#include "log.h"
int logpipefd[2]={0};
int pipesize = 0;

FILE* errorfp = NULL;
FILE* warningfp = NULL;
FILE* debugfp = NULL;

char errlog[256]={0};
char warnlog[256]={0};
char debuglog[256]={0};
char logbuff[2048]={0};

char flagdebugfile[256]={0};
char flagwarnfile[256]={0};
char flagerrfile[256]={0};

time_t TimeNow = 0;

int InitLog()  
{
    pid_t pid = 0;
    if(pipe(logpipefd) < 0)
    {
        printf("log process create error -->%s\n",strerror(errno));
        exit(0);
    }
    pid = fork();
    if(pid == 0)
    {
        close(logpipefd[1]);
        //日志初始化,启动日志进程
        InitLogModel();
    }
    else if(pid > 0)
    {
        close(logpipefd[0]);
        pipesize = fpathconf(logpipefd[1],_PC_PIPE_BUF);
        if(pipesize == -1 && errno != 0)
        {
            printf("fpathconif error\n");
        }
    }
    return 0;
}

int InitLogModel()
{
    signal(SIGALRM, freshen);
    snprintf(flagdebugfile,256,"%s/%s",LOGDIR,"NoDebug");
    snprintf(flagwarnfile,256,"%s/%s",LOGDIR,"NoWarn");
    snprintf(flagerrfile,256,"%s/%s",LOGDIR,"NoError");
//默认开启err日志
    if(fopen(flagdebugfile,"w+") ==NULL)
    {
        printf("InitLogModle NoWarning Create failed,error:%s\n",strerror(errno));
        exit(0);
    }        
    
    if(fopen(flagwarnfile,"w+") ==NULL) 
    {
        printf("InitLogModle NoError Create failed,error:%s\n",strerror(errno));
        exit(0);
    }

    CreateLogFile();
    alarm(2);
    ReadFromPipe(logpipefd[0]);
    return 0;
}
int ReadFromPipe()
{
    char logbuff[4096]={0};
    while(1)
    {
        if(read(logpipefd[0],logbuff,4096) > 0)
        {
            printf("%s\n",logbuff);
            WriteLog(0, logbuff);//需要获得level
        }
        else if(read <= 0)
        {
            printf("read =0\n");
            close(logpipefd[0]);
            exit(0);
        }
        memset(logbuff,0,4096);
    }
    return 0;
}

int ExistFlagFile()
{
    int flag =0;
    if(access(flagerrfile,F_OK) == 0)
    {
        flag += 1;
    }
    if(access(flagwarnfile,F_OK) == 0)
    {
        flag += 2;
    }
    if(access(flagdebugfile,F_OK) == 0)
    {
        flag += 4;  
    }
    return flag;

}

int CreateLogFile() 
{
    char filedate[64]={0};
    int level =0;

    if(!GetNowStrTime(filedate))
    {
        printf("GetStrTime error\n");
        exit(0);
    }
    level = ExistFlagFile();

    if(level < 4 )
    {
        sprintf(debuglog, "%s/%s%s", LOGDIR, filedate, ".DEBUG"); 
        debugfp = fopen(debuglog,"a+");
        if(debugfp == NULL)
        {
            printf("open %s error --> %s\n", debuglog, strerror(errno));
        }
        level -= 4;
    }
    if(level%4 < 2)
    {
        sprintf(warnlog, "%s/%s%s", LOGDIR, filedate, ".WARNING"); 
        warningfp = fopen(warnlog,"a+");
        if(warningfp == NULL)
        {
            printf("open %s error --> %s\n", warnlog, strerror(errno));
        }
        level -= 2;
    }
    if(level%2 != 1)
    { 
        sprintf(errlog, "%s/%s%s", LOGDIR, filedate, ".ERROR"); 
        errorfp = fopen(errlog,"a+");
        if(errorfp == NULL)
        {
            printf("open %s error --> %s\n", errlog, strerror(errno));
        }
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
int Errlog( char* file, int linenum, char* logstr)
{
    PackageLog(0,file,linenum,logstr);
    return 0;
}
int Warnlog( char* file, int linenum, char* logstr)
{
    PackageLog(1,file,linenum,logstr);
    return 0;
}
int Debuglog( char* file, int linenum, char* logstr)
{
    PackageLog(2,file,linenum,logstr);
    return 0;
}

int PackageLog(int level, char* file, int linenum, char* logstr)
{
    pid_t pid;
    int len=0;
    time_t nowtime;
    char strtime[128]={0};
    char* levbuf=NULL;
    if(strlen(logstr)>1750 || strlen(file) >255)
    {
        return -1;
    }
    struct tm *ptm;
    nowtime=time(NULL);
    ptm=localtime(&nowtime);
    pid=getpid();
    if(strftime(strtime,63,"%Y-%m-%d-%H:%M:%S",ptm) == 0)
    {
        return -1;
    }
    if(level == ERROR)
    {
        levbuf="ERROR";
    }
    else if(level == WARNING)
    {
        levbuf="WARNING";
    }
    else 
    {
        levbuf="DEBUG";
    }
        
    sprintf(logbuff, "[pid=%d,time=%s]  [%s:%d] [%s]-->%s\n",pid,strtime,file,linenum,levbuf,logstr);
    //发送到pipe中
    len = strlen(logbuff);
    if(len < pipesize) //需要处理一下
    {
        write(logpipefd[1],logbuff,len);
    }
    return 0;
}

int WriteLog(int level, char* logstr)
{
    int len = 0 ;
    int flags =0;

    int temp=0;
    len =strlen(logstr);
    if(level < 0 || len == 0)
    {
        return -1;
    }
    //判断是否有标志文件，没有的话才写
    flags = ExistFlagFile();

    if(OverTime() == 1)
    {
        if( CreateLogFile(level) == -1)
            return -1;
    }
    len = strlen(logstr);
    //level 用户需要的级别,flags 配置文件中配置的级别
    if( (level <= ERROR) && (flags%2 != 1) )
    {
        if(errorfp != NULL)
        {
           if((temp=fwrite(logstr, 1, len, errorfp)) != len)
           {
               printf("errno=%d\n",errno);
           }
           printf ("len = %d,temp=%d\n",len,temp);
        }
       //error
    }
    if( (level <= WARNING) && (flags%4 < 2))
    {
        if(warningfp != NULL)
        {
           if(fwrite(logstr, 1, len, warningfp) != len)
           {
               printf("errno = %d\n",errno);
           }
        }
       //error
    }
    if( (level <= DEBUG) && (flags < 4))
    {
        if(debugfp != NULL)
        {
           if(fwrite(logstr, 1, len, debugfp) != len)
           {
               printf("errno = %d\n",errno);
           }
        }
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
//        printf("errorfp\n");
        fflush(errorfp);
    }
    if(warningfp != NULL)
    {
 //       printf("warningfp\n");
        fflush(warningfp);
    }
    if(debugfp != NULL)
    {
  //      printf("debugfp\n");
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

