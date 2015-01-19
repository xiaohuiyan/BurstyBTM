#include <string.h>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BurstyBTM.h"

using namespace std;

void usage(char* pname) {
	cout << "Usage:" << pname
			<< " <K> <W> <alpha> <beta> <n_iter> <save_step> <biterm_pt> <model_dir> <fix_b>\n"
			<< "\ttype\t's' means simplified BurstyBTM, 'n' means normal BurstyBTM\n"
			<< "\tK\tint, number of topics, like 20\n"
		    << "\tW\tint, the size of vocabulary\n"
			<< "\talpha\tdouble, Symmetric Dirichlet prior of P(z), like 1\n"
			<< "\tbeta\tdouble, Symmetric Dirichlet prior of P(w|z), like 0.01\n"
			<< "\tn_iter\tint, number of iterations of Gibbs sampling\n"
			<< "\tsave_step\tint, steps to save the results\n"
			<< "\tbiterm_pt\tstring, path of training biterms, each line is a biterm with the format 'wi wj eta'\n"
			<< "\tmodel_dir\tstring, output directory\n"
			<< "\tfix_b\t'y' means fixing the background word distribution to the empirical word distribution\n";
}

int main(int argc, char* argv[]) {
	if (argc < 11) {
		usage(argv[0]);
		return 1;
	}

	int i = 1;				 // the ith argv
	char type = argv[i++][0];
	int K = atoi(argv[i++]);                  // topic num
	int W = atoi(argv[i++]);
	double alpha = atof(argv[i++]);    // hyperparameters of p(z)
	double beta = atof(argv[i++]);
	int n_iter = atoi(argv[i++]);
	int save_step = atoi(argv[i++]);
	string eta_pt(argv[i++]);
	string dwid_pt(argv[i++]);
	string model_dir(argv[i++]);
	bool fix_b = false;
	if (argv[i++][0] == 'y')
		fix_b = true;

	cout << "==== BurstyBTM Gibbs, K=" << K
			<<", W=" << W
			<< ", alpha=" << alpha
			<< ", beta=" << beta
			<< ", n_iter=" << n_iter
			<< ", save_step=" << save_step
			<< ", fix_b=" << fix_b
			<< " ====" << endl;

	BurstyBTM model(type, K, W, alpha, beta, n_iter, save_step, fix_b);
	model.run(eta_pt, dwid_pt, model_dir);
}
