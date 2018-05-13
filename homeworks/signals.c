#include <stdio.h> //for printf()
#include <signal.h> //for sigemptyset() ...
#include <string.h> //for strsignal()
#include <stdlib.h> //for malloc()
#include <unistd.h> //for alarm() 
#include <fcntl.h> //for open()

void handler(int signo);
void print_sigset(sigset_t *set);
sigset_t* critical_code(void);

int fd; //global variable so handler function can access

int main(void)
{
	/** open/create text file to write keep track of delivered signals **/
	fd = open("signal_footprint.txt", O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);

	/** declare three sets of signals **/	
	sigset_t *newset = malloc(sizeof(sigset_t));
	sigset_t *oldset = malloc(sizeof(sigset_t));
	sigset_t *waitset = malloc(sizeof(sigset_t));
	sigset_t *pending_signals = malloc(sizeof(sigset_t));

	/** initilize signal sets to empty sets **/
	sigemptyset(newset);
	sigemptyset(oldset);
	sigemptyset(waitset);
	sigemptyset(pending_signals);

	/** Add signals to sets using sigaddset() **/
	//sigaddset(oldset, SIGALRM); 
	//sigaddset(waitset, SIGALRM);

	sigaddset(newset, SIGALRM); //timer expired
	sigaddset(newset, SIGSEGV); //invalid memory reference
	sigaddset(newset, SIGILL); //illegal instruction
	sigaddset(newset, SIGPIPE); //write to pipe with no readers
	sigaddset(newset, SIGFPE); //arithmetic exception
	sigaddset(newset, SIGQUIT); //terminal quit character
	sigaddset(newset, SIGTTOU); //background write to control tty

	/** specify a set of blocked signals using sigprocmask() **/
	sigprocmask(SIG_SETMASK, newset, NULL);
	printf("Signal mask:\n");
	print_sigset(newset);

	/** set and establish handler for signals you wish to trigger **/
	signal(SIGALRM, handler);
	signal(SIGSEGV, handler);
	signal(SIGILL, handler);
	signal(SIGPIPE, handler);
	signal(SIGFPE, handler);
	signal(SIGQUIT, handler);
	signal(SIGTTOU, handler);


	//sigprocmask(SIG_UNBLOCK, newset, oldset);

	waitset = critical_code();

	/** release signals in the blocked mask **/
	if(sigsuspend(oldset) == -1)
		printf("sigsuspend returned as expected.\n");

	sigprocmask(SIG_SETMASK, waitset, newset);
	printf("wait signal mask now set:\n");
	print_sigset(waitset);

	raise(SIGCHLD); //change in status of child
	//raise(SIGABRT); //abnormal termination
	raise(SIGUSR1); //user-defined signal

	sigpending(pending_signals);
	printf("pending signals:\n");
	print_sigset(pending_signals);

	sigprocmask(SIG_SETMASK, newset, waitset);
	printf("newset signal mask now set:\n");
	print_sigset(newset);

	return 0;
}
void handler(int signo)
{
	ssize_t num_bytes;

	switch(signo)
	{
	case SIGALRM:
		num_bytes=write(fd, "SIGALRM received\n", strlen("SIGALRM received\n"));
		break;

	case SIGILL:
		num_bytes=write(fd, "SIGILL received\n", strlen("SIGILL received\n"));
		break;
	
	case SIGPIPE:
		num_bytes=write(fd, "SIGPIPE received\n", strlen("SIGPIPE received\n"));
		break;

	case SIGFPE:
		num_bytes=write(fd, "SIGFPE received\n", strlen("SIGFPE received\n"));
		break;

	case SIGQUIT:
		num_bytes=write(fd, "SIGQUIT received\n", strlen("SIGQUIT received\n"));
		break;

	case SIGTTOU:	
		num_bytes=write(fd, "SIGTTOU received\n", strlen("SIGTTOU received\n"));
		break;
	}
}
void print_sigset(sigset_t *set)
{
	for(int i=1; i<32; i++)
	{
		if(sigismember(set, i))
			printf("Signal %d: %s\n", i, strsignal(i));
	}
}
sigset_t* critical_code(void)
{
	ssize_t num_bytes;	
	
	num_bytes=write(fd, "-----------------------------------------------\n", strlen("-----------------------------------------------\n"));
	num_bytes=write(fd, "Critical Code Here\n", strlen("Critical Code Here\n"));
	num_bytes=write(fd, "-----------------------------------------------\n", strlen("-----------------------------------------------\n"));

	raise(SIGQUIT);

	sigset_t *returnset = malloc(sizeof(sigset_t));
	sigfillset(returnset); //block every signal

	return returnset;
}
