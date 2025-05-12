#include "matrix.hpp"

Matrix::Matrix() {
    n = 0;
    m = 0;
}

Matrix::Matrix(int _n, int _m) {
    n = _n;
    m = _m;
    for (int i = 0; i < n; ++i) {
        mat.push_back(vector<float>(m, 0));
    }
}

Matrix::Matrix(const Matrix &other) {
    n = other.n;
    m = other.m;
    mat.clear();
    if (mat.empty()) {
        for (int i = 0; i < n; ++i) {
            mat.push_back(vector<float>(m, 0));
        }
    }
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            mat[i][j] = other.mat[i][j];
}

Matrix Matrix::copy() const {
    Matrix res(n, m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            res.mat[i][j] = mat[i][j];
        }
    }
    return res;
}

Matrix Matrix::toMatrix(int n, float a[]) {
    Matrix res(n, 1);
    for (int i = 0; i < n; ++i) res.mat[i][0] = a[i];
    return res;
}

Matrix Matrix::toMatrix(const vector<vector<float>> &v) {
    int rows = (int)v.size(), cols = (int)v[0].size();
    Matrix res(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            res.mat[i][j] = v[i][j];
        }
    }
    return res.transpose();
}

void Matrix::toArray(float res[]) {
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            res[i * m + j] = mat[i][j];
}

void Matrix::toVector(vector<vector<float>> &v) {
    Matrix curT = this->transpose();
    v.assign(curT.n, vector<float>(curT.m, 0));

    for (int i = 0; i < curT.n; ++i) {
        for (int j = 0; j < curT.m; ++j) {
            v[i][j] = curT.mat[i][j];
        }
    }
}

void Matrix::print() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) 
            cout << mat[i][j] << " ";
        cout << '\n';
    }
}

void Matrix::randomize() {
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            mat[i][j] = random(-1, 1);
}

void Matrix::mapSigmoid() {
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            mat[i][j] = sigmoid(mat[i][j]);
}

void Matrix::mapTanh() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            mat[i][j] = tanh(mat[i][j]);
        }
    }
}

Matrix Matrix::getDerivativeSigmoid() const {
    // Assume that the matrix's element has been mapped to sigmoid function
    Matrix res(n, m);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            res.mat[i][j] = mat[i][j] * (1 - mat[i][j]);
        }
    }
    
    return res;
}

void Matrix::mutate(float rate) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (random(0.0, 1.0) < rate) {
                mat[i][j] += randomGaussian(0.0f, 0.1f);
            }
        }
    }
}

Matrix Matrix::transpose() {
    Matrix res(m, n);
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            res.mat[j][i] = mat[i][j];
    return res;
}

void Matrix::add(const Matrix &other) {
    if (n != other.n || m != other.m) {
        cout << "Add matrix requires same length!\n";
        cout << n << " " << m << " " << other.n << " " << other.m << '\n';
        return;
    }

    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            this->mat[i][j] += other.mat[i][j];
}

void Matrix::add(const float &num) {
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            this->mat[i][j] += num;
}

Matrix Matrix::add(const Matrix &a, const Matrix &b) {
    if (a.n != b.n || a.m != b.m) {
        cout << "(static) Add matrix requires same length!\n";
        return Matrix();
    }
    Matrix res(a.n, a.m);
    for (int i = 0; i < a.n; ++i) 
        for (int j = 0; j < a.m; ++j)
            res.mat[i][j] = a.mat[i][j] + b.mat[i][j];
    return res;
}

void Matrix::sub(const Matrix &other) {
    if (n != other.n || m != other.m) {
        cout << "Subtract matrix requires same length!\n";
        return;
    }

    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            this->mat[i][j] -= other.mat[i][j];
}

void Matrix::sub(const float &num) {
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            this->mat[i][j] -= num;
}

Matrix Matrix::sub(const Matrix &a, const Matrix &b) {
    if (a.n != b.n || a.m != b.m) {
        cout << "(static) Add matrix requires same length!\n";
        return Matrix();
    }
    Matrix res(a.n, a.m);
    for (int i = 0; i < a.n; ++i) 
        for (int j = 0; j < a.m; ++j)
            res.mat[i][j] = a.mat[i][j] - b.mat[i][j];
    return res;
}

void Matrix::mul(const float &k) {
    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < m; ++j) 
            this->mat[i][j] *= k;
}

Matrix Matrix::mul(const Matrix &a, const Matrix &b) {
    if (a.m != b.n) {
        cout << "Multiplying matrix length not appropriate!\n";
        return Matrix();
    }
    Matrix res(a.n, b.m);
    for (int i = 0; i < a.n; ++i) 
        for (int j = 0; j < b.m; ++j)
            for (int k = 0; k < a.m; ++k)
                res.mat[i][j] += a.mat[i][k] * b.mat[k][j];
    return res;
}

Matrix Matrix::elementWise(const Matrix &a, const Matrix &b) {
    if (a.n != b.n || a.m != b.m) {
        cout << "Element wise invalid!\n";
        return Matrix();
    }
    Matrix res(a.n, a.m);
    for (int i = 0; i < a.n; ++i) {
        for (int j = 0; j < a.m; ++j) {
            res.mat[i][j] = a.mat[i][j] * b.mat[i][j];
        }
    }
    return res;
}

void Matrix::broadcast(const Matrix &other) {
    if (n != other.n || other.m != 1) {
        cout << "Invalid broadcast!\n";
        return;
    }

    for (int j = 0; j < m; ++j) {
        for (int i = 0; i < n; ++i) {
            mat[i][j] += other.mat[i][0];
        }
    }
}

Matrix Matrix::broadcast(const Matrix &a, const Matrix &b) {
    if (a.n != b.n || b.m != 1) {
        cout << "(static) Invalid broadcast!\n";
        return Matrix();
    }

    Matrix res = a.copy();
    for (int j = 0; j < res.m; ++j) {
        for (int i = 0; i < res.n; ++i) {
            res.mat[i][j] += b.mat[i][0];
        }
    }

    return res;
}

Matrix Matrix::getAvg() const {
    Matrix avg(n, 1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            avg.mat[i][0] += mat[i][j];
        }
        avg.mat[i][0] /= (float)m;
    }
    return avg;
}
