#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define SPEED 5
#define MARIO_STANDINGR "res/mario-standing.png"
#define MARIO_WALKING_1R "res/mario-walking-1.png"
#define MARIO_WALKING_2R "res/mario-walking-2.png"
#define MARIO_WALKING_3R "res/mario-walking-3.png"
#define MARIO_JUMPINGR "res/mario-jumping.png"
#define MARIO_STANDINGL "res/mario-standing-left.png"
#define MARIO_WALKING_1L "res/mario-walking-1-left.png"
#define MARIO_WALKING_2L "res/mario-walking-2-left.png"
#define MARIO_WALKING_3L "res/mario-walking-3-left.png"
#define MARIO_JUMPINGL "res/mario-jumping-left.png"
#define GROUND "res/ground.png"
#define GROUND_HEIGHT 380
#define MARIO_WIDTH 100
#define MARIO_HEIGHT 200
#define GRAVITY 0.8f
#define JUMP_VELOCITY 20

typedef struct {
    float x,y,dy;
    SDL_Texture *right[5], *left[5];
    int onGround;
    int texState; // possible values 0 1 2 3
} Sprite;

typedef struct {
  Sprite mario;
  SDL_Texture *ground;
  int texState;
  SDL_Renderer *rend;
} GameState;

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
void destroySDL(SDL_Window *win, SDL_Renderer *rend) {
  // close resources
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  // quit
  SDL_Quit();
}

SDL_Texture *createTexture(const char* file, SDL_Renderer *rend) {
  SDL_Texture *tex;
  SDL_Surface *surf = IMG_Load(file);
  if(surf==NULL) {
    printf("Error: couldn't find image with name %s: %s\n", file, SDL_GetError());
    return NULL;
  }

  tex = SDL_CreateTextureFromSurface(rend,surf);
  SDL_FreeSurface(surf);
  if(tex==NULL) {
    printf("Error: couldn't create texture with file name %s: %s\n", file, SDL_GetError());
    return NULL;
  }
  return tex;
}

int initGame(GameState *game) {
  char* right[5] = {MARIO_STANDINGR, MARIO_WALKING_1R, MARIO_WALKING_2R, MARIO_WALKING_3R, MARIO_JUMPINGR};
  char* left[5] = {MARIO_STANDINGL, MARIO_WALKING_1L, MARIO_WALKING_2L, MARIO_WALKING_3L, MARIO_JUMPINGL};
  game->mario.x=50;
  game->mario.y=WINDOW_HEIGHT-GROUND_HEIGHT;
  game->mario.texState=1;
  game->mario.onGround=1;
  game->texState=1;
  game->mario.dy=0;

  for(int i=0; i<5; i++) {
    game->mario.right[i] = createTexture(right[i], game->rend);
    if(game->mario.right[i]==NULL) return 0;
  }
  for(int i=0; i<5; i++) {
    game->mario.left[i] = createTexture(left[i], game->rend);
    if(game->mario.left[i]==NULL) return 0;
  }
  game->ground = createTexture(GROUND, game->rend);
  if(game->ground==NULL) return 0;

  return 1;
}
void destroyGame(GameState* game) {
  for(int i=0; i<5; i++) {
    SDL_DestroyTexture(game->mario.right[i]);
  }
  for(int i=0; i<5; i++) {
    SDL_DestroyTexture(game->mario.left[i]);
  }
}

void renderGame(GameState *game) {
  SDL_SetRenderDrawColor(game->rend,0,255,0,255); // set drawing color to green
  SDL_RenderClear(game->rend); // get screen (background) to green

  SDL_SetRenderDrawColor(game->rend,255,255,255,255);

  SDL_Rect marioRect = {game->mario.x,game->mario.y,MARIO_WIDTH,MARIO_HEIGHT};
  int s=game->mario.texState;
  SDL_Rect groundRect = {0,0,1280,720};
  SDL_RenderCopy(game->rend, game->ground, NULL, &groundRect);
  if(s<0)
    SDL_RenderCopy(game->rend, game->mario.left[(-s)/10], NULL, &marioRect);
  else
    SDL_RenderCopy(game->rend, game->mario.right[s/10], NULL, &marioRect);

  SDL_RenderPresent(game->rend);
}

void verticalVelocity(GameState *game) {
  Sprite *mario = &game->mario;
  mario->y+=mario->dy;

  if(!game->mario.onGround) {
    mario->dy += GRAVITY;
  }
}

void collision(GameState* game) {
  if(game->mario.y >= WINDOW_HEIGHT-GROUND_HEIGHT) {
    game->mario.y=WINDOW_HEIGHT-GROUND_HEIGHT;
    game->mario.onGround = 1;
  } else if(game->mario.y <= 0) {
    game->mario.y = 0;
  }
  if(game->mario.x + MARIO_WIDTH >= WINDOW_WIDTH) {
    game->mario.x=WINDOW_WIDTH-MARIO_WIDTH;
  } else if(game->mario.x <= 0) {
    game->mario.x=0;
  }
}

void processEvent(SDL_Event *event, GameState *game) {
  while(SDL_PollEvent(event)) {
    switch ((*event).type) {
      case SDL_QUIT: {
        game->texState=0;
        break;
      }
      case SDL_KEYDOWN: {
        switch ((*event).key.keysym.sym) {
          case SDLK_ESCAPE: {
            game->texState=0;
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
  if(kstate[SDL_SCANCODE_LSHIFT]) s*=2;
  int *texState = &game->mario.texState;
  if(kstate[SDL_SCANCODE_UP]) {
    if(game->mario.onGround) {
      game->mario.dy=-JUMP_VELOCITY;
      game->mario.onGround=0;
    }
  }
  if(kstate[SDL_SCANCODE_RIGHT] && kstate[SDL_SCANCODE_LEFT]) {
    if(*texState>0) *texState=1;
    else *texState=-1;
  } else if(kstate[SDL_SCANCODE_RIGHT]) {
    game->mario.x+=s;
    *texState = (*texState<2 || *texState==39?10:game->mario.texState+1);
  }else if(kstate[SDL_SCANCODE_LEFT]) {
    game->mario.x-=s;
    *texState = ((*texState>-2 || *texState==-39)?-10:game->mario.texState-1);
  } else {
    if(*texState>0) *texState=1;
    else *texState=-1;
  }
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

  while(game.texState) {
    processEvent(&event, &game); // catch any user input events, if occured
    verticalVelocity(&game); // processing the vertical velocity for each frame
    collision(&game);
    renderGame(&game); // render on the screen
  }

  destroyGame(&game);
  destroySDL(win,rend);
  return 0;
}