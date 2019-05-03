#include <iostream>
#include <vector>
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

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define textWidth 20
#define textHeight 40

using namespace std;

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

    if(TTF_Init() == -1)
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

void initialize_mine_positions();
void calculate_mines(int i, int j);
void reveal_blocks(int i, int j);
void open_block(int i, int j);
void place_or_remove_flag(int i, int j);
void handle_mouse_background(int mouseX, int mouseY, bool leftMouseClicked);
void get_mouse_coordinates();
bool end_game_win_check();
void rend_game();
void play_game();

int boardWidth;
int boardHeight;
int mineNumber;
int blockSize;
vector<vector<char>> board;
vector<vector<char>> boardMinePositions;
int flagCounter;
int minesFlaggedCounter;
bool endGameLose;
time_t initGameTime;
time_t currentGameTime;

SDL_Renderer* renderer;
SDL_Color Color1 = {0, 0, 255};
SDL_Color Color2 = {0, 255, 0};
SDL_Color Color3 = {255, 0, 0};
SDL_Color Color4 = {128, 0, 255};
SDL_Color Color5 = {255, 128, 0};
SDL_Color Color6 = {0, 128, 128};
SDL_Color Color7 = {128, 128, 64};
SDL_Color Color8 = {0, 0, 0};
SDL_Color TextColor = {255, 255, 255};

void set_game_difficulty(int gameDifficulty)        //determine game properties based on difficulty
{
    switch(gameDifficulty)
    {
        case 1:     //easy
            {
                boardWidth = 9;
                boardHeight = 9;
                mineNumber = 10;
                blockSize = 64;
                break;
            }

        case 2:     //medium
            {
                boardWidth = 16;
                boardHeight = 16;
                mineNumber = 40;
                blockSize = 48;
                break;
            }

        case 3:     //hard
            {
                boardWidth = 30;
                boardHeight = 16;
                mineNumber = 99;
                blockSize = 40;
                break;
            }

        case 4:     //expert
            {
                boardWidth = 30;
                boardHeight = 24;
                mineNumber = 150;
                blockSize = 32;
                break;
            }
    }

    board = vector<vector<char>>(boardHeight, vector<char>(boardWidth, '*'));
    boardMinePositions = vector<vector<char>>(boardHeight, vector<char>(boardWidth, '0'));

    printf("Board %dx%d\n", boardHeight, boardWidth);
}

