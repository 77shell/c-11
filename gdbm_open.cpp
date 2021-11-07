#include <gdbm.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

namespace {
	FILE*
	_create_temp_file()
	{
		FILE *fp = tmpfile();
		if(fp == NULL)
			perror(__func__);

		char fname[FILENAME_MAX], link[FILENAME_MAX] = {0};
		sprintf(fname, "/proc/self/fd/%d", fileno(fp));
		printf("%s\n", fname);
		if(readlink(fname, link, sizeof link - 1) > 0)
			printf("File name: %s\n", link);

		return fp;
	}

	long
	flen(FILE *fp) {
		if(fseek(fp, 0L, SEEK_END) != 0)
			return -1;

		long fsize {ftell(fp)};
		if(fsize == -1L) {
			perror("Get file size");
			return -1;
		}

		return fsize;
	}
}

GDBM_FILE mFile;

int
gdbm_open_filep()
{
        static const int block_size {4096};
	FILE *tmp_dbm {_create_temp_file()};
        if( (mFile = gdbm_fd_open(fileno(tmp_dbm), "", block_size,
			       GDBM_WRCREAT, nullptr)) == nullptr)
        {
                perror("gdbm_open");
                exit(EXIT_FAILURE);
        }

        assert(mFile != nullptr);
        gdbm_count_t count;
        gdbm_count(mFile, &count);
        fprintf(stderr, "DBM> Open fd(%d) successful\n", fileno(tmp_dbm));
        fprintf(stderr, "Number of entry: %lld\n", count);
	gdbm_close(mFile);
	return 0;
}

int
gdbm_import_file(const char *file)
{
	gdbm_load(&mFile, file, GDBM_REPLACE, 0, NULL);
        assert(mFile != nullptr);
        gdbm_count_t count;
        gdbm_count(mFile, &count);
        fprintf(stderr, "DBM> Open file (%s) successful\n", file);
        fprintf(stderr, "Number of entry: %lld\n", count);
	gdbm_close(mFile);
	return 0;
}

int
main(int argc, char *argv[])
{
	// if(gdbm_open_filep() != 0) {
	// 	exit(EXIT_FAILURE);
	// }
	gdbm_import_file(argv[1]);
	return 0;
}
