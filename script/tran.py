
import sys

def read_voca(pt):
    return dict([l.strip().split('\t')[:2] for l in open(pt)])
    
def tran_eta(pt, voca):
    for l in open(pt):
        b, s = l.strip().split('\t')
        w1, w2 = b.split()
        print '%s %s\t%s' % (voca[w1], voca[w2], s)
        
if __name__ == '__main__':        
    voca_pt = sys.argv[1]
    eta_pt = sys.argv[2]
    voca = read_voca(voca_pt)
    tran_eta(eta_pt, voca)
