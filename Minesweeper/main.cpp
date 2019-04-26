#include <iostream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define boardWidth 30
#define boardHeight 24
#define mineNumber 150
#define blockSize 32        //will vary depending on game difficulty
#define backgroundTextWidth 20
#define backgroundTextHeight 40

using namespace std;

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const string WINDOW_TITLE = "Minesweeper 1.0";

void log_SDL_error(ostream& os, const string &msg, bool fatal)    //in case we f*ck up
{
    os << msg << "We f*cked up SDL load: " << SDL_GetError() << endl;
    if(fatal)
    {
        SDL_Quit();
        exit(1);
    }
}

void init_SDL(SDL_Window* &window, SDL_Renderer* &renderer)      //initialize SDL
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        log_SDL_error(cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) log_SDL_error(cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(TTF_Init()== -1)
    {
        printf("We f*cked up TTF_Init: %s\n", TTF_GetError());
    }
}

void quit_SDL(SDL_Window* window, SDL_Renderer* renderer)        //quit SDL
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void initialize_board();
void coordinate_blocks(int i, int j);
void initialize_mine_positions();
void reveal_blocks(int i, int j);
void open_block(int i, int j);
void place_or_remove_flag(int i, int j);
void get_user_input(int mouseX, int mouseY, bool leftMouseClicked);
void get_mouse_coordinates();
bool end_game_win_check();
void game();

char userInput;
char board[boardHeight][boardWidth];
char boardMinePositions[boardHeight][boardWidth];
int flagCounter = 0;
int minesFlaggedCounter = 0;
bool endGameLose = false;
time_t timeElapsed = time(0);
time_t gameTime;
void rend_game();

SDL_Renderer* renderer;
SDL_Color Color1 = {0, 0, 255};
SDL_Color Color2 = {0, 255, 0};
SDL_Color Color3 = {255, 0, 0};
SDL_Color Color4 = {128, 0, 255};
SDL_Color Color5 = {255, 128, 0};
SDL_Color Color6 = {0, 128, 128};
SDL_Color Color7 = {128, 128, 64};
SDL_Color Color8 = {0, 0, 0};
SDL_Color BackgroundTextColor = {255, 255, 255};

void draw_background()       //draw the background
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

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

    SDL_Rect copyRectangle;

    SDL_Surface* flagBackground = TTF_RenderText_Solid(consolasFont, "Flag:", BackgroundTextColor);
    SDL_Texture* flagBackgroundTexture = SDL_CreateTextureFromSurface(renderer, flagBackground);
    copyRectangle.x = 0;
    copyRectangle.y = 0;
    copyRectangle.w = backgroundTextWidth * 5;
    copyRectangle.h = backgroundTextHeight;
    SDL_RenderCopy(renderer, flagBackgroundTexture, NULL, &copyRectangle);

    SDL_Surface* timeBackground = TTF_RenderText_Solid(consolasFont, "Time:", BackgroundTextColor);
    SDL_Texture* timeBackgroundTexture = SDL_CreateTextureFromSurface(renderer, timeBackground);
    copyRectangle.x = SCREEN_WIDTH - backgroundTextWidth * 5;
    copyRectangle.y = 0;
    copyRectangle.w = backgroundTextWidth * 5;
    copyRectangle.h = backgroundTextHeight;
    SDL_RenderCopy(renderer, timeBackgroundTexture, NULL, &copyRectangle);

    SDL_Surface* backBackground = TTF_RenderText_Solid(consolasFont, "Back", BackgroundTextColor);
    SDL_Texture* backBackgroundTexture = SDL_CreateTextureFromSurface(renderer, backBackground);
    copyRectangle.x = 0;
    copyRectangle.y = SCREEN_HEIGHT - backgroundTextHeight;
    copyRectangle.w = backgroundTextWidth * 4;
    copyRectangle.h = backgroundTextHeight;
    SDL_RenderCopy(renderer, backBackgroundTexture, NULL, &copyRectangle);

    SDL_Surface* helpBackground = TTF_RenderText_Solid(consolasFont, "Help", BackgroundTextColor);
    SDL_Texture* helpBackgroundTexture = SDL_CreateTextureFromSurface(renderer, helpBackground);
    copyRectangle.x = SCREEN_WIDTH - backgroundTextWidth * 4;
    copyRectangle.y = SCREEN_HEIGHT - backgroundTextHeight;
    copyRectangle.w = backgroundTextWidth * 4;
    copyRectangle.h = backgroundTextHeight;
    SDL_RenderCopy(renderer, helpBackgroundTexture, NULL, &copyRectangle);

    TTF_CloseFont(consolasFont);

    SDL_FreeSurface(flagBackground);
    SDL_DestroyTexture(flagBackgroundTexture);
    SDL_FreeSurface(timeBackground);
    SDL_DestroyTexture(timeBackgroundTexture);
    SDL_FreeSurface(backBackground);
    SDL_DestroyTexture(backBackgroundTexture);
    SDL_FreeSurface(helpBackground);
    SDL_DestroyTexture(helpBackgroundTexture);

}

