#ifndef _INFERLDA_H
#define _INFERLDA_H

#include <string>
#include <map>
#include "biterm.h"
#include "Pvec.h"
#include "Pmat.h"
#include "SpMat.h"
#include "docs.h"

using namespace std;

class Infer {
private:
  int K;
  int W;

  Pmat<double> pw_z;    // p(w|z) = phi, (size K+1) * M
  Pvec<double> pz;
  SpMat<double> etas;  // P(e=1|wi, wj)

  string model_dir;
  string res_dir;

private:
  void load_model();

  Pvec<double> biterms_infer(vector<Biterm> &bs);
  Pvec<double> biterms_infer2(vector<Biterm> &bs);
  Pvec<double> inferByPw_z(Docs &docs);

  // compute condition distribution p(z|b)
  Pvec<double> compute_pz_b(Biterm &bi);
  Pvec<double> compute_pz_b2(Biterm &bi);

public:

  Infer(double a, string eta_pt, string model_dir, string res_dir) :
          model_dir(model_dir), res_dir(res_dir) {
    cout << "load eta:" << eta_pt << endl;
    etas.loadFromTriples(eta_pt);
    load_model();
  }

  void run(string docs_pt);
};

#endif
