//
// Created by dan on 11/17/25.
//

#ifndef VECTOR2D_H
#define VECTOR2D_H

typedef struct {
    double x;
    double y;
} Vector2D;

Vector2D Vector2D_FromExisting(const Vector2D *v);

/**
 * Adds another vector to a vector
 * @param v Vector to modify
 * @param u Vector whose values should be added to v
 */
void Vector2D_AddVec(Vector2D *v, const Vector2D *u);

/**
 *
 * @param v Vector modify
 * @param x X value to add to v
 * @param y Y value to add to v
 */
void Vector2D_Add(Vector2D *v, double x, double y);

Vector2D Vector2D_Diff(const Vector2D *v, const Vector2D *u);
Vector2D Vector2D_Sum(const Vector2D *v, const Vector2D *u);
Vector2D Vector2D_Scaled(const Vector2D *v, double alpha);

void Vector2D_Scale(Vector2D *v, double alpha);

double Vector2D_Abs(const Vector2D *v);

Vector2D Vector2D_GetZeroVec();

/**
 * Zeroes out a vector
 * @param v Vector to zero
 */
void Vector2D_Zero(Vector2D *v);

#endif //VECTOR2D_H
