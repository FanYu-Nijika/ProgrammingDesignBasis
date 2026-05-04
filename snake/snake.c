#include "snake.h"

Snake snake;
Food food;
char now_dir = RIGHT; // 蛇的初始方向
char direction = RIGHT; // 根据用户输入的预期方向
Obstacle obstacles[OBSTACLE_COUNT]; // 地图上的障碍物

int Menu(){
    GotoXY(40, 12);
    printf("Welcome to Snake\r\n");
    GotoXY(43, 14);
    printf("1. Start Game\r\n");
    GotoXY(43, 16);
    printf("2. Help\r\n");
    GotoXY(43, 18);
    printf("3. About\r\n");
    GotoXY(43, 20);
    printf("4. Leaderboard\r\n");
    GotoXY(43, 22);
    printf("0. Exit\r\n");
    Hide();

    char ch;
    int result = 0;
    ch = _getch();
    switch (ch){
        case '1': result = 1; break;
        case '2': result = 2; break;
        case '3': result = 3; break;
        case '4': result = 4; break;
    }
    system("cls"); // 清屏
    return result;
}
void GotoXY(int x, int y){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出的句柄
    COORD coord; // 定义 COORD 结构体用于保存坐标
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hOut, coord);
}
void Hide(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}
void About(){
    GotoXY(40, 12);
    printf("This is a Snake game in C\r\n");
    GotoXY(40, 14);
    printf("Press any key to return\r\n");
    Hide();
    char ch = _getch();
    system("cls");
}
void Help(){
    GotoXY(40, 12);
    printf("Use WASD to move\r\n");
    GotoXY(40, 14);
    printf("w UP\r\n");
    GotoXY(40, 16);
    printf("s DOWN\r\n");
    GotoXY(40, 18);
    printf("a LEFT\r\n");
    GotoXY(40, 20);
    printf("d RIGHT\r\n");

    GotoXY(40, 22);
    printf("Eat to grow, hit wall or yourself to lose\r\n");
    GotoXY(40, 24);
    printf("Press any key to return\r\n");
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

    // 首先生成障碍物，以便食物不会在它们上面生成
    PrintObstacles();
    PrintFood();
    // GotoXY(0, 0);
    // printf("Current Score: 0");
}
void PrintFood(){
    // 在不与蛇或障碍物重叠的随机位置放置食物
    int flag = 1;
    while (flag){
        flag = 0;
        food.x = rand() % (MAP_WIDTH - 2) + 1;
        food.y = rand() % (MAP_HEIGHT - 2) + 1;
        // 避开蛇
        for (int k = 0; k <= snake.length - 1; ++k){
            if (food.x == snake.snakeNode[k].x && food.y == snake.snakeNode[k].y){
                flag = 1;
                break;
            }
        }
        // 避开障碍物
        if (!flag){
            for (int o = 0; o < OBSTACLE_COUNT; ++o){
                if (food.x == obstacles[o].x && food.y == obstacles[o].y){
                    flag = 1; break;
                }
            }
        }
    }
    // 随机选择食物类型：1 小，2 中，3 大
    food.type = rand() % 3 + 1;
    GotoXY(food.x, food.y);
    // 不同食物类型对应不同符号
    if (food.type == 1) printf("$");
    else if (food.type == 2) printf("&");
    else printf("*");
}
int MoveSnake(){
    Snakenode temp = snake.snakeNode[snake.length - 1];
    int flag = 0;
    int old_length = snake.length;
    for (int i = snake.length - 1; i > 0; --i){
        snake.snakeNode[i] = snake.snakeNode[i - 1];
        Hide();
    }
    GotoXY(snake.snakeNode[1].x, snake.snakeNode[1].y);
    printf("o");

    if (_kbhit()){
        Hide();
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

    if (snake.snakeNode[0].x <= 0) snake.snakeNode[0].x = MAP_WIDTH - 2;
    if (snake.snakeNode[0].x >= MAP_WIDTH - 1) snake.snakeNode[0].x = 1;
    if (snake.snakeNode[0].y <= 0) snake.snakeNode[0].y = MAP_HEIGHT - 2;
    if (snake.snakeNode[0].y >= MAP_HEIGHT - 1) snake.snakeNode[0].y = 1;

    GotoXY(snake.snakeNode[0].x, snake.snakeNode[0].y);
    printf("@");
    Hide();
    // 检查是否吃到了食物
    if (snake.snakeNode[0].x == food.x && snake.snakeNode[0].y == food.y){
        int grow = food.type; // 生长长度取决于食物类型
        // 将之前尾部的副本来充当生长的部分
        for (int g = 0; g < grow; ++g){
            if (snake.length < 1000){
                snake.snakeNode[snake.length] = temp;
                snake.length++;
            }
        }
        flag = 1;
    }

    if (!flag){
        // 擦除之前的尾部
        GotoXY(temp.x, temp.y);
        printf(" ");
    } else {
        // 放置新食物并更新分数显示
        PrintFood();
        GotoXY(50, 5);
        printf("Current Score: %d", snake.length - 3);
    }

    // 处理自身碰撞通过剪断尾巴而不是直接结束游戏
    for (int i = 1; i < snake.length; ++i){
        if (snake.snakeNode[0].x == snake.snakeNode[i].x && snake.snakeNode[0].y == snake.snakeNode[i].y){
            int new_length = i; // 蛇身长变为到碰撞索引的位置
            // 将截断的部分从屏幕上擦除
            for (int j = new_length; j < snake.length; ++j){
                GotoXY(snake.snakeNode[j].x, snake.snakeNode[j].y);
                printf(" ");
            }
            snake.length = new_length;
            break;
        }
    }

    if (!IsCorrect()){
        system("cls");
        GotoXY(45, 14);
        printf("Final Score: %d", snake.length - 3);
        GotoXY(45, 16);
        printf("Game Over!\r\n");
        GotoXY(45, 18);
        printf("Press any key to return\r\n");
        // 将分数保存到排行榜
        SaveScore(snake.length - 3);
        _getch();
        system("cls");
        return 0;
    }

    SpeedControl();
    Sleep(snake.speed);
    return 1;
}
int IsCorrect(){
    // 检测是否与障碍物发生碰撞：撞角将结束游戏
    for (int o = 0; o < OBSTACLE_COUNT; ++o){
        if (snake.snakeNode[0].x == obstacles[o].x && snake.snakeNode[0].y == obstacles[o].y){
            return 0;
        }
    }
    return 1;
}

// 在随机位置生成并打印障碍物（避开蛇身区域）
void PrintObstacles(){
    for (int i = 0; i < OBSTACLE_COUNT; ++i){
        int ok = 0;
        while (!ok){
            ok = 1;
            obstacles[i].x = rand() % (MAP_WIDTH - 2) + 1;
            obstacles[i].y = rand() % (MAP_HEIGHT - 2) + 1;
            // 避开初始的蛇
            for (int k = 0; k < snake.length; ++k){
                if (obstacles[i].x == snake.snakeNode[k].x && obstacles[i].y == snake.snakeNode[k].y){
                    ok = 0; break;
                }
            }
        }
        GotoXY(obstacles[i].x, obstacles[i].y);
        printf("X");
    }
}

// 保存分数至排行榜文件（追加方式）
void SaveScore(int score){
    FILE *f = fopen("leaderboard.txt", "a");
    if (!f) return;
    fprintf(f, "%d\n", score);
    fclose(f);
}

// 读取排行榜并在屏幕上显示前几名的分数
void Leaderboard(){
    // 读取分数
    FILE *f = fopen("leaderboard.txt", "r");
    int scores[1000];
    int cnt = 0;
    if (f){
        while (cnt < 1000 && fscanf(f, "%d", &scores[cnt]) == 1) cnt++;
        fclose(f);
    }
    // 降序排序
    for (int i = 0; i < cnt; ++i){
        for (int j = i + 1; j < cnt; ++j){
            if (scores[j] > scores[i]){
                int t = scores[i]; scores[i] = scores[j]; scores[j] = t;
            }
        }
    }
    system("cls");
    GotoXY(40, 10);
    printf("Leaderboard - Top %d\r\n", cnt < 10 ? cnt : 10);
    for (int i = 0; i < cnt && i < 10; ++i){
        GotoXY(40, 12 + i * 2);
        printf("%d. %d\r\n", i + 1, scores[i]);
    }
    GotoXY(40, 12 + (cnt < 10 ? cnt : 10) * 2 + 2);
    printf("Press any key to return\r\n");
    _getch();
    system("cls");
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
