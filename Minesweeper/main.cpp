#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const string WINDOW_TITLE = "Minesweeper 1.0";

void logSDLError(ostream& os, const string &msg, bool fatal)
{
    os << msg << "WE FCKED UP: " << SDL_GetError() << endl;
    if(fatal)
    {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logSDLError(cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPress()
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
void open_block();
void place_or_remove_flag();
bool end_game_win_check();
void game();

const int boardWidth = 9;
const int boardHeight = 9;
const int mineNumber = 10;
char userInput;
char board[boardHeight][boardWidth];
char boardMinePositions[boardHeight][boardWidth];
int flagCounter = 0;
int minesFlaggedCounter = 0;
bool endGameLose = false;
time_t timeElapsed = time(0);
time_t gameTime;

SDL_Renderer* renderer;
const int blockSize = 60;

void draw_backgroud()
{
    //SDL_Renderer* renderer;

    SDL_Rect background;

    /*
    background.x = 0;
    background.y = 0;
    background.w = SCREEN_WIDTH;
    background.h = SCREEN_HEIGHT;
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &background);
    */

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

void draw_board(char block_property, int i, int j)
{
    //SDL_Renderer* renderer;

    SDL_Rect block;

    switch(block_property)
    {
        case '*':
            block.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + 1;
            block.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + 1;
            block.w = blockSize - 2;
            block.h = blockSize - 2;
            SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case 'X':
            block.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + 1;
            block.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + 1;
            block.w = blockSize - 2;
            block.h = blockSize - 2;
            SDL_SetRenderDrawColor(renderer, 128, 32, 32, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        //needs addition
    }


}

int main(int argc, char* argv[])
{
    SDL_Window* window;
    //SDL_Renderer* renderer;
    initSDL(window, renderer);

    //SDL_RenderClear(renderer);


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

    /*
    SDL_Rect block;

    block.x = 0;
    block.y = 0;
    block.w = 800;
    block.h = 600;
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &block);

    block.x = SCREEN_WIDTH/2 - (60*boardWidth)/2;
    block.y = SCREEN_HEIGHT/2 - (60*boardHeight)/2;
    block.w = 60*boardWidth;
    block.h = 60*boardHeight;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &block);

    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            block.x = SCREEN_WIDTH/2 - (60*boardWidth)/2 + 60*j;
            block.y = SCREEN_HEIGHT/2 - (60*boardHeight)/2 + 60*i;
            block.w = 60;
            block.h = 60;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &block);

            switch(board[i][j])
            {
                case '*':
                    block.x = SCREEN_WIDTH/2 - (60*boardWidth)/2 + 60*j + 1;
                    block.y = SCREEN_HEIGHT/2 - (60*boardHeight)/2 + 60*i + 1;
                    block.w = 58;
                    block.h = 58;
                    SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
                    SDL_RenderFillRect(renderer, &block);
                    break;

                case 'X':
                    block.x = SCREEN_WIDTH/2 - (60*boardWidth)/2 + 60*j + 1;
                    block.y = SCREEN_HEIGHT/2 - (60*boardHeight)/2 + 60*i + 1;
                    block.w = 58;
                    block.h = 58;
                    SDL_SetRenderDrawColor(renderer, 128, 32, 32, 255);
                    SDL_RenderFillRect(renderer, &block);
                    break;

                //still to come
            }
        }
    }
    */
    //end my drawings

    SDL_RenderPresent(renderer);

    waitUntilKeyPress();
    quitSDL(window, renderer);


    return 0;
}

void initialize_board()
{
    for(int i = 0; i < boardHeight; i++)
        for(int j = 0; j < boardWidth; j++)
            board[i][j] = '*';
}

void coordinate_blocks(int i, int j)
{
    if(i >= 0 && i < boardHeight && j >= 0 && j < boardWidth && boardMinePositions[i][j] != 'X')
    boardMinePositions[i][j]++;
}

void initialize_mine_positions()
{
    int counter = 0;
    srand(time(0));

    for(int i = 0; i < boardHeight; i++)
        for(int j = 0; j < boardWidth; j++)
            boardMinePositions[i][j] = '0';

    while(counter < mineNumber)
    {
        int i = rand() % mineNumber;
        int j = rand() % mineNumber;
        if(boardMinePositions[i][j] == '0')
        {
            boardMinePositions[i][j] = 'X';

            coordinate_blocks(i - 1, j - 1);
            coordinate_blocks(i - 1, j);
            coordinate_blocks(i - 1, j + 1);
            coordinate_blocks(i, j - 1);
            coordinate_blocks(i, j + 1);
            coordinate_blocks(i + 1, j - 1);
            coordinate_blocks(i + 1, j);
            coordinate_blocks(i + 1, j + 1);
            counter++;
        }
    }
}

void reveal_blocks(int i, int j)
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

void open_block()
{
    int i, j;

    do
        cin >> i >> j;
    while(i < 0 || i > boardHeight - 1 || j < 0 || j > boardWidth - 1);

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

void place_or_remove_flag()
{
    int i, j;
    do
        cin >> i >> j;
    while(i < 0 || i > boardHeight - 1 || j < 0 || j > boardWidth - 1);

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

void get_user_input()   //needs refactoring
{
    cin >> userInput;
    switch (userInput)
    {
        case 'o' : open_block(); break;
        case 'f' : place_or_remove_flag(); break;
        default  : get_user_input();
    }
}

bool end_game_win_check()
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