void draw_menu()        //draw the menu
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    SDL_Rect textMenu;

    SDL_Surface* title = TTF_RenderText_Solid(consolasFont, "MINESWEEPER", TextColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, title);
    textMenu.x = (SCREEN_WIDTH - textWidth*2 * 11) / 2;
    textMenu.y = SCREEN_HEIGHT/4 - textHeight * 2 / 2;
    textMenu.w = textWidth*2 * 11;
    textMenu.h = textHeight*2;
    SDL_RenderCopy(renderer, titleTexture, NULL, &textMenu);

    SDL_Surface* easy = TTF_RenderText_Solid(consolasFont, "Easy", TextColor);
    SDL_Texture* easyTexture = SDL_CreateTextureFromSurface(renderer, easy);
    textMenu.x = SCREEN_WIDTH/4 - textWidth * 4 / 2;
    textMenu.y = (SCREEN_HEIGHT - textHeight) / 2;
    textMenu.w = textWidth * 4;
    textMenu.h = textHeight;
    SDL_RenderCopy(renderer, easyTexture, NULL, &textMenu);

    SDL_Surface* medium = TTF_RenderText_Solid(consolasFont, "Medium", TextColor);
    SDL_Texture* mediumTexture = SDL_CreateTextureFromSurface(renderer, medium);
    textMenu.x = SCREEN_WIDTH*3/4 - textWidth * 6 / 2;
    textMenu.y = (SCREEN_HEIGHT - textHeight) / 2;
    textMenu.w = textWidth * 6;
    textMenu.h = textHeight;
    SDL_RenderCopy(renderer, mediumTexture, NULL, &textMenu);

    SDL_Surface* hard = TTF_RenderText_Solid(consolasFont, "Hard", TextColor);
    SDL_Texture* hardTexture = SDL_CreateTextureFromSurface(renderer, hard);
    textMenu.x = SCREEN_WIDTH/4 - textWidth * 4 / 2;
    textMenu.y = SCREEN_HEIGHT*3/4 - textHeight / 2;
    textMenu.w = textWidth * 4;
    textMenu.h = textHeight;
    SDL_RenderCopy(renderer, hardTexture, NULL, &textMenu);

    SDL_Surface* expert = TTF_RenderText_Solid(consolasFont, "Expert", TextColor);
    SDL_Texture* expertTexture = SDL_CreateTextureFromSurface(renderer, expert);
    textMenu.x = SCREEN_WIDTH*3/4 - textWidth * 6 / 2;
    textMenu.y = SCREEN_HEIGHT*3/4 - textHeight / 2;
    textMenu.w = textWidth * 6;
    textMenu.h = textHeight;
    SDL_RenderCopy(renderer, expertTexture, NULL, &textMenu);

    SDL_Rect imgMenu;

    imgMenu.x = SCREEN_WIDTH/4 - 200;
    imgMenu.y = SCREEN_HEIGHT/4 - 100;
    imgMenu.w = 200;
    imgMenu.h = 200;
    SDL_Surface* imgMine = IMG_Load("mine.bmp");
    if(imgMine == NULL)
    {
        cout << "We f*cked up IMG_Load: " << SDL_GetError() << endl;
    }
    SDL_Texture* imgMineTexture = SDL_CreateTextureFromSurface(renderer, imgMine);
    SDL_RenderCopy(renderer, imgMineTexture, NULL, &imgMenu);

    imgMenu.x = SCREEN_WIDTH*3/4;
    imgMenu.y = SCREEN_HEIGHT/4 - 100;
    imgMenu.w = 200;
    imgMenu.h = 200;
    SDL_Surface* imgFlag = IMG_Load("flag.bmp");
    if(imgFlag == NULL)
    {
        cout << "We f*cked up IMG_Load: " << SDL_GetError() << endl;
    }
    SDL_Texture* imgFlagTexture = SDL_CreateTextureFromSurface(renderer, imgFlag);
    SDL_RenderCopy(renderer, imgFlagTexture, NULL, &imgMenu);

    SDL_RenderPresent(renderer);

    TTF_CloseFont(consolasFont);

    SDL_FreeSurface(easy);
    SDL_DestroyTexture(easyTexture);
    SDL_FreeSurface(medium);
    SDL_DestroyTexture(mediumTexture);
    SDL_FreeSurface(hard);
    SDL_DestroyTexture(hardTexture);
    SDL_FreeSurface(expert);
    SDL_DestroyTexture(expertTexture);
    SDL_FreeSurface(imgMine);
    SDL_DestroyTexture(imgMineTexture);
    SDL_FreeSurface(imgFlag);
    SDL_DestroyTexture(imgFlagTexture);

    SDL_Event eventMenu;

    while(true)
    {
        while(SDL_PollEvent(&eventMenu) != 0)
        {
            //If the event is quitting SDL or ESC is pressed
            if((eventMenu.type == SDL_QUIT) || (eventMenu.type == SDL_KEYDOWN && eventMenu.key.keysym.sym == SDLK_ESCAPE)) break;

            //If mouse is pressed
            if (eventMenu.type == SDL_MOUSEBUTTONDOWN)
            {
                if(eventMenu.button.x >= SCREEN_WIDTH/4 - textWidth * 4 / 2 &&
                   eventMenu.button.x <= SCREEN_WIDTH/4 + textWidth * 4 / 2 &&
                   eventMenu.button.y >= (SCREEN_HEIGHT - textHeight) / 2 &&
                   eventMenu.button.y <= (SCREEN_HEIGHT + textHeight) / 2) {set_game_difficulty(1); return;}

                if(eventMenu.button.x >= SCREEN_WIDTH*3/4 - textWidth * 6 / 2 &&
                   eventMenu.button.x <= SCREEN_WIDTH*3/4 + textWidth * 6 / 2 &&
                   eventMenu.button.y >= (SCREEN_HEIGHT - textHeight) / 2 &&
                   eventMenu.button.y <= (SCREEN_HEIGHT + textHeight) / 2) {set_game_difficulty(2); return;}

                if(eventMenu.button.x >= SCREEN_WIDTH/4 - textWidth * 4 / 2 &&
                   eventMenu.button.x <= SCREEN_WIDTH/4 + textWidth * 4 / 2 &&
                   eventMenu.button.y >= SCREEN_HEIGHT*3/4 - textHeight / 2 &&
                   eventMenu.button.y <= SCREEN_HEIGHT*3/4 + textHeight / 2) {set_game_difficulty(3); return;}

                if(eventMenu.button.x >= SCREEN_WIDTH*3/4 - textWidth * 6 / 2 &&
                   eventMenu.button.x <= SCREEN_WIDTH*3/4 + textWidth * 6 / 2 &&
                   eventMenu.button.y >= SCREEN_HEIGHT*3/4 - textHeight / 2 &&
                   eventMenu.button.y <= SCREEN_HEIGHT*3/4 + textHeight / 2) {set_game_difficulty(4); return;}
            }
        }
    }
}

