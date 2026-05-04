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
#define OBSTACLE_COUNT 10

typedef struct {
    int x;
    int y;
    int type; // 1 for small, 2 for med, 3 for big
} Food;

typedef struct {
    int x;
    int y;
} Snakenode, Obstacle;

typedef struct {
    Snakenode snakeNode[1000];
    int length;
    int speed;
    int score;
} Snake;

extern Snake snake;
extern Food food;
extern Obstacle obstacles[OBSTACLE_COUNT];

void GotoXY(int x, int y);  // Move the cursor
void Hide();                // Hide the cursor
int Menu();                 // Display menu
void Help();                // Display help
void About();               // Display about
void Leaderboard();         // Display leaderboard
void SaveScore(int score);  // Save score
void InitMap();             // Initialize map
void PrintFood();           // Print food
void PrintObstacles();      // Print obstacles
int MoveSnake();            // Move snake
int IsCorrect();            // Check if valid
void SpeedControl();        // Control speed
