#!/bin/bash

# There are 5 previous RA (pra) and 8 tau combinations for 40 networks total
# This script runs ReluVal to compute an over-approximation of batches of ten networks at a time
# The results are saved in a text file, which can be read by functions in the Reachability directory

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau0_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau0_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau1_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau1_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau5_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau5_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau10_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau10_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau20_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau20_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau40_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau40_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau60_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau60_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau80_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau80_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau100_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau100_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v3_DNNs_rect/ACAS_sXu_v3_coc_rect_model1_vertical_tau101_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v3_tau101_pra${pra}.txt &
done
wait

