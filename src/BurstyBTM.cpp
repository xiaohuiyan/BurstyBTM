#include <iostream>
#include <fstream>
#include <algorithm>

#include "sampler.h"
#include "BurstyBTM.h"
#include "doc.h"

void BurstyBTM::run(string docs_pt, string model_dir) {
  cout << "load docs: " << docs_pt << endl;
  load_docs(docs_pt);

  cout << "Begin iteration" << endl;
  int it = 1;
  for (; it < n_iter + 1; ++it) {
    for (size_t b = 0; b < bs.size(); ++b) 
      update_biterm(bs[b]);
	
    cout << "iter " << it
		 << " nb/N = " << nb
		 << "/" << bs.size()
		 << " = " << double(nb) / bs.size() << endl;
	if (it % 10 == 0)
	  save_res(model_dir);
  }

  save_res(model_dir);
}


/**
 * load biterms from documents
 *   Input file format: wid  wid  wid ...
 *   each line is a doc
 */
void BurstyBTM::load_docs(string pt) {
  ifstream rf(pt.c_str());
  if (!rf)
    logError("file not find:" + pt);

  string line;
  int n_filter = 0;
  while (getline(rf, line)) {
    Doc doc(line);
    vector<Biterm> doc_bs = doc.gen_biterms();
	
    // randomly assign a topic to biterm
    for (size_t i = 0; i < doc_bs.size(); ++i) {
	  Biterm& bi = doc_bs[i];
      rand_assign(bi);

      // simplified BurstyBTM will just drop biterms with z = 0
      if (type == 's' && bi.z == 0)
        continue;
	  if (etas[bi.wi][bi.wj] == 0) {
		n_filter++;
		continue;
	  }
      bs.push_back(bi);
    }
  }
  
  cout << "n(sampling biterm)=" << bs.size() 
	   << ", n(filter)=" << n_filter 
	   << ", filter ratio=" << float(n_filter)/ (bs.size() + n_filter) << endl;
}

// random assign a topic to bi
void BurstyBTM::rand_assign(Biterm& bi) {
  int k = 0;
  int e = Sampler::Bern_sample(etas[bi.wi][bi.wj]);
  if (e == 1)
    k = Sampler::uni_sample(K) + 1;

  assign_biterm_topic(bi, k);
}

// sample procedure for ith biterm 
void BurstyBTM::update_biterm(Biterm& bi) {
  reset_biterm_topic(bi);
  // compute p(z|b)
  Pvec<double> pz = compute_pz_b(bi);
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
Pvec<double> BurstyBTM::compute_pz_b(Biterm& bi) {
  Pvec<double> pz(K + 1);
  int w1 = bi.wi;
  int w2 = bi.wj;
  double eta = etas[w1][w2];

  // common topic
  if (type == 's') {
    pz[0] = 0;
  } else {
    double deno = 2 * (bs.size() - nb) + W * beta;
    double pw1k = (nwz[0][w1] + beta) / deno;
    double pw2k = (nwz[0][w2] + beta) / (deno + 1);
    pz[0] = (1 - eta) * pw1k * pw2k;
  }

  // bursty topic
  for (int k = 1; k < K + 1; ++k) {
    double deno = 2 * nb_z[k] + W * beta;
    double pw1k = (nwz[k][w1] + beta) / deno;
    double pw2k = (nwz[k][w2] + beta) / (deno + 1);
    double pk_t = (nb_z[k] + alpha) / (nb + K * alpha);
    pz[k] = eta * pk_t * pw1k * pw2k;
  }

  return pz.normalize();
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
  string pt = dir + ".pz";
  cout << "write p(z): " << pt << endl;
  save_pz(pt);

  string pt2 = dir + ".pw_z";
  cout << "write p(w|z): " << pt2 << endl;
  save_pw_z(pt2);
}

// p(z) is determinated by the overall proportions
// of biterms in it
void BurstyBTM::save_pz(string pt) {
  Pvec<double> pz = nb_z.normalize(alpha);
  pz.write(pt);
}

// save the K bursty topics: a KxW matrix
void BurstyBTM::save_pw_z(string pt) {
  Pmat<double> pw_z = nwz.normr(beta);
  pw_z.write(pt);
  //nwz.write(pt);
}
