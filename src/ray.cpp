#include "ray.hpp"

Ray::Ray(Vector2 pos, float angle) {
    this->angle = angle;
    this->pos = pos;
    maxSight = 250;
}

Ray::Ray(const Ray &other) {
    pos = other.pos;
    angle = other.angle;
    maxSight = 250;
}

void Ray::setAngle(float angle) {
    this->angle = angle;
}

void Ray::updateAngle(float updAngle) {
    angle += updAngle;  
}

pair<Vector2, float> Ray::looking(const Vector2 &vehiclePos, const vector<Wall> &walls) {
    pos = vehiclePos;

    float x3 = pos.x, y3 = pos.y;
    float x4 = x3 + cos(angle), y4 = y3 + sin(angle);

    float best = INF;
    Vector2 bestPoint = Vector2(INF, INF);

    for (int i = 0; i < (int)walls.size(); ++i) {
        float x1 = walls[i].x1, y1 = walls[i].y1;
        float x2 = walls[i].x2, y2 = walls[i].y2;

        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) continue;
        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
        if (t > 0 && t < 1 && u > 0) {
            Vector2 pt = Vector2(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
            if (pos.dist(pt) < best) {
                best = pos.dist(pt); 
                bestPoint = pt;
            }
        }
    }
    // best *= cos(angle);
    return {bestPoint, best};
}
