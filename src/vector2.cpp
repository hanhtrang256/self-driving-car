#include "vector2.hpp"

Vector2::Vector2(){
    x = 0;
    y = 0;
} 
Vector2::Vector2(float _x, float _y){
    x = _x;
    y = _y;
}
Vector2::Vector2(const Vector2 &other){
    x = other.x;
    y = other.y;
}
void Vector2::add(const Vector2 &other){
    x += other.x;
    y += other.y;
}
void Vector2::add(const int &num){
    x += num;
    y += num;
}
Vector2 Vector2::add(const Vector2 &a, const Vector2 &b){
    return Vector2(a.x + b.x, a.y + b.y);
}
void Vector2::sub(const Vector2 &other){
    x -= other.x;
    y -= other.y;
}
void Vector2::sub(const int &num){
    x -= num;
    y -= num;
}
Vector2 Vector2::sub(const Vector2 &a, const Vector2 &b){
    return Vector2(a.x - b.x, a.y - b.y);
}

void Vector2::mul(const float &num) {
    x *= num;
    y *= num;
}

float Vector2::dot(const Vector2 &a, const Vector2 &b) {
    return a.x * b.x + a.y * b.y;
}

float Vector2::getMag() const {
    return sqrt(x * x + y * y);
}

void Vector2::setMag(float newMag) {
    float curMag = getMag();
    if (curMag == 0.0) {
        // cout << "Cannot set magnitude!!!\n";
        return;
    }
    if (newMag == 0.0) {
        this->x = 0;
        this->y = 0;
    }
    float scale = newMag / curMag;
    this->mul(scale);
}

void Vector2::limit(float limitMag) {
    if (getMag() > limitMag) setMag(limitMag);
}

void Vector2::print() {
    cout << "[" << x << ", " << y << "]\n";
}

float Vector2::dist(const Vector2 &other) {
    return sqrt((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y));
}

float Vector2::dist(const Vector2 &a, const Vector2 &b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void Vector2::rotate(float angle) {
    float newX = this->x * cos(angle) - this->y * sin(angle);
    float newY = this->x * sin(angle) + this->y * cos(angle);
    float oldMag = getMag();
    this->x = newX;
    this->y = newY;
    setMag(oldMag);
}

Vector2 Vector2::newRotate(const Vector2 &v, float angle) {
    float oldMag = v.getMag();
    Vector2 newVector = Vector2(v.x * cos(angle) - v.y * sin(angle), v.x * sin(angle) + v.y * cos(angle));
    newVector.setMag(oldMag);
    return newVector;
}

sf::Vector2f Vector2::to2f() {
    return sf::Vector2f(this->x, this->y);
}
