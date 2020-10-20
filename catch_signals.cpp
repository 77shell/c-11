
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <signal.h>

using namespace std;

static void
sigHander_TERM(int sig)
{
        fprintf(stderr, "Signal handler catches signal (%d)\n", sig);
        exit(EXIT_FAILURE);
}


void h()
{
        int r {1};
        string s = "not";
        throw r;
        string s2 = "at all";
}


int main(int argc, char *argv[])
{
        int signals[] = {SIGTERM, SIGQUIT, SIGABRT, SIGINT, SIGKILL};
        for(int sig=0; sig<sizeof signals / sizeof(int); sig++)
        {
                if(signal(signals[sig], sigHander_TERM) == SIG_ERR)
                {
                        cerr << "Register SIGNAL HANDLER error" << sig << endl;
                }
        }

        for(int i=0;;i++, sleep(1))
        {
                if(i == 3)
                        h();
        }
        return 0;
}
