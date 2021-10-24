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
	eUNZIP_err_nofile_exist
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
	if(fd < 0)
		return -eUNZIP_err_create_file;
 
	zip_uint64_t sum {0};
	while(sum != fsize) {
		zip_int64_t len = zip_fread(zf, rbuf.get(), BUFSIZ);
		if(len < 0)
			return -eUNZIP_err_read_file;

		write(fd, rbuf.get(), len);
		sum += len;
	}
	close(fd);
	zip_fclose(zf);
	return 0;
}

int
unzip(const char *zip_file, const char *des_dirpath = nullptr)
{
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
			printf("%s\t%lu\t%s\n", asctime_r(localtime_r(&st.mtime, &t), timestamp.get()), st.size, st.name);
			
			strcpy(fname.get(), path.c_str());
			size_t l {strlen(st.name)};
			strcat(fname.get(), st.name);
			l = strlen(fname.get());

			if(fname[l - 1] == '/') {
				safe_create_dir(fname.get());
			} else {
				struct zip_file *zf = zip_fopen_index(za, i, 0);
				if(!zf)
					return -eUNZIP_err_open_file;
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
		return -eUNZIP_err_nofile_exist;
	}

	struct zip_file *zf = zip_fopen(za, target_file, 0);
	if(zf == NULL)
		return -eUNZIP_err_open_file;

	printf("---------------------------------\n");
	printf("%s\t%lu\t%s\n", asctime_r(localtime_r(&st.mtime, &t), timestamp.get()), st.size, st.name);
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
 
int main(int argc, char *argv[])
{
	const char *prg {argv[0]};
	if(argc == 2) {
		unzip(argv[1]);
	}
	else if(argc == 3) {
		if(unzip_file(argv[1], argv[2]) != 0)
			fprintf(stderr, "Extract %s failed!\n", argv[2]);
	}
	else {
		fprintf(stderr, "usage: %s archive.zip\n", prg);
		return 1;
	}

#if 0
	int err;
	const char *archive = argv[1];
	struct zip *za = zip_open(archive, 0, &err);
	if(za == NULL) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't open zip archive '%s': %s\n", prg, archive, errstr);
		return 1;
	}

	std::unique_ptr<char[]> buf {new char[BUFSIZ]};
	zip_int64_t nbr = zip_get_num_entries(za, 0);
	for(zip_int64_t i = 0; i < nbr; i++)
	{
		struct zip_stat st;
		if(zip_stat_index(za, i, 0, &st) == 0) {
			printf("==================\n");
			printf("Name: [%s], ", st.name);
			printf("Size: [%lu], ", st.size);
			printf("mtime: [%u]\n", (unsigned int)st.mtime);

			size_t len = strlen(st.name);
			if(st.name[len - 1] == '/') {
				safe_create_dir(st.name);
			} else {
				struct zip_file *zf = zip_fopen_index(za, i, 0);
				if(!zf) {
					exit(100);
				}
 
				int fd = open(st.name, O_RDWR | O_TRUNC | O_CREAT, 0644);
				if(fd < 0) {
					exit(101);
				}
 
				unsigned long long sum {0};
				while(sum != st.size) {
					len = zip_fread(zf, buf.get(), BUFSIZ);
					if(len < 0) {
						exit(102);
					}
					write(fd, buf.get(), len);
					sum += len;
				}
				close(fd);
				zip_fclose(zf);
			}
		} else {
			printf("File[%s] Line[%d]/n", __FILE__, __LINE__);
		}
	}   
 
	if (zip_close(za) == -1) {
		fprintf(stderr, "%s: can't close zip archive `%s'/n", prg, archive);
		return 1;
	}
#endif
 
	return 0;
}
