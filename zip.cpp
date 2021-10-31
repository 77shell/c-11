
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
		: zip_arch {nullptr}
	{
		path_zarch = strdup(path_name);
	}

	ZIP(const ZIP &) = delete;
	ZIP& operator=(const ZIP &) = delete;

	~ZIP() {
		free(path_zarch);
		printf("%s\n", __func__);
	}

	int add_file(const char *path_fname) {
		if(open() != 0)
			return -eZIP_err_null_archive;

		if(!path_fname)
			return -eZIP_err_invalid_filename;

		int r {eZIP_ok};
		zip_error_t err;
		zip_error_init(&err);
		zip_source_t *zs = zip_source_file_create(path_fname, 0, 0, &err);

		if(zs == nullptr) {
			r = -eZIP_err_create_file;
		}
		else if(zip_file_add(zip_arch, path_fname, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			r = -eZIP_err_add_file;
		}

		printf("%s: %s\n", __func__, zip_error_strerror(&err));
		zip_error_fini(&err);
		if(zip_close(zip_arch))
			r = -eZIP_err_close_zipfile;
		zip_arch = nullptr;
		return r;
	}

	int add_filep(FILE *fp, const char *zfile_name) {
		if(open() != 0)
			return -eZIP_err_null_archive;

		if(!fp)
			return -eZIP_err_invalid_filep;

		fseek(fp, 0L, SEEK_END);
		long f_len = ftell(fp);
		if(f_len == -1L) {
			perror("Get file size");
			return -eZIP_err_create_file;
		}
		else {
			printf("File len: %ld\n", f_len);
		}

		rewind(fp);
		int r {eZIP_ok};
		zip_error_t err;
		zip_error_init(&err);
		zip_source_t *zs = zip_source_filep_create(fp, 0, 0, &err);

		if(zs == nullptr) {
			printf("%s: %s\n", __func__, zip_error_strerror(&err));
			zip_error_fini(&err);
			r = -eZIP_err_create_file;
		}
		else if(zip_file_add(zip_arch, zfile_name, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			r = -eZIP_err_add_file;
		}

		printf("%s: %s\n", __func__, zip_error_strerror(&err));
		zip_error_fini(&err);
		if(zip_close(zip_arch))
			r = -eZIP_err_close_zipfile;
		zip_arch = nullptr;
		return r;
	}

	int open() {
		if(zip_arch) {
			fprintf(stderr, "%s has been opened\n", path_zarch);
			return 0;
		}

		int err;
		zip_arch = zip_open(path_zarch, ZIP_CREATE, &err);
		if(zip_arch == nullptr) {
			constexpr size_t buflen {64};
			char errstr[buflen];
			zip_error_to_str(errstr, buflen, err, errno);
			fprintf(stderr, "%s: can't create '%s': %s\n", __func__, path_zarch, errstr);
			return -eZIP_err_create_zipfile;
		}
		else {
			printf("Open archive: %s\n", path_zarch);
		}

		return 0;
	}

private:
	char *path_zarch;
	struct zip *zip_arch;
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

	printf("---------> Open zip archive: %s\n", zip_archive);

	int err;
	zip_t *za = zip_open(zip_archive, ZIP_CREATE, &err);
	if(za == nullptr) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't create '%s': %s\n", __func__, zip_archive, errstr);
		return -eZIP_err_create_zipfile;
	}

	{
		fseek(filep, 0L, SEEK_END);
		long f_len = ftell(filep);
		if(f_len == -1L) {
			perror("Get file size");
			return -eZIP_err_create_file;
		}
		else {
			printf("File len: %ld\n", f_len);
		}

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
print_filep_len(FILE *fp)
{
	printf("fileno(%d)\t", fileno(fp));
	fseek(fp, 0L, SEEK_END);
	long flen = ftell(fp);
	printf("File size: %ld\n", flen);
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
/*
 * zip_source_filep_create(FILE *file, zip_uint64_t start, zip_int64_t len, zip_error_t *error)
 *
 * The file stream is closed when the source is being freed, usually by zip_close(3).
 */
	//
	// Due to upon reason, we nned a fp2 to keep initial fd.
	//
	FILE *fp2 = fdopen (dup (fileno (fp)), "r");
	if(dump_dbm(dbm, fp)) {
		exit(0);
	}

	print_filep_len(fp);
	zip_filep(argv[1], fp, argv[2]);
	print_filep_len(fp);

	print_filep_len(fp2);
	zip_filep(argv[1], fp2, "second");
	print_filep_len(fp2);

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

	FILE *fp2 = fdopen (dup (fileno (fp)), "r");

	if(dump_dbm(dbm, fp)) {
		exit(0);
	}

	{
		std::string name {argv[1]};
		name += "-v2";
		ZIP zip {name.c_str()};
		print_filep_len(fp);
		zip.add_filep(fp, argv[2]);
		print_filep_len(fp);

		//
		// After add_filep(), FILE stream will be close.
		// So fp2 is necessary.
		//
		print_filep_len(fp2);
		zip.add_filep(fp2, "second");
		print_filep_len(fp2);
	}

	if(gdbm_close(dbm) != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}
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
	printf("\n-------------\n");
	zip_gdbm2(argv);
	sleep(2);
}
