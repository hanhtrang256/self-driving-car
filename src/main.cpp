#include <iostream>
#include "SFML/Graphics.hpp"
#include "utils/utils.hpp"
#include "car.hpp"
#include "wall.hpp"
#include "ray.hpp"
#include "genetic_algo.hpp"
#include "race.hpp"
#include "button.hpp"
using namespace std;

int generation = 0;
int frameCount = 0;
bool bestMode = false;
RectButton bestButton, trainButton, mapButton;

vector<Wall> walls;
vector<Vector2> checkPoints; 
vector<pair<Vector2, Vector2>> checkLines;
vector<Car*> cars;
vector<Car*> savedCars;
Car *carBest = nullptr; 
float maxFitness = 0;
int maxReachCheckPt = 0;
NeuralNetwork bestBrain;

sf::Font font;
sf::Text textGen, textMF;

void saveBest(const char filename[]) {
    cout << "save best brain!\n";
    bestBrain.saveNetwork(filename);
}

void update(sf::RenderWindow &window, vector<Vector2> &checkPoints, vector<pair<Vector2, Vector2>> &checkLines) {
    textGen.setString("Generation: " + to_string(generation));
    textMF.setString("Max fitness: " + to_string(maxFitness));

    if (bestButton.isClick(window)) {
        cout << "Switch to best mode!\n";
        bestMode = true;

        if (carBest != nullptr) {
            delete carBest;
            carBest = nullptr;
        }

        carBest = new Car(checkPoints[0].x, checkPoints[0].y);
        carBest->nn.loadNetwork("best.txt");
    }

    if (trainButton.isClick(window)) {
        cout << "Switch to train mode!\n";
        if (bestMode) {
            bestMode = false;
            if (carBest != nullptr) {
                delete carBest; 
                carBest = nullptr;
            }
        }
    }

    if (mapButton.isClick(window)) {
        cout << "Switch map!\n";
        newRaceTrack(checkPoints, checkLines, walls);
        // Reset information of remaining cars
        for (int i = 0; i < (int)cars.size(); ++i) {
            cars[i]->pos = checkPoints[0];
            cars[i]->fitness = 0;
            cars[i]->reachedCheckPts.clear();            
        }
    }

    if (bestMode) {
        if (carBest->checkCollision(walls, false)) {
            cout << "car best hit walls!\n";
            delete carBest;
            carBest = new Car(checkPoints[0].x, checkPoints[0].y);
            carBest->nn.loadNetwork("best.txt");
        }

        if (carBest != nullptr) {
            // cout << "Car best sensoring!\n";
            vector<float> sensors = carBest->getSensors(walls, false);
            carBest->decide(sensors);
            carBest->move();
        }
    }
    else {
        for (int i = 0; i < (int)cars.size(); ++i) {
            for (int j = 0; j < (int)checkPoints.size(); ++j) {
                if (cars[i]->passCheckPoint(checkLines[j])) {
                    if (cars[i]->reachedCheckPts.empty()) {
                        cars[i]->reachedCheckPts.push_back(checkPoints[j]);
                    }
                    else {
                        Vector2 lastCheckPt = cars[i]->reachedCheckPts.back();
                        if (checkPoints[j].x != lastCheckPt.x || checkPoints[j].y != lastCheckPt.y) {
                            cars[i]->reachedCheckPts.push_back(checkPoints[j]);
                        }
                    }
                } 
            }
        }

        for (int i = (int)cars.size() - 1; i >= 0; --i) {
            bool hasBound = true;
            if ((int)cars[i]->reachedCheckPts.size() >= 3) {
                hasBound = false;
            }
            if (cars[i]->checkCollision(walls, hasBound)) {
                savedCars.push_back(new Car(*cars[i]));
                delete cars[i];
                cars.erase(cars.begin() + i);
            }
            else {
                vector<float> sensors = cars[i]->getSensors(walls, hasBound);
                cars[i]->decide(sensors);
                cars[i]->move();
            }
        }

        if (cars.empty()) {
            nextGeneration(generation, cars, savedCars, checkPoints[0], maxFitness, bestBrain);
            frameCount = 0;
            return;
        }

        int numExtreme = 0;
        for (int i = 0; i < (int)cars.size(); ++i) {
            if ((int)cars[i]->reachedCheckPts.size() >= (int)checkPoints.size() + 3) {
                ++numExtreme;
            }
        }

        if (numExtreme >= 6) {
            newRaceTrack(checkPoints, checkLines, walls);
            for (int i = 0; i < (int)cars.size(); ++i) {
                cars[i]->pos = checkPoints[0];
                cars[i]->fitness = 0;
                cars[i]->reachedCheckPts.clear();            
            }
        }
    }

    if (bestMode) frameCount = 0;
    if (!bestMode) {
        ++frameCount;
        if (frameCount >= 60 * 50) {
            nextGeneration(generation, cars, savedCars, checkPoints[0], maxFitness, bestBrain);
            frameCount = 0;
        }
        else if (frameCount >= 60 * 8) {
            bool hasRun = false;
            for (int i = 0; i < (int)cars.size(); ++i) {
                if (cars[i]->velocity.getMag() >= 0.5) {
                    hasRun = true;
                }
            }
            if (!hasRun) {
                nextGeneration(generation, cars, savedCars, checkPoints[0], maxFitness, bestBrain);
                frameCount = 0; 
            }
        }
    }
}

