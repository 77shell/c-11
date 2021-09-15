
/*
  Compile file: -lzip

  * Test 
  Extract file (setpoint.db-export_2021-09-05_2125.sha) in zip file (setpoint.db-export.zip)

  $ ./zip setpoint.db-export.zip setpoint.db-export_2021-09-05_2125.sha

 */

#include <zip.h>

int main(int argc, char *argv[])
{
	if(argc < 3) {
		fprintf(stderr, "usage: %s zip-file target-file\n", argv[0]);
		return 1;
	}
	// Open the ZIP archive
	int err = 0;
	zip *z = zip_open(argv[1], 0, &err);

	//Search for the file of given name
	const char *name = argv[2];
	struct zip_stat st;
	zip_stat_init(&st);
	zip_stat(z, name, 0, &st);

	//Alloc memory for its uncompressed contents
	uint8_t *contents = new uint8_t[st.size];

	//Read the compressed file
	zip_file *f = zip_fopen(z, name, 0);
	zip_fread(f, contents, st.size);
	fprintf(stderr, "%s file size: %ld\n", argv[2], st.size);
	zip_fclose(f);

	//And close the archive
	zip_close(z);

	//Do something with the contents
	//delete allocated memory
	for(unsigned long int i=0; i<st.size; i++)
		fprintf(stderr, "%02x ", contents[i]);

	delete[] contents;
}

