#include "snake.h"

Snake snake;
Food food;
char now_dir = RIGHT; // Initial direction of the snake
char direction = RIGHT; // expected direction based on user input
Obstacle obstacles[OBSTACLE_COUNT]; // obstacles on map

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

    // generate obstacles first so food won't spawn on them
    PrintObstacles();
    PrintFood();
    // GotoXY(0, 0);
    // printf("Current Score: 0");
}
void PrintFood(){
    // Place food at random position not overlapping snake or obstacles
    int flag = 1;
    while (flag){
        flag = 0;
        food.x = rand() % (MAP_WIDTH - 2) + 1;
        food.y = rand() % (MAP_HEIGHT - 2) + 1;
        // avoid snake
        for (int k = 0; k <= snake.length - 1; ++k){
            if (food.x == snake.snakeNode[k].x && food.y == snake.snakeNode[k].y){
                flag = 1;
                break;
            }
        }
        // avoid obstacles
        if (!flag){
            for (int o = 0; o < OBSTACLE_COUNT; ++o){
                if (food.x == obstacles[o].x && food.y == obstacles[o].y){
                    flag = 1; break;
                }
            }
        }
    }
    // choose a random food type: 1 small, 2 medium, 3 big
    food.type = rand() % 3 + 1;
    GotoXY(food.x, food.y);
    // different symbol per food type
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
    // check if ate food
    if (snake.snakeNode[0].x == food.x && snake.snakeNode[0].y == food.y){
        int grow = food.type; // growth depends on food type
        // append copies of previous tail position to represent growth
        for (int g = 0; g < grow; ++g){
            if (snake.length < 1000){
                snake.snakeNode[snake.length] = temp;
                snake.length++;
            }
        }
        flag = 1;
    }

    if (!flag){
        // erase previous tail
        GotoXY(temp.x, temp.y);
        printf(" ");
    } else {
        // place new food and update score display
        PrintFood();
        GotoXY(50, 5);
        printf("Current Score: %d", snake.length - 3);
    }

    // handle self-collision by trimming tail instead of ending game
    for (int i = 1; i < snake.length; ++i){
        if (snake.snakeNode[0].x == snake.snakeNode[i].x && snake.snakeNode[0].y == snake.snakeNode[i].y){
            int new_length = i; // snake becomes length up to collision index
            // erase the removed parts from screen
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
        // save score to leaderboard
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
    // check collision with obstacles: hitting obstacle ends game
    for (int o = 0; o < OBSTACLE_COUNT; ++o){
        if (snake.snakeNode[0].x == obstacles[o].x && snake.snakeNode[0].y == obstacles[o].y){
            return 0;
        }
    }
    return 1;
}

// Generate and print obstacles at random positions (avoid snake area)
void PrintObstacles(){
    for (int i = 0; i < OBSTACLE_COUNT; ++i){
        int ok = 0;
        while (!ok){
            ok = 1;
            obstacles[i].x = rand() % (MAP_WIDTH - 2) + 1;
            obstacles[i].y = rand() % (MAP_HEIGHT - 2) + 1;
            // avoid initial snake
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

// Save score to leaderboard file (append)
void SaveScore(int score){
    FILE *f = fopen("leaderboard.txt", "a");
    if (!f) return;
    fprintf(f, "%d\n", score);
    fclose(f);
}

// Read leaderboard and display top scores
void Leaderboard(){
    // read scores
    FILE *f = fopen("leaderboard.txt", "r");
    int scores[1000];
    int cnt = 0;
    if (f){
        while (cnt < 1000 && fscanf(f, "%d", &scores[cnt]) == 1) cnt++;
        fclose(f);
    }
    // sort descending
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
