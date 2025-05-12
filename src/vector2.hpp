#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <iostream>
#include <cmath>
#include "SFML/Graphics.hpp"
using namespace std;

struct Vector2 {
    float x, y;
    
    Vector2(); 
    Vector2(float _x, float _y);
    Vector2(const Vector2 &other);
    void add(const Vector2 &other);
    void add(const int &num);
    static Vector2 add(const Vector2 &a, const Vector2 &b);
    void sub(const Vector2 &other);
    void sub(const int &num);
    static Vector2 sub(const Vector2 &a, const Vector2 &b);
    void mul(const float &num);
    static float dot(const Vector2 &a, const Vector2 &b);
    float getMag() const;
    void setMag(float newMag);
    void limit(float limitMag);
    void print();
    float dist(const Vector2 &other);
    static float dist(const Vector2 &a, const Vector2 &b);
    void rotate(float angle);
    static Vector2 newRotate(const Vector2 &v, float angle);
    sf::Vector2f to2f();
};

#endif 