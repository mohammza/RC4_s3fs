# s3fs-secure

s3fs-secure takes the existing s3fs-fuse and implements an additional layer of encryption using RC4 cipher

## Pre-requisites

Before installing this s3fs-secure ensure that you run 

```
sudo apt-get update
```
Additionally, it is assumed that the user has an AWS IAM user set up with S3 read/write access granted.

So that all the necessary software are updated for installation of this software.

## s3fs-secure Installation

To install s3fs-secure simply pull the repo from github

```
git pull https://github.com/mohammza/s3fs-secure.git
cd s3fs-secure
./configure
make 
sudo make install
```

## Usage

s3fs supports the standard
[AWS credentials file](https://docs.aws.amazon.com/cli/latest/userguide/cli-config-files.html)
stored in `${HOME}/.aws/credentials`.  Alternatively, s3fs supports a custom passwd file.

The default location for the s3fs password file can be created:

* using a .passwd-s3fs file in the users home directory (i.e. ${HOME}/.passwd-s3fs)
* using the system-wide /etc/passwd-s3fs file

Enter your credentials in a file `${HOME}/.passwd-s3fs` and set
owner-only permissions:

```
echo ACCESS_KEY_ID:SECRET_ACCESS_KEY > ${HOME}/.passwd-s3fs
chmod 600 ${HOME}/.passwd-s3fs
```

Run s3fs with an existing bucket `mybucket` and directory `/path/to/mountpoint`:

```
s3fs mybucket /path/to/mountpoint -o passwd_file=${HOME}/.passwd-s3fs
```

If you encounter any errors, enable debug output:

```
s3fs mybucket /path/to/mountpoint -o passwd_file=${HOME}/.passwd-s3fs -o dbglevel=info -f -o curldbg
```

Note: You may also want to create the global credential file first

```
echo ACCESS_KEY_ID:SECRET_ACCESS_KEY > /etc/passwd-s3fs
chmod 600 /etc/passwd-s3fs
```

To unmount the bucket, run

```
sudo umount /path/to/mountpoint
```

The encryption password is stored in the src file and by default set to password. To change the encryption password simply open up the text file and add a desired password.


## Limitations

As of now this can has been tested with file sizes of up to 10mb, anything exceeding this file size can result in the application not running as intended.

## Frequently Asked Questions about s3fs-fuse

* [FAQ wiki page](https://github.com/s3fs-fuse/s3fs-fuse/wiki/FAQ)
* [s3fs on Stack Overflow](https://stackoverflow.com/questions/tagged/s3fs)
* [s3fs on Server Fault](https://serverfault.com/questions/tagged/s3fs)



## RC4_standalone compilation

To compile, go to the rc4_standalone folder and run the following

```
export LD_LIBRARY_PATH=path_to_openssl:$LD_LIBRARY_PATH
g++ main.cpp RC4.cpp -Lpath_to_openssl -lssl -lcrypto -o rc4
```

## Limitations

This standalone implementation can encrypt files up to 100 mb with efficient speed. This includes .txt and .jpg files.

## License

Copyright (C) 2010 Randy Rizun <rrizun@gmail.com>

Licensed under the GNU GPL version 2



