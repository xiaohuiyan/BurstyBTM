#!/bin/bash
# run an toy example for BTM
set -e

type='n'  #'n' is normal, 's' is simplified BurstyBTM
K=20   # number of topics

alpha=`echo "scale=3;50/$K"|bc`
beta=0.01
n_day=3
n_iter=100
save_step=20
fix_b='n'	# 'n' is not fix, 'y' is fix

input_dir=../sample-data/
output_dir=../output/
dwid_dir=${output_dir}doc_wids/
bdf_pt=${output_dir}biterm_dayfreq.txt
eta_dir=${output_dir}eta/
model_dir=${output_dir}model/

voca_pt=${output_dir}voca.txt

echo "================== Index Docs =================="
python indexDocs.py $input_dir $dwid_dir $voca_pt

echo "======= Statistic Biterm Daily Frequency ======="
python bitermDayFreq.py $dwid_dir $bdf_pt

echo "======= Compute eta for Biterms == ======="
python eta.py $n_day $bdf_pt $eta_dir

## learning parameters p(z) and p(z|w)
echo "=============== Topic Learning ============="
W=`wc -l < $voca_pt`
make -C ../src
for day in `seq 1 $[$n_day-1]`; do
	echo "---------- day $day --------------"
	dwid_pt=$dwid_dir$day.txt
	eta_pt=$eta_dir$day.txt
	res_dir=${model_dir}k$K.day$day.type-$type.
	../src/bbtm $type $K $W $alpha $beta $n_iter $save_step $eta_pt $dwid_pt $res_dir $fix_b
done

# ## output top words of each topic
echo "================ Topic Display ============="
for day in `seq 1 $[$n_day-1]`; do
	echo "---------- day $day --------------"
	res_dir=${model_dir}k$K.day$day.type-$type.iter$n_iter.
	python topicDisplay.py $voca_pt $res_dir 
done
