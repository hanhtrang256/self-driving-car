#include "genetic_algo.hpp"

void calculateFitness(vector<Car*> &savedCars, float &maxFitness, NeuralNetwork &bestBrain) {
    // Score (fitness now) = 2560 * (check points reached) + offset
    for (int i = 0; i < (int)savedCars.size(); ++i) {
        float fitness = 2560.0f * (float)savedCars[i]->reachedCheckPts.size() + (savedCars[i]->reachedCheckPts.empty() ? 0.0f : savedCars[i]->pos.dist(savedCars[i]->reachedCheckPts.back()));
        savedCars[i]->fitness = fitness;

        if (savedCars[i]->fitness > maxFitness) {
            maxFitness = savedCars[i]->fitness;
            bestBrain = savedCars[i]->nn;
        }
    }

    // Sum all fitness
    float total = 0;
    for (int i = 0; i < (int)savedCars.size(); ++i) {
        total += savedCars[i]->fitness;
    }
    
    // Mapping fitness to (0, 1)
    for (int i = 0; i < (int)savedCars.size(); ++i) {
        if (total == 0) savedCars[i]->fitness = 0;
        else savedCars[i]->fitness /= total;
    }
}

// roulette wheel selection
int rouletteWheel(const vector<Car*> &savedCars) {
    int index = 0;
    float r = random(0.0, 1.0);
    while (r > 0 && index < (int)savedCars.size()) {
        r = r - savedCars[index]->fitness;
        index++;
    }
    index--;
    index = max(0, index);
    return index;
}

void nextGeneration(int &generation, vector<Car*> &cars, vector<Car*> &savedCars, const Vector2 &startPoint, float &maxFitness, NeuralNetwork &bestBrain) {
    ++generation;
    while (!cars.empty()) {
        savedCars.push_back(new Car(*cars.back()));
        delete cars.back();
        cars.pop_back();
    }
    
    // Calculate fitness for selection
    calculateFitness(savedCars, maxFitness, bestBrain);

    sort(savedCars.begin(), savedCars.end(), [](Car* a, Car* b) {
        return a->fitness > b->fitness;
    });

    int keep = 5;
    // Keeping 5 best cars to not losing all the best individuals
    for (int i = 0; i < keep; ++i) {
        cars.push_back(new Car(*savedCars[i]));
    }

    // Crossover
    for (int i = 0; i < POPULATION - keep; ++i) {  
        int parentA = rouletteWheel(savedCars);
        int parentB = rouletteWheel(savedCars);
        Car child(0, 0);
        child.nn = NeuralNetwork::crossover(savedCars[parentA]->nn, savedCars[parentB]->nn);
        cars.push_back(new Car(child));
    }

    // Mutation
    for (int i = keep; i < POPULATION; ++i) {
        cars[i]->nn.mutate(0.05);
    }

    // Delete previous generation
    for (int i = 0; i < (int)savedCars.size(); ++i) {
        delete savedCars[i];
    }
    savedCars.clear();

    // New population ready
    for (int i = 0; i < POPULATION; ++i) {
        cars[i]->pos = startPoint;
        cars[i]->fitness = 0;
        cars[i]->reachedCheckPts.clear();
    }
}