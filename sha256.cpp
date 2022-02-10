
// compile with: gcc -o sha256 sha256.c -lcrypto

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <memory>
#include <gdbm.h>
#include <unistd.h>
#include <fcntl.h>

static void digest_message(const unsigned char *message, size_t message_len, unsigned char *digest, unsigned int *digest_len);
static void digest_message_file(FILE *f, unsigned char *digest, unsigned int *digest_len);

namespace util {

	struct SHA256 {
		SHA256(const char *file);
		SHA256(FILE *);
		~SHA256();

		std::string digest() const;
		unsigned int digest_len() const;
		uint8_t operator[](size_t idx);

		/*
		 * 0: successful
		 * 0<: failed
		 */
		int write_to_file(const char *file_path);
		int write_to_filep(FILE *out_fp);

		/*
		 * 0: equal
		 * >0: not equal
		 */
		int compare(const char *sha_file);
		int compare(const void *sha);

	private:
		struct impl_t;
		std::unique_ptr<impl_t> mp_Impl;
	};

}

using namespace util;

struct SHA256::impl_t {
	impl_t(const char *file)
		: f {nullptr}, f_len {0}, digest {nullptr}, digest_len {0}, mdctx {nullptr}, message_buf {new unsigned char [BUFSIZ]}
		{
			f = fopen(file, "r");
			if (!f) {
				fprintf(stderr, "%s: couldn't open %s\n", __func__, file);
				return;
			}

			fseek(f, 0L, SEEK_END);
			f_len = ftell(f);
			if(f_len == -1L) {
				perror("Get file size");
				return;
			}
			printf("File size: %ld\n", f_len);
			rewind(f);

			if((mdctx = EVP_MD_CTX_new()) == NULL)
				return;

			digest_message();
			format_digest();
		}

	impl_t(FILE *fp)
		: f {fp}, f_len {0}, digest {nullptr}, digest_len {0}, mdctx {nullptr}, message_buf {new unsigned char [BUFSIZ]}
		{
			if (!f) {
				fprintf(stderr, "%s: couldn't open FILE descriptor %d\n", __func__, fileno(f));
				return;
			}

			fseek(f, 0L, SEEK_END);
			f_len = ftell(f);
			if(f_len == -1L) {
				perror("Get file size");
				return;
			}
			printf("File size: %ld\n", f_len);
			rewind(f);

			if((mdctx = EVP_MD_CTX_new()) == NULL)
				return;

			digest_message();
			format_digest();
		}

	~impl_t() {
		message_buf.reset();

		if(f)
			fclose(f);

		if(digest)
			OPENSSL_free(digest);

		if(mdctx)
			EVP_MD_CTX_free(mdctx);
	}

