/*
 * (dense) Positive Matrix, each row is a Pvec
 *
 *  Created on: 2012-7-31
 *      Author: xhcloud@gmail.com
 */
#ifndef _PMAT_H
#define _PMAT_H

#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>
#include <ctime>

#include "Pvec.h"
#include "log_util.h"

using namespace std;
using namespace log_util;

template<class T>
class Pmat {
private:
  vector<Pvec<T> > mat;

public:
  Pmat() {
  }

  Pmat(int M, int N) :
          mat(M) {
    for (int i = 0; i < M; i++)
      mat[i].resize(N);
  }

  Pmat(int M, int N, T v) :
          mat(M) {
    for (int i = 0; i < M; i++)
      mat[i].resize(N, v);
  }

  template<class T2>
  Pmat(const Pmat<T2> &ma) {
    resize(ma.rowNum(), ma.colNum());
    for (int m = 0; m < ma.rowNum(); ++m)
      for (int n = 0; n < ma.colNum(); ++n)
        mat[m][n] = ma[m][n];
  }

  template<class T2>
  Pmat(const vector<vector<T2> > &ma) {
    mat.resize(ma.size());
    for (int r = 0; r < ma.size(); ++r) {
      mat[r].resize(ma[r].size());
      for (int c = 0; c < ma[r].size(); ++c) {
        mat[r][c] = ma[r][c];
      }
    }
  }

  void resize(int M, int N) {
    mat.resize(M);
    for (int i = 0; i < M; i++)
      mat[i].resize(N);
  }

  void resize(int M, int N, T v) {
    mat.resize(M);
    for (int i = 0; i < M; i++)
      mat[i].resize(N, v);
  }

  void rand_init() {
    for (int i = 0; i < rowNum(); ++i)
      mat[i].rand_init();
  }

  void fill(T v) {
    for (int i = 0; i < mat.size(); ++i)
      mat[i].fill(v);
  }

  // input format: v v v ...
  // dimensions of the matrix are determined by input data
  void load(const string &inf) {
    ifstream rf(inf.c_str());
    assert(rf);

    loadFileStream(rf);
  }

  void loadFileStream(ifstream &rf) {
    string line;
    while (getline(rf, line)) {
      // add a new row
      Pvec<T> r(line);
      add_row(r);
    }
  }

  int rowNum() const {
    return mat.size();
  }

  int colNum() const {
    return rowNum() ? mat[0].size() : 0;
  }

  /**
   * @return number of elements
   */
  int size() const {
    return rowNum() * colNum();
  }

  Pvec<T> &operator[](int m) {
    assert (m < mat.size());
    return mat[m];
  }

  const Pvec<T> &operator[](int m) const {
    return mat[m];
  }

  template<class T2>
  Pmat<T> &operator=(const vector<vector<T2> > &ma) {
    mat.resize(ma.size());
    for (int i = 0; i < mat.size(); ++i) {
      mat[i].resize(ma[i].size());
      for (int j = 0; j < mat[i].size(); ++j)
        mat[i][j] = ma[i][j];
    }
    return *this;
  }

