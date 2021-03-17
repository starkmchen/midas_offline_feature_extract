import sys
from collections import defaultdict


def main(argv):
  feature_count = defaultdict(set)
  f = open(argv[0])
  fw = open(argv[1], 'w')
  for line in f:
    fea_k, fea_v, click, install, imp = line.strip().split('\t')
    feature_count[fea_k].add(fea_v)
  for fea_k, v in feature_count.iteritems():
    if len(v) == 1:
      continue
    if fea_k == "ctx_ip":
      fea_cnt = 100000
    elif "user_ad_uac" in fea_k or "ad_ac" in fea_k or "up_uc" in fea_k:
      fea_cnt = 10000
    elif fea_k == "ad_ai_cid":
      fea_cnt = 10000
    else:
      fea_cnt = len(v) * 10
    output = [fea_k, 0, fea_cnt, 'int', 0, 0, 0]
    fw.write('\t'.join(map(str, output)) + '\n')



if __name__ == '__main__':
  main(sys.argv[1:])
