#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "sampler.h"
#include "BurstyBTM.h"
#include "doc.h"
#include "str_util.h"

void BurstyBTM::run(string eta_pt, string dwid_pt, string model_dir) {
	load_etas(eta_pt);

	load_docs(dwid_pt);

	cout << "Begin iteration" << endl;
	int it = 1;
	for (; it < n_iter + 1; ++it) {
		for (size_t b = 0; b < bs.size(); ++b) {
			update_biterm(bs[b]);
		}
		cout << "iter " << it << " nb/N = " << nb << "/" << bs.size() << " = "
				<< double(nb) / bs.size() << endl;

		if (it % save_step == 0 || it == n_iter)
			save_res(model_dir + "iter" + str_util::itos(it) + ".");
	}
}

/**
 * load eta for each biterms
 * Input file format: w1 w2    eta
 */
void BurstyBTM::load_etas(string pt) {
	cout << "load etas: " << pt << endl;
	ifstream rf(pt.c_str());
	if (!rf) {
		cout << "[Error] Cannot open file:" << pt << endl;
		exit(1);
	}

	string line;
	int wi, wj;
	double eta;
	while (getline(rf, line)) {
		istringstream iss(line);
		iss >> wi >> wj >> eta;
		etas[wi][wj] = eta;
	}
}

/**
 * load biterms from documents
 *   Input file format: wid  wid  wid ...
 *   each line is a doc
 */
void BurstyBTM::load_docs(string pt) {
	cout << "load docs: " << pt << endl;
	ifstream rf(pt.c_str());
	if (!rf) {
		cout << "file not find:" << pt << endl;
		exit(-1);
	}

	string line;
	while (getline(rf, line)) {
		vector<Biterm> doc_bs;
		Doc doc(line);
		doc.gen_biterms(doc_bs);
		// statistic the exmperial word distribution
		for (int i = 0; i < doc.size(); ++i) {
			int w = doc.get_w(i);
			pw_b[w] += 1;
		}

		// randomly assign a topic to biterm
		for (size_t i = 0; i < doc_bs.size(); ++i) {
			rand_assign(doc_bs[i]);

			// simplified BurstyBTM will just drop biterms with z = 0
			if (type == 's' && doc_bs[i].z == 0)
				continue;
			bs.push_back(doc_bs[i]);
		}
	}

	pw_b.normalize();
	cout << "n(sampling biterm)=" << bs.size() << endl;
}

// random assign a topic to bi
void BurstyBTM::rand_assign(Biterm& bi) {
	int e = Sampler::Bern_sample(etas[bi.wi][bi.wj]);
	int k = 0;
	if (e == 1)
		k = Sampler::uni_sample(K) + 1;
	// k = Sampler::uni_sample(K+1);
	assign_biterm_topic(bi, k);
}

// sample procedure for ith biterm 
void BurstyBTM::update_biterm(Biterm& bi) {
	reset_biterm_topic(bi);
	// compute p(z|b)
	Pvec<double> pz;
	compute_pz_b(bi, pz);
	// sample topic for biterm b
	int k = Sampler::mult_sample(pz.to_vector());
	assign_biterm_topic(bi, k);
}

// reset topic assignment of biterm i
void BurstyBTM::reset_biterm_topic(Biterm& bi) {
	int w1 = bi.wi;
	int w2 = bi.wj;
	int k = bi.z;

	if (k > 0)
		nb--;
	nb_z[k]--;
	nwz[k][w1]--;
	nwz[k][w2]--;
	assert(nb >= 0 && nb_z[k] >= 0 && nwz[k][w1] >= 0 && nwz[k][w2] >= 0);
	bi.reset_z();
}

// compute p(z|w_i, w_j)
void BurstyBTM::compute_pz_b(Biterm& bi, Pvec<double>& pz) {
	pz.resize(K + 1);
	int w1 = bi.wi;
	int w2 = bi.wj;
	double eta = etas[w1][w2];

	double pw1k, pw2k, pk_t, deno;
	// common topic
	deno = 2 * (bs.size() - nb) + W * beta;
	if (type == 's') {
		pz[0] = 0;
	} else if (fixed_background) {
		pw1k = pw_b[w1];
		pw2k = pw_b[w2];
		pz[0] = (1 - eta) * pw1k * pw2k;

	} else {
		pw1k = (nwz[0][w1] + beta) / deno;
		pw2k = (nwz[0][w2] + beta) / (deno + 1);
		pz[0] = (1 - eta) * pw1k * pw2k;
	}

	// bursty topic
	for (int k = 1; k < K + 1; ++k) {
		deno = 2 * nb_z[k] + W * beta;
		pw1k = (nwz[k][w1] + beta) / deno;
		pw2k = (nwz[k][w2] + beta) / (deno + 1);
		pk_t = (nb_z[k] + alpha) / (nb + K * alpha);
		pz[k] = eta * pk_t * pw1k * pw2k;
	}

	pz.normalize();
}

// assign topic k to biterm i
void BurstyBTM::assign_biterm_topic(Biterm& bi, int k) {
	int w1 = bi.wi;
	int w2 = bi.wj;

	if (k > 0)
		nb++;
	nb_z[k]++;
	nwz[k][w1]++;
	nwz[k][w2]++;
	bi.set_z(k);
}

void BurstyBTM::save_res(string dir) {
	string pt = dir + "pz";
	cout << "write p(z): " << pt << endl;
	save_pz(pt);

	string pt2 = dir + "pw_z";
	cout << "write p(w|z): " << pt2 << endl;
	save_pw_z(pt2);
}

// p(z) is determinated by the overall proportions
// of biterms in it
void BurstyBTM::save_pz(string pt) {
	Pvec<double> pz(nb_z);
	pz.normalize(alpha);
	pz.write(pt);
}

// save the K bursty topics: a KxW matrix
void BurstyBTM::save_pw_z(string pt) {
	Pmat<double> pw_z(K, W, beta);
	pw_z += nwz;
	pw_z.normr();
	pw_z.write(pt);
}
