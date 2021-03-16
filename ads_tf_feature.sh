#!/bin/bash
set -x
day=$(date -d "1 day ago" +%Y%m%d)
aws=/usr/local/bin/aws

echo $day

function file_exist()
{
dname=$1
while [ 1 ];do
  f_succ=$($aws s3 ls $dname/_SUCCESS)
  if [ -n "$f_succ" ];then
    echo $dname, $f_succ
    break
  fi
  sleep 600
done
}

file_exist s3://sprs.push.us-east-1.prod/data/warehouse/model/train_data/$day

rm -rf /root/train_data/$day

$aws s3 sync s3://sprs.push.us-east-1.prod/data/warehouse/model/train_data/$day /root/train_data/$day

rm -rf /root/ads_train_data/$day
mkdir /root/ads_train_data/$day

ls /root/train_data/$day/p* | xargs python extract_feature.py

cd /root/ads_train_data/$day

mkdir feature_index
mkdir pre_data
mkdir train_data
mv part-0019* pre_data
mv part* train_data
cp /root/chenmin/feature_index feature_index
touch _SUCCESS
