#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iostream>
#include <cstring>
#include <fstream>
#include "matrix.hpp"
#include "../utils/utils.hpp"
using namespace std; 

struct NeuralNetwork {
    int numInput, numHidden, numOutput;
    Matrix IH, HO, BH, BO;
    Matrix outputHidden, outputOutput;
    float LEARNING_RATE;

    NeuralNetwork(int _numInput = 0, int _numHidden = 0, int _numOutput = 0);
    NeuralNetwork(const NeuralNetwork &other);
    void SGD_feedForward(float inputs[], float outputs[]);
    void SGD_train(float inputs[], float targets[]);
    void MBGD_feedForward(const vector<vector<float>> &inputs, vector<vector<float>> &outputs);
    void MBGD_train(const vector<vector<float>> &inputs, const vector<vector<float>> &targets);
    void mutate(float rate);
    void saveNetwork(const char filename[]);
    void loadNetwork(const char filename[]);
    static NeuralNetwork crossover(const NeuralNetwork &parA, const NeuralNetwork &parB);
};

#endif 