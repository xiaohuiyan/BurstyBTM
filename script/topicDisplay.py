#!/usr/bin/env python
#coding=utf-8
# Function: translate the results from BTM
# Input:
#    mat/pw_z.k20

import sys

# return:    {wid:w, ...}
def read_voca(pt):
    voca = {}
    for l in open(pt):
        wid, w = l.strip().split('\t')[:2]
        voca[int(wid)] = w
    return voca

def read_pz(pt):
    print "read:", pt
    return [float(p) for p in open(pt).readline().strip().split()]
    
# voca = {wid:w,...}
def dispTopics(pt, voca, pz):
    k = 0
    topics = []
    print "read:", pt
    for l in open(pt):
        vs = [float(v) for v in l.split()]
        wvs = zip(range(len(vs)), vs)
        wvs = sorted(wvs, key=lambda d:d[1], reverse=True)
        tmps = ' '.join(['%s' % voca[w] for w, v in wvs[:20]])
        # tmps = ' '.join(['%s:%f' % (voca[w],v) for w,v in wvs[:10]])
        topics.append((pz[k], tmps))
        k += 1
        
    print 'p(z)\t\tTop words'
    for pz, s in sorted(topics, reverse=True):
        print '%f\t%s' % (pz, s)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print 'Usage: python %s <model_dir> <voca_pt>' % sys.argv[0]
        print '\tmodel_dir    the output dir of BTM'
        print '\tK    the number of topics'
        print '\tvoca_pt    the vocabulary file'
        exit(1)
        
    voca_pt = sys.argv[1]
    model_dir = sys.argv[2]
    voca = read_voca(voca_pt)    
    print 'n(word)=', len(voca)
    print 'Notice: The first topic is background topic, while others are bursty ones.'
    pz_pt = model_dir + '.pz'
    pz = read_pz(pz_pt)
        
    zw_pt = model_dir + '.pw_z'
    dispTopics(zw_pt, voca, pz)
