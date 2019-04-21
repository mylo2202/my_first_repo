#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const string WINDOW_TITLE = "Minesweeper 1.0";

void logSDLError(ostream& os, const string &msg, bool fatal)    //in case we f*ck up
{
    os << msg << "WE F*CKED UP: " << SDL_GetError() << endl;
    if(fatal)
    {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)      //initialize SDL
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logSDLError(cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)        //quit SDL
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPress()        //wait for a key press
{
    SDL_Event e;
    while (true)
    {
        if (SDL_WaitEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT))
            return;
        SDL_Delay(100);
    }
}

void initialize_board();
void coordinate_blocks(int i, int j);
void initialize_mine_positions();
void reveal_blocks(int i, int j);
void open_block(int i, int j);
void place_or_remove_flag(int i, int j);
bool end_game_win_check();
void game();

const int boardWidth = 30;
const int boardHeight = 24;
const int mineNumber = 150;
char userInput;
char board[boardHeight][boardWidth];
char boardMinePositions[boardHeight][boardWidth];
int flagCounter = 0;
int minesFlaggedCounter = 0;
bool endGameLose = false;
time_t timeElapsed = time(0);
time_t gameTime;

SDL_Renderer* renderer;
const int blockSize = 32;

void draw_backgroud()       //draw the background
{
    SDL_Rect background;

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    background.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 - 1;
    background.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 - 1;
    background.w = blockSize*boardWidth + 2;
    background.h = blockSize*boardHeight + 2;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &background);

    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            background.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j;
            background.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i;
            background.w = blockSize;
            background.h = blockSize;
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &background);
        }
    }
}

void draw_board(char blockProperty, int i, int j)       //draw the board
{
    SDL_Rect block;

    block.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + 1;
    block.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + 1;
    block.w = blockSize - 2;
    block.h = blockSize - 2;

    switch(blockProperty)   //missing number textures
    {
        case '*':
            SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case 'X':
            SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '0':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '1':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '2':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '3':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '4':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '5':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '6':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '7':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '8':
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case 'F':
            SDL_SetRenderDrawColor(renderer, 128, 128, 0, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        //needs refactoring
        //a lot
    }


}

int main(int argc, char* argv[])
{
    SDL_Window* window;
    initSDL(window, renderer);

    initialize_board();
    initialize_mine_positions();

    //my drawings here

    draw_backgroud();
    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            draw_board(board[i][j], i, j);
        }
    }


    //end my drawings

    SDL_RenderPresent(renderer);

    waitUntilKeyPress();
    quitSDL(window, renderer);


    return 0;
}

void initialize_board()     //create the board with the given size
{
    for(int i = 0; i < boardHeight; i++)
        for(int j = 0; j < boardWidth; j++)
            board[i][j] = '*';
}

void calculate_mines(int i, int j)      //calculate how many mines touching the block
{
    if(i >= 0 && i < boardHeight && j >= 0 && j < boardWidth && boardMinePositions[i][j] != 'X')
    boardMinePositions[i][j]++;
}

void initialize_mine_positions()        //create mines with the given number at randomized positions
{
    int counter = 0;
    srand(time(0));

    for(int i = 0; i < boardHeight; i++)
        for(int j = 0; j < boardWidth; j++)
            boardMinePositions[i][j] = '0';

    while(counter < mineNumber)
    {
        int i = rand() % boardHeight;
        int j = rand() % boardWidth;
        if(boardMinePositions[i][j] != 'X')
        {
            boardMinePositions[i][j] = 'X';

            calculate_mines(i - 1, j - 1);
            calculate_mines(i - 1, j);
            calculate_mines(i - 1, j + 1);
            calculate_mines(i, j - 1);
            calculate_mines(i, j + 1);
            calculate_mines(i + 1, j - 1);
            calculate_mines(i + 1, j);
            calculate_mines(i + 1, j + 1);
            counter++;
        }
    }
}

void reveal_blocks(int i, int j)        //reveal block and adjacent blocks when opened
{
    if(board[i][j] == '*' && boardMinePositions[i][j] != 'X' && i >= 0 && i < boardHeight && j >= 0 && j < boardWidth)
    {
        board[i][j] = boardMinePositions[i][j];

        if(boardMinePositions[i][j] == '0')
        {
            reveal_blocks(i - 1, j - 1);
            reveal_blocks(i - 1, j);
            reveal_blocks(i - 1, j + 1);
            reveal_blocks(i, j - 1);
            reveal_blocks(i, j + 1);
            reveal_blocks(i + 1, j - 1);
            reveal_blocks(i + 1, j);
            reveal_blocks(i + 1, j + 1);
        }
    }
}

void open_block(int i, int j)       //open the block and check whether it is a mine
{
    if(boardMinePositions[i][j] == 'X')
    {
        board[i][j] = 'X';
        endGameLose = true;

        for(int i = 0; i < boardHeight; i++)
            for(int j = 0; j < boardWidth; j++)
                if(boardMinePositions[i][j] == 'X')
                    board[i][j] = 'X';
    }
    else
        reveal_blocks(i, j);
}

void place_or_remove_flag(int i, int j)     //place or remove a flag on the block
{
    if (board[i][j] == '*')
    {
        board[i][j] = 'F';
        flagCounter++;

        if(boardMinePositions[i][j] == 'X')
            minesFlaggedCounter++;
    }

    else if (board[i][j] == 'F')
    {
        board[i][j] = '*';
        flagCounter--;
        if(boardMinePositions[i][j] == 'X')
            minesFlaggedCounter--;
    }
}

void get_user_input(int mouseX, int mouseY, bool leftMouseClicked)   //respond to mouse events
{
    int i, j;

    if(mouseX >= (SCREEN_WIDTH - blockSize*boardWidth) / 2 &&
       mouseX <= (SCREEN_WIDTH - blockSize*boardWidth) / 2 &&
       mouseY >= (SCREEN_HEIGHT - blockSize*boardHeight) / 2 &&
       mouseY <= (SCREEN_HEIGHT - blockSize*boardHeight) / 2)
    {
        j = ((mouseX - (SCREEN_WIDTH - blockSize*boardWidth)) / 2) / blockSize;
        i = ((mouseY - (SCREEN_HEIGHT - blockSize*boardHeight)) / 2) / blockSize;
    }

    if(leftMouseClicked) open_block(i, j);
    else place_or_remove_flag(i, j);
}

bool end_game_win_check()       //check for game end conditions
{
    if(flagCounter == mineNumber && minesFlaggedCounter == mineNumber)
        return 1;
    else
        return 0;
}


//broken function
/*void rend_game()
{
    initialize_board();
    initialize_mine_positions();

    while(!endGameLose && !end_game_win_check())
    {
        gameTime = time(0);
        //print_table(board);
        //cout << endl << "Flags:" << flagCounter << endl;
        //cout << "Time:" << gameTime - timeElapsed << endl;
        get_user_input();
    }

    if(endGameLose)
    {
        //print_table(board);
        //cout << endl << "GAME OVER" << endl;
    }

    if(end_game_win_check())
    //cout<< "Time to complete:" << gameTime - timeElapsed << endl;
    //cout<< endl << "YOU WIN!" << endl;
}
*/
