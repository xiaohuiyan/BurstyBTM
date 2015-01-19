#!/usr/bin/env python
#coding=utf-8
# translate word into id in documents
import sys, os
from collections import Counter, defaultdict

# processing files in a directory
# return {b:'d:f d:f ...', ...}
def proc_dir(dwid_dir):
    bf = defaultdict(str)
    for fname in sorted(os.listdir(dwid_dir), key=lambda d:int(d.split('.')[0])):
        day_bf = bitermFreq(dwid_dir + fname)
        for b, f in day_bf.items():
            bf[b] += '%s:%d ' % (fname.split('.')[0], f)
    
    return bf
    
# statisitc the biterm frequency in a file
# input format:wid wid ...
# return  bf   Counter({'w1 w2':freq, ...})
def bitermFreq(pt):
    bf = Counter()
    print 'read: ', pt
    for l in open(pt):
        ws = map(int, l.strip().split())
        bs = genBiterms(ws)
        bf.update(bs)
        
    return bf

# extract biterms from a sequence of words
# input:  ws   [w1, w2, ...]
# return:  bs  ['w1 w2', 'w1 w3', ...]
def genBiterms(ws):
    bs = []
    # Since somebody may run this code on normal texts, 
    # I add a window size.
    win = 15  
    for i in range(len(ws)-1):
        for j in range(i+1, min(i+win+1, len(ws))):
            wi = min(ws[i], ws[j])
            wj = max(ws[i], ws[j])
            #if wi == wj: continue
            b = '%d %d' % (wi, wj)
            bs.append(b)
    return bs

# write the frequency of biterms
#  format: wi wj    day:f day:f ...
def write_bf(bf, res_pt):
    print 'write:', res_pt
    wf = open(res_pt, 'w')
    for b, s in bf.items():
        print >>wf, '%s\t%s' % (b, s)
        
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print 'Statistic the frequency of each biterm'
        print 'Usage: python %s <dwid_dir> <res_dir>' % sys.argv[0]
        print '\tdwid_pt   input docs, each line is a doc with the format "word word ..."'
        print '\tres_pt   output the frequencies of the biterms in the format "w1 w2    freq"'
        exit(1)
        
    dwid_dir = sys.argv[1]
    bf_pt = sys.argv[2]
    bf = proc_dir(dwid_dir)
    write_bf(bf, bf_pt)
