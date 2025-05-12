#include "utils.hpp"

float random(float low, float high) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dis(low, high);
    return dis(gen);
}

float sigmoid(float x) {
    return 1.0 / (1.0 + exp(-x));
}

float radians(float a) {
    return a * M_PI / 180.0f;
}

void drawLine(sf::RenderWindow &window, float x1, float y1, float x2, float y2, sf::Color color) {
    sf::Vertex line[2];
    line[0].position = sf::Vector2f(x1, y1);
    line[1].position = sf::Vector2f(x2, y2);
    for (int i = 0; i < 2; ++i) line[i].color = color;
    window.draw(line, 2, sf::Lines);
}

// Create a random number generator and distribution
float randomGaussian(float mean, float stdDev) {
    static std::mt19937 generator(std::random_device{}()); // seeded once
    static std::normal_distribution<float> distribution(mean, stdDev);

    return distribution(generator);
}