void draw_background()       //draw the background
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Rect border;

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    border.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 - 1;
    border.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 - 1;
    border.w = blockSize*boardWidth + 2;
    border.h = blockSize*boardHeight + 2;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &border);

    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            border.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j;
            border.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i;
            border.w = blockSize;
            border.h = blockSize;
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &border);
        }
    }

    SDL_Rect textBackground;

    SDL_Surface* flagBackground = TTF_RenderText_Solid(consolasFont, "Flag:", TextColor);
    SDL_Texture* flagBackgroundTexture = SDL_CreateTextureFromSurface(renderer, flagBackground);
    textBackground.x = 0;
    textBackground.y = 0;
    textBackground.w = textWidth * 5;
    textBackground.h = textHeight;
    SDL_RenderCopy(renderer, flagBackgroundTexture, NULL, &textBackground);

    SDL_Surface* timeBackground = TTF_RenderText_Solid(consolasFont, "Time:", TextColor);
    SDL_Texture* timeBackgroundTexture = SDL_CreateTextureFromSurface(renderer, timeBackground);
    textBackground.x = SCREEN_WIDTH - textWidth * 5;
    textBackground.y = 0;
    textBackground.w = textWidth * 5;
    textBackground.h = textHeight;
    SDL_RenderCopy(renderer, timeBackgroundTexture, NULL, &textBackground);

    SDL_Surface* backBackground = TTF_RenderText_Solid(consolasFont, "Back", TextColor);
    SDL_Texture* backBackgroundTexture = SDL_CreateTextureFromSurface(renderer, backBackground);
    textBackground.x = 0;
    textBackground.y = SCREEN_HEIGHT - textHeight;
    textBackground.w = textWidth * 4;
    textBackground.h = textHeight;
    SDL_RenderCopy(renderer, backBackgroundTexture, NULL, &textBackground);

    SDL_Surface* helpBackground = TTF_RenderText_Solid(consolasFont, "Help", TextColor);
    SDL_Texture* helpBackgroundTexture = SDL_CreateTextureFromSurface(renderer, helpBackground);
    textBackground.x = SCREEN_WIDTH - textWidth * 4;
    textBackground.y = SCREEN_HEIGHT - textHeight;
    textBackground.w = textWidth * 4;
    textBackground.h = textHeight;
    SDL_RenderCopy(renderer, helpBackgroundTexture, NULL, &textBackground);

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
        cout << "We f*cked up TTF_OpenFont: " << SDL_GetError() << endl;
    }

    SDL_Rect block;
    block.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + 1;
    block.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + 1;
    block.w = blockSize - 2;
    block.h = blockSize - 2;

    SDL_Rect numberBackground;
    numberBackground.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + blockSize / 4;
    numberBackground.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + blockSize / 8;
    numberBackground.w = blockSize / 2;
    numberBackground.h = blockSize * 3 / 4;

    SDL_Rect imgBackground;
    imgBackground.x = SCREEN_WIDTH/2 - (blockSize*boardWidth)/2 + blockSize*j + 1;
    imgBackground.y = SCREEN_HEIGHT/2 - (blockSize*boardHeight)/2 + blockSize*i + 1;
    imgBackground.w = blockSize - 2;
    imgBackground.h = blockSize - 2;

    switch(blockProperty)   //draw numbers, mines and flags
    {
        case '*':       //hidden
            SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case 'X':       //mine
        {
            SDL_Surface* imgMine = IMG_Load("mine.bmp");
            if(imgMine == NULL)
            {
                cout << "We f*cked up IMG_Load: " << SDL_GetError() << endl;
            }
            SDL_Texture* imgMineTexture = SDL_CreateTextureFromSurface(renderer, imgMine);
            SDL_RenderCopy(renderer, imgMineTexture, NULL, &imgBackground);

            SDL_FreeSurface(imgMine);
            SDL_DestroyTexture(imgMineTexture);
            break;
        }

        case 'F':       //flag
        {
            SDL_Surface* imgFlag = IMG_Load("flag.bmp");
            if(imgFlag == NULL)
            {
                cout << "We f*cked up IMG_Load: " << SDL_GetError() << endl;
            }
            SDL_Texture* imgFlagTexture = SDL_CreateTextureFromSurface(renderer, imgFlag);
            SDL_RenderCopy(renderer, imgFlagTexture, NULL, &imgBackground);

            SDL_FreeSurface(imgFlag);
            SDL_DestroyTexture(imgFlagTexture);
            break;
        }

        case '0':       //zero
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);
            break;

        case '1':       //one
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* one = TTF_RenderText_Solid(segoeUIFont, "1", Color1);
            SDL_Texture* oneTexture = SDL_CreateTextureFromSurface(renderer, one);
            SDL_RenderCopy(renderer, oneTexture, NULL, &numberBackground);

            SDL_FreeSurface(one);
            SDL_DestroyTexture(oneTexture);
            break;
        }

        case '2':       //two
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* two = TTF_RenderText_Solid(segoeUIFont, "2", Color2);
            SDL_Texture* twoTexture = SDL_CreateTextureFromSurface(renderer, two);
            SDL_RenderCopy(renderer, twoTexture, NULL, &numberBackground);

            SDL_FreeSurface(two);
            SDL_DestroyTexture(twoTexture);
            break;
        }

        case '3':       //three
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* three = TTF_RenderText_Solid(segoeUIFont, "3", Color3);
            SDL_Texture* threeTexture = SDL_CreateTextureFromSurface(renderer, three);
            SDL_RenderCopy(renderer, threeTexture, NULL, &numberBackground);

            SDL_FreeSurface(three);
            SDL_DestroyTexture(threeTexture);
            break;
        }

        case '4':       //four
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* four = TTF_RenderText_Solid(segoeUIFont, "4", Color4);
            SDL_Texture* fourTexture = SDL_CreateTextureFromSurface(renderer, four);
            SDL_RenderCopy(renderer, fourTexture, NULL, &numberBackground);

            SDL_FreeSurface(four);
            SDL_DestroyTexture(fourTexture);
            break;
        }

        case '5':       //five
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* five = TTF_RenderText_Solid(segoeUIFont, "5", Color5);
            SDL_Texture* fiveTexture = SDL_CreateTextureFromSurface(renderer, five);
            SDL_RenderCopy(renderer, fiveTexture, NULL, &numberBackground);

            SDL_FreeSurface(five);
            SDL_DestroyTexture(fiveTexture);
            break;
        }

        case '6':       //six
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* six = TTF_RenderText_Solid(segoeUIFont, "6", Color6);
            SDL_Texture* sixTexture = SDL_CreateTextureFromSurface(renderer, six);
            SDL_RenderCopy(renderer, sixTexture, NULL, &numberBackground);

            SDL_FreeSurface(six);
            SDL_DestroyTexture(sixTexture);
            break;
        }

        case '7':       //seven
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* seven = TTF_RenderText_Solid(segoeUIFont, "7", Color7);
            SDL_Texture* sevenTexture = SDL_CreateTextureFromSurface(renderer, seven);
            SDL_RenderCopy(renderer, sevenTexture, NULL, &numberBackground);

            SDL_FreeSurface(seven);
            SDL_DestroyTexture(sevenTexture);
            break;
        }

        case '8':       //eight
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &block);

            SDL_Surface* eight = TTF_RenderText_Solid(segoeUIFont, "8", Color8);
            SDL_Texture* eightTexture = SDL_CreateTextureFromSurface(renderer, eight);
            SDL_RenderCopy(renderer, eightTexture, NULL, &numberBackground);

            SDL_FreeSurface(eight);
            SDL_DestroyTexture(eightTexture);
            break;
        }

        //needs refactoring
    }

    TTF_CloseFont(segoeUIFont);
}

