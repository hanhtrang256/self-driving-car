#include "wall.hpp"

Wall::Wall() {
    x1 = y1 = 0;
    x2 = y2 = 0;
}

Wall::Wall(float x1, float y1, float x2, float y2) {
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

Wall::Wall(const Wall &other) {
    this->x1 = other.x1;
    this->y1 = other.y1;
    this->x2 = other.x2;
    this->y2 = other.y2;
}

void Wall::show(sf::RenderWindow &window) {
    drawLine(window, this->x1, this->y1, this->x2, this->y2, sf::Color::White);   
}
