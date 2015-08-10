#ifndef _BITERM_H
#define _BITERM_H

#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <cassert>

using namespace std;

class Biterm {
public:
  int wi;
  int wj;
  int z;				// topic assignment

public:
  Biterm(int w1, int w2) :
	z(-1) {
	wi = min(w1, w2);
	wj = max(w1, w2);
  }

  // s format:wi wj   z
  Biterm(string s) :
	wi(0), wj(0), z(-1) {
	istringstream iss(s);
	iss >> wi >> wj >> z;
  }

  void set_z(int k) {
	z = k;
  }
  void reset_z() {
	z = -1;
  }

  string str() const {
	ostringstream os;
	os << wi << ' ' << wj << '\t' << z;
	return os.str();
  }
};

#endif
