
#include "raylib.h"
#include "utils.h"

char *startListeningAndPrintMessagesFromTheServer(int socketFD);

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    int socketFD = CreateIPV4Socket();
    struct sockaddr_in *address = CreateIPV4Address("127.0.0.1", 2000);
    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
    if (result == 0)
        printf("successful conection");

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        const char *buffer = startListeningAndPrintMessagesFromTheServer(socketFD);
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(buffer, 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        free((void *)buffer);
    }

    CloseWindow();

    return 0;
}
char *startListeningAndPrintMessagesFromTheServer(int socketFD)
{
    char *buffer = malloc(1024); // Allocate memory dynamically
    if (!buffer)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    int bytesReceived = recv(socketFD, buffer, 1023, 0);
    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0'; // Null-terminate received data
        return buffer;
    }
    else if (bytesReceived == 0)
    {
        printf("Connection closed by server\n");
    }
    else
    {
        printf("recv failed: %d\n", WSAGetLastError());
    }

    free(buffer); // Free memory before returning NULL
    return NULL;
}