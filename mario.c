#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

typedef struct {
  int x;
  int y;
} Position;

typedef struct {
    Position pos;
    char* name;
    SDL_Texture *tex;
} Sprite;

typedef struct {
  Sprite mario;
  Position groundPos;
  int state;
  SDL_Renderer *rend;
} GameState;

#define WINDOW_WIDTH (1280)
#define WINDOW_HEIGHT (720)
#define SPEED (5)
#define MARIO_IMAGE "res/mario-standing.png"

void initSDL(SDL_Window **win, SDL_Renderer **rend) {
  SDL_Init(SDL_INIT_VIDEO);

  *win = SDL_CreateWindow("Viren Khatri",           // Window Title
                        SDL_WINDOWPOS_UNDEFINED,   // Initial x position
                        SDL_WINDOWPOS_UNDEFINED,   // Initial y position
                        WINDOW_WIDTH, WINDOW_HEIGHT, 0); // Dimentions of the Window
  if(*win==NULL) {
    printf("Error creating Window: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  *rend = SDL_CreateRenderer(*win,-1,render_flags);
  if(*rend==NULL) {
    printf("Error creating Renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(*win);
    SDL_Quit();
    exit(1);
  }
}
void destroySDL(SDL_Window **win, SDL_Renderer **rend) {
  // close resources
  SDL_DestroyWindow(*win);
  SDL_DestroyRenderer(*rend);
  // quit
  SDL_Quit();
}

void renderGame(GameState *game) {
  SDL_SetRenderDrawColor(game->rend,0,255,0,255); // set drawing color to green
  SDL_RenderClear(game->rend); // get screen (background) to green

  SDL_SetRenderDrawColor(game->rend,255,255,255,255);

  SDL_Rect marioRect = {game->mario.pos.x,game->mario.pos.y,100,200};
  SDL_RenderCopy(game->rend,game->mario.tex, NULL, &marioRect);

  SDL_RenderPresent(game->rend);
}

int initGame(GameState *game) {
  game->mario.pos.x=50;
  game->mario.pos.y=420;

  SDL_Surface *surf = IMG_Load(MARIO_IMAGE);
  if(surf==NULL) {
    printf("Error: couldn't find mario image: %s\n", SDL_GetError());
    return 0;
  }

  game->mario.tex = SDL_CreateTextureFromSurface(game->rend,surf);
  SDL_FreeSurface(surf);
  if(game->mario.tex==NULL) {
    printf("Error: couldn't create mario texture: %s\n", SDL_GetError());
    return 0;
  }

  return 1;
}
void destroyGame(GameState* game) {
  SDL_DestroyTexture(game->mario.tex);
}

void processEvent(SDL_Event *event, GameState *game) {
  while(SDL_PollEvent(event)) {
    switch ((*event).type) {
      case SDL_QUIT: {
        game->state=0;
        break;
      }
      case SDL_KEYDOWN: {
        switch ((*event).key.keysym.sym) {
          case SDLK_ESCAPE: {
            game->state=0;
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
    game->mario.pos.x-=s;
  else if(kstate[SDL_SCANCODE_RIGHT])
    game->mario.pos.x+=s;
  if(kstate[SDL_SCANCODE_UP])
    game->mario.pos.y-=s;
  else if(kstate[SDL_SCANCODE_DOWN])
    game->mario.pos.y+=s;
}

int main() {
  SDL_Window* win;
  SDL_Renderer* rend;
  initSDL(&win,&rend);
  GameState game;
  game.rend=rend;
  if(!initGame(&game)) {
    destroySDL(win,rend);
    return 1;
  }

  SDL_Event event;

  while(game.state) {
    // catch any events, if occured
    processEvent(&event, &game);

    // render on the screen
    renderGame(&game);
  }

  destroyGame(&game);
  destroySDL(&win,&rend);
  return 0;
}