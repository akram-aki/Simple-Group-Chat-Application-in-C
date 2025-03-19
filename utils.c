#include "utils.h" // Include the header file

int CreateIPV4Socket()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized successfully!\n");

    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in *CreateIPV4Address(char *IP, int port)
{
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    if (!address)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    if (strlen(IP) == 0)
        address->sin_addr.S_un.S_addr = INADDR_ANY;

    else
        inet_pton(AF_INET, IP, &address->sin_addr);

    return address;
}
struct acceptedSocket *foo(int *serverSocketFD)
{
    // 🔹 Allocate memory for client address
    struct acceptedSocket *clientSocket = malloc(sizeof(struct acceptedSocket));
    struct sockaddr_in clientAddress;
    int size = sizeof(clientAddress);

    // Accept connection
    int clientSocketFD = accept(*serverSocketFD, (struct sockaddr *)&clientAddress, &size);
    if (clientSocketFD == INVALID_SOCKET)
    {
        printf("Accept failed: %d\n", WSAGetLastError());
        clientSocket->error = WSAGetLastError();
        return NULL;
    }
    printf("Client connected!\n");

    clientSocket->clientSocketFD = clientSocketFD;
    clientSocket->acceptedSuccessfully = clientSocketFD > 0;
    clientSocket->address = clientAddress;
    return clientSocket;
}
