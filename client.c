#include "utils.h"

void *startListeningAndPrintMessagesFromTheServer(void *socketFD);
int main()
{
    pthread_t id;
    int socketFD = CreateIPV4Socket();
    struct sockaddr_in *address = CreateIPV4Address("127.0.0.1", 2000);
    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
    if (result == 0)
        printf("successful conection");
    printf("\nyour message:\n");
    pthread_create(&id, NULL, startListeningAndPrintMessagesFromTheServer, (void *)&socketFD);
    while (1)
    {
        char message[1024];
        fgets(message, 1024, stdin);
        if (strcmp(message, "exit\n") == 0)
        {
            break;
        }
        int bytesSent = send(socketFD, message, strlen(message), 0);
        if (bytesSent == SOCKET_ERROR)
            printf("Send failed: %d\n", WSAGetLastError());
    }
    pthread_join(id, NULL);
    Sleep(1000); // Wait 1 second
    closesocket(socketFD);
    WSACleanup();
    return 0;
}

void *startListeningAndPrintMessagesFromTheServer(void *arg)
{
    int socketFD = *(int *)arg; // Correctly cast and dereference

    char buffer[1024];
    while (1)
    {
        int bytesReceived = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0'; // Null-terminate received data
            printf("%s", buffer);
        }
        else if (bytesReceived == 0)
        {
            printf("failed to get msg from server");
            break;
        }
        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }
    }
}