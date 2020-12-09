#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (1280)
#define WINDOW_HEIGHT (720)
#define SPEED (5);

typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  Position pos;
  SDL_Texture *star;
} GameState;

int processEvent(SDL_Event *event, GameState *game) {
  int state=1;
  while(SDL_PollEvent(event)) {
    switch ((*event).type) {
      case SDL_QUIT: {
        state=0;
        break;
      }
      case SDL_KEYDOWN: {
        switch ((*event).key.keysym.sym) {
          case SDLK_ESCAPE: {
            state=0;
            break;
          }
          default:
            break;
        }
      }
      default:
        break;
    }
  }
  const Uint8 *kstate = SDL_GetKeyboardState(NULL);
  int s=SPEED;
  if(kstate[SDL_SCANCODE_LSHIFT]) {
    s*=2;
  }
  if(kstate[SDL_SCANCODE_LEFT])
    game->pos.x-=s;
  else if(kstate[SDL_SCANCODE_RIGHT])
    game->pos.x+=s;
  if(kstate[SDL_SCANCODE_UP])
    game->pos.y-=s;
  else if(kstate[SDL_SCANCODE_DOWN])
    game->pos.y+=s;
  return state;
}

void render(SDL_Renderer *rend, GameState *game) {
  SDL_SetRenderDrawColor(rend,0,0,255,255);

  SDL_RenderClear(rend);

  SDL_SetRenderDrawColor(rend,255,255,255,255);

  SDL_Rect rect = {game->pos.x,game->pos.y,100,100};
  SDL_RenderFillRect(rend,&rect);

  SDL_RenderPresent(rend);
}

int main(void) {
  GameState game;
  game.pos.x=220; game.pos.y=140;
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)!=0) {
    printf("Error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Window* win;
  SDL_Renderer* rend;

  win = SDL_CreateWindow("Viren Khatri",           // Window Title
                        SDL_WINDOWPOS_UNDEFINED,   // Initial x position
                        SDL_WINDOWPOS_UNDEFINED,   // Initial y position
                        WINDOW_WIDTH,
                        WINDOW_HEIGHT,
                        0);

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win,-1,render_flags);

  int state=1;
  SDL_Event event;
  while(state) {
    // catch any events, if occured
    state=processEvent(&event, &game);

    // render on the screen
    render(rend, &game);
  }

  // close resources
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  // quit
  SDL_Quit();
  return 0;
}