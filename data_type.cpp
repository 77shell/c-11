#include <cstdint>
#include <stdio.h>


int main(int argc, char *argv[])
{
        fprintf(stdout, "uint32_t size: %zu\n", sizeof(uint32_t));
        fprintf(stdout, "int size: %zu\n", sizeof(int));
        fprintf(stdout, "long int size: %zu\n", sizeof(long));
        fprintf(stdout, "long long int size: %zu\n", sizeof(long long));
}
