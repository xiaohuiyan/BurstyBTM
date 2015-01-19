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
#include "pvec.h"
#include "pmat.h"

using namespace std;

class BurstyBTM {
private:
	char type;    // 's' is simplified, 'n' is normal
	// Notice: K is number of bursty topic, the total number of topics
	// is K+1, since there is a background topic
	int K;
	int W;				// vocabulary size

	double alpha;	      // hyperparameters of p(z|t0)
	double beta;        // hyperparameters of p(w|z,t0)

	int n_iter;			// maximum number of iteration of Gibbs Sampling
	int save_step;

	Pvec<int> nb_z;	  // number of biterms assigned to topic z, size (K+1)*1
	int nb;			  // number of biterms assgined to bursty topics
	Pmat<int> nwz;	  // times of w in bursty topic z, size (K+1)*W

	Pvec<double> pw_b;   // the background word distribution
	bool fixed_background;  // fix the background word distribution to the empirical word distribution

	vector<Biterm> bs;
	vector<map<int, double> > etas;

public:
	BurstyBTM(char t, int k, int w, double a, double b, int iter, int step, bool fix_b = false) :
			type(t), K(k), W(w), alpha(a), beta(b), n_iter(iter),
			save_step(step), nb(0), fixed_background(fix_b) {
		srand(time(NULL));
		pw_b.resize(W);
		nwz.resize(K + 1, W);
		nb_z.resize(K + 1);
		etas.resize(W);
	}

	// run estimate procedures
	void run(string eta_pt, string dwid_pt, string model_dir);

protected:
	// intialize memeber varibles and biterms
	void rand_assign(Biterm&);

	void load_etas(string eta_pt);
	void load_docs(string pt);

	// update estimate of a biterm
	void update_biterm(Biterm& bi);

	// reset topic proportions for biterm b
	void reset_biterm_topic(Biterm& bi);

	// assign topic proportions for biterm b
	void assign_biterm_topic(Biterm& bi, int k);

	// compute condition distribution p(z|b)
	void compute_pz_b(Biterm& bi, Pvec<double>& p);

	void save_res(string model_dir);
	void save_pz(string pt);
	void save_pw_z(string pt);
};

#endif
