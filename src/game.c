//
// Created by dan on 11/17/25.
//

#include "../include/game.h"

#include <math.h>
#include <SDL3/SDL.h>

Game_Camera *Game_Camera_Create() {
    Game_Camera *cam = SDL_malloc(sizeof(Game_Camera));
    cam->pos = Vector2D_GetZeroVec();
    cam->vel = Vector2D_GetZeroVec();
    cam->zoom = 1.0;
    return cam;
}

void Game_Camera_Destroy(Game_Camera *cam) {
    SDL_free(cam);
}

Vector2D Game_GetScreenCoords(const Game_Camera *cam, SDL_Window *window, const Vector2D *pos) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    const double wd = (double)w;
    const double hd = (double)h;
    const Vector2D screen_coords = {
        .x = (pos->x - cam->pos.x) * cam->zoom + (wd*0.5 - wd*cam->zoom*0.5),
        .y = (pos->y - cam->pos.y) * cam->zoom + (hd*0.5 - hd*cam->zoom*0.5),
    };
    return screen_coords;
}

Vector2D Game_GetGameCoords(const Game_Camera *cam, SDL_Window *window, const Vector2D *pos) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    const double wd = (double)w;
    const double hd = (double)h;
    const Vector2D game_coords = {
        .x = ( ( pos->x - (wd*0.5 - wd*cam->zoom*0.5) ) / cam->zoom ) + cam->pos.x,
        .y = ( ( pos->y - (hd*0.5 - hd*cam->zoom*0.5) ) / cam->zoom ) + cam->pos.y,
    };
    return game_coords;
}

Game_Material *Game_Material_Create() {
    Game_Material *mat = SDL_malloc(sizeof(Game_Material));
    mat->type = GAME_MATERIALTYPE_DEBUG;
    mat->color.r = 255;
    mat->color.g = 0;
    mat->color.b = 0;
    mat->color.a = 255;
    mat->texture = nullptr;
    return mat;
}

void Game_Material_Destroy(Game_Material *mat) {
    SDL_free(mat);
}

Game_Object *Game_Object_Create() {
    Game_Object *obj = SDL_malloc(sizeof(Game_Object));
    obj->active = true;
    obj->rbody = Rigidbody2D_Create();
    obj->material = Game_Material_Create();
    obj->size.x = 8;
    obj->size.y = 8;
    obj->index = -1; // makes segfault likely if Object not added to AppState but index still used
    return obj;
}

Game_Object *Game_Object_CreateAt(const double x, const double y) {
    Game_Object *obj = Game_Object_Create();
    obj->rbody->pos.x = x;
    obj->rbody->pos.y = y;
    return obj;
}

void Game_Object_Destroy(Game_Object *obj) {
    Rigidbody2D_Destroy(obj->rbody);
    Game_Material_Destroy(obj->material);
    SDL_free(obj);
}

Game_AppState *Game_AppState_Create() {
    Game_AppState *state = SDL_malloc(sizeof(Game_AppState));
    state->window = nullptr;
    state->renderer = nullptr;
    state->cam = Game_Camera_Create();
    state->prev_ticks = 0;
    // Since every field of the Game_KeyState struct is a boolean (int) and we want
    // to initialize every field to 0 (false), we can use calloc to zero out the struct
    // while allocating the memory.
    state->key_state = SDL_calloc(1, sizeof(Game_KeyState));
    state->objects_cap = GAME_INITIAL_OBJECTS_CAP;
    state->n_objects = 0;
    state->objects = SDL_malloc(state->objects_cap * sizeof(Game_Object *));
    state->global_gravity = Vector2D_GetZeroVec();
    state->delta_time_scale = 0.001;
    return state;
}

void Game_AddObject(Game_AppState *state, Game_Object *obj) {
    if (state->n_objects >= state->objects_cap) {
        state->objects_cap <<= 1; // double objects capacity
        state->objects = SDL_realloc(state->objects, state->objects_cap * sizeof(Game_Object *));
    }
    const size_t index = state->n_objects;
    state->objects[index] = obj;
    obj->index = index;
    state->n_objects++;
}

void Game_RemoveObject(Game_AppState *state, Game_Object *obj) {
    state->objects[obj->index] = nullptr;
    Game_Object_Destroy(obj);
}

void Game_AppState_Destroy(Game_AppState *state) {
    Game_Camera_Destroy(state->cam);
    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->renderer);
    SDL_free(state->key_state);
    SDL_free(state);
}

void Game_Compute(Game_AppState *state) {
    const Uint64 curr_ticks = SDL_GetTicksNS();
    const Uint64 delta = curr_ticks - state->prev_ticks;
    state->prev_ticks = curr_ticks;
    const double delta_time = ((double) delta) * state->delta_time_scale;

    // Abrupt Camera Movement
    const Vector2D scaled_cam_vec = Vector2D_Scaled(&state->cam->vel, 0.1 / state->cam->zoom);
    Vector2D_AddVec(&state->cam->pos, &scaled_cam_vec);

    Game_SimulationStep(state, delta_time);
}

