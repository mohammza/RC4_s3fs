#ifndef RC4_H
#define RC4_H

#define RC4_HASH_KEY "password"
#define RC4_KEY_LEN (sizeof(RC4_HASH_KEY) - 1)

int transform_RC4(int fd);
#endif
