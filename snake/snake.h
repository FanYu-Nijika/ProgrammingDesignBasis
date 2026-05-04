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
    int type; // 1代表小，2代表中，3代表大
} Food;

typedef struct {
    int x;
    int y;
} Snakenode, Obstacle;

typedef struct {
    int x;
    int y;
    int dir;
} DynamicObstacle;

typedef struct {
    Snakenode snakeNode[1000];
    int length;
    int speed;
    int score;
} Snake;

extern Snake snake;
extern Food food;
extern Obstacle obstacles[OBSTACLE_COUNT];
extern DynamicObstacle moving_obs;
extern int invincible_timer;
extern DWORD slow_timer;

void GotoXY(int x, int y);  // 移动光标
void Hide();                // 隐藏光标
int Menu();                 // 显示菜单
void Help();                // 显示帮助
void About();               // 显示关于
void Leaderboard();         // 显示排行榜
void SaveScore(int score);  // 保存分数
void InitMap();             // 初始化地图
void PrintFood();           // 打印食物
void PrintObstacles();      // 打印障碍物
int MoveSnake();            // 移动蛇
int IsCorrect();            // 检查是否有效
void SpeedControl();        // 控制速度
void SkillControl();        // 技能控制
void InitDynamicObstacle(); // 初始化动态障碍物
void MoveDynamicObstacle(); // 移动动态障碍物