void Game_Render(const Game_AppState *state, const int clear_screen) {
    if (clear_screen) {
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
        SDL_RenderClear(state->renderer);
    }
    Game_RenderObjects(state);
}

SDL_Texture *Game_TextureFromPNG(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surf = SDL_LoadPNG(path);
    SDL_Log("Guadalajara: %s", path);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    return text;
}

void RenderObject(const Game_AppState *state, const Game_Object *obj) {
    if (obj->material == nullptr) {
        SDL_SetError("Object has no material!");
        return;
    }
    if (obj->rbody == nullptr) {
        SDL_SetError("Object has no rigidbody!");
        return;
    }
    const Uint8 r = obj->material->color.r;
    const Uint8 g = obj->material->color.g;
    const Uint8 b = obj->material->color.b;
    const Uint8 a = obj->material->color.a;
    const Vector2D half_size = Vector2D_Scaled(&obj->size, 0.5);
    const Vector2D pos = Vector2D_Diff(&obj->rbody->pos, &half_size);
    const Vector2D screen_pos = Game_GetScreenCoords(state->cam, state->window, &pos);
    const Vector2D screen_size = Vector2D_Scaled(&obj->size, state->cam->zoom);
    const SDL_FRect rect = {
        .x = (float) screen_pos.x,
        .y = (float) screen_pos.y,
        .w = (float) screen_size.x,
        .h = (float) screen_size.y,
    };
    switch (obj->material->type) {
        case GAME_MATERIALTYPE_DEBUG:
            SDL_SetRenderDrawColor(state->renderer, r, g, b, a);
            SDL_RenderFillRect(state->renderer, &rect);
            break;
        case GAME_MATERIALTYPE_SPRITE:
            if (obj->material->texture == nullptr) {
                SDL_SetError("Object material has no texture!");
                return;
            }
            SDL_RenderTexture(state->renderer, obj->material->texture, nullptr, &rect);
            break;
    }
}

void Game_RenderObjects(const Game_AppState *state) {
    for (size_t i = 0; i < state->n_objects; i++) {
        RenderObject(state, state->objects[i]);
    }
}

void Game_SimulationStep(Game_AppState *state, const double delta_time) {
    for (size_t i = 0; i < state->n_objects; i++) {
        Rigidbody2D *body = state->objects[i]->rbody;
        if (!body->simulated) continue;

        Vector2D new_vel = Vector2D_FromExisting(&body->vel);
        Vector2D term1 = Vector2D_FromExisting(&body->acc);
        Vector2D_Scale(&term1, delta_time * 0.5);
        Vector2D_AddVec(&new_vel, &term1);

        Vector2D new_pos = Vector2D_FromExisting(&body->pos);
        Vector2D term2 = Vector2D_FromExisting(&new_vel);
        Vector2D_Scale(&term2, delta_time);
        Vector2D_AddVec(&new_pos, &term2);

        Vector2D new_acc = {.x=0.0,.y=0.0};
        for (size_t j = 0; j < state->n_objects; j++) {
            const Rigidbody2D *obody = state->objects[j]->rbody;
            if (!obody->has_gravity || obody == body) continue;
            Vector2D F = Vector2D_Diff(&obody->pos, &new_pos);
            const double r_abs = Vector2D_Abs(&F);
            if (r_abs < GAME_SIMULATOR_MIN_DIST) continue;
            Vector2D_Scale(&F, GAME_SIMULATOR_GRAVITY_CONST * body->mass * obody->mass * (1. / (r_abs*r_abs*r_abs)));
            Vector2D_AddVec(&new_acc, &F);
        }
        Vector2D_Scale(&new_acc, 1. / body->mass);
        Vector2D_AddVec(&new_acc, &state->global_gravity);

        Vector2D term3 = Vector2D_FromExisting(&new_acc);
        Vector2D_Scale(&term3, delta_time * 0.5);
        Vector2D_AddVec(&new_vel, &term3);

        body->pos = new_pos;
        body->vel = new_vel;
        body->acc = new_acc;
    }
}

void Game_PrintObjects(const Game_AppState *state) {
    for (size_t i = 0; i < state->n_objects; i++) {
        const Rigidbody2D *body = state->objects[i]->rbody;
        char *str = Rigidbody2D_CreateString(body);
        SDL_Log("%lu: %s\n", i, str);
        SDL_free(str);
    }
}

void Game_RenderPolygon(SDL_Renderer *renderer, const Vector2D *pos, const double radius, const int n_points) {
    SDL_FPoint *points = SDL_malloc((n_points + 1) * sizeof(SDL_FPoint));
    const double step_angle = 2 * M_PI / n_points;
    double angle = 0;
    for (size_t i = 0; i <= n_points; i++) {
        if (i == n_points) angle = 0;
        points[i].x = (float)(pos->x + cos(angle) * radius);
        points[i].y = (float)(pos->y + sin(angle) * radius);
        angle += step_angle;
    }
    SDL_RenderLines(renderer, points, n_points + 1);
    SDL_free(points);
}

void Game_RenderCircle(SDL_Renderer *renderer, const Vector2D *pos, const double radius) {
    constexpr int n_points = 64;
    Game_RenderPolygon(renderer, pos, radius, n_points);
}