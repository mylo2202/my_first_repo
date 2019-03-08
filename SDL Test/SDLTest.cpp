#include <bits/stdc++.h>
#include <SDL.h>

using namespace std;

void logSDLError(std::ostream& os, const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if(fatal)
    {
        SDL_Quit();
        exit(1);
    }
}

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const string WINDOW_TITLE = "MICROSOFT";

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logSDLError(std::cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true)
    {
        if ( SDL_WaitEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer);

    SDL_RenderClear(renderer);

    SDL_Rect filled_rect;

    filled_rect.x = SCREEN_WIDTH/2 - 240;
    filled_rect.y = SCREEN_HEIGHT/2 - 240;
    filled_rect.w = 240;
    filled_rect.h = 240;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &filled_rect);

    filled_rect.x = SCREEN_WIDTH/2 - 240;
    filled_rect.y = SCREEN_HEIGHT/2;
    filled_rect.w = 240;
    filled_rect.h = 240;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &filled_rect);

    filled_rect.x = SCREEN_WIDTH/2;
    filled_rect.y = SCREEN_HEIGHT/2 - 240;
    filled_rect.w = 240;
    filled_rect.h = 240;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &filled_rect);

    filled_rect.x = SCREEN_WIDTH/2;
    filled_rect.y = SCREEN_HEIGHT/2;
    filled_rect.w = 240;
    filled_rect.h = 240;
    SDL_SetRenderDrawColor(renderer, 225, 255, 0, 255);
    SDL_RenderFillRect(renderer, &filled_rect);

    SDL_RenderPresent(renderer);

    waitUntilKeyPressed();
    quitSDL(window, renderer);

    return 0;
}