void draw_flag()        //draw current flag
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Surface* flagBackgroundNumber = TTF_RenderText_Solid(consolasFont, to_string(mineNumber - flagCounter).c_str(), TextColor);
    SDL_Texture* flagBackgroundNumberTexture = SDL_CreateTextureFromSurface(renderer, flagBackgroundNumber);
    SDL_Rect textBackground;
    textBackground.x = 0;
    textBackground.y = 0 + textHeight;
    textBackground.w = textWidth * strlen(to_string(mineNumber - flagCounter).c_str());
    textBackground.h = textHeight;
    SDL_RenderCopy(renderer, flagBackgroundNumberTexture, NULL, &textBackground);

    TTF_CloseFont(consolasFont);

    SDL_FreeSurface(flagBackgroundNumber);
    SDL_DestroyTexture(flagBackgroundNumberTexture);
}

void draw_time()        //draw current time
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Surface* timeBackgroundNumber = TTF_RenderText_Solid(consolasFont, to_string(currentGameTime - initGameTime).c_str(), TextColor);
    SDL_Texture* timeBackgroundNumberTexture = SDL_CreateTextureFromSurface(renderer, timeBackgroundNumber);
    SDL_Rect textBackground;
    textBackground.x = SCREEN_WIDTH - textWidth * (int(log10(currentGameTime - initGameTime)) + 1);
    textBackground.y = 0 + textHeight;
    textBackground.w = textWidth * strlen(to_string(currentGameTime - initGameTime).c_str());
    textBackground.h = textHeight;
    SDL_RenderCopy(renderer, timeBackgroundNumberTexture, NULL, &textBackground);

    TTF_CloseFont(consolasFont);

    SDL_FreeSurface(timeBackgroundNumber);
    SDL_DestroyTexture(timeBackgroundNumberTexture);
}

