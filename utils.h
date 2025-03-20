#ifndef UTILS_H // Include guard to prevent multiple inclusion
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>

struct acceptedSocket
{
    int clientSocketFD;
    struct sockaddr_in address;
    int error;
    BOOL acceptedSuccessfully;
};

struct acceptedSocket *foo(int *serverSocketFD);
int CreateIPV4Socket();
struct sockaddr_in *CreateIPV4Address(char *IP, int port);
#endif // UTIL_H
