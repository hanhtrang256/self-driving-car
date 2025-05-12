#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "SFML/Graphics.hpp"
#include "vector2.hpp"
#include <string>
using namespace std;

struct RectButton {
    float x, y, width, height;
    string color; 
    RectButton(); 
    RectButton(float _x, float _y, float _width, float _height, string _color = "white");
    RectButton(const RectButton &other);
    void show(sf::RenderWindow &window); 
    bool isClick(sf::RenderWindow &window); 
};

#endif 