void draw_everything()      //draw everything on the screen
{
    draw_background();

    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            draw_board(board[i][j], i, j);
        }
    }

    currentGameTime = time(0);

    draw_time();
    draw_flag();

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    SDL_Window* window;
    init_SDL(window, renderer);

    play_game();

    quit_SDL(window, renderer);
    return 0;
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

    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            cout << boardMinePositions[i][j] << " ";
        }
        cout << endl;
    }
}

void reveal_blocks(int i, int j)        //reveal block and adjacent blocks when opened
{
    if(i >= 0 && i < boardHeight && j >= 0 && j < boardWidth && board[i][j] == '*' && boardMinePositions[i][j] != 'X')
    {
        board[i][j] = boardMinePositions[i][j];

        if(boardMinePositions[i][j] == '0')
        {
            if(i != 0 && j != 0 && board[i - 1][j - 1] == '*') reveal_blocks(i - 1, j - 1);
            if(i != 0 && board[i - 1][j] == '*') reveal_blocks(i - 1, j);
            if(i != 0 && j != boardWidth - 1 && board[i - 1][j + 1] == '*') reveal_blocks(i - 1, j + 1);
            if(j != 0 && board[i][j - 1] == '*') reveal_blocks(i, j - 1);
            if(j != boardWidth - 1 && board[i][j + 1] == '*') reveal_blocks(i, j + 1);
            if(i != boardHeight - 1 && j != 0 && board[i + 1][j - 1] == '*') reveal_blocks(i + 1, j - 1);
            if(i != boardHeight - 1 && board[i + 1][j] == '*') reveal_blocks(i + 1, j);
            if(i != boardHeight - 1 && j != boardWidth - 1 && board[i + 1][j + 1] == '*') reveal_blocks(i + 1, j + 1);
        }
    }
    printf("Reveal [%d][%d]\n", i, j);
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

void handle_mouse_background(int mouseX, int mouseY, bool leftMouseClicked)   //respond to mouse events
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

            if(i != 0 && j != 0 && board[i - 1][j - 1] == 'F') adjacentFlags++;
            if(i != 0 && board[i - 1][j] == 'F') adjacentFlags++;
            if(i != 0 && j != boardWidth - 1 && board[i - 1][j + 1] == 'F') adjacentFlags++;
            if(j != 0 && board[i][j - 1] == 'F') adjacentFlags++;
            if(j != boardWidth - 1 && board[i][j + 1] == 'F') adjacentFlags++;
            if(i != boardHeight - 1 && j != 0 && board[i + 1][j - 1] == 'F') adjacentFlags++;
            if(i != boardHeight - 1 && board[i + 1][j] == 'F') adjacentFlags++;
            if(i != boardHeight - 1 && j != boardWidth - 1 && board[i + 1][j + 1] == 'F') adjacentFlags++;

            if(adjacentFlags == board[i][j] - 48)
            {
                if(i != 0 && j != 0 && board[i - 1][j - 1] == '*') open_block(i - 1, j - 1);
                if(i != 0 && board[i - 1][j] == '*') open_block(i - 1, j);
                if(i != 0 && j != boardWidth - 1 && board[i - 1][j + 1] == '*') open_block(i - 1, j + 1);
                if(j != 0 && board[i][j - 1] == '*') open_block(i, j - 1);
                if(j != boardWidth - 1 && board[i][j + 1] == '*') open_block(i, j + 1);
                if(i != boardHeight - 1 && j != 0 && board[i + 1][j - 1] == '*') open_block(i + 1, j - 1);
                if(i != boardHeight - 1 && board[i + 1][j] == '*') open_block(i + 1, j);
                if(i != boardHeight - 1 && j != boardWidth - 1 && board[i + 1][j + 1] == '*') open_block(i + 1, j + 1);
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

void rend_game()        //process the game
{
    TTF_Font* consolasFont = TTF_OpenFont("consola.ttf", 128);
    if(consolasFont == NULL)
    {
        cout << "We f*cked up TTF_OpenFont : \n" << SDL_GetError();
    }

    SDL_Event eventBackground;

    initGameTime = time(0);

    unsigned int initialTime = 0, currentTime;

    draw_everything();

    while(!endGameLose && !end_game_win_check())
    {
        while(SDL_PollEvent(&eventBackground) != 0)
        {
            //If the event is quitting SDL or ESC is pressed
            if((eventBackground.type == SDL_QUIT) || (eventBackground.type == SDL_KEYDOWN && eventBackground.key.keysym.sym == SDLK_ESCAPE)) break;

            //If mouse is pressed
            if (eventBackground.type == SDL_MOUSEBUTTONDOWN)
            {
                if(eventBackground.button.button == SDL_BUTTON_LEFT)
                    handle_mouse_background(eventBackground.button.x, eventBackground.button.y, true);
                else
                    handle_mouse_background(eventBackground.button.x, eventBackground.button.y, false);

                draw_everything();

                cout << "Time: " << currentGameTime - initGameTime << endl;
                cout << "Flag: " << mineNumber - flagCounter << endl;
                printf("(mouseX, mouseY) = (%d, %d)\n", eventBackground.button.x, eventBackground.button.y);
            }
        }

        currentTime = SDL_GetTicks();
        if(currentTime - initialTime >= 1000)
        {
            draw_everything();
            initialTime = currentTime;
        }
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

        SDL_Rect textBackground;

        SDL_Surface* gameLose = TTF_RenderText_Solid(consolasFont, "GAME OVER", TextColor);
        SDL_Texture* gameLoseTexture = SDL_CreateTextureFromSurface(renderer, gameLose);
        textBackground.x = (SCREEN_WIDTH - textWidth*9) / 2;
        textBackground.y = 0;
        textBackground.w = textWidth * 9;
        textBackground.h = textHeight;
        SDL_RenderCopy(renderer, gameLoseTexture, NULL, &textBackground);

        SDL_RenderPresent(renderer);

        SDL_Delay(2000);

        SDL_FreeSurface(gameLose);
        SDL_DestroyTexture(gameLoseTexture);

        TTF_CloseFont(consolasFont);

        play_game();
    }

    if(end_game_win_check())
    {
        cout << "Time to complete: " << currentGameTime - initGameTime << endl;
        cout << "YOU WIN!" << endl;

        SDL_Rect textBackground;

        SDL_Surface* gameWin = TTF_RenderText_Solid(consolasFont, "YOU WIN!", TextColor);
        SDL_Texture* gameWinTexture = SDL_CreateTextureFromSurface(renderer, gameWin);
        textBackground.x = (SCREEN_WIDTH - textWidth*8) / 2;
        textBackground.y = 0;
        textBackground.w = textWidth * 8;
        textBackground.h = textHeight;
        SDL_RenderCopy(renderer, gameWinTexture, NULL, &textBackground);

        SDL_RenderPresent(renderer);

        SDL_Delay(2000);

        SDL_FreeSurface(gameWin);
        SDL_DestroyTexture(gameWinTexture);

        TTF_CloseFont(consolasFont);

        play_game();
    }
}

void play_game()        //execute the game
{

    endGameLose = false;
    flagCounter = 0;
    minesFlaggedCounter = 0;

    draw_menu();
    initialize_mine_positions();
    rend_game();
}
