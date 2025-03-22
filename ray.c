#include "raylib.h"
#include "utils.h"
#include <math.h>
#include <time.h>

#define MAX_VALUES 10
#define GRAPH_WIDTH 600
#define GRAPH_HEIGHT 400
#define GRAPH_X 100
#define GRAPH_Y 100

int values[MAX_VALUES];
char timeLabels[MAX_VALUES][9];
int valueCount = 0;
float animationProgress = 1.0f;
int animatingIndex = -1;

void getCurrentTime(char *buffer, int len);
void AddValue(int newValue);
void DrawGraph(float lineThickness);
char *startListeningAndPrintMessagesFromTheServer(int socketFD);

int main(void)
{
    InitWindow(800, 600, "Raylib Graph");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    float lineThickness = 5;

    int socketFD = CreateIPV4Socket();
    struct sockaddr_in *address = CreateIPV4Address("127.0.0.1", 2000);
    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
    if (result == 0)
        printf("successful conection");

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        const char *buffer = startListeningAndPrintMessagesFromTheServer(socketFD);
        char *endptr;
        errno = 0;
        long value = strtol(buffer, &endptr, 10);

        AddValue(value);

        if (animationProgress < 1.0f)
        {
            animationProgress += 0.05f; // Control animation speed
            if (animationProgress > 1.0f)
                animationProgress = 1.0f;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawGraph(lineThickness);
        EndDrawing();
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

void getCurrentTime(char *buffer, int len)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buffer, len, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
}

void AddValue(int newValue)
{
    if (valueCount < MAX_VALUES)
    {
        values[valueCount] = newValue;
        getCurrentTime(timeLabels[valueCount], sizeof(timeLabels[valueCount]));
        valueCount++;
    }
    else
    {
        for (int i = 1; i < MAX_VALUES; i++)
        {
            values[i - 1] = values[i];
            snprintf(timeLabels[i - 1], sizeof(timeLabels[i - 1]), "%s", timeLabels[i]);
        }
        values[MAX_VALUES - 1] = newValue;
        getCurrentTime(timeLabels[MAX_VALUES - 1], sizeof(timeLabels[MAX_VALUES - 1]));
    }
    animatingIndex = valueCount - 1;
    animationProgress = 0.0f; // Reset animation progress
}

void DrawGraph(float lineThickness)
{
    // Draw grid lines
    Color gridColor = (Color){50, 50, 50, 255}; // Dark gray for grid lines

    // Draw horizontal grid lines
    for (int i = 0; i <= 5; i++)
    {
        int y = GRAPH_Y + GRAPH_HEIGHT - (i * (GRAPH_HEIGHT / 5));
        DrawLine(GRAPH_X, y, GRAPH_X + GRAPH_WIDTH, y, gridColor);
    }

    // Draw vertical grid lines
    for (int i = 0; i <= MAX_VALUES - 1; i++)
    {
        int x = GRAPH_X + i * (GRAPH_WIDTH / (MAX_VALUES - 1));
        DrawLine(x, GRAPH_Y, x, GRAPH_Y + GRAPH_HEIGHT, gridColor);
    }

    // Draw graph border
    DrawRectangleLines(GRAPH_X, GRAPH_Y, GRAPH_WIDTH, GRAPH_HEIGHT, WHITE);

    // Draw Y-axis labels
    for (int i = 0; i <= 5; i++)
    {
        int y = GRAPH_Y + GRAPH_HEIGHT - (i * (GRAPH_HEIGHT / 5));
        char label[10];
        sprintf(label, "%d", i * 20);
        DrawText(label, GRAPH_X - 30, y - 5, 15, WHITE);
    }

    // Draw X-axis labels (timestamps)
    for (int i = 0; i < valueCount; i++)
    {
        int x = GRAPH_X + i * (GRAPH_WIDTH / (MAX_VALUES - 1));
        DrawText(timeLabels[i], x - 10, GRAPH_Y + GRAPH_HEIGHT + 5, 15, WHITE);
    }

    if (valueCount > 1)
    {
        for (int i = 1; i < valueCount; i++)
        {
            int x1 = GRAPH_X + (i - 1) * (GRAPH_WIDTH / (MAX_VALUES - 1));
            int y1 = GRAPH_Y + GRAPH_HEIGHT - (values[i - 1] * GRAPH_HEIGHT / 100);
            int x2 = GRAPH_X + i * (GRAPH_WIDTH / (MAX_VALUES - 1));
            int y2 = GRAPH_Y + GRAPH_HEIGHT - (values[i] * GRAPH_HEIGHT / 100);
            if (i == animatingIndex)
            {
                int animatedY2 = y1 + (y2 - y1) * animationProgress; // Animate only the latest update
                DrawLineEx((Vector2){x1, y1}, (Vector2){x2, animatedY2}, lineThickness, RED);
            }
            else
            {
                DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, lineThickness, RED);
            }
        }
    }
}