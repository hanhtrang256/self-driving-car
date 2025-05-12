#ifndef WALL_HPP
#define WALL_HPP

#include <iostream>
#include "utils/utils.hpp"
#include "SFML/Graphics.hpp"
using namespace std;

struct Wall {
    float x1, y1, x2, y2;

    Wall();
    Wall(float x1, float y1, float x2, float y2);
    Wall(const Wall &other);

    void show(sf::RenderWindow &window);
};

#endif 