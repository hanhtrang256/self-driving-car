#include "button.hpp"

RectButton::RectButton() {
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    color = "green";
}

RectButton::RectButton(float _x, float _y, float _width, float _height, string _color) {
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    color = _color;
}

RectButton::RectButton(const RectButton &other) {
    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    color = other.color;
}

void RectButton::show(sf::RenderWindow &window) {
    sf::RectangleShape shape({width, height});
    sf::Color chooseColor;
    if (color == "green") chooseColor = sf::Color::Green;
    else if (color == "blue") chooseColor = sf::Color::Blue;
    else chooseColor = sf::Color::White;
    shape.setFillColor(chooseColor);
    shape.setPosition(Vector2(x, y).to2f());
    window.draw(shape);
}

bool RectButton::isClick(sf::RenderWindow &window) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (mousePos.x >= x && mousePos.x <= x + width) {
            if (mousePos.y >= y && mousePos.y <= y + height) {
                return true;
            }
        }
    }
    return false;
}
