#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

void Input(int);
void PrintStatus(int status);

void StatusErrorInterpreter(int status);
void RusageValues(struct rusage);

int main(int argc, char *argv[])
{
	int fd = open("file5.txt", O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
	int ret, status;
	struct rusage usage;

	for(int n=0; n<3; n++)
	{
		if((ret=fork())==0) //child process
		{
			Input(fd);
			printf("Child process, ret=%d.\n", ret);
			status=wait(&status);

			switch(n)
			{
				case 0:
					exit(7);
					break;
				case 1:
					abort();
					break;
				case 2:
					status=status/0;
					break;
			}
		}

		else	//parent process
		{
			printf("Parent process, child ret==%d.\n", ret);
			wait3(&status,0,&usage);
			//wait(&status);
			PrintStatus(status);
			getrusage(RUSAGE_SELF, &usage);
			RusageValues(usage);

		}
			
	}
	
	return 0;
}
void Input(int filedes)
{
	char **name = calloc(3, 20*sizeof(char));
	char **it;
	name[0]="Jesse Olson\n";
	name[1]="Jean-Luc Picard\n";
	name[2]="Benjamin Cisco\n";
	
	for(it=&name[0]; *it != NULL; it++)
	{
		ssize_t num_bytes = write(filedes, *it, strlen(*it));
		sleep(2);
	}
}

void PrintStatus(int status)
{
	int lower_8_bits;
	printf("The error status is %d\n", status);

	if((lower_8_bits=WIFEXITED(status))==true)
	{
		printf("WIFEXITED(status)=%d\n", WIFEXITED(status));
		printf("Exit status of child=%d\n", WEXITSTATUS(status));
		printf("lower_8_bits=%x\n", status);

		StatusErrorInterpreter(WEXITSTATUS(status));

	}

	else if(WIFSIGNALED(status)==true)
	{
		printf("WIFSIGNALED(status)=%d\n", WIFSIGNALED(status));
		printf("Exit status for child=%d\n", WTERMSIG(status));
		printf("Exit status for child=%d\n", WCOREDUMP(status));

		StatusErrorInterpreter(WTERMSIG(status));
	}

	else if(WIFSTOPPED(status)==true)
	{
		printf("WIFSTOPPED(status)=%d\n", WIFSTOPPED(status));
		printf("Exit status of child=%d\n", WSTOPSIG(status));

		StatusErrorInterpreter(WSTOPSIG(status));
	}


}
void StatusErrorInterpreter(int status)
{	
	switch(status)
	{
		case 1:
			printf("Termination status: SIGHNUP\n");
			break;
		case 2:
			printf("Termination status: SIGINT\n");
			break;
		case 3:
			printf("Termination status: SIGQUIT\n");
			break;
		case 4:
			printf("Termination status: SIGILL\n");
			break;
		case 5:
			printf("Termination status: SIGTRAP\n");
			break;
		case 6:
			printf("Termination status: SIGABRT\n");
			break;
		case 7:
			printf("Termination status: SIGBUS\n");
			break;
		case 8:
			printf("Termination status: SIGFPE\n");
			break;
		case 9:
			printf("Termination status: SIGKILL\n");
			break;
		case 10:
			printf("Termination status: SIGUSR1\n");
			break;
		case 11:
			printf("Termination status: SIGSEGV\n");
			break;
		case 12:
			printf("Termination status: SIGUSR2\n");
			break;
		case 13:
			printf("Termination status: SIGPIPE\n");
			break;
		case 14:
			printf("Termination status: SIGALRM\n");
			break;
		case 15:
			printf("Termination status: SIGTERM\n");
			break;
		case 16:
			printf("Termination status: SIGSTKFLT\n");
			break;
		case 17:
			printf("Termination status: SIGCHLD\n");
			break;
		case 18:
			printf("Termination status: SIGCONT\n");
			break;
		case 19:
			printf("Termination status: SIGSTOP\n");
			break;
			break;
		case 20:
			printf("Termination status: SIGTSTP\n");
			break;
		case 21:
			printf("Termination status: SIGTTIN\n");
			break;
		case 22:
			printf("Termination status: SIGTTOU\n");
			break;
		case 23:
			printf("Termination status: SIGURG\n");
			break;
		case 24:
			printf("Termination status: SIGXCPU\n");
			break;
		case 25:
			printf("Termination status: SIGXFSZ\n");
			break;
		case 26:
			printf("Termination status: SIGVTALRM\n");
			break;
		case 27:
			printf("Termination status: SIGPROF\n");
			break;
		case 28:
			printf("Termination status: SIGWINCH\n");
			break;
		case 29:
			printf("Termination status: SIGIO\n");
			break;
		case 30:
			printf("Termination status: SIGPWR\n");
			break;
		case 31:
			printf("Termination status: SIGSYS\n");
			break;
	}
}
void RusageValues(struct rusage usage)
{
	printf("-------------struct rusage stats-----------------:\n");
	printf("user CPU time used: %ld.%06ld\n",usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
	printf("system CPU time used: %ld.%06ld\n",usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
	printf("maximum resident set size: %li\n",usage.ru_maxrss);
	printf("integral shared memory size: %li\n",usage.ru_ixrss);
	printf("integral unshared data size: %li\n",usage.ru_idrss);
	printf("integral unshared stack size: %li\n",usage.ru_isrss);
	printf("page reclaims: %li\n",usage.ru_minflt);
	printf("page faults: %li\n",usage.ru_majflt);
	printf("swaps: %li\n",usage.ru_nswap);
	printf("block input operations: %li\n",usage.ru_inblock);
	printf("block output opperations: %li\n",usage.ru_oublock);
	printf("IPC messages sent: %li\n",usage.ru_msgsnd);
	printf("IPC messages received: %li\n",usage.ru_msgrcv);
	printf("signals received: %li\n",usage.ru_nsignals);
	printf("voluntary context switches: %li\n",usage.ru_nvcsw);
	printf("involuntary context switches: %li\n",usage.ru_nivcsw);
	printf("---------------------------------------------------\n");
}
