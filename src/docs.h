#ifndef _DOCS_H
#define _DOCS_H

#include <string>
#include <vector>
#include <list>
#include <cassert>
#include <sstream>

#include "doc.h"
#include "biterm.h"

using namespace std;

class Docs {
public:
  string id;
  vector<Doc> ds;  // doc sequence

public:
  Docs(const string &s) {
    read_docs(s);
  }

  /**
   * Extract biterms from a docsument
   *   `win`:  window size for biterm extraction
   *   `bs`: the output biterms
   */
  vector<Biterm> gen_biterms(int win = 15) const {
    vector<Biterm> bs;
    for (int i = 0; i < ds.size(); ++i) {
      vector<Biterm> d_bs = ds[i].gen_biterms(win);
      bs.insert(bs.end(), d_bs.begin(), d_bs.end());
    }

    return bs;
  }

  vector<int> get_ws() const {
    vector<int> ws;
    for (int d = 0; d < ds.size(); ++d) {
      vector<int> d_ws = ds[d].ws;
      ws.insert(ws.end(), d_ws.begin(), d_ws.end());
    }
    return ws;
  }

private:
  void read_docs(const string &s) {
    vector<string> tmps = str_util::split(s, '\t');
    id = tmps[0];
    // parse docs
    string docs_str = tmps[1];
    vector<string> docs = str_util::split(docs_str, '.');
    for (int d = 0; d < docs.size(); ++d)
      ds.push_back(Doc(docs[d]));
  }
};

#endif
