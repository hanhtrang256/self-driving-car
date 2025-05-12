#include "neuralnetwork.hpp"

NeuralNetwork::NeuralNetwork(int _numInput, int _numHidden, int _numOutput) {
    numInput = _numInput;
    numHidden = _numHidden;
    numOutput = _numOutput;
    LEARNING_RATE = 0.1;

    IH = Matrix(numHidden, numInput);
    BH = Matrix(numHidden, 1);
    HO = Matrix(numOutput, numHidden);
    BO = Matrix(numOutput, 1);
    outputHidden = Matrix();
    outputOutput = Matrix();
    IH.randomize();
    BH.randomize();
    HO.randomize();
    BO.randomize();
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork &other) {
    numInput = other.numInput;
    numHidden = other.numHidden;
    numOutput = other.numOutput;
    LEARNING_RATE = 0.1;
    IH = other.IH;
    HO = other.HO;
    BH = other.BH;
    BO = other.BO;
}

void NeuralNetwork::SGD_feedForward(float inputs[], float outputs[]) {
    Matrix inputMatrix = Matrix::toMatrix(numInput, inputs);
    outputHidden = Matrix::mul(IH, inputMatrix);
    outputHidden.add(BH);
    outputHidden.mapSigmoid();
    // outputHidden.mapTanh();

    outputOutput = Matrix::mul(HO, outputHidden);
    outputOutput.add(BO);
    outputOutput.mapSigmoid();
    // outputOutput.mapTanh();

    outputOutput.toArray(outputs);
}

void NeuralNetwork::SGD_train(float inputs[], float targets[]) {
    // Feedforward and predict the result 
    // prediction in array outputs
    float outputs[numOutput];
    SGD_feedForward(inputs, outputs);

    // Calculating delta matrix of output nodes
    float deltaOutputArray[numOutput];
    for (int i = 0; i < numOutput; ++i) {
        deltaOutputArray[i] = outputs[i] * (1 - outputs[i]) * (targets[i] - outputs[i]);
    }
    Matrix deltaOutput = Matrix::toMatrix(numOutput, deltaOutputArray); // Δ(output)

    // Calculating delta matrix of hidden nodes
    Matrix deltaHidden = Matrix::mul(HO.transpose(), deltaOutput);
    // Multiply with the gradient
    deltaHidden = Matrix::elementWise(outputHidden.getDerivativeSigmoid(), deltaHidden); // Δ(hidden)

    // Adjusting weights connecting hidden and output layer
    deltaOutput.mul(LEARNING_RATE); // α * Δ(output)
    BO.add(deltaOutput); // bias(output) = bias(output) + α * Δ(output)
    deltaOutput = Matrix::mul(deltaOutput, outputHidden.transpose()); // α * Δ(output) * a(hidden)
    HO.add(deltaOutput); // w = w + α * Δ(output) * a(hidden)

    // Adjusting weights connecting input and hidden layer
    deltaHidden.mul(LEARNING_RATE); // α * Δ(hidden)
    BH.add(deltaHidden); // bias(hidden) = bias(hidden) + α * Δ(hidden)
    deltaHidden = Matrix::mul(deltaHidden, Matrix::toMatrix(numInput, inputs).transpose()); // α * Δ(hidden) * input
    IH.add(deltaHidden); // w = w + α * Δ(hidden) * input
}

void NeuralNetwork::MBGD_feedForward(const vector<vector<float>> &inputs, vector<vector<float>> &outputs) {
    Matrix inputMatrix = Matrix::toMatrix(inputs);
    // Calculate output hidden matrix (64 x 20)
    outputHidden = Matrix::mul(IH, inputMatrix);
    // Add the bias of hidden layer (64 x 20 + 64 x 1)
    outputHidden.broadcast(BH);
    outputHidden.mapSigmoid();

    // Calculate the output output matrix (10 x 20)
    outputOutput = Matrix::mul(HO, outputHidden);
    // Add the bias of output layer (10 x 20 + 10 x 1)
    outputOutput.broadcast(BO);
    outputOutput.mapSigmoid();

    outputOutput.toVector(outputs);
}

