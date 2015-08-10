#include <fstream>
#include <iostream>
#include "str_util.h"
#include "infer.h"

void Infer::load_model() {
  string pz_pt = model_dir + ".pz";
  pz.load(pz_pt);

  string pt = model_dir + ".pw_z";
  cout << "load p(w|z):" << pt << endl;
  pw_z.load(pt);
  K = pw_z.rowNum();
  W = pw_z.colNum();
  cout << "K=" << K << ", W=" << W << endl;
}

void Infer::run(string docs_pt) {
  cout << "Infer p(z|d) for docs in: " << docs_pt << endl;
  ifstream rf(docs_pt.c_str());
  assert(rf);

  string pt = res_dir + ".pz_d";
  ofstream wf(pt.c_str());
  assert(wf);
  cout << "write p(z|d): " << pt << endl;

  string line;
  int nd = 0;
  while (getline(rf, line)) {
    nd++;
    if (nd % 10000 == 0)
      cout << "\r" << nd << flush;
    Docs docs(line);
    vector<Biterm> bs = docs.gen_biterms();
    //Pvec<double> pz_d = biterms_infer2(bs);
    Pvec<double> pz_d = inferByPw_z(docs);

    if (!docs.id.empty())
      wf << docs.id << "\t";
    wf << pz_d.str() << endl;
  }
}

Pvec<double> Infer::biterms_infer(vector<Biterm> &bs) {
  Pvec<double> pz_d(K);
  // randomly assign a topic to biterm
  for (size_t i = 0; i < bs.size(); ++i)
    pz_d += compute_pz_b(bs[i]);

  return pz_d.normalize();
}

Pvec<double> Infer::biterms_infer2(vector<Biterm> &bs) {
  Pvec<double> pz_d(K);
  // randomly assign a topic to biterm
  for (size_t i = 0; i < bs.size(); ++i)
    pz_d += compute_pz_b2(bs[i]);

  return pz_d / bs.size();
}

Pvec<double> Infer::inferByPw_z(Docs &docs) {
  Pvec<double> pz_d(K);
  vector<int> ws = docs.get_ws();
  for (int i = 0; i < ws.size(); ++i) {
    for (int k = 0; k < K; ++i)
      pz_d[k] += pw_z[k][ws[i]];
  }
  return pz_d;
}

// compute p(z|w_i, w_j)
Pvec<double> Infer::compute_pz_b2(Biterm &bi) {
  Pvec<double> pz_b(K);
  int w1 = bi.wi;
  int w2 = bi.wj;

  // bursty topic
  for (int k = 0; k < K; ++k) {
    pz_b[k] = pw_z[k][w1] * pw_z[k][w2];
  }

  return pz_b;
}

// compute p(z|w_i, w_j)
Pvec<double> Infer::compute_pz_b(Biterm &bi) {
  Pvec<double> pz_b(K);
  int w1 = bi.wi;
  int w2 = bi.wj;
  double eta = etas[w1][w2];

  // common topic
  pz_b[0] = (1 - eta) * pw_z[0][w1] * pw_z[0][w2];

  // bursty topic
  for (int k = 1; k < K; ++k) {
    pz_b[k] = eta * pw_z[k][w1] * pw_z[k][w2] * pz[k];
  }

  return pz_b.normalize();
}


int main(int argc, char *argv[]) {
  int i = 0;
  double a = atof(argv[++i]);
  string etas_pt(argv[++i]);
  string model_dir(argv[++i]);
  string docs_pt(argv[++i]);
  string res_dir(argv[++i]);
  cout << "----- Run inference -----" << endl;
  Infer inf(a, etas_pt, model_dir, res_dir);
  inf.run(docs_pt);
}
