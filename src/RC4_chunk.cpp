#include <openssl/rc4.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
//#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>

#include "RC4.h"

#define BUFFER_SIZE 5120000

int transform_RC4(int fd, char *param) {
	RC4_KEY key;	//for RC4 encryption
	off_t offset = lseek(fd, 0, SEEK_END);
	const char *MAGIC = "Salted__";

	int salt_nosalt;	//1 value means salt; 0 value means nosalt -- salt by default
	if(param == NULL){
		salt_nosalt = 1;
	}
	else{
		strcmp((const char*)param, "nosalt") == 0 ? salt_nosalt = 0 : salt_nosalt = 1;
	}



	if (offset < 0) {
		perror("*Cannot obtain file offset");
		exit(EXIT_FAILURE);
	}

	lseek(fd, 0, SEEK_SET);
	printf("*File size: %d bytes\n", offset);

	//FILE *out = fopen("cipherText", "w");
	FILE *fp = fdopen(fd, "r+");
	FILE *temp_fp = tmpfile();	//temp file storage that stores output_buffer
	char input_buffer[BUFFER_SIZE];
	char *output_buffer = (char *)malloc(BUFFER_SIZE);

	if (temp_fp == NULL){
		perror("*Could not open TEMP filestream\n");
		exit(EXIT_FAILURE);
	}

	//store the first 16 bytes of input file into is_salted to check later
	char is_salted[16];
	fread(is_salted, 1, 16, fp);
	//printf("is_salted: %s\n", is_salted);
	rewind(fp);	//need to rewind offset back to start of file

	//rc4 key Encryption
	const EVP_CIPHER *cipher;	//rc4
	const EVP_MD *dgst;	//sha256
	unsigned char salt[8];
	unsigned char tkey[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
	cipher = EVP_get_cipherbyname("rc4");
	dgst = EVP_get_digestbyname("sha256");
	unsigned char magic_salt[16];	//holds MAGIC + salt


	//if is_salted contains MAGIC then use the salt from the file
	if(strstr(is_salted, MAGIC)){
		printf("\n*is_salted value found..\n");
		for (size_t i = 8; i < 16; i++){
			salt[i-8] = is_salted[i];
		}
		//uncomment for debugging purposes
		// printf("*salt: ");
		// for(int i = 0; i < 8; i++){
		// 	printf("%x ", salt[i]);
		// }
		//printf("\nExtracted salt: %s\n", salt);
		if(fread(magic_salt, 1, 16, fp) < 0){
			perror("Error extracting magic_salt from encrpyted file\n");
		}
	}
	else if(salt_nosalt != 0){
		//SALT handling
		printf("*Generating salt\n");
		if(RAND_bytes(salt, sizeof(salt)) <= 0){
			perror("Error adding some salt. Continuing encryption with no salt\n");
		}
		printf("*Out of salt\n");
		//uncomment for debugging purposes
		// printf("*salt: ");
		// for(int i = 0; i < 8; i++){
		// 	printf("%x ", salt[i]);
		// }
		//END SALT handling
		strcpy((char*)magic_salt, (char*)MAGIC);
		strcat((char*)magic_salt, (char*)salt);
		//printf("\nMagic Salt: %s\n", magic_salt);
		fwrite(magic_salt, 1, 16, temp_fp);
		printf("*Wrote magic_salt to temp_fp\n");

		//uncomment for debugging purposes
		// rewind(temp_fp);
		// char tempBuffer[16];
		// fread(tempBuffer, 1, 16, temp_fp);
		// printf("Temp_fp: %s", tempBuffer);
	}

	printf("\n*Entering RC4 Enc\n");
	if(salt_nosalt != 0){
		if(!EVP_BytesToKey(cipher, dgst, salt,
		 (unsigned char *)RC4_HASH_KEY, RC4_KEY_LEN,1,tkey, iv)){
			 perror("EVP_BytesToKey failed\n");
			 exit(EXIT_FAILURE);
		}
	}
	else{
		if(!EVP_BytesToKey(cipher, dgst, NULL,
		 (unsigned char *)RC4_HASH_KEY, RC4_KEY_LEN,1,tkey, iv)){
			 perror("EVP_BytesToKey failed\n");
			 exit(EXIT_FAILURE);
		}
	}
	printf("*Exiting RC4 Enc\n");
	//end rc4 key Encryption
	RC4_set_key(&key, 16, (const unsigned char *)tkey);

	int ret;	//for number of bytes read in reading input file
	std::vector<int> ret_values;	//stores all the number of bytes read in during each iteration
	if(!strstr(is_salted, MAGIC) && salt_nosalt != 0) ret_values.push_back(16); //to compensate the salt that was written


	while((ret = fread(input_buffer, 1, sizeof(input_buffer), fp)) > 0){
		printf("*Reading chunk\n");
		//printf("Input: %s\n", input_buffer);

		ret_values.push_back(ret);
		printf("*Ret: %d\n",ret);
		RC4(&key, ret, (unsigned char *)input_buffer, (unsigned char*)output_buffer);
		printf("*Writing chunk\n");
		if (fwrite(output_buffer, 1, ret, temp_fp) < 0) {
			perror("*Could not write file");
			exit(EXIT_FAILURE);
		}
	}
	//uncomment for debugging purposes
	// rewind(temp_fp);
	// char tempBuffer[50];
	// printf("Before reading temp_fp\n");
	// fread(tempBuffer, 1, 50, temp_fp);
	// printf("\nTemp_fp: %s", tempBuffer);


	//rewind back to the beginning of each file
	rewind(temp_fp);
	rewind(fp);

	//if salted input file, there is the problem of it being 16bytes larger than original input file
	//so we need to close and completely overwrite
	if(strstr(is_salted, MAGIC)){
		fp = freopen(NULL, "w", fp);
	}	//want to write to only

	//moving contents of temp_fp to fp
	printf("*Writing from temp_fp to fp...\n");
	int count = 1;
	std::vector<int>::const_iterator itr = ret_values.cbegin();
	for(itr; itr != ret_values.cend(); itr++){
		fread(output_buffer, 1, *itr, temp_fp);
		printf("*Writing chunk %d to fp\n", count++);
		//uncomment for debugging purposes
		//printf("output_buffer: %s\n", output_buffer);
		fwrite(output_buffer, 1, *itr, fp);
	}
	printf("*Finished writing to fp\n");

	fclose(fp);
	free(output_buffer);
	return 1;
};