void draw_board(char blockProperty, int i, int j)       //draw the board
{
    TTF_Font* segoeUIFont = TTF_OpenFont("segoeuib.ttf", 128);
    if(segoeUIFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Rect block;
    block.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + 1;
    block.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + 1;
    block.w = blockSize - 2;
    block.h = blockSize - 2;

    SDL_Rect copyRectangle;
    copyRectangle.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + blockSize / 4;
    copyRectangle.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + blockSize / 8;
    copyRectangle.w = blockSize / 2;
    copyRectangle.h = blockSize * 3 / 4;

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
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* one = TTF_RenderText_Solid(segoeUIFont, "1", Color1);
            SDL_Texture* oneTexture = SDL_CreateTextureFromSurface(renderer, one);
            SDL_RenderCopy(renderer, oneTexture, NULL, &copyRectangle);

            SDL_FreeSurface(one);
            SDL_DestroyTexture(oneTexture);
            break;
        }
        case '2':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* two = TTF_RenderText_Solid(segoeUIFont, "2", Color2);
            SDL_Texture* twoTexture = SDL_CreateTextureFromSurface(renderer, two);
            SDL_RenderCopy(renderer, twoTexture, NULL, &copyRectangle);

            SDL_FreeSurface(two);
            SDL_DestroyTexture(twoTexture);
            break;
        }
        case '3':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* three = TTF_RenderText_Solid(segoeUIFont, "3", Color3);
            SDL_Texture* threeTexture = SDL_CreateTextureFromSurface(renderer, three);
            SDL_RenderCopy(renderer, threeTexture, NULL, &copyRectangle);

            SDL_FreeSurface(three);
            SDL_DestroyTexture(threeTexture);
            break;
        }
        case '4':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* four = TTF_RenderText_Solid(segoeUIFont, "4", Color4);
            SDL_Texture* fourTexture = SDL_CreateTextureFromSurface(renderer, four);
            SDL_RenderCopy(renderer, fourTexture, NULL, &copyRectangle);

            SDL_FreeSurface(four);
            SDL_DestroyTexture(fourTexture);
            break;
        }
        case '5':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* five = TTF_RenderText_Solid(segoeUIFont, "5", Color5);
            SDL_Texture* fiveTexture = SDL_CreateTextureFromSurface(renderer, five);
            SDL_RenderCopy(renderer, fiveTexture, NULL, &copyRectangle);

            SDL_FreeSurface(five);
            SDL_DestroyTexture(fiveTexture);
            break;
        }
        case '6':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* six = TTF_RenderText_Solid(segoeUIFont, "6", Color6);
            SDL_Texture* sixTexture = SDL_CreateTextureFromSurface(renderer, six);
            SDL_RenderCopy(renderer, sixTexture, NULL, &copyRectangle);

            SDL_FreeSurface(six);
            SDL_DestroyTexture(sixTexture);
            break;
        }
        case '7':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* seven = TTF_RenderText_Solid(segoeUIFont, "7", Color7);
            SDL_Texture* sevenTexture = SDL_CreateTextureFromSurface(renderer, seven);
            SDL_RenderCopy(renderer, sevenTexture, NULL, &copyRectangle);

            SDL_FreeSurface(seven);
            SDL_DestroyTexture(sevenTexture);
            break;
        }
        case '8':
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* eight = TTF_RenderText_Solid(segoeUIFont, "8", Color8);
            SDL_Texture* eightTexture = SDL_CreateTextureFromSurface(renderer, eight);
            SDL_RenderCopy(renderer, eightTexture, NULL, &copyRectangle);

            SDL_FreeSurface(eight);
            SDL_DestroyTexture(eightTexture);
            break;
        }
        case 'F':
            SDL_SetRenderDrawColor(renderer, 128, 128, 0, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        //needs refactoring
    }

    TTF_CloseFont(segoeUIFont);
}

