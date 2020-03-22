#include <openssl/rc4.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
 #include <openssl/md5.h>

#include "RC4.h"

int transform_RC4(int fd) {
	RC4_KEY key;
	off_t offset = lseek(fd, 0, SEEK_END);
	
	//MD5 encryption
	unsigned char enc_key[MD5_DIGEST_LENGTH];
	char in[sizeof(RC4_HASH_KEY)];
	strcpy(in, RC4_HASH_KEY);
	MD5((unsigned char*)&in, RC4_KEY_LEN, (unsigned char*)&enc_key);
	
	printf("MD5 Encryption Key: ");
	for (int i = 0; i < 16; i++){ // loop for debugging purposes
		printf("%02x", (unsigned int)enc_key[i]);
	}
	printf("\n\n");
	
	//end MD5 encryption
	if (offset < 0) {
		perror("Cannot obtain file offset");
		exit(EXIT_FAILURE);
	}

	lseek(fd, 0, SEEK_SET);

	unsigned char input_buffer[offset];
	unsigned char *output_buffer = (unsigned char*)malloc(offset+1);

	if (pread(fd, &input_buffer, offset, 0) == -1) {
		perror("Could not read file");
		exit(EXIT_FAILURE);
	}
	
	RC4_set_key(&key, sizeof(enc_key), (const unsigned char *)enc_key);
	RC4(&key, offset, input_buffer, output_buffer);
	if (pwrite(fd, output_buffer, offset, 0) == -1) {
		perror("Could not write file");
		exit(EXIT_FAILURE);
	}

	free(output_buffer);
	return 1;
};

