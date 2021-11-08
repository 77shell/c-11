#include <gdbm.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <zip.h>
#include <errno.h>
#include <memory>

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

	GDBM_FILE mFile;

	struct dbm_t {
		GDBM_FILE dbm_file;
		FILE *dbm_filep;

		dbm_t()
			: dbm_file {nullptr}, dbm_filep {nullptr}
			{
				constexpr int block_size {4096};
				FILE *dbm_filep {_create_temp_file()};
				if ((dbm_file = gdbm_fd_open(fileno(dbm_filep), "", block_size, GDBM_WRCREAT, nullptr)) == nullptr)
					perror("gdbm_open");

				assert(dbm_file != nullptr);
				gdbm_count_t count;
				gdbm_count(dbm_file, &count);
				fprintf(stderr, "DBM> Open fd(%d) successful\n", fileno(dbm_filep));
				fprintf(stderr, "Number of entry: %lld\n", count);
		}

		dbm_t(const dbm_t &db) = delete;
		dbm_t& operator=(const dbm_t &db) = delete;

		~dbm_t() {
			if(dbm_file)
				gdbm_close(dbm_file);

			if(dbm_filep)
				fclose(dbm_filep);
		}
	};
}

int
gdbm_import_file(const char *file)
{
	mFile = nullptr;
	if(gdbm_load(&mFile, file, GDBM_REPLACE, 0, NULL)) {
		fprintf(stderr, "GDBM error: %s\n", gdbm_strerror(gdbm_errno));
		fprintf(stderr, "GDBM error nbr: %d\n", gdbm_errno);	
	}
        assert(mFile != nullptr);
        gdbm_count_t count;
        gdbm_count(mFile, &count);
        fprintf(stderr, "DBM> Open file (%s) successful\n", file);
        fprintf(stderr, "Number of entry: %lld\n", count);
	gdbm_close(mFile);
	return 0;
}

int
gdbm_import_filep(dbm_t &dbm, FILE *fp)
{
	dbm_t dbm {};
	unsigned long errline;
	printf("fp len: %ld\n", flen(fp));
	rewind(fp);
	int r = gdbm_load_from_file(&dbm.dbm_file, fp, 1, (GDBM_META_MASK_MODE | GDBM_META_MASK_OWNER), &errline);

	if(r) {
		fprintf(stderr, "gdbm_load_from_file failed (%d)\n", r);
		fprintf(stderr, "GDBM error: %s, errline (%ld)\n", gdbm_strerror(gdbm_errno), errline);
		fprintf(stderr, "GDBM error nbr: %d\n", gdbm_errno);
		return -1;
	}

	gdbm_count_t count;
	gdbm_count(dbm.dbm_file, &count);
	fprintf(stderr, "DBM> Open FD (%d) successful\n", fileno(fp));
	fprintf(stderr, "Number of entry: %lld\n", count);
	return 0;
}

enum unzip_error_e {
	eUNZIP_ok,
	eUNZIP_err_open_zipfile,
	eUNZIP_err_mkdir,
	eUNZIP_err_create_file,
	eUNZIP_err_open_file,
	eUNZIP_err_read_file,
	eUNZIP_err_close_zipfile,
	eUNZIP_err_invalid_filename,
	eUNZIP_err_nofile_exist,
	eUNZIP_err_invalid_filep,
	eUNZIP_err_writing_filep
};

static int
write_to_filep(zip_file *zf, zip_uint64_t fsize, FILE *out_f)
{
	if(!out_f)
		return -eUNZIP_err_invalid_filep;

	int r {0};
	std::unique_ptr<char[]> rbuf {new char [BUFSIZ]};
	zip_uint64_t sum {0};
	while(sum != fsize) {
		zip_int64_t len = zip_fread(zf, rbuf.get(), BUFSIZ);
		if(len < 0) {
			zip_error *ptr = zip_file_get_error(zf);
			printf("f_read fail(%ld): zip_err %d, sys_err %d %s\n", len, ptr->zip_err, ptr->sys_err, zip_error_strerror(ptr));
			return -eUNZIP_err_read_file;
		}

		if(fwrite(rbuf.get(), sizeof(char), len, out_f) != len) {
			r = -eUNZIP_err_writing_filep;
			break;
		}
		sum += len;
	}
	zip_fclose(zf);
	return r;
}

int
unzip_filep(const char *zip_file, const char *target_file, FILE *out_fp)
{
	if(!target_file)
		return -eUNZIP_err_invalid_filename;

	if(!out_fp)
		return -eUNZIP_err_invalid_filep;

	int err;
	struct zip *za = zip_open(zip_file, 0, &err);
	if(za == NULL) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't open '%s': %s\n", __func__, zip_file, errstr);
		return -eUNZIP_err_open_zipfile;
	}

	std::unique_ptr<char[]> timestamp {new char [64]};
	zip_int64_t nbr = zip_get_num_entries(za, 0);
	struct zip_stat st;
	struct tm t;

	zip_stat_init(&st);
	if(zip_stat(za, target_file, 0, &st) == -1) {
		printf("zip_stat error!\n");
		return -eUNZIP_err_nofile_exist;
	}

	struct zip_file *zf = zip_fopen(za, target_file, 0);
	if(zf == NULL)
		return -eUNZIP_err_open_file;

	printf("---------------------------------\n");
	printf("%s\t%lu\t%s\tCRC %X\t\n", asctime_r(localtime_r(&st.mtime, &t), timestamp.get()), st.size, st.name, st.crc);
	int r = write_to_filep(zf, st.size, out_fp);
	if(r != 0)
		fprintf(stderr, "%s: create FILE failed! (%d)\n", __func__, r);

	if (zip_close(za) == -1) {
		fprintf(stderr, "%s: can't close zip '%s'\n", __func__, zip_file);
		return -eUNZIP_err_close_zipfile;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	printf("argc %d\n", argc);
	// gdbm_import_file(argv[1]);

	if(argc != 3) {
		printf("%s ziparchive targetfile\n", argv[0]);
		return 0;
	}
	FILE *fp {_create_temp_file()};
	if(unzip_filep(argv[1], argv[2], fp)) {
		fprintf(stderr, "UNZIP file %s failed!\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	if(gdbm_import_filep(fp)) {
		fprintf(stderr, "GDBM import from fd (%d) failed\n", fileno(fp));
	}

	fclose(fp);
	return 0;
}
