# Simple-Group-Chat-Application-in-C

compile clientGui : gcc ray.c -o ray.exe -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32
compile server : gcc server.c utils.c -o server.exe -lws2_32
compile client : gcc client.c utils.c -o client.exe -lws2_32
