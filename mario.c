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
} GameState;

#define WINDOW_WIDTH (1280)
#define WINDOW_HEIGHT (720)
#define SPEED (5)
#define MARIO_IMAGE "res/mario-standing.png"

int initSDL(SDL_Window *win, SDL_Renderer *rend) {
  SDL_Init(SDL_INIT_VIDEO);

  win = SDL_CreateWindow("Viren Khatri",           // Window Title
                        SDL_WINDOWPOS_UNDEFINED,   // Initial x position
                        SDL_WINDOWPOS_UNDEFINED,   // Initial y position
                        WINDOW_WIDTH, WINDOW_HEIGHT, 0); // Dimentions of the Window
  if(win==NULL) {
    printf("Error creating Window: %s\n", SDL_GetError());
    SDL_Quit();
    return 0;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win,-1,render_flags);
  if(rend==NULL) {
    printf("Error creating Renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
  }
  return 1;
}
void destroySDL(SDL_Window *win, SDL_Renderer *rend) {
  // close resources
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  // quit
  SDL_Quit();
}

void render(SDL_Renderer *rend, GameState *game) {
  SDL_SetRenderDrawColor(rend,0,255,0,255); // set drawing color to green
  SDL_RenderClear(rend); // get screen (background) to green

  SDL_SetRenderDrawColor(rend,255,255,255,255);

  SDL_Rect marioRect = {game->mario.pos.x,game->mario.pos.y,100,200};
  SDL_RenderCopy(rend,game->mario.tex, NULL, &marioRect);

  SDL_RenderPresent(rend);
}

int initGame(SDL_Renderer *rend, GameState *state) {
  state->mario.pos.x=50;
  state->mario.pos.y=420;

  SDL_Surface *surf = IMG_Load(MARIO_IMAGE);
  if(surf==NULL) {
    printf("Error: couldn't find mario image: %s\n", SDL_GetError());
    return 0;
  }

  state->mario.tex = SDL_CreateTextureFromSurface(rend,surf);
  SDL_FreeSurface(surf);
  if(state->mario.tex==NULL) {
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
  // if(!initSDL(win,rend)) return 1;
  SDL_Init(SDL_INIT_VIDEO);

  win = SDL_CreateWindow("Viren Khatri",           // Window Title
                        SDL_WINDOWPOS_UNDEFINED,   // Initial x position
                        SDL_WINDOWPOS_UNDEFINED,   // Initial y position
                        WINDOW_WIDTH, WINDOW_HEIGHT, 0); // Dimentions of the Window
  if(win==NULL) {
    printf("Error creating Window: %s\n", SDL_GetError());
    SDL_Quit();
    return 0;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  rend = SDL_CreateRenderer(win,-1,render_flags);
  if(rend==NULL) {
    printf("Error creating Renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
  }

  GameState game;
  if(initGame(rend,&game)==0) {
    destroySDL(win,rend);
    return 1;
  }

  SDL_Event event;

  while(game.state) {
    // catch any events, if occured
    processEvent(&event, &game);

    // render on the screen
    render(rend, &game);
  }

  destroyGame(&game);
  destroySDL(win,rend);
  return 0;
}