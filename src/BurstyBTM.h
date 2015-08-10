/**
 * Bursty Biterm topic model(BTM) with Gbbis sampling implementation
 * Author: Xiaohui Yan(xhcloud@gmail.com)
 * Paper:
 *     `Xiaohui Yan, et al. A Probabilistic Model for Bursty Topic Discovery in Microblogs. AAAI 2015.`
 * 2014-6-25
 */
#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include <string>
#include <map>

#include "biterm.h"
#include "Pvec.h"
#include "Pmat.h"
#include "SpMat.h"
#include "log_util.h"

using namespace std;
using namespace log_util;

class BurstyBTM {
private:
  char type;    // 's' is simplified, 'n' is normal
  // Notice: K is number of bursty topic, the total number of topics
  // is K+1, since there is a background topic
  int K;
  int W;        // vocabulary size

  double alpha;        // hyperparameters of p(z|t0)
  double beta;        // hyperparameters of p(w|z,t0)

  int n_iter;      // maximum number of iteration of Gibbs Sampling

  Pvec<int> nb_z;    // number of biterms assigned to topic z, size (K+1)*1
  int nb;        // number of biterms assgined to bursty topics
  Pmat<int> nwz;    // times of w in bursty topic z, size (K+1)*W

  vector<Biterm> bs;
  SpMat<double> etas;


public:
  BurstyBTM(char t, int k, int w, double a, double b, int iter, string etas_pt) :
          type(t), K(k), W(w), alpha(a), beta(b), n_iter(iter), nb(0) {
    srand(time(NULL));
    nwz.resize(K + 1, W);
    nb_z.resize(K + 1);
    cout << "load eta: " << etas_pt << endl;
    etas.loadFromTriples(etas_pt);
  }

  // run estimate procedures
  void run(string docs_pt, string model_dir);

protected:
  // initialize memeber varibles and biterms
  void rand_assign(Biterm &);

  void load_docs(string pt);

  // update estimate of a biterm
  void update_biterm(Biterm &bi);

  // reset topic proportions for biterm b
  void reset_biterm_topic(Biterm &bi);

  // assign topic proportions for biterm b
  void assign_biterm_topic(Biterm &bi, int k);

  // compute condition distribution p(z|b)
  Pvec<double> compute_pz_b(Biterm &bi);

  void save_res(string model_dir);

  void save_pz(string pt);

  void save_pw_z(string pt);
};

#endif
