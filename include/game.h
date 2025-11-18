//
// Created by dan on 11/17/25.
//

#ifndef GAME_H
#define GAME_H
#include "vector2d.h"
#include <SDL3/SDL_video.h>

#include "rigidbody2d.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"

#define GAME_INITIAL_OBJECTS_CAP 64
#define GAME_SIMULATOR_GRAVITY_CONST (6.674e-11)
#define GAME_SIMULATOR_MIN_DIST (10)

typedef struct {
    Vector2D vel;
    Vector2D pos;
    double zoom;
} Game_Camera;

Game_Camera *Game_Camera_Create();
void Game_Camera_Destroy(Game_Camera *cam);

Vector2D Game_GetScreenCoords(const Game_Camera *cam, SDL_Window *window, const Vector2D *pos);
Vector2D Game_GetGameCoords(const Game_Camera *cam, SDL_Window *window, const Vector2D *pos);

typedef enum {
    GAME_MATERIALTYPE_DEBUG = 0,
    GAME_MATERIALTYPE_SPRITE = 1,
} Game_MaterialType;

typedef struct {
    Game_MaterialType type;
    SDL_Color color;
    SDL_Texture *texture;
} Game_Material;

Game_Material *Game_Material_Create();
void Game_Material_Destroy(Game_Material *mat);

typedef struct {
    size_t index;
    Rigidbody2D *rbody;
    Game_Material *material;
    Vector2D size;
    int active;
} Game_Object;

Game_Object *Game_Object_Create();
Game_Object *Game_Object_CreateAt(double x, double y);
void Game_Object_Destroy(Game_Object *obj);


typedef struct {
    int left;
    int right;
    int up;
    int down;
} Game_KeyState;

typedef struct {
    Uint64 prev_ticks;
    Game_Camera *cam;
    Game_KeyState *key_state;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Game_Object **objects;
    size_t n_objects;
    size_t objects_cap;
    Vector2D global_gravity;
    double delta_time_scale;
} Game_AppState;

Game_AppState *Game_AppState_Create();

void Game_AddObject(Game_AppState *state, Game_Object *obj);
void Game_RemoveObject(Game_AppState *state, Game_Object *obj);

void Game_AppState_Destroy(Game_AppState *state);

void Game_SimulationStep(Game_AppState *state, const double delta_time);
void Game_PrintObjects(const Game_AppState *state);
void Game_RenderObjects(const Game_AppState *state);
void Game_Compute(Game_AppState *state);
void Game_Render(const Game_AppState *state, int clear_screen);

SDL_Texture *Game_TextureFromPNG(SDL_Renderer *renderer, const char *path);

void Game_RenderPolygon(SDL_Renderer *renderer, const Vector2D *pos, const double radius, const int n_points);
void Game_RenderCircle(SDL_Renderer *renderer, const Vector2D *pos, const double radius);

#endif //GAME_H
