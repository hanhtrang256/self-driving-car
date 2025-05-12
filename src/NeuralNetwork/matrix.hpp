#ifndef MATRIX_H
#define MATRIX_H

#include <iostream> 
#include <random>
#include <cmath>
#include "../utils/utils.hpp"
using namespace std;

struct Matrix {
    int n, m; 
    vector<vector<float>> mat;
    Matrix();
    Matrix(int _n, int _m);
    Matrix(const Matrix &other);
    Matrix copy() const;
    static Matrix toMatrix(int n, float a[]);
    static Matrix toMatrix(const vector<vector<float>> &v);
    void toArray(float res[]);
    void toVector(vector<vector<float>> &v);
    void print();
    void randomize();
    void mapSigmoid();
    void mapTanh();
    Matrix getDerivativeSigmoid() const;
    void mutate(float rate);
    Matrix transpose();
    void add(const Matrix &other);
    void add(const float &num);
    static Matrix add(const Matrix &a, const Matrix &b);
    void sub(const Matrix &other);
    void sub(const float &num);
    static Matrix sub(const Matrix &a, const Matrix &b);
    void mul(const float &k);
    static Matrix mul(const Matrix &a, const Matrix &b);
    static Matrix elementWise(const Matrix &a, const Matrix &b);
    void broadcast(const Matrix &other);
    static Matrix broadcast(const Matrix &a, const Matrix &b);
    Matrix getAvg() const;
};

#endif