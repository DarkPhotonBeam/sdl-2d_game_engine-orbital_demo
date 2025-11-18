//
// Created by dan on 11/17/25.
//

#include "../include/rigidbody2d.h"
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>

Rigidbody2D *Rigidbody2D_Create() {
    Rigidbody2D *rbody = SDL_malloc(sizeof(Rigidbody2D));
    rbody->simulated = true;
    rbody->has_gravity = true;
    rbody->mass = 1.0;
    rbody->rot_angle = 0.0;
    rbody->rot_vel = 0.0;
    Vector2D_Zero(&rbody->pos);
    Vector2D_Zero(&rbody->vel);
    Vector2D_Zero(&rbody->acc);
    return rbody;
}

char *Rigidbody2D_CreateString(const Rigidbody2D *rbody) {
    char *buf = SDL_malloc(sizeof(char) * 512);
    sprintf(buf,
        "Rigidbody2D {\n"
        "\tsimulated   = %s\n"
        "\thas_gravity = %s\n"
        "\tmass        = %f\n"
        "\trot_angle   = %f\n"
        "\trot_vel     = %f\n"
        "\tpos.x       = %f\n"
        "\tpos.y       = %f\n"
        "\tvel.x       = %f\n"
        "\tvel.y       = %f\n"
        "\tacc.x       = %f\n"
        "\tacc.y       = %f\n"
        "}"
        , rbody->simulated ? "true" : "false",
        rbody->has_gravity ? "true" : "false",
        rbody->mass,
        rbody->rot_angle,
        rbody->rot_vel,
        rbody->pos.x,
        rbody->pos.y,
        rbody->vel.x,
        rbody->vel.y,
        rbody->acc.x,
        rbody->acc.y);
    return buf;
}

void Rigidbody2D_Destroy(Rigidbody2D *rbody) {
    SDL_free(rbody);
}