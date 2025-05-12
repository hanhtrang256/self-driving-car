#include "race.hpp"

float cross(const Vector2 &O, const Vector2 &A, const Vector2 &B) {
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

vector<Vector2> buildConvexHull(int noisy) {
    vector<Vector2> P;
    float trackOffset = 60;
    for (int i = 0; i < noisy; ++i) {
        P.push_back(Vector2(random(offset + trackOffset, gameWidth - offset - trackOffset), 
                            random(offset + trackOffset, screenHeight - offset - trackOffset)));
    }

	int n = P.size(), k = 0;
	if (n <= 3) return P;
	vector<Vector2> H(2*n);

	// Sort Points lexicographically
	sort(P.begin(), P.end(), [](const Vector2 &a, const Vector2 &b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });

	// Build lower hull
	for (int i = 0; i < n; ++i) {
		while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}

	// Build upper hull
	for (int i = n-1, t = k+1; i > 0; --i) {
		while (k >= t && cross(H[k-2], H[k-1], P[i-1]) <= 0) k--;
		H[k++] = P[i-1];
	}

	H.resize(k-1);
	return H;
}

vector<pair<Vector2, Vector2>> buildRaceTrack(vector<Wall> &walls, const vector<Vector2> &checkPoints) {
    vector<pair<Vector2, Vector2>> checkLines;
    vector<Vector2> outerPts, innerPts;
    for (int i = 0; i < (int)checkPoints.size(); ++i) {
        int pre = (i - 1 + (int)checkPoints.size()) % (int)checkPoints.size();
        int nxt = (i + 1) % (int)checkPoints.size();
        
        Vector2 preLine = Vector2::sub(checkPoints[i], checkPoints[pre]);
        Vector2 nxtLine = Vector2::sub(checkPoints[nxt], checkPoints[i]);

        Vector2 biVector = Vector2::add(preLine, nxtLine);
        biVector.setMag(1);

        Vector2 normalIn = Vector2::newRotate(biVector, radians(90));
        Vector2 normalOut = Vector2::newRotate(biVector, radians(-90));
        normalOut.setMag(raceRadius);
        normalIn.setMag(raceRadius);

        if (i == 0) {
            Vector2 normalBound = Vector2::newRotate(normalOut, radians(-90));
            normalBound.mul(0.5);
            walls.push_back(Wall(checkPoints[i].x + normalBound.x + 1.5 * normalOut.x,
                                 checkPoints[i].y + normalBound.y + 1.5 * normalOut.y,
                                 checkPoints[i].x + normalBound.x + 1.5 * normalIn.x,
                                 checkPoints[i].y + normalBound.y + 1.5 * normalIn.y));
        }

        outerPts.push_back(Vector2(checkPoints[i].x + normalOut.x, 
                                   checkPoints[i].y + normalOut.y));
        innerPts.push_back(Vector2(checkPoints[i].x + normalIn.x, 
                                   checkPoints[i].y + normalIn.y));
        checkLines.push_back({outerPts.back(), innerPts.back()});
    }

    for (int i = 0; i < (int)outerPts.size(); ++i) {
        int nxt = (i + 1) % (int)outerPts.size();
        walls.push_back(Wall(outerPts[i].x, outerPts[i].y, outerPts[nxt].x, outerPts[nxt].y));
    }

    for (int i = 0; i < (int)innerPts.size(); ++i) {
        int nxt = (i + 1) % (int)innerPts.size();
        walls.push_back(Wall(innerPts[i].x, innerPts[i].y, innerPts[nxt].x, innerPts[nxt].y));
    }

    return checkLines;
}

void newRaceTrack(vector<Vector2> &checkPoints, vector<pair<Vector2, Vector2>> &checkLines, vector<Wall> &walls) {
    checkPoints.clear();
    while ((int)walls.size() > 4) walls.pop_back();

    checkPoints = buildConvexHull(30);
    checkLines = buildRaceTrack(walls, checkPoints);
}
