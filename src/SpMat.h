/*
 * Sparse Matrix, sotred by row
 *
 *  Created on: 2012-9-23
 *      Author: Xiaohui Yan(l0he1g@gmail.com)
 */
#ifndef _SpMat_H
#define _SpMat_H

#include <cstdlib>
#include <cassert>
#include <fstream>
#include <sstream>
#include <exception>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include "str_util.h"
#include "log_util.h"

using namespace std;
using namespace log_util;

template<class T>
class Triple {
public:
  int i, j;
  T v;
public:
  Triple() : i(0), j(0), v(0) { }

  Triple(int i, int j, T v) : i(i), j(j), v(v) { }

  string str() {
    ostringstream os;
    os << i << '\t' << j << '\t' << v;
    return os.str();
  }
};

// T: int or float, double
template<class T>
class SpMat {
private:
  vector<map<int, T> > mat;

public:
  SpMat() { }

  SpMat(int M) : mat(M) { }

  SpMat(const SpMat &ma) : mat(ma.mat) { }

  void resize(int M) { mat.resize(M); }

  /**
   * Load sparse matrix in row format
   * each line is a row with format: "c:v c:v ..."
   */
  void load(const string &pt) {
    ifstream rf(pt.c_str());
    if(!rf) logError("cannot open file" + pt);
    mat.clear();

    string line;
    try {
      while (getline(rf, line)) {
        map<int, T> row = str_util::s2map(line);
        mat.push_back(row);
      }
    }
    catch (...) {
      logError("Bad Format in:" + line);
    }
  }

  /**
   * Load sparse matrix from triples
   * input format: i  j  v
   */
  void loadFromTriples(const string &pt) {
    ifstream rf(pt.c_str());
    if(!rf) logError("cannot open file" + pt);

    string line;
    try {
      int i, j;
      T v;
      while (getline(rf, line)) {
        istringstream iss(line);
        iss >> i >> j >> v;
        add(i, j, v);
      }
    }
    catch (...) {
      logError("Bad line:" + line);
    }
  }

  /** the number of rows **/
  int rowNum() const { return mat.size(); }

  /** the number of columns **/
  int colNum() const {
    int j = 0;
    for (int i = 0; i < mat.size(); ++i) {
      for (typename map<int, T>::const_iterator it = mat[i].begin();
           it != mat[i].end();
           ++it) {
        j = max(j, it->first);
      }
    }
    return j + 1;
  }

  int size() const {
    return rowNum() * colNum();
  }

  /** number of non-zero values **/
  int nnz() const {
    int n = 0;
    for (int i = 0; i < mat.size(); ++i) {
      n += mat[i].size();
    }
    return n;
  }

  /** sum of values **/
  T sum() const {
    T s = 0;
    for (int i = 0; i < mat.size(); ++i) {
      for (typename map<int, T>::const_iterator it = mat[i].begin();
           it != mat[i].end();
           ++it) {
        s += it->second;
      }
    }
    return s;
  }

  /** get M[i,j], if not exists, return 0 **/
  T get(int i, int j) const {
    if (i >= mat.size()) {
      return 0;
    }

    const map<int, T> &irow = mat[i];
    typename map<int, T>::const_iterator it = irow.find(j);
    if (it == irow.end()) {
      return 0;
    } else {
      return it->second;
    }
  }

  map<int, T> &operator[](int m) { return mat[m]; }

  const map<int, T> &operator[](int m) const { return mat[m]; }

  /** add v to M[i, j] **/
  void add(int i, int j, T v) {
    if (i >= mat.size()) {
      mat.resize(i + 1);
    }

    map<int, T> &irow = mat[i];
    typename map<int, T>::iterator it = irow.find(j);
    if (it == irow.end()) {
      irow.insert(make_pair(j, v));
    } else {
      it->second += v;    // accumulate
    }
  }

  /** set M[i,j] = v **/
  void set(int i, int j, T v) {
    if (i >= mat.size()) {
      mat.resize(i + 1);
    }

    map<int, T> &irow = mat[i];
    typename map<int, T>::iterator it = irow.find(j);
    if (it == irow.end()) {
      irow.insert(make_pair(j, v));
    } else {
      it->second = v;
    }
  }

  /** generate triples (i, j, v) **/
  vector<Triple<T> > toTriples() {
    vector<Triple<T> > vec;
    for (int i = 0; i < mat.size(); ++i) {
      for (typename map<int, T>::iterator it = mat[i].begin();
           it != mat[i].end();
           ++it) {
        vec.push_back(Triple<T>(i, it->first, it->second));
      }
    }
    return vec;
  }

  /**
   * Return a new Sparse Matrix by sum-1 normalizing over all elements
   **/
  SpMat<double> normalize(double smoother = 0.0) {
    SpMat<double> mat2(mat2.rowNum());
    double s = sum() + size() * smoother;
    for (int i = 0; i < rowNum(); ++i) {
      for (typename map<int, T>::iterator it = mat[i].begin();
           it != mat[i].end();
           ++it) {
        mat2[i][it->first] = it->second / s;
      }
    }
    return mat2;
  }

  /**
   * Return a new Sparse Matrix by normalizing each row
   **/
  SpMat<double> normr(double smoother = 0.0) {
    SpMat<double> mat2(mat2.rowNum());
    int C = colNum();

    for (int i = 0; i < rowNum(); ++i) {
      // calculate row sum
      double s = C * smoother;
      for (typename map<int, T>::iterator it = mat[i].begin();
           it != mat[i].end();
           ++it)
        s += it->second + smoother;

      for (typename map<int, T>::iterator it = mat[i].begin();
           it != mat[i].end();
           ++it)
        mat2[i][it->first] = it->second / s;
    }
    return mat2;
  }

  /** transform to string, each row with format:col1:v col2:v ... **/
  string str() {
    ostringstream os;

    for (int i = 0; i < mat.size(); ++i) {
      for (typename map<int, T>::iterator it = mat[i].begin();
           it != mat[i].end();
           ++it) {
        os << it->second << ':' << it->second << ' ';
      }
    }

    return os.str();
  }
};

#endif
