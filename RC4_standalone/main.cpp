#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "RC4.h"

using namespace std;

static void help(char *progName){
	cout << "*This is a standalone encryption/decrpytion program that uses RC4 encryption\n";
	cout << "*Usage: " << endl
	<< "*"<< progName << " [file_name -- so far tested .jpg, .txt]" << endl << endl;
}

int main(int argc, char* argv[]) {
	help(argv[0]);
	char *file = "starry_sky.jpg";	//default value
	
	if(argc != 2){
		cerr << "*Incorrect usage!" << endl;
		return EXIT_FAILURE;
	}
	file = argv[1];
	int fd = open(file, O_RDWR);
	cout << "*File opened..." << endl;
	

	if(transform_RC4(fd)){
		cout << "*Encryption/Decrption of " << file << " successful" << endl;	
	}
	else{
		cout << "Something went wrong encrypting!" << endl << endl;
	}
	//transform_RC4(fd);
	
	close(fd);
	return 0;
}