void draw_flag()
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Surface* flagBackgroundNumber = TTF_RenderText_Solid(consolasFont, to_string(mineNumber - flagCounter).c_str(), BackgroundTextColor);
    SDL_Texture* flagBackgroundNumberTexture = SDL_CreateTextureFromSurface(renderer, flagBackgroundNumber);
    SDL_Rect copyRectangle;
    copyRectangle.x = 0;
    copyRectangle.y = 0 + backgroundTextHeight;
    copyRectangle.w = backgroundTextWidth * strlen(to_string(mineNumber - flagCounter).c_str());
    copyRectangle.h = backgroundTextHeight;
    SDL_RenderCopy(renderer, flagBackgroundNumberTexture, NULL, &copyRectangle);

    TTF_CloseFont(consolasFont);

    SDL_FreeSurface(flagBackgroundNumber);
    SDL_DestroyTexture(flagBackgroundNumberTexture);
}

void draw_time()
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Surface* timeBackgroundNumber = TTF_RenderText_Solid(consolasFont, to_string(gameTime - timeElapsed).c_str(), BackgroundTextColor);
    SDL_Texture* timeBackgroundNumberTexture = SDL_CreateTextureFromSurface(renderer, timeBackgroundNumber);
    SDL_Rect copyRectangle;
    copyRectangle.x = SCREEN_WIDTH - backgroundTextWidth * (int(log10(gameTime - timeElapsed)) + 1);
    copyRectangle.y = 0 + backgroundTextHeight;
    copyRectangle.w = backgroundTextWidth * strlen(to_string(gameTime - timeElapsed).c_str());
    copyRectangle.h = backgroundTextHeight;
    SDL_RenderCopy(renderer, timeBackgroundNumberTexture, NULL, &copyRectangle);

    TTF_CloseFont(consolasFont);

    SDL_FreeSurface(timeBackgroundNumber);
    SDL_DestroyTexture(timeBackgroundNumberTexture);
}

void draw_everything()
{
    SDL_RenderClear(renderer);

    draw_background();

    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            draw_board(board[i][j], i, j);
        }
    }

    gameTime = time(0);

    draw_time();
    draw_flag();

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    SDL_Window* window;
    init_SDL(window, renderer);

    initialize_board();
    initialize_mine_positions();

    gameTime = time(0);

    //my drawings here

    SDL_RenderPresent(renderer);

    rend_game();

    //end my drawings

    //wait_until_key_press();
    //get_mouse_coordinates();
    quit_SDL(window, renderer);
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

    printf("Open [%d][%d]\n", i, j);
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

    printf("Flag/Unflag [%d][%d]\n", i, j);
}

