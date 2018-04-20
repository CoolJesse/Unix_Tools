/* Jesse Olson */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define MAXARG 50 /* max args in command */
#define MAXFNAME 500 /* max chars in file name */
#define MAXWORD 500 /* max chars in arg */

extern char **environ;

bool store_char(char *word, size_t maxword, int c, size_t *np)
{
	errno = E2BIG;
 	word[(*np)++] = c;
 	return true;

    //EC_CLEANUP_BGN
 	return false;
    //EC_CLEANUP_END
}
void set(int argc, char *argv[])
{
	int i;
	
	if(argc != 1)
		printf("Extra args\n");
	else
		for(i = 0; environ[i] != NULL; i++)
			printf("%s\n", environ[i]);
}
void asg(int argc, char *argv[])
{
	char *name, *val;
	if(argc != 1)
		printf("Extra args\n");
	else
	{	name = strtok(argv[0], "=");
		val = strtok(NULL, ""); /*Get all that's left*/
		if(name == NULL || val == NULL)
			printf("Bad command\n");
		else
			if(setenv(name, val, true) != 0)
				printf("Error setting environment, error number: %d %s\n", errno, strerror(errno));
	}
	return;
}
void display_status(pid_t pid, int status)
{
	if(pid != 0)
		printf("Process %ld: ", (long)pid);
	if(WIFEXITED(status))
		printf("Exit value %d\n", WEXITSTATUS(status));
	else
		if(WCOREDUMP(status))
			printf(" - core dump");
		if(WIFSTOPPED(status))
			printf(" (stopped)");
}

typedef enum {T_WORD, T_BAR, T_AMP, T_SEMI, T_GT, T_GTGT, T_LT, T_NL, T_EOF, T_ERROR} TOKEN;

static void fd_check(void)
{
    int fd;
    bool ok = true;

    for (fd = 3; fd < 20; fd++)
        if (fcntl(fd, F_GETFL) != -1 || errno != EBADF) 
	{	ok = false;
        	fprintf(stderr, "*** fd %d is open ***\n", fd);
        }
    if (!ok)
    	_exit(EXIT_FAILURE);
}
static bool wait_and_display(pid_t pid)
{
	pid_t wpid;
 	int status;

 	do 
	{	wpid = waitpid(-1, &status, 0);
		display_status(wpid, status);
 	} while (wpid != pid);

 	return true;

return false;
}
static bool builtin(int argc, char *argv[], int srcfd, int dstfd)
{
 	char *path;

 	if (strchr(argv[0], '=') != NULL)
 		asg(argc, argv);
 	else if (strcmp(argv[0], "set") == 0)
 		set(argc, argv);
 	else if (strcmp(argv[0], "cd") == 0) {
 		if (argc > 1)
 			path = argv[1];
 		else if ((path = getenv("HOME")) == NULL)
 			path = ".";
 		if (chdir(path) == -1)
 			fprintf(stderr, "%s: bad directory\n", path);
 	}
 	else
 		return false;

 	if (srcfd != STDIN_FILENO || dstfd != STDOUT_FILENO)
 		fprintf(stderr, "Illegal redirection or pipeline\n");
 	return true;
}

