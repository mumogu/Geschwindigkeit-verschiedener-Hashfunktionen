#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <openssl/sha.h>
#include <openssl/md5.h>

// Apple has deprecated the OpenSSL routines because they want you to use their "Common Data Security
// Architecture" but since OpenSSL isn't going to go away anytime soon, this disables the deprecated-warnings.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void usage() {
	printf("Usage:\n");
	printf("\thashspeed <filename> <steps> <chunksize in MB>\n\n");
	printf("\tBenchmarks various different hashing algorithms and\n");
	printf("\twirtes the results to a csv file.\n");
}

void fill_with_random_data(unsigned char* buffer, size_t length) {
	FILE *devRand = fopen("/dev/urandom", "r");
	if(devRand == NULL) {
		printf("Error opening /dev/urandom. Aborting...");
		exit(EXIT_FAILURE);
	}
		
	size_t ret = fread(buffer, sizeof(unsigned char), length, devRand);
	if(ret != length) {
		printf("Error reading from /dev/urandom. Aborting...");
		exit(EXIT_FAILURE);
	}

	fclose(devRand);
}

int main(int argc, char * const argv[]) {		
	if(argc != 4) {
		usage();
		exit(EXIT_SUCCESS);
	}

	int steps = atoi(argv[2]);
	unsigned int chunk_size = atoi(argv[3]) * 1024 * 1024;
	
	// First, we need a chunk of random data
	unsigned char* random_data = (unsigned char *) malloc(chunk_size);
	if(random_data == NULL) {
		printf("Error allocating buffer. Aborting...");
		exit(EXIT_FAILURE);
	}
	fill_with_random_data(random_data, chunk_size);

	// Build the test data (i times the chunk from above)
	unsigned char* test_data = (unsigned char *) malloc(steps * chunk_size);
	if(test_data == NULL) {
		printf("Error allocation test_data. Aborting...");
		exit(EXIT_FAILURE);
	}

	for(int i=0; i<steps; i++) {
		memcpy(test_data + i * chunk_size, random_data, chunk_size);	
	}
	
	free(random_data);

	// Open the csv file for recording runtimes
	FILE *record = fopen(argv[1], "w");
	if(record == NULL) {
		printf("Error opening results file. Aborting...");
		exit(EXIT_FAILURE);
	}

	printf("step\tsize (MB)\tMD5\tSHA1\tSHA224\tSHA256\tSHA384\tSHA512\n");
	
	size_t ret = fprintf(record, "step;size in mb;MD5;SHA1;SHA224;SHA256;SHA384;SHA512\n");
	if(ret < 0) {
		printf("Error writing results to file. Aborting...");
		exit(EXIT_FAILURE);
	}
	
	// Buffer for the hash function to write to. This is never read.
	// 64Byte is the longest result expected from one of the hash funcitons.
	unsigned char sha_hash[64];

	// Initialize the timing
	clock_t start_c, end_c;

	// The test loop
	for(int i=1; i<=steps; i++){		
		// Measure MD5
		start_c = clock();
		MD5(test_data, i * chunk_size, sha_hash);
		end_c = clock();
		int md5_time = end_c - start_c;

		// Measure SHA1
		start_c = clock();
		SHA1(test_data, i * chunk_size, sha_hash);
		end_c = clock();
		int sha1_time = end_c - start_c;

		// Measure SHA224
		start_c = clock();
		SHA224(test_data, i * chunk_size, sha_hash);
		end_c = clock();
		int sha224_time = end_c - start_c;

		// Measure SHA256
		start_c = clock();
		SHA256(test_data, i * chunk_size, sha_hash);
		end_c = clock();
		int sha256_time = end_c - start_c;

		// Measure SHA384
		start_c = clock();
		SHA384(test_data, i * chunk_size, sha_hash);
		end_c = clock();
		int sha384_time = end_c - start_c;

		// Measure SHA512
		start_c = clock();
		SHA512(test_data, i * chunk_size, sha_hash);
		end_c = clock();
		int sha512_time = end_c - start_c;
		
		printf("%d \t%d \t%d \t%d \t%d \t%d \t%d \t%d\n", 
			i, i*chunk_size/1024/1024, md5_time, sha1_time, 
			sha224_time, sha256_time, sha384_time, sha512_time);
		fprintf(record, "%d;%d;%d;%d;%d;%d;%d;%d\n", i, i*chunk_size/1024/1024, md5_time, sha1_time, 
			sha224_time, sha256_time, sha384_time, sha512_time);
		fflush(record);

		
	}
	fclose(record);

	return 0;
}
