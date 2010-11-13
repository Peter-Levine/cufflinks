#ifndef BIASCORRECTION_H
#define BIASCORRECTION_H

/*
 *  biascorrection.h
 *  cufflinks
 *
 *  Created by Adam Roberts on 5/20/10.
 *  Copyright 2010 Adam Roberts. All rights reserved.
 *
 */


#include "bundles.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <list>
#include <string>
#include <boost/tr1/unordered_map.hpp>

namespace ublas = boost::numeric::ublas;
using namespace std;

void get_compatibility_list(const vector<Scaffold>& transcripts,
							const vector<MateHit>& alignments,
							vector<list<int> >& compatibilities);

class BiasLearner{
	static const int pow4[];
	static const int paramTypes[];
	static const int MAX_SLICE;
	static const int CENTER;
	static const int _M;
	static const int _N;
	
	static const int lengthBins[];
	static const double positionBins[];
	
	shared_ptr<EmpDist const> _frag_len_dist;
	ublas::matrix<long double> _startSeqParams;
	ublas::matrix<long double> _startSeqExp;
	ublas::matrix<long double> _endSeqParams;
	ublas::matrix<long double> _endSeqExp;
	ublas::matrix<long double> _startPosParams;
	ublas::matrix<long double> _startPosExp;
	ublas::matrix<long double> _endPosParams;
	ublas::matrix<long double> _endPosExp;

	int seqToInt(const char* seqSlice, int n) const;
	void getSlice(const char* seq, char* slice, int start, int end) const;
	void genNList(const char* seqSlice, int start, int n, list<int>& nList) const;
	
public:
	
	BiasLearner(shared_ptr<EmpDist const> frag_len_dist);
	void processTranscript(const vector<double>& startHist, const vector<double>& endHist, const Scaffold& transcript);
	void normalizeParameters();
	void output();
	
	void getBias(const Scaffold& transcript, vector<double>& startBiases, vector<double>& endBiases) const;

};

void learn_bias(BundleFactory& bundle_factory, BiasLearner& bl);
void process_bundle(HitBundle& bundle, BiasLearner& bl);

// Helps with the complexities of bias correction with replicates in cond_probs and eff_lens
class BiasCorrectionHelper{
	
	shared_ptr<Scaffold> _transcript;
    boost::unordered_map<shared_ptr<ReadGroupProperties const>, int> _rg_index;
	int _size;
	bool _mapped;
	
	vector<vector<double> > _start_biases;
	vector<vector<double> > _end_biases;
	vector<vector<double> > _pos_biases;
	vector<vector<double> > _tot_biases_for_len;
	vector<vector<double> > _start_biases_for_len;
	vector<vector<double> > _end_biases_for_len;
	
	vector<double> _eff_lens;
	vector<double> _rg_masses;
	
	int add_read_group(shared_ptr<ReadGroupProperties const> rgp);	
	int get_index(shared_ptr<ReadGroupProperties const> rgp);
	
public:
	
	BiasCorrectionHelper(shared_ptr<Scaffold> transcript) 
	{ 
		_transcript = transcript;
		_mapped = false;
		_size = 0; 
	}
	

	double get_cond_prob(const MateHit& hit);
	
	double get_effective_length();
	bool is_mapped() { return _mapped; }

};

#endif