void get_user_input(int mouseX, int mouseY, bool leftMouseClicked)   //respond to mouse events
{
    int i, j;

    if(mouseX >= (SCREEN_WIDTH - blockSize*boardWidth) / 2 &&
       mouseX < (SCREEN_WIDTH + blockSize*boardWidth) / 2 &&
       mouseY >= (SCREEN_HEIGHT - blockSize*boardHeight) / 2 &&
       mouseY < (SCREEN_HEIGHT + blockSize*boardHeight) / 2)
    {
        j = ((mouseX - (SCREEN_WIDTH - blockSize*boardWidth)/2)) / blockSize;
        i = ((mouseY - (SCREEN_HEIGHT - blockSize*boardHeight)/2))  / blockSize;
    }
    else return;

    if(leftMouseClicked)
    {
        if(board[i][j] == '*')
            open_block(i, j);
        else if(board[i][j] != 'F' && board[i][j] != '0')
        {
            int adjacentFlags = 0;

            if(board[i - 1][j - 1] == 'F' && i != 0 && j != 0) adjacentFlags++;
            if(board[i - 1][j] == 'F' && i != 0) adjacentFlags++;
            if(board[i - 1][j + 1] == 'F' && i != 0 && j != boardWidth) adjacentFlags++;
            if(board[i][j - 1] == 'F' && j != 0) adjacentFlags++;
            if(board[i][j + 1] == 'F' && j != boardWidth) adjacentFlags++;
            if(board[i + 1][j - 1] == 'F' && i != boardHeight && j != 0) adjacentFlags++;
            if(board[i + 1][j] == 'F' && i != boardHeight) adjacentFlags++;
            if(board[i + 1][j + 1] == 'F' && i != boardHeight && j != boardWidth) adjacentFlags++;

            if(adjacentFlags == board[i][j] - 48)
            {
                if(board[i - 1][j - 1] == '*' && i != 0 && j != 0) open_block(i - 1, j - 1);
                if(board[i - 1][j] == '*' && i != 0) open_block(i - 1, j);
                if(board[i - 1][j + 1] == '*' && i != 0 && j != boardWidth) open_block(i - 1, j + 1);
                if(board[i][j - 1] == '*' && j != 0) open_block(i, j - 1);
                if(board[i][j + 1] == '*' && j != boardWidth) open_block(i, j + 1);
                if(board[i + 1][j - 1] == '*' && i != boardHeight && j != 0) open_block(i + 1, j - 1);
                if(board[i + 1][j] == '*' && i != boardHeight) open_block(i + 1, j);
                if(board[i + 1][j + 1] == '*' && i != boardHeight && j != boardWidth) open_block(i + 1, j + 1);
            }
        }
    }
    else place_or_remove_flag(i, j);
}

bool end_game_win_check()       //check for game end conditions
{
    if(flagCounter == mineNumber && minesFlaggedCounter == mineNumber)
        return true;
    else
        return false;
}


//needs very much refactoring

void rend_game()
{
    SDL_Event e;
    //int mouseX, mouseY;

    draw_background();
    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            draw_board(board[i][j], i, j);
        }
    }

    SDL_RenderPresent(renderer);

    while(!endGameLose && !end_game_win_check())
    {
        while(SDL_PollEvent(&e) != 0)
        {
            //Wait for 10 miliseconds
            //SDL_Delay(10);

            //If there's no event
            //if(SDL_WaitEvent(&e) == 0) continue;

            //If the event is quitting SDL or ESC is pressed
            if((e.type == SDL_QUIT) || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) break;

            //If mouse is pressed
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if(e.button.button == SDL_BUTTON_LEFT)
                    get_user_input(e.button.x, e.button.y, true);
                else
                    get_user_input(e.button.x, e.button.y, false);

                draw_everything();

                cout << "Time: " << gameTime - timeElapsed << endl;
                cout << "Flag: " << mineNumber - flagCounter << endl;
                printf("(mouseX, mouseY) = (%d, %d)\n", e.button.x, e.button.y);
            }
        }

        draw_everything();
        //get_mouse_coordinates();
        //SDL_RenderPresent(renderer);
        //get_user_input();


    }

    if(endGameLose)
    {
        for(int i = 0; i < boardHeight; i++)
        {
            for(int j = 0; j < boardWidth; j++)
            {
                draw_board(board[i][j], i, j);
            }
        }
        cout << "GAME OVER" << endl;

        //sleep(1000);
        //a delay for a few seconds
        //then clear everything and draw game lose message and retry/home/quit button
    }

    if(end_game_win_check())
    {
        cout << "Time to complete: " << gameTime - timeElapsed << endl;
        cout << "YOU WIN!" << endl;

        //sleep(1000);
        //a delay for a few seconds
        //then clear everything and draw game win message and retry/home/quit button
    }
}


