#!/bin/bash

# There are 5 previous RA (pra) and 8 tau combinations for 40 networks total
# This script runs ReluVal to compute an over-approximation of batches of ten networks at a time
# The results are saved in a text file, which can be read by functions in the Reachability directory

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau0_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau0_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau1_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau1_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau2_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau2_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau3_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau3_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau4_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau4_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau5_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau5_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau6_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau6_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau7_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau7_pra${pra}.txt &
done
wait

for pra in `seq 0 4`;
do
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau8_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau8_pra${pra}.txt &
    ./network_test_HCAS ../networks_sxu/ACAS_sXu_v2_DNNs_wCoc/ACAS_sXu_v2_model5_vertical_tau9_pra${pra}_200Epochs.nnet > ./Results/ACAS_sXu_v2_tau9_pra${pra}.txt &
done
wait

