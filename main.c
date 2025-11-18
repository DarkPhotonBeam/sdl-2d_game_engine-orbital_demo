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
#include <unistd.h>

#include "game.h"

#define WINDOW_FLAGS (SDL_WINDOW_FULLSCREEN) // SDL_WINDOW_FULLSCREEN

Game_Object *earth_ref = nullptr;
Game_Object *moon_ref = nullptr;
constexpr double moon_init_speed = 0.0001;
constexpr double moon_dist = 400;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Init(SDL_INIT_GAMEPAD);

    Game_AppState *app_state = Game_AppState_Create();

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Simple Physics Sim", 2560, 1440, WINDOW_FLAGS, &app_state->window, &app_state->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    const char* env_path_fallback = "/usr/local/share/sdlvv_resources";
    const char* env_path = getenv("SDL_VELOCITY_VERLET_RESOURCE_PATH");
    const char* resource_path = env_path == nullptr ? env_path_fallback : env_path;
    SDL_Log("Resource Path: %s", resource_path);
    const char* file_name = "/earth.png";
    const char* moon_file_name = "/moon.png";

    char *buf = SDL_calloc(strlen(resource_path) + strlen(file_name) + 2, sizeof(char));
    strcpy(buf, resource_path);
    strncat(buf, file_name, strlen(file_name));
    SDL_Log("Image Src: %s", buf);
    SDL_Surface *surf = SDL_LoadPNG(buf);
    SDL_Texture *text = SDL_CreateTextureFromSurface(app_state->renderer, surf);
    SDL_DestroySurface(surf);
    SDL_free(buf);

    char *buf2 = SDL_calloc(strlen(resource_path) + strlen(moon_file_name) + 2, sizeof(char));
    strcpy(buf2, resource_path);
    strncat(buf2, moon_file_name, strlen(moon_file_name));
    SDL_Log("Moon Image Src: %s", buf2);
    SDL_Surface *surf2 = SDL_LoadPNG(buf2);
    SDL_Texture *text2 = SDL_CreateTextureFromSurface(app_state->renderer, surf2);
    SDL_DestroySurface(surf2);
    SDL_free(buf2);

    int w;
    int h;
    SDL_GetWindowSize(app_state->window, &w, &h);
    SDL_Log("GetWindowSize -> %d, %d", w, h);

    constexpr double earth_mass = 50000;
    constexpr double moon_mass = 0.0123 * earth_mass;

    Game_Object *obj1 = Game_Object_CreateAt((double)(w>>1), (double)(h>>1));
    obj1->rbody->mass = earth_mass;
    obj1->material->type = GAME_MATERIALTYPE_SPRITE;
    obj1->material->texture = text;
    obj1->size.x = 512;
    obj1->size.y = 512;
    earth_ref = obj1;
    Game_Object *obj2 = Game_Object_CreateAt((double)(w>>1), (double)(h>>1) + moon_dist);
    obj2->material->type = GAME_MATERIALTYPE_SPRITE;
    obj2->material->texture = text2;
    obj2->rbody->mass = moon_mass;
    obj2->rbody->vel.x = moon_init_speed;
    obj2->size.x = 128;
    obj2->size.y = 128;
    moon_ref = obj2;
    Game_AppState_AddObject(app_state, obj1);
    Game_AppState_AddObject(app_state, obj2);
    Game_PrintObjects(app_state);

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
                break;
            case SDLK_COMMA:
                game->delta_time_scale /= 2;
                if (game->delta_time_scale < 0.001) game->delta_time_scale = 0.001;
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
        Game_AppState_AddObject(game, obj);

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

int cntr = 0;
int cntr_cntr = 0;

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    return Game_Iterate(appstate);
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    Game_AppState *state = appstate;
    Game_AppState_Destroy(state);
}

