#!/bin/bash

#g++ -o ad_feature.so --shared -fPIC -I/mnt/chenmin/ad_feature -I/mnt/chenmin/lib/include ads_feature.cpp hash.cpp ../proto/*.cc -L/mnt/chenmin/lib/lib/ -Wl,-rpath=/mnt/chenmin/lib/lib -lprotobuf --std=c++11
#g++ -o main -fPIC -I/mnt/chenmin/ad_feature -I/mnt/chenmin/lib/include ads_feature.cpp hash.cpp ../proto/*.cc main.cpp /mnt/chenmin/lib/lib/libprotobuf.a -lpthread --std=c++11
#g++ -o libpyfeature_extract.so --shared -fPIC -I/usr/include/python2.7 -I/mnt/chenmin/ad_feature -I/mnt/chenmin/lib/include ads_feature.cpp hash.cpp pybind.cpp ../proto/*.cc -L/mnt/chenmin/lib/lib/ -Wl,-rpath=/mnt/chenmin/lib/lib -lprotobuf --std=c++11
#g++ -shared -fPIC -o libpyfeature_extract.so -I/usr/include/python2.7 -I/mnt/chenmin/ad_feature -I/mnt/chenmin/lib/include ads_feature.cpp hash.cpp pybind.cpp ../proto/*.cc -Wl,-whole-archive /mnt/chenmin/lib/lib/libprotobuf.a -Wl,-no-whole-archive --std=c++11

g++ -o libpyfeature_extract.so --shared -fPIC -I/usr/include/python2.7 -I/mnt/chenmin/model_feature_extract/src -I/mnt/chenmin/model_feature_extract -I/mnt/chenmin/model_feature_extract/proto -I/mnt/chenmin/lib/include src/ads_feature.cpp pybind.cpp proto/*.cc -L/mnt/chenmin/lib/lib/ -Wl,-rpath=lib -lprotobuf --std=c++11

