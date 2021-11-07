#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void
wait_child(int signbr)
{
	int wstatus;
	printf("Signal number: %d\n", signbr);
	wait(&wstatus);
	printf("wstatus (%d)\n", wstatus);
}

int
main(int argc, char *argv[])
{
	int i;
	int pid = fork();

	if (pid == 0) {
		for (i=0; i<20; i++)
			printf("I'm child\n");
	}
	else {
		// Option-1
		// Wait child process, and collect its exit status from system process table
		// then the entry in process table is deleted.
		// wait(&i);

		// Option-2
		// Ignore signal: SIGCHILD
		// signal(SIGCHLD, SIG_IGN);

		// Option-3
		// Register a signal handler to catch SIGCHLD
		// Collets child's exit status in the signal-handler by calling wait()
		signal(SIGCHLD, wait_child);
		printf("I'm parent, wstatus(%d)\n", i);
		while(1);
	}
}
