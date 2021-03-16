#!/usr/bin/env python
# coding=utf-8
from multiprocessing import Process
import sys
import datetime
import ujson as json
import time
import random
import libpyfeature_extract
import tensorflow as tf


fe = libpyfeature_extract.PyFeatureExtract('feature_index')

def process(in_path):
    out_path = in_path.replace('train_data', 'ads_train_data')
    print(in_path, out_path)
    filename = out_path
    options_zlib = tf.python_io.TFRecordOptions(tf.python_io.TFRecordCompressionType.GZIP)
    writer = tf.python_io.TFRecordWriter(filename, options=options_zlib)
    cnt = 0
    with open(in_path) as f:
        for line in f:
            cnt += 1
            s = fe.extract_tf_example(line)
            writer.write(s)
            if cnt % 1000 == 0:
                print('process %s' % cnt)
    writer.close()


def batch(paths):
    print(paths)
    x = []
    for in_path in paths:
        p = Process(target=process, args=(in_path,))
        p.start()
        x.append(p)

    for p in x:
        p.join()


if __name__ == '__main__':
    size = 200
    for x in range(1, len(sys.argv[1:])+size-1, size):
        batch(sys.argv[x:x+size])
