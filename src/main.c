/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/game.h"
#include "../include/string.h"

#define WINDOW_FLAGS (0) // SDL_WINDOW_FULLSCREEN

Game_Object *earth_ref = nullptr;
Game_Object *moon_ref = nullptr;
constexpr double moon_init_speed = 0.0001;
constexpr double moon_dist = 400;
unsigned warp_factor = 1;
char *warp_buf = nullptr;

void refresh_warp_buf() {
    constexpr size_t buf_size = 64;
    if (warp_buf == nullptr) {
        warp_buf = SDL_calloc(buf_size, sizeof(char));
    } else {
        for (size_t i = 0; i < buf_size; i++) warp_buf[i] = 0;
    }
    sprintf(warp_buf, "Time Warp: %ux", warp_factor);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Init(SDL_INIT_GAMEPAD);

    Game_AppState *app_state = Game_AppState_Create();

    app_state->cam->zoom = 0.5;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Simple Physics Sim", 1280, 720, WINDOW_FLAGS, &app_state->window, &app_state->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    const char* env_path_fallback = "/usr/local/share/sdl_2dengine_resources";
    const char* env_path = getenv("SDL_2DENGINE_RESOURCE_PATH");
    String *resource_path = String_FromCstr(env_path == nullptr ? env_path_fallback : env_path);
    SDL_Log("Resource Path: %s", resource_path);
    String *earth_path = String_ConcatCstr(resource_path, "/earth.png");
    String *moon_path = String_ConcatCstr(resource_path, "/moon.png");

    SDL_Log("Earth path: %s", earth_path->str);
    SDL_Log("Moon path: %s", moon_path->str);

    SDL_Texture *moon_texture = Game_TextureFromPNG(app_state->renderer, moon_path->str);
    SDL_Texture *earth_texture = Game_TextureFromPNG(app_state->renderer, earth_path->str);

    String_Destroy(resource_path);
    String_Destroy(earth_path);
    String_Destroy(moon_path);

    int w;
    int h;
    SDL_GetWindowSize(app_state->window, &w, &h);
    SDL_Log("GetWindowSize -> %d, %d", w, h);

    constexpr double earth_mass = 50000;
    constexpr double moon_mass = 0.0123 * earth_mass;

    Game_Object *earth = Game_Object_CreateAt((double)(w>>1), (double)(h>>1));
    earth->rbody->mass = earth_mass;
    earth->material->type = GAME_MATERIALTYPE_SPRITE;
    earth->material->texture = earth_texture;
    earth->size.x = 512;
    earth->size.y = 512;
    earth_ref = earth;

    Game_Object *moon = Game_Object_CreateAt((double)(w>>1), (double)(h>>1) + moon_dist);
    moon->material->type = GAME_MATERIALTYPE_SPRITE;
    moon->material->texture = moon_texture;
    moon->rbody->mass = moon_mass;
    moon->rbody->vel.x = moon_init_speed;
    moon->size.x = 128;
    moon->size.y = 128;
    moon_ref = moon;

    Game_AddObject(app_state, earth);
    Game_AddObject(app_state, moon);

    Game_PrintObjects(app_state);

    refresh_warp_buf();

    *appstate = app_state;
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    Game_AppState *game = appstate;
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_PERIOD:
                game->delta_time_scale *= 2;
                warp_factor <<= 1;
                refresh_warp_buf();
                break;
            case SDLK_COMMA:
                game->delta_time_scale /= 2;
                if (game->delta_time_scale < 0.001) game->delta_time_scale = 0.001;
                else warp_factor >>= 1;
                refresh_warp_buf();
                break;
            case SDLK_Q:
                SDL_Log("Q");
                return SDL_APP_SUCCESS;
            case SDLK_A:
            case SDLK_LEFT:
                SDL_Log("Left");
                game->key_state->left = 1;
                game->cam->vel.x = -1;
                break;
            case SDLK_D:
            case SDLK_RIGHT:
                SDL_Log("Right");
                game->key_state->right = 1;
                game->cam->vel.x = 1;
                break;
            case SDLK_S:
            case SDLK_DOWN:
                SDL_Log("Down");
                game->key_state->down = 1;
                game->cam->vel.y = 1;
                break;
            case SDLK_W:
            case SDLK_UP:
                SDL_Log("Up");
                game->key_state->up = 1;
                game->cam->vel.y = -1;
                break;
            default:
                SDL_Log("Unhandled Key Down");
                break;
        }
    } else if (event->type == SDL_EVENT_KEY_UP) {
        switch (event->key.key) {
            case SDLK_A:
            case SDLK_LEFT:
                SDL_Log("Left");
                game->key_state->left = 0;
                if (!game->key_state->right) game->cam->vel.x = 0;
                break;
            case SDLK_D:
            case SDLK_RIGHT:
                SDL_Log("Right");
                game->key_state->right = 0;
                if (!game->key_state->left) game->cam->vel.x = 0;
                break;
            case SDLK_S:
            case SDLK_DOWN:
                SDL_Log("Down");
                game->key_state->down = 0;
                if (!game->key_state->up) game->cam->vel.y = 0;
                break;
            case SDLK_W:
            case SDLK_UP:
                SDL_Log("Up");
                game->key_state->up = 0;
                if (!game->key_state->down) game->cam->vel.y = 0;
                break;
            default:
                SDL_Log("Unhandled Key Up");
                break;
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        SDL_Log("Mouse x: %f, Mouse y: %f", event->button.x, event->button.y);
        const Vector2D mouse_pos = {.x=event->button.x,.y=event->button.y};
        const Vector2D mouse_pos_game_coords = Game_GetGameCoords(game->cam, game->window, &mouse_pos);
        Vector2D r = Vector2D_Diff(&mouse_pos_game_coords, &earth_ref->rbody->pos);

        Vector2D tangent_r = {.x = r.y, .y = -r.x};
        Vector2D tangent_r_norm = Vector2D_Scaled(&tangent_r, 1. / Vector2D_Abs(&tangent_r));

        double dist = Vector2D_Abs(&r);
        double ratio = moon_dist / dist;

        Game_Object *obj = Game_Object_Create();
        obj->rbody->pos = mouse_pos_game_coords;
        obj->rbody->mass = moon_ref->rbody->mass;
        obj->rbody->vel = Vector2D_Scaled(&tangent_r_norm, moon_init_speed * ratio);
        obj->material->texture = moon_ref->material->texture;
        obj->material->type = GAME_MATERIALTYPE_SPRITE;
        obj->size = moon_ref->size;
        Game_AddObject(game, obj);

        SDL_Log("r.x = %f, r.y = %f", r.x, r.y);
    } else if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    } else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
        SDL_Log("Gamepad Button down Event!");
    } else if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        SDL_Log("Scrollwheel Event x: %f, y: %f", event->wheel.x, event->wheel.y);
        constexpr double min_zoom = 0.001;
        const double amt = event->wheel.y * game->cam->zoom * 0.5;
        game->cam->zoom += amt;
        if (game->cam->zoom < min_zoom) game->cam->zoom = min_zoom;
        SDL_Log("zoom_diff: %f, new zoom: %f", amt, game->cam->zoom);
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Game_AppState *state = appstate;

    Game_Compute(appstate);
    Game_Render(appstate, 1);

    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);

    SDL_RenderDebugText(state->renderer, 10, 10, warp_buf);

    SDL_RenderPresent(state->renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_free(warp_buf);
    warp_buf = nullptr;
    Game_AppState_Destroy(appstate);
}

