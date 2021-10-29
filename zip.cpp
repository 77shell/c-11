
/*
 * Pack files into a zip-file
 *
 */

#include <zip.h>
#include <errno.h>
#include <string.h>
#include <gdbm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

enum zip_error_e {
	eZIP_ok,
	eZIP_err_create_zipfile,
	eZIP_err_mkdir,
	eZIP_err_create_file,
	eZIP_err_add_file,
	eZIP_err_read_file,
	eZIP_err_close_zipfile,
	eZIP_err_invalid_filename,
	eZIP_err_invalid_filep,
	eZIP_err_nofile_exist,
	eZIP_err_null_archive
};

struct ZIP {
	ZIP(const char *path_name)
	{
		int err;
		zip_arch = zip_open(path_name, ZIP_CREATE, &err);
		if(zip_arch == nullptr) {
			constexpr size_t buflen {64};
			char errstr[buflen];
			zip_error_to_str(errstr, buflen, err, errno);
			fprintf(stderr, "%s: can't create '%s': %s\n", __func__, path_name, errstr);
			// return -eZIP_err_create_zipfile;
		}
		else {
			printf("Open archive: %s\n", path_name);
		}
	}

	~ZIP() {
		zip_error_fini(&zip_err);
		if(zip_arch)
			zip_close(zip_arch);
		printf("%s\n", __func__);
	}

