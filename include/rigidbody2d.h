//
// Created by dan on 11/17/25.
//

#ifndef RIGIDBODY2D_H
#define RIGIDBODY2D_H

#include "vector2d.h"

typedef struct {
    int simulated;
    int has_gravity;
    double mass;
    double rot_angle;
    double rot_vel;
    Vector2D pos;
    Vector2D vel;
    Vector2D acc;
} Rigidbody2D;

/**
 * Create a Rigidbody Instance
 * @return Rigidbody2D Instance Pointer
 */
Rigidbody2D *Rigidbody2D_Create();

/**
 * Creates and returns a string; must be freed
 * @param rbody The rigidbody to turn to a string
 * @return String of rigidbody
 */
char *Rigidbody2D_CreateString(const Rigidbody2D *rbody);

void Rigidbody2D_Destroy(Rigidbody2D *rbody);

#endif //RIGIDBODY2D_H
