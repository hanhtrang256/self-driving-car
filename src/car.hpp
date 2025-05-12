#ifndef CAR_HPP
#define CAR_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include "vector2.hpp"
#include "utils/utils.hpp"
#include "SFML/Graphics.hpp"
#include "wall.hpp"
#include "ray.hpp"
#include "NeuralNetwork/neuralnetwork.hpp"

using namespace std;

struct Car {
    Vector2 pos;
    Vector2 velocity;
    float acceleration;
    float angle;
    float maxSpeed;
    float maxSight;
    float width, height;
    float friction;
    float fov;
    int numRays;
    vector<Ray> rays;
    NeuralNetwork nn;
    sf::Clock lifeClock;
    float fitness;
    vector<Vector2> reachedCheckPts;

    Car(float x, float y); 
    Car(const Car &other);
    void updateAcceleration(const float &acc);
    void move();
    void steer(float steerAngle);
    void show(sf::RenderWindow &window);
    vector<float> getSensors(const vector<Wall> &walls, bool checkBound);
    void decide(const vector<float> &sensors);
    void showRays(sf::RenderWindow &window, const vector<Wall> &walls, bool checkBound);
    vector<float> getViews(const vector<Wall> &walls, bool checkBound);
    bool checkCollision(const vector<Wall> &walls,bool checkBound);
    bool passCheckPoint(pair<Vector2, Vector2> line);
};

#endif 