	int add_file(const char *path_fname) {
		if(!zip_arch)
			return -eZIP_err_null_archive;

		if(!path_fname)
			return -eZIP_err_invalid_filename;

		zip_error_init(&zip_err);
		zip_source_t *zs = zip_source_file_create(path_fname, 0, 0, &zip_err);
		if(zs == nullptr) {
			return -eZIP_err_create_file;
		}

		if(zip_file_add(zip_arch, path_fname, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			return -eZIP_err_add_file;
		}
		return 0;
	}

	int add_filep(FILE *fp, const char *zfile_name) {
		if(!zip_arch)
			return -eZIP_err_null_archive;

		if(!fp)
			return -eZIP_err_invalid_filep;

		rewind(fp);
		zip_error_init(&zip_err);
		zip_source_t *zs = zip_source_filep_create(fp, 0, 0, &zip_err);
		if(zs == nullptr) {
			printf("%s: %s\n", __func__, zip_error_strerror(&zip_err));
			return -eZIP_err_create_file;
		}

		if(zip_file_add(zip_arch, zfile_name, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			return -eZIP_err_add_file;
		}
		printf("%s: %s\n", __func__, zip_error_strerror(&zip_err));

		// int fno {fileno(fp)};
		// if(fno == -1)
		// 	perror("fileno()");
		// else {
		// 	printf("file no %d\n", fno);
		// 	if(fclose(fp) != 0)
		// 		perror("fclose");
		// }
		return 0;
	}

private:
	struct zip *zip_arch;
	zip_error_t zip_err;
};

int
zip(const char *zip_archive, const char *new_file)
{
	if(!new_file) {
		return eZIP_err_invalid_filename;
	}

	int err;
	struct zip *za = zip_open(zip_archive, ZIP_CREATE, &err);
	if(za == nullptr) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't create '%s': %s\n", __func__, zip_archive, errstr);
		return -eZIP_err_create_zipfile;
	}

	{
		zip_error_t err;
		zip_source_t *zs = zip_source_file_create(new_file, 0, 0, &err);
		if(zs == nullptr) {
			zip_close(za);
			return -eZIP_err_create_file;
		}

		if(zip_file_add(za, new_file, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			zip_close(za);
			return eZIP_err_add_file;
		}
	}

	zip_close(za);
	return 0;
}

int
zip_filep(const char *zip_archive, FILE *filep, const char *filename)
{
	if(!filep) {
		printf("Invalid filep\n");
		return eZIP_err_invalid_filep;
	}

	int err;
	struct zip *za = zip_open(zip_archive, ZIP_CREATE, &err);
	if(za == nullptr) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't create '%s': %s\n", __func__, zip_archive, errstr);
		return -eZIP_err_create_zipfile;
	}

	{
		rewind(filep);
		zip_error_t err;
		zip_error_init(&err);
		printf("zip_source_filep_create()\n");
		zip_source_t *zs = zip_source_filep_create(filep, 0, 0, &err);
		if(zs == nullptr) {
			printf("%s: %s\n", __func__, zip_error_strerror(&err));
			zip_close(za);
			zip_error_fini(&err);
			return -eZIP_err_create_file;
		}
		zip_error_fini(&err);

		printf("zip_file_add()\n");
		if(zip_file_add(za, filename, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			zip_close(za);
			printf("Add file failed\n");
			return eZIP_err_add_file;
		}
		printf("%s: %s\n", __func__, zip_error_strerror(&err));
	}

	zip_close(za);
	return 0;
}

static FILE*
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

static void
write_random_data(FILE *fp)
{
	char buf[BUFSIZ];
	strcpy(buf, "Hello everyone!");
	size_t w = fwrite(buf, sizeof(char), BUFSIZ, fp);
	printf("Wrote %ld-byte\n", w);
}

static int
dump_dbm(GDBM_FILE dbm, FILE *fp)
{
	int r = gdbm_dump_to_file(dbm, fp, GDBM_DUMP_FMT_ASCII);
	if(r != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}
	return r;
}

static GDBM_FILE
open_dbm(const char *dbm_path)
{
	GDBM_FILE dbm;
        static const int block_size {4096};
        if( (dbm = gdbm_open(dbm_path, block_size, GDBM_READER, 0666, nullptr)) == nullptr)
        {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
                perror("gdbm_open");
        }
	return dbm;
}

static void
zip_gdbm(char *argv[])
{
	printf("%s\n", __func__);
	GDBM_FILE dbm = open_dbm("./setpoint.db");
	if(!dbm) {
		exit(0);
	}

	FILE *fp = _create_temp_file();
	if(!fp) {
		perror("Create temp file");
		exit(0);
	}

	if(dump_dbm(dbm, fp)) {
		exit(0);
	}

	//write_random_data(fp);
	zip_filep(argv[1], fp, argv[2]);

	// int fno {fileno(fp)};
	// if(fno == -1) {
	// 	perror("fileno()");
	// }
	// else {
	// 	printf("file no %d\n", fno);
	// 	if(fclose(fp) != 0)
	// 		perror("fclose");
	// }

	if(gdbm_close(dbm) != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}
}


static void
zip_gdbm2(char *argv[])
{
	printf("%s\n", __func__);
	GDBM_FILE dbm = open_dbm("./setpoint.db");
	if(!dbm) {
		exit(0);
	}

	FILE *fp = _create_temp_file();
	if(!fp) {
		perror("Create temp file");
		exit(0);
	}

	if(dump_dbm(dbm, fp)) {
		exit(0);
	}

	std::string name {argv[1]};
	name += "v2";
	ZIP zip {name.c_str()};
	if(zip.add_filep(fp, argv[2]) != 0) {
		printf("add_filep %s failed\n", argv[2]);
	}

	if(gdbm_close(dbm) != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}

	// if(fp) {
	// 	int fno {fileno(fp)};
	// 	if(fno == -1)
	// 		perror("fileno()");
	// 	else {
	// 		printf("file no %d\n", fno);
	// 		if(fclose(fp) != 0)
	// 			perror("fclose");
	// 	}
	// }
}

int
main(int argc, char *argv[])
{
	if(argc < 3) {
		fprintf(stderr, "usage: %s zip-file target-file\n", argv[0]);
		return 1;
	}

	//zip(argv[1], argv[2]);
	zip_gdbm(argv);
	zip_gdbm2(argv);
}