	int digest_message() {
		if(EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1)
			return -1;

		long len {f_len};
		do {
			unsigned int message_len = len > BUFSIZ ? BUFSIZ : len;
			size_t rlen = fread(message_buf.get(), sizeof(unsigned char), message_len, f);
			if(rlen != message_len)
				return -2;

			if(EVP_DigestUpdate(mdctx, message_buf.get(), message_len) != 1)
				return -3;

			if(len > BUFSIZ)
				len -= BUFSIZ;
			else
				len = 0;
		} while(len);

		if((digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
			return -4;

		if(EVP_DigestFinal_ex(mdctx, digest, &digest_len) != 1)
			return -5;
		
		return 0;
	}

	void format_digest() {
		size_t buf_size {digest_len * 2 + 1}; // Each byte consists of two characters, one is for null-terminator
		std::unique_ptr<char[]> buf {new char [buf_size]};
		char *ptr {buf.get()};

		std::memset(buf.get(), 0, buf_size);
		for(unsigned int len=0; len<digest_len; len++)
			ptr += sprintf(ptr, "%02x", digest[len]);
		digest_str = buf.get();
	}

	FILE *f;
	long f_len;
	unsigned char *digest;
	unsigned int digest_len;
	std::string digest_str;
	EVP_MD_CTX *mdctx;
	std::unique_ptr<unsigned char[]> message_buf;
};

SHA256::SHA256(const char *file)
	: mp_Impl {new impl_t {file}}
{}

SHA256::SHA256(FILE *fp)
	: mp_Impl {new impl_t {fp}}
{}

SHA256::~SHA256() {
	mp_Impl.reset();
}

std::string
SHA256::digest() const
{
	return mp_Impl->digest_str;
}

unsigned int
SHA256::digest_len() const
{
	return mp_Impl->digest_len;
}

uint8_t
SHA256::operator[](size_t data_idx)
{
	return data_idx < mp_Impl->digest_len
		? mp_Impl->digest[data_idx]
		: 0xff;
}
	
int
SHA256::write_to_file(const char *file_path)
{
	FILE *f = fopen(file_path, "w+");
	if(!f) {
		fprintf(stderr, "%s: couldn't open %s\n", __func__, file_path);
		perror(__func__);
		return -1;
	}

	fwrite(mp_Impl->digest, sizeof(unsigned char), mp_Impl->digest_len, f);
	fclose(f);

	printf("%s: Digest len %d-byte\n", __func__, mp_Impl->digest_len);
	for(unsigned int len=0; len<mp_Impl->digest_len; len++)
		fprintf(stderr, "%02x", mp_Impl->digest[len]);
	putchar('\n');

	return 0;
}

int
SHA256::write_to_filep(FILE *out_fp)
{
	if(!out_fp) {
		fprintf(stderr, "%s: couldn't open file descriptor %d\n", __func__, fileno(out_fp));
		perror(__func__);
		return -1;
	}

	fwrite(mp_Impl->digest, sizeof(unsigned char), mp_Impl->digest_len, out_fp);

	printf("%s: Digest len %d-byte\n", __func__, mp_Impl->digest_len);
	for(unsigned int len=0; len<mp_Impl->digest_len; len++)
		fprintf(stderr, "%02x", mp_Impl->digest[len]);
	putchar('\n');

	return 0;
}

int
SHA256::compare(const char *sha_file)
{
	FILE *f = fopen(sha_file, "r");
	if(!f) {
		fprintf(stderr, "%s: couldn't open %s\n", __func__, sha_file);
		perror(__func__);
		return -1;
	}

	if(fseek(f, 0L, SEEK_END) != 0)
		return -2;

	long fsize {ftell(f)};
	if(fsize == -1L) {
		perror("Get file size");
		return -3;
	}
	printf("File size: %ld\n", fsize);
	if(mp_Impl->digest_len != static_cast<unsigned int>(fsize))
		return 1;

	std::unique_ptr<unsigned char[]> buf {new unsigned char [fsize]};
	rewind(f);
	size_t r = fread(buf.get(), fsize, sizeof(unsigned char), f);
	if(r != static_cast<unsigned int>(fsize)) {
		fprintf(stderr, "%s: file size is different\n", __func__);
		return -4;
	}

	return std::memcmp(mp_Impl->digest, buf.get(), mp_Impl->digest_len);
}
	
int
SHA256::compare(const void *sha_data)
{
	return sha_data
		? memcmp(sha_data, mp_Impl->digest, mp_Impl->digest_len)
		: 1;
}

static int digest_message_file(FILE *f, unsigned char *digest, unsigned int *digest_len, EVP_MD_CTX **);

int
sha256sum(const char *in_filepath, const char *out_filepath)
{
	FILE *f = fopen(in_filepath, "r");
	if(!f) {
		fprintf(stderr, "couldn't open %s\n", in_filepath);
		perror(__func__);
		return 1;
	}

	unsigned char digest[EVP_MAX_MD_SIZE];
	unsigned int digest_len;
	EVP_MD_CTX *mdctx {nullptr};
	int r = digest_message_file(f, digest, &digest_len, &mdctx);
	if(r)
		printf("%s: calculate %s failed!", __func__, in_filepath);

	if(mdctx)
		EVP_MD_CTX_free(mdctx);

	fclose(f);
	if(r) {
		if(digest)
			OPENSSL_free(digest);
		return 2;
	}

	// Write digest to file
	f = fopen(out_filepath, "w+");
	if(!f) {
		fprintf(stderr, "%s: couldn't open %s\n", __func__, out_filepath);
		perror(__func__);
		return 3;
	}
	fwrite(digest, sizeof(unsigned char), digest_len, f);
	fclose(f);

	printf("%s: Digest len %d-byte\n", __func__, digest_len);
	for(unsigned int len=0; len<digest_len; len++)
		fprintf(stderr, "%02x", digest[len]);
	putchar('\n');

	OPENSSL_free(digest);
	return 0;
}

static int
digest_message_file(FILE *f, unsigned char *digest, unsigned int *digest_len, EVP_MD_CTX **pmdctx)
{
	EVP_MD_CTX *mdctx = *pmdctx;
	if((mdctx = EVP_MD_CTX_new()) == NULL)
		return -1;

	if(EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1)
		return -2;

	fseek(f, 0L, SEEK_END);
	long flen = ftell(f);
	printf("%s: File size %ld\n", __func__, flen);
	rewind(f);

	unsigned char message[BUFSIZ];
	do {
		unsigned int message_len = flen > BUFSIZ ? BUFSIZ : flen;
		size_t rlen = fread(message, sizeof(unsigned char), message_len, f);
		if(rlen != message_len)
			return -3;

		if(1 != EVP_DigestUpdate(mdctx, message, message_len))
			return -4;

		if(flen > BUFSIZ)
			flen -= BUFSIZ;
		else
			flen = 0;
	} while(flen);

	if(1 != EVP_DigestFinal_ex(mdctx, digest, digest_len))
		return -6;

	return 0;
}

static void
handleErrors()
{
	fprintf(stderr, "EVP error!\n");
}

static void
digest_message_file(FILE *f, unsigned char *digest, unsigned int *digest_len)
{
	fseek(f, 0L, SEEK_END);
	long flen = ftell(f);
	printf("File size: %ld\n", flen);
	rewind(f);

	unsigned char message[BUFSIZ];
	EVP_MD_CTX *mdctx;
	printf("BUFSIZ %d\n", BUFSIZ);

	if((mdctx = EVP_MD_CTX_new()) == NULL)
		handleErrors();

	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
		handleErrors();

	do {
		unsigned int message_len = flen > BUFSIZ ? BUFSIZ : flen;
		size_t rlen = fread(message, sizeof(unsigned char), message_len, f);
		if(rlen != message_len) {
			handleErrors();
			return;
		}
		if(1 != EVP_DigestUpdate(mdctx, message, message_len)) {
			handleErrors();
			EVP_MD_CTX_free(mdctx);
			return;
		}

		if(flen > BUFSIZ)
			flen -= BUFSIZ;
		else
			flen = 0;
	} while(flen);

	if(1 != EVP_DigestFinal_ex(mdctx, digest, digest_len))
		handleErrors();

	EVP_MD_CTX_free(mdctx);
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

static int
dump_dbm(GDBM_FILE dbm, FILE *fp)
{
	int r = gdbm_dump_to_file(dbm, fp, GDBM_DUMP_FMT_ASCII);
	if(r != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}

	r = gdbm_dump(dbm,               /* GDBM_FILE  */
		      "dump-ascii.dbm", /* filename   */
		      GDBM_DUMP_FMT_ASCII, /* format     */
		      GDBM_WRCREAT,        /* open_flags */
		      O_CREAT | S_IRGRP | S_IRUSR | S_IROTH); /* mode, set to read permissions */
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
sha_filep_test()
{
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

	util::SHA256 sha {fp};
	printf("setpoint.db SHA256 %s\n", sha.digest().c_str());
	FILE *out_f = fopen("SHA.txt", "w+");
	sha.write_to_filep(out_f);
	fclose(out_f);
}

int main(int argc, char **argv)
{
	unsigned char buffer[BUFSIZ];
	FILE *f;
	SHA256_CTX ctx;
	size_t len;
	if (argc < 3) {
		fprintf(stderr, "usage: %s <file> <output>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "couldn't open %s\n", argv[1]);
		return 1;
	}

	/*----------------------------------------------------
	 * OpenSSL older version
	 */
	SHA256_Init(&ctx);
	do {
		len = fread(buffer, 1, BUFSIZ, f);
		SHA256_Update(&ctx, buffer, len);
		printf("len: %ld\n", len);
	} while (len == BUFSIZ);

	SHA256_Final(buffer, &ctx);
	printf("SHA256_DIGEST_LENGTH: %d\n", SHA256_DIGEST_LENGTH);
	for (len = 0; len < SHA256_DIGEST_LENGTH; ++len)
		printf("%02x", buffer[len]);
	putchar('\n');
	fclose(f);

	/*----------------------------------------------------
	 * OpenSSL 3.0
	 */
	printf("\n----\nOpenSSL 3.0\n");
	util::SHA256 sha256 {argv[1]};
	printf("Digest len: %d\n", sha256.digest_len());
	printf("%s\n", sha256.digest().c_str());
	unsigned int l {sha256.digest_len()};
	for(unsigned int i=0; i<l; i++)
		printf("%02x", sha256[i]);
	putchar('\n');

	if(sha256.compare(buffer) != 0)
		printf("Error, two digests are not equal\n");
	else
		printf("OK, two digests are the same\n");

	/*----------------------------------------------------
	 * Write digest to file
	 */
	sha256.write_to_file(argv[2]);

	/*----------------------------------------------------
	 * FILE test
	 */
	sha_filep_test();

	/*----------------------------------------------------
	 * Test copy test
	 */
	// util::SHA256 sha256_2 = sha256;
	return 0;
}

#if 0
static void
handleErrors()
{
	fprintf(stderr, "EVP Error!\n");
}

/* 
 * Reference:
 * https://wiki.openssl.org/index.php/EVP_Message_Digests
 */
static void
digest_message(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len)
{
	EVP_MD_CTX *mdctx;

	if((mdctx = EVP_MD_CTX_new()) == NULL)
		handleErrors();

	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
		handleErrors();

	if(1 != EVP_DigestUpdate(mdctx, message, message_len))
		handleErrors();

	if((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
		handleErrors();

	if(1 != EVP_DigestFinal_ex(mdctx, *digest, digest_len))
		handleErrors();

	EVP_MD_CTX_free(mdctx);
}
#endif
