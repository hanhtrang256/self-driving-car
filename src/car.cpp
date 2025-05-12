#include "car.hpp"

Car::Car(float x, float y) {
    pos = Vector2(x, y);
    maxSpeed = 3;
    maxSight = 250;
    width = 10;
    height = 30;
    friction = 0.1;
    angle = 0;
    acceleration = 0;
    fov = 360;
    numRays = 8;

    float headingAngle = angle - 90;
    for (float a = headingAngle - fov / 2.0f; a <= headingAngle + fov / 2.0f; a += fov / (float)numRays) {
        rays.push_back(Ray(pos, radians(a)));
    }
    nn = NeuralNetwork(numRays, numRays * 3, 8);
    fitness = 0;
    reachedCheckPts.clear();
}

Car::Car(const Car &other) {
    pos = other.pos;
    maxSpeed = 3;
    maxSight = 250;
    width = 10;
    height = 30;
    friction = 0.1;
    angle = 0;
    acceleration = 0;
    fov = 360;
    numRays = 8;

    rays.clear();
    float headingAngle = angle - 90;
    for (float a = headingAngle - fov / 2.0f; a <= headingAngle + fov / 2.0f; a += fov / (float)numRays) {
        rays.push_back(Ray(pos, radians(a)));
    }
    nn = other.nn;
    fitness = other.fitness;
    reachedCheckPts.clear();
    for (int i = 0; i < (int)other.reachedCheckPts.size(); ++i) {
        reachedCheckPts.push_back(other.reachedCheckPts[i]);
    }
}

void Car::updateAcceleration(const float &acc) {
    acceleration += acc;
}

void Car::move() {
    // Calculate the affected force by the acceleration update
    Vector2 affectForce(cos(radians(angle - 90)), sin(radians(angle - 90)));
    affectForce.setMag(fabs(acceleration));
    if (acceleration < 0) affectForce.mul(-1);

    // Update the velocity 
    velocity.add(affectForce);
    velocity.limit(maxSpeed);

    // Apply friction
    if (velocity.getMag() > friction) {
        velocity.setMag(velocity.getMag() - friction);
    }
    else velocity.setMag(0);
    
    // Update position
    pos.add(velocity);
    acceleration = 0;
}

void Car::steer(float steerAngle) {
    // Only steer when moving
    if (velocity.getMag() > 0) {
        int flip = 1;
        Vector2 vecHeading(cos(radians(angle - 90)), sin(radians(angle - 90)));
        if (Vector2::dot(velocity, vecHeading) < 0) flip = -1;
        angle = angle + steerAngle * (float)flip;

        for (int i = 0; i < (int)rays.size(); ++i) {
            rays[i].updateAngle(radians(steerAngle) * (float)flip);
        }
    }
}

void Car::show(sf::RenderWindow &window) {
    // Draw body
    sf::RectangleShape rect({width, height});
    rect.setOrigin({width / 2.0f, height / 2.0f});
    rect.setPosition(pos.to2f());
    rect.setFillColor(sf::Color::Cyan);
    rect.setRotation(angle);
    window.draw(rect);

    // Draw center
    // sf::CircleShape cent;
    // cent.setPosition({pos.x - 5.0f, pos.y - 5.0f});
    // cent.setFillColor(sf::Color::Red);
    // cent.setRadius(5);
    // rect.setRotation(angle);
    // window.draw(cent);

    // Draw heading vector
    // sf::RectangleShape headingVector({2, 50});
    // headingVector.setOrigin({1, 50});
    // headingVector.setPosition(pos.to2f());
    // headingVector.setFillColor(sf::Color::Red);
    // headingVector.setRotation(angle);
    // window.draw(headingVector);
}

vector<float> Car::getSensors(const vector<Wall> &walls, bool checkBound) {
    vector<Wall> tmp = walls;
    if (!checkBound) tmp.erase(tmp.begin() + 4);

    vector<float> dist;
    for (int i = 0; i < (int)rays.size(); ++i) {
        pair<Vector2, float> best = rays[i].looking(pos, tmp);
        dist.push_back(1.0f - best.second / maxSight);
    }
    return dist;
}

