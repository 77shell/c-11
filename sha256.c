
// compile with: gcc -o sha256 sha256.c -lcrypto

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>

static void digest_message(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len);
static void digest_message_file(FILE *f, unsigned char **digest, unsigned int *digest_len);

int main(int argc, char **argv)
{
	unsigned char buffer[BUFSIZ];
	FILE *f;
	SHA256_CTX ctx;
	size_t len;
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
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

	/*----------------------------------------------------
	 * OpenSSL 3.0
	 */
	printf("OpenSSL 3.0\n");
	static const size_t digest_bufsiz = 128;
	unsigned char *digest = malloc(digest_bufsiz);
	unsigned int digest_len;
	digest_message_file(f, &digest, &digest_len);
	printf("Digest len: %d\n", digest_len);
	for(len = 0; len < digest_len; ++len)
		printf("%02x", digest[len]);
	putchar('\n');
	fclose(f);

	if(strncmp(buffer, digest, digest_len) != 0)
		printf("Error, two digests are not equal\n");
	else
		printf("OK, two digests are the same\n");

	free(digest);
	return 0;
}

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
