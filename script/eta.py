#!/usr/bin/env python
# coding=utf-8
# compute eta(the bursty probability) for each biterm in each day
import sys

# read the day frequency for each biterm    
# Input format: w1 w2    freq
# compute and output the etas in a day
def proc_day(bdf_pt, day, res_pt):    
    etas = {}
    for l in open(bdf_pt):
        b, df = l.strip().split('\t')
        df = [s.split(':') for s in df.split()]
        df = dict([(int(d), int(f)) for d, f in df])    
        if not df.has_key(day): 
            continue
        etas[b] = computeEta(df, day)
        
    write_etas(etas, res_pt)

# Given the day frequencies of a biterm, compute its eta in `day`
# df  {day:freq, ...}
# return eta
def computeEta(df, day):    
    eps = 0.01  # epsilon in the paper
    # avgf = sum(df.values()) / float(len(df))
    avgf = sum([f for d, f in df.items() if d >= day - 10 and d < day]) / float(min(day, 10))
    if avgf < 5:
        return eps
    else:
        return max(df[day] - avgf - 5, eps) / df[day]
                
    
# write the frequency of biterms
#  format: wi wj    freq
def write_etas(etas, res_pt):
    print 'write:', res_pt
    wf = open(res_pt, 'w')
    for b, eta in sorted(etas.items(), key=lambda d:d[1], reverse=True):
        print >> wf, '%s\t%f' % (b, eta)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print 'Compute the bursty probability (eta) for each biterm in each day'
        print 'Usage: python %s <n_day> <bf_dir> <res_dir>' % sys.argv[0]
        print '\tn_day   number of days to be processed (count from 0)'
        print '\tbf_pt   input docs, each line is a biterm with its daily frequency. Line format: "w1 w2    day:freq day:freq ..."'
        print '\tres_dir   output the eta of the biterms. Line format: "w1 w2    eta"'
        exit(1)
        
    n_day = int(sys.argv[1])    
    bdf_pt = sys.argv[2]        
    eta_dir = sys.argv[3]
    for day in range(1, n_day):
        eta_pt = eta_dir + '%d.txt' % day
        proc_day(bdf_pt, day, eta_pt)
