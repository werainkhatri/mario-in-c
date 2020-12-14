#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 1280.0f
#define WINDOW_HEIGHT 720.0f
#define SPEED 5
#define MARIO_STANDING "res/mario-standing.png"
#define MARIO_WALKING_1 "res/mario-walking-1.png"
#define MARIO_WALKING_2 "res/mario-walking-2.png"
#define MARIO_WALKING_3 "res/mario-walking-3.png"
#define MARIO_JUMPING "res/mario-jumping.png"
#define GROUND "res/ground.png"
#define GROUND_HEIGHT 380.0f
#define MARIO_WIDTH 100.0f
#define MARIO_HEIGHT 200.0f
#define GRAVITY 0.8f
#define JUMP_VELOCITY 14.0f
#define LONGER_JUMP_ACCELARATION 0.4f // if jump key is pressed long enough, sprite jumps higher
#define BG_MOVE_PERCENT 0.1f // left and right padding (equal to percent of screen width) from where the background should start moving

typedef struct {
    float x,y,dy;
    SDL_Texture *right[5];
    int onGround;
    int texState; // possible values 0 1 2 3
} Sprite;

typedef struct {
  Sprite mario;
  SDL_Texture *ground;
  int texState;
  int bgMove;
  SDL_Renderer *rend;
} GameState;

void initSDL(SDL_Window **win, SDL_Renderer **rend) {
  SDL_Init(SDL_INIT_VIDEO);

  *win = SDL_CreateWindow("Mario by Viren Khatri",           // Window Title
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
  char* right[5] = {MARIO_STANDING, MARIO_WALKING_1, MARIO_WALKING_2, MARIO_WALKING_3, MARIO_JUMPING};
  game->mario.x=WINDOW_WIDTH/2.0f;
  game->mario.y=WINDOW_HEIGHT-GROUND_HEIGHT;
  game->mario.texState=1;
  game->mario.onGround=1;
  game->texState=1;
  game->mario.dy=0;
  game->bgMove=0;

  for(int i=0; i<5; i++) {
    game->mario.right[i] = createTexture(right[i], game->rend);
    if(game->mario.right[i]==NULL) return 0;
  }
  game->ground = createTexture(GROUND, game->rend);
  if(game->ground==NULL) return 0;

  return 1;
}
void destroyGame(GameState* game) {
  for(int i=0; i<5; i++) {
    SDL_DestroyTexture(game->mario.right[i]);
  }
}

void renderGame(GameState *game) {
  SDL_SetRenderDrawColor(game->rend,0,0,0,0); // set drawing color to black
  SDL_RenderClear(game->rend); // get screen (background) to black

  SDL_SetRenderDrawColor(game->rend,255,255,255,255);

  SDL_Rect marioRect = {game->mario.x-MARIO_WIDTH/2,game->mario.y,MARIO_WIDTH,MARIO_HEIGHT};
  int s=game->mario.texState;
  if(game->bgMove >= 1280) game->bgMove-=1280;
  else if(game->bgMove <= -1280) game->bgMove+=1280;
  for(int i=1; i>=-1; i--) {
    SDL_Rect groundRect = {i*1280+game->bgMove,0,1280,720};
    SDL_RenderCopy(game->rend, game->ground, NULL, &groundRect);
  }
  if(!game->mario.onGround) s=40*(s<0?-1:1);
  SDL_RenderCopyEx(game->rend, game->mario.right[abs(s)/10], NULL, &marioRect, 0, NULL, s<0);

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
void moveBGFrame(GameState* game) {
  if(game->mario.x < WINDOW_WIDTH*BG_MOVE_PERCENT) {
    game->bgMove+=(WINDOW_WIDTH*BG_MOVE_PERCENT-game->mario.x);
    game->mario.x=WINDOW_WIDTH*BG_MOVE_PERCENT;
  } else if(game->mario.x > WINDOW_WIDTH*(1-BG_MOVE_PERCENT)) {
    game->bgMove-=(game->mario.x-WINDOW_WIDTH*(1-BG_MOVE_PERCENT));
    game->mario.x=WINDOW_WIDTH*(1-BG_MOVE_PERCENT);
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
          case SDLK_SPACE: {
            if(game->mario.onGround) {
              game->mario.dy=-JUMP_VELOCITY;
              game->mario.onGround=0;
            }
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
  if(kstate[SDL_SCANCODE_SPACE]) {
    game->mario.dy-=LONGER_JUMP_ACCELARATION;
  }
  if(kstate[SDL_SCANCODE_RIGHT] && kstate[SDL_SCANCODE_LEFT]) {
    if(*texState>0) *texState=1;
    else *texState=-1;
  } else if(kstate[SDL_SCANCODE_RIGHT]) {
    game->mario.x+=s;
    *texState = (*texState<2 || *texState==39?10:game->mario.texState+1);
  } else if(kstate[SDL_SCANCODE_LEFT]) {
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
    collision(&game); // detect possible collisions and prevent them from being rendered
    moveBGFrame(&game); // move the background frame if the sprite is too left or too right relative to the window
    renderGame(&game); // render on the screen
  }

  destroyGame(&game);
  destroySDL(win,rend);
  return 0;
}