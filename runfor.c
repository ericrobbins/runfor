/*
 * runfor - run process for specified time
 *
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>

#define fail2(n, x, ...)    {fprintf(stderr, x, ##__VA_ARGS__); exit(n);}

int childpid;

void
child_exit(int sig)
{
	int status;
	if (waitpid(childpid, &status, WNOHANG) == childpid)
	{
		if (WIFEXITED(status))
			exit(WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			exit(WTERMSIG(status));
		else
			exit(127); // this should never happen
	}
	return;
}

int	
main(int argc, char **argv)
{
	if (argc < 3)
		fail2(127, "usage: %s <max time> <command> [args...]\n", argv[0]);

	double sleeptime = atof(argv[1]);
	if (sleeptime <= 0.0)
		fail2(127, "sleep time must be > 0\n");

	signal(SIGCHLD, child_exit);

	int newpid = fork();
	if (newpid > 0)
	{
		fd_set fds;
		struct timeval tv;
	
		childpid = newpid;

		tv.tv_sec = sleeptime;
		tv.tv_usec = (int)((sleeptime - (double)tv.tv_sec) * 1000000.0);

		FD_ZERO(&fds);
		select(0, NULL, NULL, NULL, &tv);

		kill(SIGTERM, childpid);
		if (kill(0, childpid) == 0)
			kill(SIGKILL, childpid);

		fail2(127, "child process timed out\n");
	}
	else if (newpid == -1)
	{
		fail2(127, "fork() failed\n");
	}
	else
	{
		argv += 2;
		execvp(argv[0], argv);
		fail2(127, "%s: %s\n", argv[0], strerror(errno));
	}
}	
