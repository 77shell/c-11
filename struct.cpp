#include <cstdio>

struct S {
        int a;
        int b:0;
        int c;
};

int
main(int argc, char *argv[])
{
        S s1;
        return 0;
}
