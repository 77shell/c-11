
// compile with: gcc -o sha256 sha256.c -lcrypto

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <cstdio>
#include <cstring>

static void digest_message(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len);
static void digest_message_file(FILE *f, unsigned char **digest, unsigned int *digest_len);

struct SHA256 {
	SHA256(const char *file)
		: f {nullptr}, f_len {0}, digest {nullptr}, digest_len {0}, mdctx {nullptr}, message_buf {nullptr}
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

			message_buf = malloc(BUFSIZ);
			if(!message_buf) {
				perror("Locate message buffer");
				return;
			}

			digest_message();
		}

	~SHA256() {
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
			size_t rlen = fread(message_buf, sizeof(unsigned char), message_len, f);
			if(rlen != message_len)
				return -2;

			if(EVP_DigestUpdate(mdctx, message_buf, message_len) != 1)
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

	FILE *f;
	long f_len;
	unsigned char *digest;
	unsigned int digest_len;
	EVP_MD_CTX *mdctx;
	void *message_buf;
};

static void
handleErrors()
{
	fprintf(stderr, "EVP error!\n");
}

static void
digest_message_file(FILE *f, unsigned char **digest, unsigned int *digest_len)
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

	if((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
		handleErrors();

	if(1 != EVP_DigestFinal_ex(mdctx, *digest, digest_len))
		handleErrors();

	EVP_MD_CTX_free(mdctx);
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
	printf("OpenSSL 3.0\n");
	struct SHA256 sha256 {argv[1]};
	printf("Digest len: %d\n", sha256.digest_len);
	for(len = 0; len < sha256.digest_len; len++)
		printf("%02x", sha256.digest[len]);
	putchar('\n');

	if(memcmp(buffer, sha256.digest, sha256.digest_len) != 0)
		printf("Error, two digests are not equal\n");
	else
		printf("OK, two digests are the same\n");


	/*----------------------------------------------------
	 * Write digest to file
	 */
	const char *out_filepath = argv[2];
	f = fopen(out_filepath, "w+");
	if(!f) {
		fprintf(stderr, "%s: couldn't open %s\n", __func__, out_filepath);
		perror(__func__);
		return 3;
	}
	fwrite(sha256.digest, sizeof(unsigned char), sha256.digest_len, f);
	fclose(f);
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

int
authenticate_file_sha256(const char *file, const char *digest_file)
{
	return 0;
}
