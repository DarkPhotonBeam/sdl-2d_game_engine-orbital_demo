//
// Created by dan on 11/17/25.
//

#include "../include/vector2d.h"

#include <math.h>

Vector2D Vector2D_FromExisting(const Vector2D *v) {
    const Vector2D u = {.x = v->x, .y = v->y};
    return u;
}

void Vector2D_AddVec(Vector2D *v, const Vector2D *u) {
    v->x += u->x;
    v->y += u->y;
}

void Vector2D_Add(Vector2D *v, const double x, const double y) {
    v->x += x;
    v->y += y;
}

Vector2D Vector2D_Diff(const Vector2D *v, const Vector2D *u) {
    const Vector2D vec = {
        .x = v->x - u->x,
        .y = v->y - u->y
    };
    return vec;
}

Vector2D Vector2D_Sum(const Vector2D *v, const Vector2D *u) {
    const Vector2D vec = {
        .x = v->x + u->x,
        .y = v->y + u->y
    };
    return vec;
}

Vector2D Vector2D_Scaled(const Vector2D *v, const double alpha) {
    const Vector2D vec = {
        .x = v->x * alpha,
        .y = v->y * alpha,
    };
    return vec;
}

double Vector2D_Abs(const Vector2D *v) {
    return sqrt(v->x*v->x + v->y*v->y);
}

Vector2D Vector2D_GetZeroVec() {
    Vector2D v = {.x = 0, .y = 0};
    return v;
}

void Vector2D_Scale(Vector2D *v, const double alpha) {
    v->x *= alpha;
    v->y *= alpha;
}

void Vector2D_Zero(Vector2D *v) {
    v->x = 0.0;
    v->y = 0.0;
}
