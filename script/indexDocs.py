#!/usr/bin/env python
#coding=utf-8
# translate word into id in documents
import sys, os

w2id = {}                       # {w:[id, freq], ...}

def indexDir(doc_dir, res_dir):
    for f in os.listdir(doc_dir):
        indexFile(doc_dir + f, res_dir + f)
    
def indexFile(pt, res_pt):
    print 'index file: ', pt
    wf = open(res_pt, 'w')
    for l in open(pt):
        ws = l.strip().split()
        for w in ws:
            if not w2id.has_key(w):
                w2id[w] = [len(w2id), 1]
            else:
                w2id[w][1] += 1
                
        wids = [w2id[w][0] for w in ws]        
        print >>wf, ' '.join(map(str, wids))
        
    print 'write file: ', res_pt


def write_w2id(res_pt):
    print 'write:', res_pt
    wf = open(res_pt, 'w')
    for w, id_f in sorted(w2id.items(), key=lambda d:d[1]):
        print >>wf, '%d\t%s\t%d' % (id_f[0], w, id_f[1])
        
        
if __name__ == '__main__':
    if len(sys.argv) < 4:
        print 'Usage: python %s <doc_dir> <dwid_dir> <voca_pt>' % sys.argv[0]
        print '\tdoc_dir   input doc dir to be indexed, each file records docs in a day, while each line in a file is a doc with the format "word word ..."'
        print '\tdwid_dir   output doc dir after indexing, each file records docs in a day, while each line is a doc with the format "wordId wordId ..."'
        print '\tvoca_pt   output vocabulary file, each line is a word with the format "wordId    word    freq"' 
        exit(1)
        
    doc_dir = sys.argv[1]
    dwid_dir = sys.argv[2]
    voca_pt = sys.argv[3]
    indexDir(doc_dir, dwid_dir)
    print 'n(w)=', len(w2id)
    write_w2id(voca_pt)
