#ifndef _DOC_H
#define _DOC_H

#include <string>
#include <vector>
#include <list>
#include <cassert>
#include <sstream>

#include "biterm.h"

using namespace std;

class Doc {
public:
  string id;
  vector<int> ws;  // word sequence

public:
  Doc(const string &s) {
    read_doc(s);
  }

  int size() const {
    return ws.size();
  }

  /**
   * Extract biterms from a document
   *   `win`:  window size for biterm extraction
   *   `bs`: the output biterms
   */
  vector<Biterm> gen_biterms(int win = 15) const {
    vector<Biterm> bs;
    for (int i = 0; i < ws.size() - 1; ++i)
      for (int j = i + 1; j < min(i + win, int(ws.size())); ++j)
        bs.push_back(Biterm(ws[i], ws[j]));
    return bs;
  }

private:
  void read_doc(const string &s) {
    vector<string> tmps = str_util::split(s, '\t');
    string doc;
    if (tmps.size() == 1) {
      doc = s;
    } else {
      id = tmps[0];
      doc = tmps[1];
    }
    istringstream iss(doc);
    int w;
    while (iss >> w)
      ws.push_back(w);
  }
};

#endif

