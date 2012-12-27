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
	printf("\twirtes the times to a csv file.\n");
}

int main(int argc, char * const argv[]) {		
	if(argc != 4) {
		usage();
		exit(0);
	}

	int steps = atoi(argv[2]);
	int chunk_size = atoi(argv[3]) * 1024 * 1024;

	// First, we need a chunk of random data
	FILE *devRand = fopen("/dev/urandom", "r");
	if(devRand == NULL) {
		printf("Error reading from /dev/urandom. Aborting...");
		exit(-1);
	}
	unsigned char* random_data = (unsigned char *) malloc(chunk_size);
	if(random_data == NULL) {
		printf("Error allocating buffer. Aborting...");
		exit(-1);
	}
	fread(random_data, sizeof(unsigned char), chunk_size, devRand);
	fclose(devRand);

	// Open the csv file for recording times
	FILE *record = fopen(argv[1], "w");
	if(record == NULL) {
		printf("Error opening results file. Aborting...");
		exit(-1);
	}
	printf("step\tsize (MB)\tMD5\tSHA1\tSHA224\tSHA256\tSHA384\tSHA512\n");
	fprintf(record, "step;size in mb;MD5;SHA1;SHA224;SHA256;SHA384;SHA512\n");
	
	// Build the test data (i times the chunk from above)
	unsigned char* test_data = (unsigned char *) malloc(steps * chunk_size);
	for(int i=0; i<steps; i++) {
		memcpy(test_data + i * chunk_size, random_data, chunk_size);	
	}

	// Buffer for the hash function to write to. This is never read.
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
