#ifndef RAY_HPP
#define RAY_HPP

#include <iostream>
#include "vector2.hpp"
#include "wall.hpp"
#include "utils/utils.hpp"
using namespace std;

struct Ray {
    Vector2 pos;
    float angle;
    float maxSight;

    // angle is radian unit
    Ray(Vector2 pos, float angle);
    Ray(const Ray &other);
    // angle is radian unit
    void setAngle(float angle);
    // angle is radian unit
    void updateAngle(float updAngle);
    pair<Vector2, float> looking(const Vector2 &vehiclePos, const vector<Wall> &walls);
};

#endif 