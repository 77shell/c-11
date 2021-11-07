/* Copyright (C) 2011 rocenting#gmail.com
 *
 *  Test
 *	$ unzip setpoint.db-export.zip
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <memory>
#include <zip.h>

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

static void
safe_create_dir(const char *dir)
{
	if(mkdir(dir, 0777) < 0) {
		if (errno != EEXIST) {
			perror(dir);
		}
	}
}

static int
write_to_file(zip_file *zf, zip_uint64_t fsize, const char *f)
{
	std::unique_ptr<char[]> rbuf {new char [BUFSIZ]};
	int fd = open(f, O_RDWR | O_TRUNC | O_CREAT, 0644);
	if(fd < 0) {
		printf("open %s failed\n", f);
		return -eUNZIP_err_create_file;
	}
 
	zip_uint64_t sum {0};
	while(sum != fsize) {
		zip_int64_t len = zip_fread(zf, rbuf.get(), BUFSIZ);
		if(len < 0) {
			zip_error *ptr = zip_file_get_error(zf);
			printf("f_read fail(%ld): zip_err %d, sys_err %d %s\n", len, ptr->zip_err, ptr->sys_err, zip_error_strerror(ptr));
			return -eUNZIP_err_read_file;
		}

		write(fd, rbuf.get(), len);
		sum += len;
	}
	close(fd);
	zip_fclose(zf);
	return 0;
}

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


static void
print_valid_stat(zip_stat_t *sb)
{
	constexpr zip_uint64_t valid_field[] = {
		ZIP_STAT_NAME,
		ZIP_STAT_INDEX,
		ZIP_STAT_SIZE,
		ZIP_STAT_COMP_SIZE,
		ZIP_STAT_MTIME,
		ZIP_STAT_CRC,
		ZIP_STAT_COMP_METHOD,
		ZIP_STAT_ENCRYPTION_METHOD
	};

	constexpr const char *field_name[] = {
		"ZIP_STAT_NAME",
		"ZIP_STAT_INDEX",
		"ZIP_STAT_SIZE",
		"ZIP_STAT_COMP_SIZE",
		"ZIP_STAT_MTIME",
		"ZIP_STAT_CRC",
		"ZIP_STAT_COMP_METHOD",
		"ZIP_STAT_ENCRYPTION_METHOD"
	};
	printf("Vaild: %lX\n", sb->valid);
	for(int i = 0; i < sizeof valid_field / sizeof(zip_uint64_t); i++)
		printf("%s %s\n", field_name[i], (sb->valid & valid_field[i]) ? "Valid" : "Invalid");
}

int
unzip(const char *zip_file, const char *des_dirpath = nullptr)
{
	printf("%s\n", __func__);
	int err;
	struct zip *za = zip_open(zip_file, 0, &err);
	if(za == NULL) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't open '%s': %s\n", __func__, zip_file, errstr);
		return -eUNZIP_err_open_zipfile;
	}

	std::string path;
	if(des_dirpath)
	{
		if(mkdir(des_dirpath, 0777) == 0) {
			path = des_dirpath;
			path += "/";
		}
		else {
			perror(des_dirpath);
			return -eUNZIP_err_mkdir;
		}
	}
	else
	{
		path = "./";
	}

	std::unique_ptr<char[]> fname {new char [PATH_MAX]};
	std::unique_ptr<char[]> timestamp {new char [64]};
	zip_int64_t nbr = zip_get_num_entries(za, 0);
	struct zip_stat st;
	struct tm t;

	for(zip_int64_t i = 0; i < nbr; i++)
	{
		if(zip_stat_index(za, i, 0, &st) == 0) {
			printf("---------------------------------\n");
			printf("%s\t%lu\t%s\tCRC %X\t\n", asctime_r(localtime_r(&st.mtime, &t), timestamp.get()), st.size, st.name, st.crc);
			print_valid_stat(&st);
			strcpy(fname.get(), path.c_str());
			size_t l {strlen(st.name)};
			strcat(fname.get(), st.name);
			l = strlen(fname.get());

			if(fname[l - 1] == '/') {
				safe_create_dir(fname.get());
			} else {
				struct zip_file *zf = zip_fopen_index(za, i, 0);
				if(!zf) {
					printf("zip_fopen_index error!\n");
					return -eUNZIP_err_open_file;
				}
				write_to_file(zf, st.size, fname.get());
			}
		} else {
			printf("File[%s] Line[%d]\n", __FILE__, __LINE__);
		}
	}   
 
	if (zip_close(za) == -1) {
		fprintf(stderr, "%s: can't close zip `%s'\n", __func__, zip_file);
		return -eUNZIP_err_close_zipfile;
	}

	return 0;
}

/*
 * The limit of unzip_file:
 *        target_file cannot be in directory, otherwise unzipping will fail.
 *        Because currently, unzip_file doesn't support create directories in target_file.
 */
int
unzip_file(const char *zip_file, const char *target_file, const char *des_dirpath = nullptr)
{
	if(!target_file || strlen(target_file) == 0)
	{
		return -eUNZIP_err_invalid_filename;
	}

	int err;
	struct zip *za = zip_open(zip_file, 0, &err);
	if(za == NULL) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't open '%s': %s\n", __func__, zip_file, errstr);
		return -eUNZIP_err_open_zipfile;
	}

	std::string path;
	if(des_dirpath)
	{
		if(mkdir(des_dirpath, 0777) == 0) {
			path = des_dirpath;
			path += "/";
		}
		else {
			perror(des_dirpath);
			return -eUNZIP_err_mkdir;
		}
	}
	else
	{
		path = "./";
	}

	std::unique_ptr<char[]> fname {new char [PATH_MAX]};
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
	strcpy(fname.get(), path.c_str());
	strcat(fname.get(), st.name);
	int r = write_to_file(zf, st.size, fname.get());
	if(r != 0)
		fprintf(stderr, "%s: create %s failed! (%d)\n", __func__, fname.get(), r);

	if (zip_close(za) == -1) {
		fprintf(stderr, "%s: can't close zip '%s'\n", __func__, zip_file);
		return -eUNZIP_err_close_zipfile;
	}

	return 0;
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


int main(int argc, char *argv[])
{
	const char *prg {argv[0]};
	if(argc == 2) {
		unzip(argv[1]);
	}
	else if(argc == 3) {
		if(unzip_file(argv[1], argv[2]) != 0)
			fprintf(stderr, "Extract %s failed!\n", argv[2]);

		FILE *fp {_create_temp_file()};
		if(!fp)
			return 1;

		if(unzip_filep(argv[1], argv[2], fp) != 0) {
			fprintf(stderr, "Extract %s to FILE failed!\n", argv[2]);
		}
		else {
			printf("FILE len: %ld\n", flen(fp));
		}
		fclose(fp);
	}
	else {
		fprintf(stderr, "usage: %s archive.zip\n", prg);
		return 1;
	}
 
	return 0;
}
