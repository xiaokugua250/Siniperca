// https://lwn.net/Articles/531114/
// liangdu1992@gmail.com
// date 2020-11-25


#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define errExit(msg) do {perror(msg);exit(EXIT_FAILURE);\
} while (0)
    
static int childFunc(void *args){
    struct utsname uts;
    if (sethostname(args,strlen(args))==-1)
    errExit("sethostname")   ;
    if (uname(&uts)==-1)
    errExit("uname");
    printf("uts.nodename in child:%s\n",uts.nodename);
    sleep(100);
    return 0;
}

#define STACK_SIZE (1024*1024)

static char child_stack[STACK_SIZE];

int main(int argc,char *argv[]){
    
    pid_t child_pid;
    struct utsname uts;
    if (argc<2){
     fprintf(stderr,"Usage:%s <child-hostname>\n",argv[0]);
     exit(EXIT_FAILURE);
    
    }
    child_pid=clone(childFunc,child_stack+STACK_SIZE,CLONE_NEWUTS|SIGCHLD,argv[1]);
    if (child_pid==-1)
        errExit("clone");
    printf("PID of child created by clone is %ld\n",(long)child_pid);
    sleep(1);
    if (uname(&uts)==-1)
     errExit("uname");

    printf("uts.nodename in parent is %s\n",uts.nodename);
    
    if(waitpid(child_pid,NULL,0)==-1)
	    errExit("waitpid");
    printf("child has terminated\n");
    exit(EXIT_SUCCESS);

}
