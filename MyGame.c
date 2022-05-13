#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#define ENEMGENX 1
#define ENEMGENY 2
#define ENEMGENTX 8
#define ENEMGENTY 10

int enemy1X, enemy1Y, playerX, playerY, move1, move2, enemy2X, enemy2Y;

int msleep(long msec);
void PrintGameState(int board[12][12]);
int enemyMove(int board[12][12], int posX, int posY, int dirMoved);
void createEnemy(int gen);

int main(void) {

  //Sets up game board
  int maze[12][12] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };
  printGameState(maze);

  enemy1X = -1;
  enemy1Y = -1;
  createEnemy(1);
  createEnemy(2);
  int x = 1;
  move1 = 3;
  move2 = 3;
  while(!(x == 5)) {
    if (enemy1X != -1) {
      move1 = enemyMove(maze, enemy1X, enemy1Y, move1);
      switch (move1) {
        case 0:
          enemy1Y--;
          break;
        case 1:
          enemy1X--;
          break;
        case 2:
          enemy1Y++;
          break;
        case 3:
          enemy1X++;
          break;
        default:
          break;
      }
      if (enemy2X != -1) {
        move2 = enemyMove(maze, enemy2X, enemy2Y, move2);
        switch (move2) {
          case 0:
            enemy2Y--;
            break;
          case 1:
            enemy2X--;
            break;
          case 2:
            enemy2Y++;
            break;
          case 3:
            enemy2X++;
            break;
          default:
            break;
        }
      }
    }
    printGameState(maze);
    msleep(200);
  }
}


void createEnemy(int gen) {
  if (gen == 1) {
    enemy1X = ENEMGENX;
    enemy1Y = ENEMGENY;
  }
  if (gen == 2) {
    enemy2X = ENEMGENTX;
    enemy2Y = ENEMGENTY;
  }
}


//Given a location and sends out a new location of the enemy
//Returns 0 for up, 1 for left, 2 for down, 3 for right
int enemyMove(int board[12][12], int posX, int posY, int dirMoved) {
  //Given an initial movement that it just took, chooses which way to turn and what that means
  switch (dirMoved) {
    //Moved up
    case 0:
      //Attempts to turn right
      if (board[posY][posX+1] == 0) {
        return 3;
      //attempts to keep going
    } else if (board[posY-1][posX] == 0) {
        return 0;
      //attempts to turn left
    } else if (board[posY][posX-1] == 0) {
        return 1;
      //otherwise turns around
      } else {
        return 2;
      }
      break;

    //Moved left
    case 1:
      //Attempts to turn right
      if (board[posY-1][posX] == 0) {
        return 0;
      //attempts to keep going
    } else if (board[posY][posX-1] == 0) {
        return 1;
      //attempts to turn left
    } else if (board[posY+1][posX] == 0) {
        return 2;
      //otherwise turns around
      } else {
        return 3;
      }
      break;

    //moved down
    case 2:
      //Attempts to turn right
      if (board[posY][posX-1] == 0) {
        return 1;
      //attempts to keep going
    } else if (board[posY+1][posX] == 0) {
        return 2;
      //attempts to turn left
    } else if (board[posY][posX+1] == 0) {
        return 3;
      //otherwise turns around
      } else {
        return 0;
      }
      break;

    //moved right
    case 3:
      //Attempts to turn right
      if (board[posY+1][posX] == 0) {
        return 2;
      //attempts to keep going
      } else if (board[posY][posX+1] == 0) {
        return 3;
      //attempts to turn left
      } else if (board[posY-1][posX] == 0) {
        return 0;
      //otherwise turns around
      } else {
        return 1;
      }
      break;
  }
  //Just in case something fails, default case for if nothing happens.
  printf("Uh oh, there was an error");
  return 0;
}


//takes in current board state, and prints it out
void printGameState(int board[12][12]) {
  //Space so it looks like the thing is actually moving and you don't see the after images going up
  for(int i = 0; i < 20; i++) {
    printf("\n");
  }

  //prints all the rows
  for(int i = 0; i < 12; i++) {
    //prints a row
    for (int j = 0; j < 12; j++) {
      if ((i == ENEMGENY && j == ENEMGENX) || (i == ENEMGENTY && j == ENEMGENTX)) {
        if((i == enemy1Y && j == enemy1X) || (i == enemy2Y && j == enemy2X)) {
          printf("[M ");
        } else if (i == playerY && j == playerX) {
          printf("[G ");
        } else {
          printf("[  ");
        }
      } else if (playerY == i && playerX == j) {
        //prints you (the gamer)
        printf(" G ");
      } else if(enemy1Y == i && enemy1X == j) {
        //prints monster
        printf(" M ");
      } else if(enemy2Y == i && enemy2X == j) {
        //prints monster
        printf(" M ");
      } else if (board[i][j] == 1) {
        //Prints a wall if the corresponding number is 1
        printf("=#=");
      } else {
        //Prints an empty space if there's a corresponding 0
        printf("   ");
      }
    }
    printf("\n");
  }

}


//Sleep function for less than a second
//input x/1000 seconds
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}
