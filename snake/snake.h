#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#define MAP_HEIGHT 20
#define MAP_WIDTH 40
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

typedef struct {
    int x;
    int y;
} Food, Snakenode;

typedef struct {
    Snakenode snakeNode[1000];
    int length;
    int speed;
} Snake;

void GotoXY(int x, int y);  // Move the cursor to the specified coordinates
void Hide();                // Hide the cursor
int Menu();                 // Display the menu and return the user's choice
void Help();                // Display help information
void About();               // Display about information
void InitMap();             // Initialize the game map
void PrintFood();           // Print the food on the map
int MoveSnake();            // Move the snake based on user input
int IsCorrect();            // Check if the snake's move is valid
void SpeedControl();        // Control the speed of the snake