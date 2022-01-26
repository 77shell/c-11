#include <cstring>
#include <stdio.h>

int
main(int argc, char *argv[])
{
        static const size_t dest_len {16};
        const char *src {"0123456789ABCEDF"};
        char dest[dest_len + 1]; /* null character */
        const size_t src_len = strlen(src);

        for(int i=0; i<dest_len; i++)
                dest[i] = 'X';
        
        for(int i=0; i<dest_len; i++)
                fprintf(stderr, "%c - %02X\n", dest[i], dest[i]);
        fprintf(stderr, "\n");

        fprintf(stderr, "src : %s\n", src);
        fprintf(stderr, "dest: %s\n", dest);
        
        if(src_len == dest_len) {
                fprintf(stderr,  "src(%ld) == dest(%ld)\n", src_len, dest_len);
                strcpy(dest, src);       
        }
        else {
                if(src_len > dest_len) {
                        fprintf(stderr, "src(%ld) > dest(%ld)\n", src_len, dest_len);
                        strncpy(dest, src, src_len);
                        dest[dest_len] = '\0';
                }
                else {
                        fprintf(stderr, "src(%ld) < dest(%ld)\n", src_len, dest_len);
                        strcpy(dest, src);
                }
        }
        
        fprintf(stderr, "\n");
        fprintf(stderr, "src : %s\n", src);
        fprintf(stderr, "dest: %s\n", dest);
        
        for(int i=0; i<dest_len; i++)
                fprintf(stderr, "%c - %02X\n", dest[i], dest[i]);
        return 0;
}
