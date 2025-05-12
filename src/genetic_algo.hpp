#ifndef GENETIC_ALGO_HPP
#define GENETIC_ALGO_HPP

#include "utils/utils.hpp"
#include "NeuralNetwork/neuralnetwork.hpp"
#include "car.hpp"
#include <random>
#include <iomanip>

#define POPULATION 100

void calculateFitness(vector<Car*> &savedCars, float &maxFitness, NeuralNetwork &bestBrain);

int rouletteWheel(const vector<Car*> &savedCars);

void nextGeneration(int &generation, vector<Car*> &cars, vector<Car*> &savedCars, const Vector2 &startPoint, float &maxFitness, NeuralNetwork &bestBrain);

#endif 