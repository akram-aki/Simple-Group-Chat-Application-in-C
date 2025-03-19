#include "utils.h"

int main()
{

    int socketFD = CreateIPV4Socket();
    struct sockaddr_in *address = CreateIPV4Address("127.0.0.1", 2000);
    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
    if (result == 0)
        printf("successful conection");
    printf("\nyour message:\n");
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
        else
            printf("\nsuccessfully sent data \n");
    }
    Sleep(1000); // Wait 1 second
    closesocket(socketFD);
    WSACleanup();
    return 0;
}