void NeuralNetwork::MBGD_train(const vector<vector<float>> &inputs, const vector<vector<float>> &targets) {
    // Feedforward and predict the result 
    // prediction in vector outputs
    vector<vector<float>> outputs;
    MBGD_feedForward(inputs, outputs);

    // Input matrix (784 x 20)
    Matrix inputMatrix = Matrix::toMatrix(inputs);

    // Output matrix (10 x 20), Target matrix (10 x 20), and Error matrix (10 x 20)
    Matrix outputMatrix = Matrix::toMatrix(outputs);
    Matrix targetMatrix = Matrix::toMatrix(targets);
    Matrix errorMatrix = Matrix::sub(targetMatrix, outputMatrix);

    // Calculate gradient output (10 x 20)
    Matrix gradientOutput = outputMatrix.copy().getDerivativeSigmoid();

    // Calculate delta output (10 x 20)
    Matrix deltaOutput = Matrix::elementWise(gradientOutput, errorMatrix);

    // Calculate gradient hidden (64 x 20)
    Matrix gradientHidden = outputHidden.copy().getDerivativeSigmoid();

    // Calculate error proportion of hidden layer (64 x 10 * 10 x 20 = 64 x 20)
    Matrix errorProportion = Matrix::mul(HO.transpose(), deltaOutput);

    // Calculate delta hidden (64 x 20)
    Matrix deltaHidden = Matrix::elementWise(gradientHidden, errorProportion);

    // Multiply learning rate to delta output (10 x 20)
    deltaOutput.mul(LEARNING_RATE);

    // Adjust bias of hidden-output (10 x 1 + average(10 x 20) = 10 x 1)
    BO.add(deltaOutput.getAvg());

    // Calculate the delta for weights of hidden-output and adjust them
    // (10 x 20 * 20 x 64 = 10 x 64) 
    Matrix deltaWeightsHO = Matrix::mul(deltaOutput, outputHidden.transpose());
    // (10 x 64 + 10 x 64 = 10 x 64)
    HO.add(deltaWeightsHO);

    // Multiply learning rate to delta hidden
    deltaHidden.mul(LEARNING_RATE);

    // Adjust bias of input-hidden (64 x 1 + average(64 x 20) = 64 x 1)
    BH.add(deltaHidden.getAvg());

    // Calculate the delta for weights of input-hidden and adjust them 
    Matrix deltaWeightsIH = Matrix::mul(deltaHidden, inputMatrix.transpose());
    IH.add(deltaWeightsIH);
}

void NeuralNetwork::mutate(float rate) {
    IH.mutate(rate); 
    BH.mutate(rate);
    HO.mutate(rate);
    BO.mutate(rate);
}

void NeuralNetwork::saveNetwork(const char filename[]) {
    ofstream fout;
    fout.open(filename, std::ios_base::out);

    fout << numInput << " " << numHidden << " " << numOutput << '\n';

    for (int i = 0; i < IH.n; ++i) {
        for (int j = 0; j < IH.m; ++j) {
            fout << IH.mat[i][j] << " ";
        }
        fout << '\n';
    }

    fout << '\n';

    for (int i = 0; i < BH.n; ++i) {
        for (int j = 0; j < BH.m; ++j) {
            fout << BH.mat[i][j] << " ";
        }
        fout << '\n';
    }

    for (int i = 0; i < HO.n; ++i) {
        for (int j = 0; j < HO.m; ++j) {
            fout << HO.mat[i][j] << " ";
        }
        fout << '\n';
    }

    for (int i = 0; i < BO.n; ++i) {
        for (int j = 0; j < BO.m; ++j) {
            fout << BO.mat[i][j] << " ";
        }
        fout << '\n';
    }

    fout.close();
}

void NeuralNetwork::loadNetwork(const char filename[]) {
    ifstream fin;
    fin.open(filename);
    fin >> numInput >> numHidden >> numOutput;

    for (int i = 0; i < numHidden; ++i) {
        for (int j = 0; j < numInput; ++j) {
            fin >> IH.mat[i][j];
        }
    }

    for (int i = 0; i < numHidden; ++i) {
        for (int j = 0; j < 1; ++j) {
            fin >> BH.mat[i][j];
        }
    }

    for (int i = 0; i < numOutput; ++i) {
        for (int j = 0; j < numHidden; ++j) {
            fin >> HO.mat[i][j];
        }
    }

    for (int i = 0; i < numOutput; ++i) {
        for (int j = 0; j < 1; ++j) {
            fin >> BO.mat[i][j];
        }
    }

    fin.close();
}

NeuralNetwork NeuralNetwork::crossover(const NeuralNetwork &parA, const NeuralNetwork &parB) {
    NeuralNetwork child(parA.numInput, parA.numHidden, parA.numOutput);
    for (int i = 0; i < parA.IH.n; ++i) {
        for (int j = 0; j < parA.IH.m; ++j) {
            child.IH.mat[i][j] = random(0, 1) < 0.5f ? parA.IH.mat[i][j] : parB.IH.mat[i][j];
        }
    }

    for (int i = 0; i < parA.HO.n; ++i) {
        for (int j = 0; j < parA.HO.m; ++j) {
            child.HO.mat[i][j] = random(0, 1) < 0.5f ? parA.HO.mat[i][j] : parB.HO.mat[i][j];
        }
    }

    for (int i = 0; i < parA.BH.n; ++i) {
        for (int j = 0; j < parA.BH.m; ++j) {
            child.BH.mat[i][j] = random(0, 1) < 0.5f ? parA.BH.mat[i][j] : parB.BH.mat[i][j];
        }
    }

    for (int i = 0; i < parA.BO.n; ++i) {
        for (int j = 0; j < parA.BO.m; ++j) {
            child.BO.mat[i][j] = random(0, 1) < 0.5f ? parA.BO.mat[i][j] : parB.BO.mat[i][j];
        }
    }
    return child;
}
