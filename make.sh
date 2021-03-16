#!/bin/bash
set -x

cwd=$(pwd)

g++ -o libpyfeature_extract.so --shared -fPIC -I/usr/include/python2.7 -I${cwd}/src -I${cwd} -I${cwd}/proto -I/mnt/chenmin/lib/include src/ads_feature.cpp pybind.cpp proto/*.cc -L/mnt/chenmin/lib/lib/ -Wl,-rpath=lib -lprotobuf --std=c++11

