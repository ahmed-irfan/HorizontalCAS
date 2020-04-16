/*
 ------------------------------------------------------------------
 ** Top contributors:
 **   Shiqi Wang and Suman Jana
 ** This file is part of the ReluVal project.
 ** Copyright (c) 2018-2019 by the authors listed in the file LICENSE
 ** and their institutional affiliations.
 ** All rights reserved.
 -----------------------------------------------------------------
 *
 * This is the main file of ReluVal, here is the usage:
 * ./network_test [property] [network] [target] 
 *      [need to print=0] [test for one run=0] [check mode=0]
 *
 * [property]: the saftety property want to verify
 *
 * [network]: the network want to test with
 *
 * [target]: Wanted label of the property
 *
 * [need to print]: whether need to print the detailed info of each split.
 * 0 is not and 1 is yes. Default value is 0.
 *
 * [test for one run]: whether need to estimate the output range without
 * split. 0 is no, 1 is yes. Default value is 0.
 *
 * [check mode]: normal split mode is 0. Check adv mode is 1.
 * Check adv mode will prevent further splits as long as the depth goes
 * upper than 20 so as to locate the concrete adversarial examples faster.
 * Default value is 0.
 * 
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "split.h"
#include <math.h>

//extern int thread_tot_cnt;

/* print the progress if getting SIGQUIT */
void sig_handler(int signo)
{

    if (signo == SIGQUIT) {
        printf("progress: %d/1024\n", progress);
    }

}



