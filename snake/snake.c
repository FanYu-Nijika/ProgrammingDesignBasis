#include "snake.h"

Snake snake;
Food food;
char now_dir = RIGHT; // Initial direction of the snake
char direction = RIGHT; // expected direction based on user input

int Menu(){
    GotoXY(40, 12);
    printf("欢迎来到贪吃蛇\r\n");
    GotoXY(43, 13);
    printf("1. 开始游戏\r\n");
    GotoXY(43, 16);
    printf("2. 帮助\r\n");
    GotoXY(43, 18);
    printf("3. 关于\r\n");
    GotoXY(43, 20);
    printf("按任意键退出\r\n");
    Hide();

    char ch;
    int result = 0;
    ch = _getch();
    switch (ch){
        case '1': result = 1; break;
        case '2': result = 2; break;
        case '3': result = 3; break;
    }
    system("cls"); // Clear the screen
    return result;
}
void GotoXY(int x, int y){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // Get the handle to the console output
    COORD coord; // Define a COORD structure to hold the coordinates
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hOut, coord);
}
void Hide(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, 0};
    GetConsoleCursorInfo(hOut, &cursorInfo);
    // cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}
void About(){
    GotoXY(40, 12);
    printf("这是一个用C语言编写的贪吃蛇游戏\r\n");
    GotoXY(40, 14);
    printf("按任意键返回菜单\r\n");
    Hide();
    char ch = _getch();
    system("cls");
}
void Help(){
    GotoXY(40, 12);
    printf("使用WASD键控制蛇的移动\r\n");
    GotoXY(40, 14);
    printf("w 上\r\n");
    GotoXY(40, 16);
    printf("s 下\r\n");
    GotoXY(40, 18);
    printf("a 左\r\n");
    GotoXY(40, 20);
    printf("d 右\r\n");

    GotoXY(40, 22);
    printf("吃到食物会变长，撞到自己或墙壁会游戏结束\r\n");
    GotoXY(40, 24);
    printf("按任意键返回菜单\r\n");
    Hide();
    char ch = _getch();
    system("cls");
}
void InitMap(){
    Hide();
    snake.snakeNode[0].x = MAP_WIDTH / 2 - 1;
    snake.snakeNode[0].y = MAP_HEIGHT / 2 - 1;
    GotoXY(snake.snakeNode[0].x, snake.snakeNode[0].y);
    printf("@");
    snake.length = 3;
    snake.speed = 250;
    now_dir = RIGHT;

    for (int i = 1; i < snake.length; ++i){
        snake.snakeNode[i].x = snake.snakeNode[i - 1].x - 1;
        snake.snakeNode[i].y = snake.snakeNode[i - 1].y;
        GotoXY(snake.snakeNode[i].x, snake.snakeNode[i].y);
        printf("o");
    }
    for (int i = 1; i < MAP_HEIGHT - 1; ++i){
        GotoXY(0, i);
        printf("|");
        GotoXY(MAP_WIDTH - 1, i);
        printf("|");
    }

    PrintFood();
    GotoXY(0, 50);
    prinf("当前得分: 0");
}
void PrintFood(){
    int flag = 1;
    while (flag){
        flag = 0;
        food.x = rand() % (MAP_WIDTH - 2) + 1;
        food.y = rand() % (MAP_HEIGHT - 2) + 1;
        for (int k = 0; k <= snake.length - 1; ++k){
            if (food.x == snake.snakeNode[k].x && food.y == snake.snakeNode[k].y){
                flag = 1;
                break;
            }
        }
    }
    GotoXY(food.x, food.y);
    printf("$");
}
int MoveSnake(){
    Snakenode temp = snake.snakeNode[snake.length - 1];
    int flag = 0;
    for (int i = snake.length - 1; i > 0; --i){
        snake.snakeNode[i] = snake.snakeNode[i - 1];
    }
    GotoXY(snake.snakeNode[1].x, snake.snakeNode[1].y);
    printf("o");

    if (_kbhit()){
        direction = _getch();
        switch (direction){
            case UP:
                if (now_dir != DOWN) now_dir = UP;
                break;
            case DOWN:
                if (now_dir != UP) now_dir = DOWN;
                break;
            case LEFT:
                if (now_dir != RIGHT) now_dir = LEFT;
                break;
            case RIGHT:
                if (now_dir != LEFT) now_dir = RIGHT;
                break;
        }
    }
    switch (now_dir){
        case UP: snake.snakeNode[0].y -= 1; break;
        case DOWN: snake.snakeNode[0].y += 1; break;
        case LEFT: snake.snakeNode[0].x -= 1; break;
        case RIGHT: snake.snakeNode[0].x += 1; break;
    }

    GotoXY(snake.snakeNode[0].x, snake.snakeNode[0].y);
    printf("@");

    if (snake.snakeNode[0].x == food.x && snake.snakeNode[0].y == food.y){
        ++snake.length;
        flag = 1;
        snake.snakeNode[snake.length - 1] = temp;
    }
    
    if (!flag){
        GotoXY(temp.x, temp.y);
        printf(" ");
    }
    else {
        PrintFood();
        GotoXY(50, 5);
        printf("当前得分: %d", snake.length - 3);
    }

    if (IsCorrect()){
        system("cls");
        GotoXY(45, 14);
        printf("最终得分: %d", snake.length - 3);
        GotoXY(45, 16);
        printf("你输了!\r\n");
        GotoXY(45, 18);
        printf("按任意键返回菜单\r\n");
        system("cls");
        return 0;
    }

    SpeedControl();
    Sleep(snake.speed);
    return 1;
}
int IsCorrect(){
    if (snake.snakeNode[0].x <= 0 || snake.snakeNode[0].x >= MAP_WIDTH - 1 || snake.snakeNode[0].y <= 0 || snake.snakeNode[0].y >= MAP_HEIGHT - 1){
        return 0;
    }
    for (int i = 1; i < snake.length; ++i){
        if (snake.snakeNode[0].x == snake.snakeNode[i].x && snake.snakeNode[0].y == snake.snakeNode[i].y){
            return 0;
        }
    }
    return 1;
}
void SpeedControl(){
    switch (snake.length){
        case 6: snake.speed = 200; break;
        case 9: snake.speed = 180; break;
        case 12: snake.speed = 160; break;
        case 15: snake.speed = 140; break;
        case 18: snake.speed = 120; break;
        case 21: snake.speed = 100; break;
        case 24: snake.speed = 80; break;
        case 27: snake.speed = 60; break;
        case 30: snake.speed = 40; break;
        default: break;
    }
}