  Pmat<T> operator+(const T &v) {
    Pmat<T> tp(this->rowNum(), this->colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      tp[i] = mat[i] + v;
    return tp;
  }

  Pmat<T> &operator+=(const T &v) {
    for (int i = 0; i < this->rowNum(); ++i)
      for (int j = 0; j < this->colNum(); ++j)
        mat[i][j] += v;

    return *this;
  }

  Pmat<T> operator+(const Pmat<T> &v) {
    assert(this->rowNum() == v.rowNum() && this->colNum() == v.colNum());
    Pmat<T> tp(this->rowNum(), this->colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      tp[i] = mat[i] + v[i];
    return tp;
  }

  Pmat<T> &operator+=(const Pmat<T> &v) {
    assert(this->rowNum() == v.rowNum() && this->colNum() == v.colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      mat[i] += v[i];
    return *this;
  }

  Pmat<T> operator-(const T &v) {
    assert(this->rowNum() == v.rowNum() && this->colNum() == v.colNum());
    Pmat<T> tp(this->rowNum(), this->colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      tp[i] = mat[i] - v;
    return tp;
  }

  Pmat<T> &operator-=(const T &v) {
    assert(this->rowNum() == v.rowNum() && this->colNum() == v.colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      mat[i] -= v;
    return *this;
  }

  Pmat<T> operator-(const Pmat<T> &v) {
    assert(this->rowNum() == v.rowNum() && this->colNum() == v.colNum());
    Pmat<T> tp(this->rowNum(), this->colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      tp[i] = mat[i] - v[i];
    return tp;
  }

  Pmat<T> &operator-=(const Pmat<T> &v) {
    assert(this->rowNum() == v.rowNum() && this->colNum() == v.colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      mat[i] -= v[i];
    return *this;
  }

  Pmat<T> operator*(const T &v) {
    Pmat<T> tp(this->rowNum(), this->colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      tp[i] = mat[i] * v;
    return tp;
  }

  Pmat<T> &operator*=(const T &v) {
    for (int i = 0; i < this->rowNum(); ++i)
      mat[i] *= v;
    return *this;
  }

  Pmat<T> operator/(const T &v) {
    Pmat<T> tp(this->rowNum(), this->colNum());
    for (int i = 0; i < this->rowNum(); ++i)
      tp[i] = mat[i] / v;
    return tp;
  }

  Pmat<T> &operator/=(const T &v) {
    for (int i = 0; i < this->rowNum(); ++i)
      mat[i] /= v;
    return *this;
  }

  void add_row(const Pvec<T> &v) {
    mat.push_back(v);
  }

  void push_back(const Pvec<T> &v) {
    mat.push_back(v);
  }

  void add_col(const Pvec<T> &v) {
    if (mat.size() == 0)
      mat.resize(v.size());
    else
      assert(mat.size() == v.size());

    for (int i = 0; i < mat.size(); ++i)
      mat[i].push_back(v[i]);
  }

  void erase_row(int start, int end) {
    assert(end >= start && end <= mat.size());
    mat.erase(mat.begin() + start, mat.begin() + end);
  }

  void erase_col(int start, int end) {
    for (int r = 0; r < mat.size(); ++r) {
      assert(end >= start && end <= mat[r].size());
      mat[r].erase(mat[r].begin() + start, mat[r].begin() + end);
    }
  }

  void clear() {
    mat.clear();
  }

  T sum() {
    T s = 0;
    for (int i = 0; i < rowNum(); ++i)
      s += mat[i].sum();
    return s;
  }

  Pvec<T> rowSum() {
    Pvec<T> s(mat.size());
    for (int i = 0; i < rowNum(); ++i)
      s[i] = mat[i].sum();
    return s;
  }

  T norm() {
    T s = 0;
    for (int i = 0; i < rowNum(); ++i)
      for (int j = 0; j < colNum(); ++j)
        s += mat[i][j] * mat[i][j];
    return sqrt(s);
  }

  /**
   * Returan a new matrix by normalizing to all entries sum to 1
   **/
  Pmat<double> normalize(double smoother = 0.0) {
    Pmat<double> mat2(rowNum(), colNum());
    double s = sum() + smoother * size();

    for (int i = 0; i < rowNum(); ++i)
      for (int j = 0; j < colNum(); ++j)
        mat2[i][j] = (mat[i][j] + smoother) / s;
    return mat2;
  }

  /**
   * Return a new matrix by normalizing elements in each row
   **/
  Pmat<double> normr(double smoother = 0.0) {
    Pmat<double> mat2(rowNum(), colNum());
    for (int i = 0; i < rowNum(); ++i)
      mat2[i] = mat[i].normalize(smoother);
    return mat2;
  }

  /**
   * Return a new matrix by normalizing elements in each column
   **/
  Pmat<double> normc(double smoother = 0.0) {
    Pmat<double> mat2(rowNum(), colNum());
    for (int j = 0; j < colNum(); ++j) {
      // calculate column sum
      double s = 0.0;
      for (int i = 0; i < rowNum(); ++i)
        s += mat[i][j] + smoother;

      for (int i = 0; i < rowNum(); ++i)
        mat2[i][j] = (mat[i][j] + smoother) / s;
    }
    return mat2;
  }

  void add1_log() {
    for (int i = 0; i < rowNum(); ++i)
      mat[i].add1_log();
  }

  Pmat<T> transpose() const {
    int N = rowNum();
    int M = colNum();
    Pmat<T> tmat(M, N);

    for (int i = 0; i < N; ++i)
      for (int j = 0; j < M; ++j)
        tmat[j][i] = mat[i][j];

    return tmat;
  }

  Pmat<double> toDouble() {
    return Pmat<double>(*this);
  }

  string str() {
    ostringstream os;
    int i;
    for (i = 0; i < rowNum(); ++i)
      os << mat[i].str() << endl;

    return os.str();
  }

  void write(const string &pt) {
    ofstream wf(pt.c_str());
    if (!wf)
      logError("Path not exists:" + pt);

    for (int i = 0; i < rowNum(); ++i)
      wf << mat[i].str() << endl;
  }
};

#endif
