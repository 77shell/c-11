
/*
 * Pack files into a zip-file
 *
 */

#include <zip.h>
#include <errno.h>

enum zip_error_e {
	eZIP_ok,
	eZIP_err_create_zipfile,
	eZIP_err_mkdir,
	eZIP_err_create_file,
	eZIP_err_add_file,
	eZIP_err_read_file,
	eZIP_err_close_zipfile,
	eZIP_err_invalid_filename,
	eZIP_err_nofile_exist
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
main(int argc, char *argv[])
{
	if(argc < 3) {
		fprintf(stderr, "usage: %s zip-file target-file\n", argv[0]);
		return 1;
	}

	zip(argv[1], argv[2]);
}