void Car::decide(const vector<float> &sensors) {
    float inputs[(int)sensors.size()];
    for (int i = 0; i < (int)sensors.size(); ++i) {
        inputs[i] = sensors[i];
    }

    float outputs[8];
    nn.SGD_feedForward(inputs, outputs);

    auto mapping = [](float value, float x, float y, float tx, float ty) -> float {
        float ratio = (value - x) / (y - x);
        return tx + ratio * (ty - tx);
    };

    if (outputs[0] >= 0.5) {
        updateAcceleration(mapping(outputs[1], 0, 1, 0.15, 0.2));
    }
    if (outputs[2] >= 0.5) {
        updateAcceleration(mapping(outputs[3], 0, 1, -0.15, -0.2));
    }
    if (outputs[4] >= 0.5) {
        steer(mapping(outputs[5], 0, 1, 1.5, 2));
    }
    if (outputs[6] >= 0.5) {
        steer(mapping(outputs[7], 0, 1, -1.5, -2));
    }
}

void Car::showRays(sf::RenderWindow &window, const vector<Wall> &walls, bool checkBound) {
    vector<Wall> tmp = walls;
    if (!checkBound) tmp.erase(tmp.begin() + 4);
    for (int i = 0; i < (int)rays.size(); ++i) {
        pair<Vector2, float> best = rays[i].looking(pos, tmp);
        if (best.first.x != INF && best.first.y != INF && best.second <= maxSight) {
            drawLine(window, pos.x, pos.y, best.first.x, best.first.y, sf::Color::Cyan);
        }
    }
}

vector<float> Car::getViews(const vector<Wall> &walls, bool checkBound) {
    vector<Wall> tmp = walls;
    if (!checkBound) tmp.erase(tmp.begin() + 4);

    float headingAngle = angle - 90;

    vector<float> dist; 
    for (float a = headingAngle - 90; a <= headingAngle + 90; ++a) {
        Ray ray(pos, radians(a));
        pair<Vector2, float> viewInfo = ray.looking(pos, tmp);
        dist.push_back(viewInfo.second);
    }

    return dist;
}

bool Car::checkCollision(const vector<Wall> &walls, bool checkBound) {
    float a = atan2(width, height) * 180.0 / M_PI;
    float mag = 0.5 * sqrt(width * width + height * height);
    Vector2 v[4];
    v[0] = Vector2(cos(radians(angle - 90 - a)), sin(radians(angle - 90 - a)));
    v[1] = Vector2(cos(radians(angle - 90 + a)), sin(radians(angle - 90 + a)));
    v[2] = Vector2(cos(radians(angle + 90 - a)), sin(radians(angle + 90 - a)));
    v[3] = Vector2(cos(radians(angle + 90 + a)), sin(radians(angle + 90 + a)));
    for (int i = 0; i < 4; ++i) {
        v[i].setMag(mag);
    }

    for (int i = 0; i < 4; ++i) {
        float x3 = pos.x, y3 = pos.y;
        float x4 = pos.x + v[i].x, y4 = pos.y + v[i].y;
        for (int j = 0; j < (int)walls.size(); ++j) {
            if (j == 4) {
                if (checkBound == false) continue;
            }
            float x1 = walls[j].x1, y1 = walls[j].y1;
            float x2 = walls[j].x2, y2 = walls[j].y2;
            float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (den == 0) continue;
            float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
            float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
            if (t > 0 && t < 1 && u > 0) {
                Vector2 pt = Vector2(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
                if (pos.dist(Vector2(pos.x + v[i].x, pos.y + v[i].y)) > pos.dist(pt)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Car::passCheckPoint(pair<Vector2, Vector2> line) {
    float a = atan2(width, height) * 180.0 / M_PI;
    float mag = 0.5 * sqrt(width * width + height * height);
    Vector2 v[4];
    v[0] = Vector2(cos(radians(angle - 90 - a)), sin(radians(angle - 90 - a)));
    v[1] = Vector2(cos(radians(angle - 90 + a)), sin(radians(angle - 90 + a)));
    v[2] = Vector2(cos(radians(angle + 90 - a)), sin(radians(angle + 90 - a)));
    v[3] = Vector2(cos(radians(angle + 90 + a)), sin(radians(angle + 90 + a)));
    for (int i = 0; i < 4; ++i) {
        v[i].setMag(mag);
    }

    for (int i = 0; i < 4; ++i) {
        float x3 = pos.x, y3 = pos.y;
        float x4 = pos.x + v[i].x, y4 = pos.y + v[i].y;
        float x1 = line.first.x, y1 = line.first.y;
        float x2 = line.second.x, y2 = line.second.y;
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) continue;
        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
        if (t > 0 && t < 1 && u > 0) {
            Vector2 pt = Vector2(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
            if (pos.dist(Vector2(pos.x + v[i].x, pos.y + v[i].y)) > pos.dist(pt)) {
                return true;
            }
        }
    }
    return false;
}