static void redirect(int srcfd, const char *srcfile, int dstfd,
	const char *dstfile, bool append, bool bckgrnd) {
 	int flags;
 	if (srcfd == STDIN_FILENO && bckgrnd) {
 		srcfile = "/dev/null";
 		srcfd = -1;
 	}
 	if (srcfile[0] != '\0')
 		srcfd = open(srcfile, O_RDONLY, 0);
 	        dup2(srcfd, STDIN_FILENO);
 	if (srcfd != STDIN_FILENO)
 		close(srcfd);
 	if (dstfile[0] != '\0') {
 		flags = O_WRONLY | O_CREAT;
 		if (append)
 			flags |= O_APPEND;
 		else
 			flags |= O_TRUNC;
 		dstfd = open(dstfile, flags, S_IRUSR);
 	}
	dup2(dstfd, STDOUT_FILENO);
 	if (dstfd != STDOUT_FILENO)
 		close(dstfd);
 	fd_check();
 	return;
 	_exit(EXIT_FAILURE); /* we are in child */
}
static pid_t invoke(int argc, char *argv[], int srcfd, const char *srcfile,int dstfd, const char *dstfile, bool append, bool bckgrnd, int closefd)
{
	pid_t pid;
 	char *cmdname, *cmdpath;

 	if (argc == 0 || builtin(argc, argv, srcfd, dstfd))
 		return 0;

	switch (pid = fork()){
 	case -1:
 		fprintf(stderr, "Can't create new process\n");
 		return 0;
 	case 0:
 		if (closefd != -1)
 			close(closefd);
 		//if (!bckgrnd)
 			//entry_sig();
 		redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
 		cmdname = strchr(argv[0], '/');

 		if (cmdname == NULL)
 			cmdname = argv[0];
 		else
 			cmdname++;

		cmdpath = argv[0];
 		argv[0] = cmdname;
 		execvp(cmdpath, argv);
 		fprintf(stderr, "Can't execute %s\n", cmdpath);
 		_exit(EXIT_FAILURE);
 	}
 	/* parent */
 	if (srcfd > STDOUT_FILENO)
 		close(srcfd);
 	if (dstfd > STDOUT_FILENO)
 		close(dstfd);
 	if (bckgrnd)
 		printf("%ld\n", (long)pid);
 	return pid;

 	if (pid == 0)
 		_exit(EXIT_FAILURE);
 	return -1;
}
static TOKEN gettoken(char *word, size_t maxword)
{
	enum {NEUTRAL, GTGT, INQUOTE, INWORD} state = NEUTRAL;
	int c;
	size_t wordn = 0;

	while ((c = getchar()) != EOF) {
		switch (state) {
 		case NEUTRAL:
 			switch (c) {
 			case ';':
 				return T_SEMI;
 			case '&':
 				return T_AMP;
 			case '|':
 				return T_BAR;
 			case '<':
 				return T_LT;
 			case '\n':
 				return T_NL;
 			case ' ':
 			case '\t':
 				continue;
 			case '>':
 				state = GTGT;
 				continue;
			case '"':
 				state = INQUOTE;
 				continue;
 			default:
 				state = INWORD;
 				store_char(word, maxword, c, &wordn);
 				continue;
		}
	case GTGT:
 		if (c == '>')
 			return T_GTGT;
 		ungetc(c, stdin);
 		return T_GT;
	case INQUOTE:
 		switch (c) {
 		case '\\':
 			if ((c = getchar()) == EOF)
 				c = '\\';
 			store_char(word, maxword, c, &wordn);
 			continue;
 		case '"':
 			store_char(word, maxword, '\0', &wordn);
 			return T_WORD;
 		default:
 			store_char(word, maxword, c, &wordn);
 			continue;
 		}
	case INWORD:
 		switch (c) {
 		case ';':
 		case '&':
 		case '|':
 		case '<':
 		case '>':
 		case '\n':
 		case ' ':
 		case '\t':
 			ungetc(c, stdin);
 			store_char(word, maxword, '\0', &wordn); 
 			return T_WORD;
 		default:
 			store_char(word, maxword, c, &wordn); 
 			continue;
 		}
 	}
    }
    return T_EOF;
//EC_CLEANUP_BGN
    return T_ERROR;
//EC_CLEANUP_END
}
static TOKEN command(pid_t *wpid, bool makepipe, int *pipefdp)
{
 	TOKEN token, term;
 	int argc, srcfd, dstfd, pid, pfd[2] = {-1, -1};
 	char *argv[MAXARG], srcfile[MAXFNAME] = "", dstfile[MAXFNAME] = "";
 	char word[MAXWORD];
 	bool append;

 	argc = 0;
 	srcfd = STDIN_FILENO;
 	dstfd = STDOUT_FILENO;

	while (true) {
 		switch (token = gettoken(word, sizeof(word))) {
 		case T_WORD:
 			if (argc >= MAXARG - 1) {
 				fprintf(stderr, "Too many args\n");
 				continue;
 			}
 			if ((argv[argc] = malloc(strlen(word) + 1)) == NULL) {
 				fprintf(stderr, "Out of arg memory\n");
 				continue;
 			}
 			strcpy(argv[argc], word);
 			argc++;
 			continue;
		case T_LT:
 			if (makepipe) {
 				fprintf(stderr, "Extra <\n");
 				break;
 			}
 			if (gettoken(srcfile, sizeof(srcfile)) != T_WORD) {
 				fprintf(stderr, "Illegal <\n");
 				break;
 			}
 			srcfd = -1;
 			continue;
 		case T_GT:
		case T_GTGT:
 			if (dstfd != STDOUT_FILENO) {
 				fprintf(stderr, "Extra > or >>\n");
 				break;
 			}
 			if (gettoken(dstfile, sizeof(dstfile)) != T_WORD) {
 				fprintf(stderr, "Illegal > or >>\n");
 				break;
 			}
 			dstfd = -1;
 			append = token == T_GTGT;
 			continue;
 		case T_BAR:
 		case T_AMP:
 		case T_SEMI:
 		case T_NL:
			argv[argc] = NULL;
			if (token == T_BAR) {
 				if (dstfd != STDOUT_FILENO) {
 					fprintf(stderr, "> or >> conflicts with |\n");
 					break;
 				}
 				term = command(wpid, true, &dstfd);
 				if (term == T_ERROR)
 					return T_ERROR;
 			}
 			else
 				term = token;
 			if (makepipe) {
 				pipe(pfd);
 				*pipefdp = pfd[1];
 				srcfd = pfd[0];
 			}
 			pid = invoke(argc, argv, srcfd, srcfile, dstfd,
 			  dstfile, append, term == T_AMP, pfd[1]);
 			if (token != T_BAR)
 				*wpid = pid;
			if (argc == 0 && (token != T_NL || srcfd > 1))
 				fprintf(stderr, "Missing command\n");
 			while (--argc >= 0)
 				free(argv[argc]);
 			return term;
 		case T_EOF:
 			exit(EXIT_SUCCESS);
 		case T_ERROR:
 			return T_ERROR;
 		}
 	}
 return T_ERROR;
}

int main(void)
{
	pid_t pid;
	TOKEN term = T_NL;
 
	while (true) 
	{
		if (term == T_NL)
			printf("%s", "@ ");
			term = command(&pid, false, NULL);
	
		if (term == T_ERROR) 
		{	fprintf(stderr, "Bad command\n");
			term = T_NL;
 		}

 		if (term != T_AMP && pid > 0)
 			wait_and_display(pid);
 		fd_check();
	}
}