void draw(sf::RenderWindow &window, const vector<Vector2> &checkPoints) {
    // Draw race track
    for (int i = 0; i < (int)checkPoints.size(); ++i) {
        // Draw checkpoints
        sf::CircleShape circ;
        if (i == 0) {
            circ.setFillColor(sf::Color::Blue);
        }
        else if (i == 1) {
            circ.setFillColor(sf::Color::Green);
        }
        else circ.setFillColor(sf::Color::White);

        circ.setRadius(5);
        circ.setPosition(sf::Vector2f(checkPoints[i].x - 5, checkPoints[i].y - 5));
        window.draw(circ);

        int pre = (i - 1 + (int)checkPoints.size()) % (int)checkPoints.size();
        int nxt = (i + 1) % (int)checkPoints.size();

        // Draw checkpoint lines
        Vector2 preLine = Vector2::sub(checkPoints[i], checkPoints[pre]);
        Vector2 nxtLine = Vector2::sub(checkPoints[nxt], checkPoints[i]);

        Vector2 biVector = Vector2::add(preLine, nxtLine);
        biVector.setMag(1);

        Vector2 normalIn = Vector2::newRotate(biVector, radians(90));
        Vector2 normalOut = Vector2::newRotate(biVector, radians(-90));
        normalOut.setMag(raceRadius);
        normalIn.setMag(raceRadius);

        drawLine(window, checkPoints[i].x, checkPoints[i].y, checkPoints[i].x + normalOut.x, checkPoints[i].y + normalOut.y, sf::Color::Yellow);
        drawLine(window, checkPoints[i].x, checkPoints[i].y, checkPoints[i].x + normalIn.x, checkPoints[i].y + normalIn.y, sf::Color::Red);
    }

    // Draw walls
    for (int i = 0; i < (int)walls.size(); ++i) {
        walls[i].show(window);
    }   

    // Draw buttons
    bestButton.show(window);
    trainButton.show(window);
    mapButton.show(window);

    // Draw text
    window.draw(textGen);
    window.draw(textMF);

    // Draw cars
    if (bestMode) {
        if (carBest != nullptr) {
            // carBest->showRays(window, walls, false);
            carBest->show(window);
        }
    }
    else {
        for (int i = 0; i < (int)cars.size(); ++i) {
            cars[i]->showRays(window, walls, true);
            cars[i]->show(window);
        }
    }

}

int main() {
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Self-Driving Car");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("TIMES.TTF")) {
        cout << "Cannot load font!\n";
        return 0;
    }

    float infoX = gameWidth + 50;
    // Setup buttons 
    bestButton = RectButton(infoX, 100, 150, 100, "green");
    trainButton = RectButton(infoX, 250, 150, 100, "blue");
    mapButton = RectButton(infoX, 400, 150, 100);

    // Information text
    textGen.setFont(font);
    textGen.setCharacterSize(30);
    textGen.setPosition(infoX, 550);

    textMF.setFont(font);
    textMF.setCharacterSize(30);
    textMF.setPosition(infoX, 650);

    // surrounding screen
    walls.push_back(Wall(offset, offset, gameWidth - offset, offset)); // top
    walls.push_back(Wall(offset, screenHeight - offset, gameWidth - offset, screenHeight - offset)); // bottom
    walls.push_back(Wall(offset, offset, offset, screenHeight - offset)); // left
    walls.push_back(Wall(gameWidth - offset, offset, gameWidth - offset, screenHeight - offset)); // right

    // Build race track information
    newRaceTrack(checkPoints, checkLines, walls);
    
    for (int i = 0; i < POPULATION; ++i) {
        cars.push_back(new Car(checkPoints[0].x, checkPoints[0].y));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Updating
        update(window, checkPoints, checkLines);

        // Save best neural network so far
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            if (bestMode) {
                carBest->nn.saveNetwork("best.txt");    
            }
            else saveBest("best.txt");
        }

        // Finish updating
        window.clear();

        // Drawing
        draw(window, checkPoints);        
        window.display();
    }

    for (int i = 0; i < (int)cars.size(); ++i) {
        delete cars[i]; 
    }

    for (int i = 0; i < (int)savedCars.size(); ++i) {
        delete savedCars[i];
    }
    return 0;
}