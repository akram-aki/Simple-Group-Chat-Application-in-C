#include "utils.h"
#include <pthread.h>

void *accpetIncommingConnection(void *serverSocketFD);
void *recieveIncommingMessages(void *arg);
void startAcceptingIncommingConnections(int serverSocketFD);
void receiveAndPrintDataOnSeparateThreads(struct acceptedSocket *clientSocket);

int main()
{
    int serverSocketFD = CreateIPV4Socket();
    struct sockaddr_in *serverAddress = CreateIPV4Address("", 2000);

    if (bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress)) == 0)
        printf("Socket was bound successfully\n");

    if (listen(serverSocketFD, 10) == 0)
        printf("Listening...\n");

    startAcceptingIncommingConnections(serverSocketFD);

    // Cleanup
    closesocket(serverSocketFD);
    WSACleanup();
    return 0;
}

void startAcceptingIncommingConnections(int serverSocketFD)
{
    int *serverFDPtr = malloc(sizeof(int));
    if (serverFDPtr == NULL)
    {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    *serverFDPtr = serverSocketFD;

    pthread_t id;
    pthread_create(&id, NULL, accpetIncommingConnection, (void *)serverFDPtr);
    pthread_detach(id); // Avoid memory leaks
}

void *accpetIncommingConnection(void *arg)
{
    int serverSocketFD = *(int *)arg; // Copy the value
    free(arg);                        // Free memory after copying
    while (TRUE)
    {
        struct acceptedSocket *clientSocket = foo(&serverSocketFD); // Pass address of local copy
        if (clientSocket == NULL)
        {
            printf("Failed to accept client.\n");
            continue;
        }
        receiveAndPrintDataOnSeparateThreads(clientSocket);
    }
    return NULL;
}

void receiveAndPrintDataOnSeparateThreads(struct acceptedSocket *clientSocket)
{
    pthread_t id;
    struct acceptedSocket *ptr = malloc(sizeof(struct acceptedSocket));
    *ptr = *clientSocket;
    if (pthread_create(&id, NULL, recieveIncommingMessages, (void *)ptr) != 0)
    {
        perror("Failed to create thread");
        free(ptr); // Free on failure
    }
    else
    {
        pthread_detach(id); // Detach to avoid memory leaks
    }
}

void *recieveIncommingMessages(void *arg)
{
    struct acceptedSocket *clientSocket = (struct acceptedSocket *)arg;
    if (clientSocket == NULL || clientSocket->clientSocketFD < 0)
    {
        free(clientSocket);
        return NULL;
    }

    char buffer[1024];
    while (1)
    {
        // Receive data
        int bytesReceived = recv(clientSocket->clientSocketFD, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0'; // Null-terminate received data
            printf("The response was:\n%s", buffer);
        }
        else if (bytesReceived == 0)
        {
            printf("Client disconnected.\n");
            free(clientSocket);
            break;
        }
        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
            free(clientSocket);
            break;
        }
    }
    closesocket(clientSocket->clientSocketFD);
    free(clientSocket);
}