#include <string.h>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BurstyBTM.h"

using namespace std;

void usage(char *pname) {
  cout << "Usage:" << pname
  << "<type> <K> <W> <alpha> <beta> <n_iter> <etas_pt> <docs_pt> <model_dir>\n"
  << "\ttype\t's' means simplified BurstyBTM, 'n' means normal BurstyBTM\n"
  << "\tK\tint, number of topics, like 20\n"
  << "\tW\tint, the size of vocabulary\n"
  << "\talpha\tdouble, Symmetric Dirichlet prior of P(z), like 1\n"
  << "\tbeta\tdouble, Symmetric Dirichlet prior of P(w|z), like 0.01\n"
  << "\tn_iter\tint, number of iterations of Gibbs sampling\n"
  << "\tetas_pt\tstring, each line is a biterm with the format 'wi wj eta'\n"
  << "\tdocs_pt\tstring, each line is a doc with the format 'w1 w2 ...'\n"
  << "\tmodel_dir\tstring, output directory\n";
}

int main(int argc, char *argv[]) {
  if (argc < 10) {
    usage(argv[0]);
    return 1;
  }

  int i = 1;         // the ith argv
  char type = argv[i++][0];
  int K = atoi(argv[i++]);                  // topic num
  int W = atoi(argv[i++]);
  double alpha = atof(argv[i++]);    // hyperparameters of p(z)
  double beta = atof(argv[i++]);
  int n_iter = atoi(argv[i++]);
  string etas_pt(argv[i++]);
  string docs_pt(argv[i++]);
  string model_dir(argv[i++]);

  cout << "==== BurstyBTM Gibbs, K=" << K
  << ", W=" << W
  << ", alpha=" << alpha
  << ", beta=" << beta
  << ", n_iter=" << n_iter
  << endl;

  BurstyBTM model(type, K, W, alpha, beta, n_iter, etas_pt);
  model.run(docs_pt, model_dir);
}
