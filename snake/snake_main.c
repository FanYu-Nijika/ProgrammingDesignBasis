#include "snake.h"

int main(){
    srand((unsigned int)time(NULL)); // Seed the random number generator
    int end = 1, result;
    while (end){
        result = Menu();
        if (result == 1){
            InitMap();
            while (MoveSnake());
        }
        else if (result == 2){
            Help();
        }
        else if (result == 3){
            About();
        }
        else if (result == 0){
            end = 0;
        }
    }
    return 0;
}