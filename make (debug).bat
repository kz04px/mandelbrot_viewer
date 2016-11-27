@echo off
gcc src\main.c src\callbacks.c src\io.c src\matrix.c src\shaders.c -g -gdwarf-2 -Wall -Wextra -lglfw3 -lglew32 -lgdi32 -lopengl32 -o "bin\main(debug).exe"
PAUSE