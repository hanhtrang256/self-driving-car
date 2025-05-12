#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <random>
#include "SFML/Graphics.hpp"
using namespace std;

# define M_PI 3.14159265358979323846  
#define INF 1006
#define screenWidth 1400
#define screenHeight 800
#define gameWidth 1000
#define offset 2
#define raceRadius 60

float random(float low, float high);
float sigmoid(float x);
float radians(float a);
void drawLine(sf::RenderWindow &window, float x1, float y1, float x2, float y2, sf::Color color);
float randomGaussian(float mean, float stdDev);

#endif 