int main( int argc, char *argv[])
{
    char *FULL_NET_PATH;

    PROPERTY=2;
    float scaleForObsError = 0.0;

    if (argc > 9 || argc < 2) {
        printf("please specify a network\n");
        printf("./network_test [network] [scaleForObsError]"
            "[print] "
            "[test for one run] [check mode]\n");
        exit(1);
    }

    for (int i=1;i<argc;++i) {

        if (i == 1) {
            FULL_NET_PATH = argv[i];
        }
        
        if (i==2) {
            // Added input parameter for HCAS application
            // The input wides the input region checked by ReluVal,
            // which represents possible errors in the observed input
            scaleForObsError = atof(argv[i]);
        }

        if (i == 3) {
            NEED_PRINT = atoi(argv[i]);
            if(NEED_PRINT != 0 && NEED_PRINT!=1){
                printf("Wrong print");
                exit(1);
            }
        }

        if (i == 4) {
            NEED_FOR_ONE_RUN = atoi(argv[i]);

            if (NEED_FOR_ONE_RUN != 0 && NEED_FOR_ONE_RUN != 1) {
                printf("Wrong test for one run");
                exit(1);
            }

        }

        if (i == 5) {

            if (atoi(argv[i]) == 0) {
                CHECK_ADV_MODE = 0;
                PARTIAL_MODE = 0;
            }

            if (atoi(argv[i]) == 1) {
                CHECK_ADV_MODE = 1;
                PARTIAL_MODE = 0;
            }

            if (atoi(argv[i]) == 2) {
                CHECK_ADV_MODE = 0;
                PARTIAL_MODE = 1;
            }

        }

    }

    openblas_set_num_threads(1);

    
    
    //clock_t start, end;
    srand((unsigned)time(NULL));
    double time_spent;
    int i,j,layer;

    struct NNet* nnet = load_network(FULL_NET_PATH, 0);  
    
    int numLayers    = nnet->numLayers;
    int inputSize    = nnet->inputSize;
    int outputSize   = nnet->outputSize;

    float u[inputSize], l[inputSize];

    int n = 0;
    int feature_range_length = 0;
    int split_feature = -1;
    int depth = 0;
    
    // Define grid
    float XS[]    = {-5000.0, -4800.0, -4600.0, -4400.0, -4200.0, -4000.0, -3800.0, -3600.0, -3400.0, -3200.0, -3000.0, -2900.0, -2800.0, -2700.0, -2600.0, -2500.0, -2400.0, -2300.0, -2200.0, -2100.0, -2000.0, -1900.0, -1800.0, -1700.0, -1600.0, -1500.0, -1400.0, -1300.0, -1200.0, -1100.0, -1000.0, -950.0, -900.0, -850.0, -800.0, -750.0, -700.0, -650.0, -600.0, -550.0, -500.0, -450.0, -400.0, -350.0, -300.0, -250.0, -200.0, -150.0, -100.0, -50.0, 0.0, 50.0, 100.0, 150.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0, 850.0, 900.0, 950.0, 1000.0, 1100.0, 1200.0, 1300.0, 1400.0, 1500.0, 1600.0, 1700.0, 1800.0, 1900.0, 2000.0, 2100.0, 2200.0, 2300.0, 2400.0, 2500.0, 2600.0, 2700.0, 2800.0, 2900.0, 3000.0, 3200.0, 3400.0, 3600.0, 3800.0, 4000.0, 4200.0, 4400.0, 4600.0, 4800.0, 5000.0, 5250.0, 5500.0, 5750.0, 6000.0, 6250.0, 6500.0, 6750.0, 7000.0, 7250.0, 7500.0, 7750.0, 8000.0, 8250.0, 8500.0, 8750.0, 9000.0, 9250.0, 9500.0, 9750.0, 10000.0, 10250.0, 10500.0, 10750.0, 11000.0, 11250.0, 11500.0, 11750.0, 12000.0, 12250.0, 12500.0, 12750.0, 13000.0, 13500.0, 14000.0, 14500.0, 15000.0};

    float YS[] = {-10000.0, -9750.0, -9500.0, -9250.0, -9000.0, -8750.0, -8500.0, -8250.0, -8000.0, -7750.0, -7500.0, -7250.0, -7000.0, -6750.0, -6500.0, -6250.0, -6000.0, -5750.0, -5500.0, -5250.0, -5000.0, -4800.0, -4600.0, -4400.0, -4200.0, -4000.0, -3800.0, -3600.0, -3400.0, -3200.0, -3000.0, -2900.0, -2800.0, -2700.0, -2600.0, -2500.0, -2400.0, -2300.0, -2200.0, -2100.0, -2000.0, -1900.0, -1800.0, -1700.0, -1600.0, -1500.0, -1400.0, -1300.0, -1200.0, -1100.0, -1000.0, -950.0, -900.0, -850.0, -800.0, -750.0, -700.0, -650.0, -600.0, -550.0, -500.0, -450.0, -400.0, -350.0, -300.0, -250.0, -200.0, -150.0, -100.0, -50.0, 0.0, 50.0, 100.0, 150.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0, 850.0, 900.0, 950.0, 1000.0, 1100.0, 1200.0, 1300.0, 1400.0, 1500.0, 1600.0, 1700.0, 1800.0, 1900.0, 2000.0, 2100.0, 2200.0, 2300.0, 2400.0, 2500.0, 2600.0, 2700.0, 2800.0, 2900.0, 3000.0, 3200.0, 3400.0, 3600.0, 3800.0, 4000.0, 4200.0, 4400.0, 4600.0, 4800.0, 5000.0, 5250.0, 5500.0, 5750.0, 6000.0, 6250.0, 6500.0, 6750.0, 7000.0, 7250.0, 7500.0, 7750.0, 8000.0, 8250.0, 8500.0, 8750.0, 9000.0, 9250.0, 9500.0, 9750.0, 10000.0};
    
    float PSIS[] = {-3.14159, -3.12414, -3.10669, -3.08923, -3.07178, -3.05433, -3.03687, -3.01942, -3.00197, -2.98451, -2.96706, -2.94961, -2.93215, -2.9147, -2.89725, -2.87979, -2.86234, -2.84489, -2.82743, -2.80998, -2.79253, -2.77507, -2.75762, -2.74017, -2.72271, -2.70526, -2.68781, -2.67035, -2.6529, -2.63545, -2.61799, -2.60054, -2.58309, -2.56563, -2.54818, -2.53073, -2.51327, -2.49582, -2.47837, -2.46091, -2.44346, -2.42601, -2.40855, -2.3911, -2.37365, -2.35619, -2.33874, -2.32129, -2.30383, -2.28638, -2.26893, -2.25147, -2.23402, -2.21657, -2.19911, -2.18166, -2.16421, -2.14675, -2.1293, -2.11185, -2.0944, -2.07694, -2.05949, -2.04204, -2.02458, -2.00713, -1.98968, -1.97222, -1.95477, -1.93732, -1.91986, -1.90241, -1.88496, -1.8675, -1.85005, -1.8326, -1.81514, -1.79769, -1.78024, -1.76278, -1.74533, -1.72788, -1.71042, -1.69297, -1.67552, -1.65806, -1.64061, -1.62316, -1.6057, -1.58825, -1.5708, -1.55334, -1.53589, -1.51844, -1.50098, -1.48353, -1.46608, -1.44862, -1.43117, -1.41372, -1.39626, -1.37881, -1.36136, -1.3439, -1.32645, -1.309, -1.29154, -1.27409, -1.25664, -1.23918, -1.22173, -1.20428, -1.18682, -1.16937, -1.15192, -1.13446, -1.11701, -1.09956, -1.0821, -1.06465, -1.0472, -1.02974, -1.01229, -0.994838, -0.977384, -0.959931, -0.942478, -0.925025, -0.907571, -0.890118, -0.872665, -0.855211, -0.837758, -0.820305, -0.802851, -0.785398, -0.767945, -0.750492, -0.733038, -0.715585, -0.698132, -0.680678, -0.663225, -0.645772, -0.628319, -0.610865, -0.593412, -0.575959, -0.558505, -0.541052, -0.523599, -0.506145, -0.488692, -0.471239, -0.453786, -0.436332, -0.418879, -0.401426, -0.383972, -0.366519, -0.349066, -0.331613, -0.314159, -0.296706, -0.279253, -0.261799, -0.244346, -0.226893, -0.20944, -0.191986, -0.174533, -0.15708, -0.139626, -0.122173, -0.10472, -0.0872665, -0.0698132, -0.0523599, -0.0349066, -0.0174533, 0.0, 0.0174533, 0.0349066, 0.0523599, 0.0698132, 0.0872665, 0.10472, 0.122173, 0.139626, 0.15708, 0.174533, 0.191986, 0.20944, 0.226893, 0.244346, 0.261799, 0.279253, 0.296706, 0.314159, 0.331613, 0.349066, 0.366519, 0.383972, 0.401426, 0.418879, 0.436332, 0.453786, 0.471239, 0.488692, 0.506145, 0.523599, 0.541052, 0.558505, 0.575959, 0.593412, 0.610865, 0.628319, 0.645772, 0.663225, 0.680678, 0.698132, 0.715585, 0.733038, 0.750492, 0.767945, 0.785398, 0.802851, 0.820305, 0.837758, 0.855211, 0.872665, 0.890118, 0.907571, 0.925025, 0.942478, 0.959931, 0.977384, 0.994838, 1.01229, 1.02974, 1.0472, 1.06465, 1.0821, 1.09956, 1.11701, 1.13446, 1.15192, 1.16937, 1.18682, 1.20428, 1.22173, 1.23918, 1.25664, 1.27409, 1.29154, 1.309, 1.32645, 1.3439, 1.36136, 1.37881, 1.39626, 1.41372, 1.43117, 1.44862, 1.46608, 1.48353, 1.50098, 1.51844, 1.53589, 1.55334, 1.5708, 1.58825, 1.6057, 1.62316, 1.64061, 1.65806, 1.67552, 1.69297, 1.71042, 1.72788, 1.74533, 1.76278, 1.78024, 1.79769, 1.81514, 1.8326, 1.85005, 1.8675, 1.88496, 1.90241, 1.91986, 1.93732, 1.95477, 1.97222, 1.98968, 2.00713, 2.02458, 2.04204, 2.05949, 2.07694, 2.0944, 2.11185, 2.1293, 2.14675, 2.16421, 2.18166, 2.19911, 2.21657, 2.23402, 2.25147, 2.26893, 2.28638, 2.30383, 2.32129, 2.33874, 2.35619, 2.37365, 2.3911, 2.40855, 2.42601, 2.44346, 2.46091, 2.47837, 2.49582, 2.51327, 2.53073, 2.54818, 2.56563, 2.58309, 2.60054, 2.61799, 2.63545, 2.6529, 2.67035, 2.68781, 2.70526, 2.72271, 2.74017, 2.75762, 2.77507, 2.79253, 2.80998, 2.82743, 2.84489, 2.86234, 2.87979, 2.89725, 2.9147, 2.93215, 2.94961, 2.96706, 2.98451, 3.00197, 3.01942, 3.03687, 3.05433, 3.07178, 3.08923, 3.10669, 3.12414, 3.14159};
    
    int NUMX = 136;
    int NUMY = 140;
    int NUMP = 360;
    int NUMREGIONS = NUMX*NUMY*NUMP;

    // Loop through each RA to compute which regions could give the RA
    for (int target=0; target<5; ++target){
        struct NNet* nnet = load_network(FULL_NET_PATH, target);
        
        printf("RA: %d\n",target);
        fflush(stdout); 

        gettimeofday(&start, NULL);
        
        // Query each region
        for (int reg=0; reg<NUMREGIONS; ++reg){

            reset_variables();
            n = 0;
            feature_range_length = 0;
            split_feature = -1;
            depth = 0;


            load_inputs(reg, inputSize, u, l,XS,YS,PSIS,NUMX,NUMY,NUMP);
            
            // SCALE FOR OBS ERROR
            // Nominally scaleForObsError is zero and this will have no effect
            float err = sqrt(u[0]*u[0]+u[1]*u[1])*scaleForObsError;
            u[0]+=err; u[1]+=err; l[0]-=err; l[1]-= err;

            
            // Set up query
            struct Matrix input_upper = {u,1,nnet->inputSize};
            struct Matrix input_lower = {l,1,nnet->inputSize};

            struct Interval input_interval = {input_lower, input_upper};

            float grad_upper[inputSize], grad_lower[inputSize];
            struct Interval grad_interval = {
                        (struct Matrix){grad_upper, 1, inputSize},
                        (struct Matrix){grad_lower, 1, inputSize}
                    };

            normalize_input_interval(nnet, &input_interval);

            float o[nnet->outputSize];
            struct Matrix output = {o, outputSize, 1};

            float o_upper[nnet->outputSize], o_lower[nnet->outputSize];
            struct Interval output_interval = {
                        (struct Matrix){o_lower, outputSize, 1},
                        (struct Matrix){o_upper, outputSize, 1}
                    };


            for (int i=0;i<inputSize;++i) {

                if (input_interval.upper_matrix.data[i] <\
                        input_interval.lower_matrix.data[i]) {
                    printf("wrong input!\n");
                    printf("Index: %d, Upper: %.5e, Lower: %.5e\n",i,input_interval.upper_matrix.data[i],input_interval.lower_matrix.data[i]);
                    exit(0);
                }

                if(input_interval.upper_matrix.data[i] !=\
                        input_interval.lower_matrix.data[i]){
                    ++n;
                }

            }



            feature_range_length = n;
            int *feature_range = (int*)malloc(n*sizeof(int));

            for (int i=0, n=0;i<nnet->inputSize;++i) {
                if(input_interval.upper_matrix.data[i] !=\
                        input_interval.lower_matrix.data[i]){
                    feature_range[n] = i;
                    ++n;
                }
            }

            // Reset variables
            int isOverlap = 0;
            float avg[100] = {0};
            adv_found = 0;
            depth_exceeded=0;
            check_adv(nnet, &input_interval);
            
            // Run ReluVal
            if (adv_found==0) {
                //forward_prop_interval_equation(nnet,\
                        &input_interval, &output_interval,\
                        &grad_interval);
                isOverlap = direct_run_check(nnet,\
                        &input_interval, &output_interval,\
                        &grad_interval, depth, feature_range,\
                        feature_range_length, split_feature);
            }

            // Check ReluVal results and print accordingly
            // Print the region index if SAT (advisory is given somewhere within the cell.
            // If ReluVal searches very deeply (which can sometimes cause a seg fault) count
            // the query as SAT too (over-approximation)
            if (isOverlap == 0 && adv_found == 0 && depth_exceeded==0) {
            } else if (adv_found!=0) {
                printf("%d,\n",reg);
            } else if (depth_exceeded==1){
                printf("%d,\n",reg);
            }
            free(feature_range);
        }

        gettimeofday(&finish, NULL);
        time_spent = ((float)(finish.tv_sec - start.tv_sec) *\
                    1000000 + (float)(finish.tv_usec - start.tv_usec)) /\
                    1000000;

        printf("time: %f \n\n\n", time_spent);
    }

    destroy_network(nnet);

}
