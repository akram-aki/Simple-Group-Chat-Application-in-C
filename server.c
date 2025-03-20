#include "utils.h"

void sendRecievedMsgToOtherClients(int clientSocketFD, char buffer[], size_t len);

void *recieveIncommingMessages(void *arg);
void startAcceptingIncommingConnections(int serverSocketFD);
void receiveAndPrintDataOnSeparateThreads(struct acceptedSocket *clientSocket);

struct acceptedSocket acceptedClients[10];
int numberOfClients = 0;

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
    free(serverAddress);
    closesocket(serverSocketFD);
    WSACleanup();
    return 0;
}

void startAcceptingIncommingConnections(int serverSocketFD)
{
    while (TRUE)
    {
        struct acceptedSocket *clientSocket = foo(&serverSocketFD); // Pass address of local copy
        if (clientSocket == NULL)
        {
            printf("Failed to accept client.\n");
            continue;
        }
        if (numberOfClients < 10)
        {
            acceptedClients[numberOfClients] = *clientSocket;
            numberOfClients++;
        }
        receiveAndPrintDataOnSeparateThreads(clientSocket);
    }
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
        free(arg);
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
            printf("%d :%s \n", clientSocket->clientSocketFD, buffer);

            size_t len = strlen(buffer);
            sendRecievedMsgToOtherClients(clientSocket->clientSocketFD, buffer, len);
        }
        else if (bytesReceived == 0)
        {
            printf("%d disconnected.\n", clientSocket->clientSocketFD);
            break;
        }
        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }
    }

    printf("%d disconnected.\n", clientSocket->clientSocketFD);
    closesocket(clientSocket->clientSocketFD);
    free(arg);
}

void sendRecievedMsgToOtherClients(int clientSocketFD, char buffer[], size_t len)
{
    for (int i = 0; i < numberOfClients; i++)
    {
        if (acceptedClients[i].clientSocketFD != clientSocketFD)
        {
            char msg[32];
            if (len < 1024 - 1) // Ensure there's space for '\n' and '\0'
            {
                buffer[len] = '\n';
                buffer[len + 1] = '\0'; // Null-terminate the string
            }
            sprintf(msg, "%d", clientSocketFD);
            send(acceptedClients[i].clientSocketFD, msg, strlen(msg), 0);
            send(acceptedClients[i].clientSocketFD, buffer, strlen(buffer), 0);
        }
    }
};