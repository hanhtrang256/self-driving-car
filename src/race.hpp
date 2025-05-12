#ifndef RACE_HPP
#define RACE_HPP

#include "utils/utils.hpp"
#include "vector2.hpp"
#include "wall.hpp"

using namespace std;

float cross(const Vector2 &O, const Vector2 &A, const Vector2 &B);

vector<Vector2> buildConvexHull(int noisy = 20);

vector<pair<Vector2, Vector2>> buildRaceTrack(vector<Wall> &walls, const vector<Vector2> &checkPoints);

void newRaceTrack(vector<Vector2> &checkPoints, vector<pair<Vector2, Vector2>> &checkLines, vector<Wall> &walls);

#endif 