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

    for (int i=1;i<argc;i++) {

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
    float XS[]    = {-7000.0, -6000.0, -5000.0, -4000.0, -3750.0, -3500.0, -3250.0, -3000.0, -2950.0, -2900.0, -2850.0, -2800.0, -2750.0, -2700.0, -2650.0, -2600.0, -2550.0, -2500.0, -2450.0, -2400.0, -2350.0, -2300.0, -2250.0, -2200.0, -2150.0, -2100.0, -2050.0, -2000.0, -1950.0, -1900.0, -1850.0, -1800.0, -1750.0, -1700.0, -1650.0, -1600.0, -1550.0, -1500.0, -1450.0, -1400.0, -1350.0, -1300.0, -1250.0, -1200.0, -1150.0, -1100.0, -1050.0, -1000.0, -980.0, -960.0, -940.0, -920.0, -900.0, -880.0, -860.0, -840.0, -820.0, -800.0, -780.0, -760.0, -740.0, -720.0, -700.0, -680.0, -660.0, -640.0, -620.0, -600.0, -580.0, -560.0, -540.0, -520.0, -500.0, -480.0, -460.0, -440.0, -420.0, -400.0, -380.0, -360.0, -340.0, -320.0, -300.0, -280.0, -260.0, -240.0, -220.0, -200.0, -180.0, -160.0, -140.0, -120.0, -100.0, -80.0, -60.0, -40.0, -20.0, 0.0, 20.0, 40.0, 60.0, 80.0, 100.0, 120.0, 140.0, 160.0, 180.0, 200.0, 220.0, 240.0, 260.0, 280.0, 300.0, 320.0, 340.0, 360.0, 380.0, 400.0, 420.0, 440.0, 460.0, 480.0, 500.0, 520.0, 540.0, 560.0, 580.0, 600.0, 620.0, 640.0, 660.0, 680.0, 700.0, 720.0, 740.0, 760.0, 780.0, 800.0, 820.0, 840.0, 860.0, 880.0, 900.0, 920.0, 940.0, 960.0, 980.0, 1000.0, 1040.0, 1080.0, 1120.0, 1160.0, 1200.0, 1240.0, 1280.0, 1320.0, 1360.0, 1400.0, 1440.0, 1480.0, 1520.0, 1560.0, 1600.0, 1640.0, 1680.0, 1720.0, 1760.0, 1800.0, 1840.0, 1880.0, 1920.0, 1960.0, 2000.0, 2040.0, 2080.0, 2120.0, 2160.0, 2200.0, 2240.0, 2280.0, 2320.0, 2360.0, 2400.0, 2440.0, 2480.0, 2520.0, 2560.0, 2600.0, 2640.0, 2680.0, 2720.0, 2760.0, 2800.0, 2840.0, 2880.0, 2920.0, 2960.0, 3000.0, 3040.0, 3080.0, 3120.0, 3160.0, 3200.0, 3240.0, 3280.0, 3320.0, 3360.0, 3400.0, 3440.0, 3480.0, 3520.0, 3560.0, 3600.0, 3640.0, 3680.0, 3720.0, 3760.0, 3800.0, 3840.0, 3880.0, 3920.0, 3960.0, 4000.0, 4040.0, 4080.0, 4120.0, 4160.0, 4200.0, 4240.0, 4280.0, 4320.0, 4360.0, 4400.0, 4440.0, 4480.0, 4520.0, 4560.0, 4600.0, 4640.0, 4680.0, 4720.0, 4760.0, 4800.0, 4840.0, 4880.0, 4920.0, 4960.0, 5000.0, 5040.0, 5080.0, 5120.0, 5160.0, 5200.0, 5240.0, 5280.0, 5320.0, 5360.0, 5400.0, 5440.0, 5480.0, 5520.0, 5560.0, 5600.0, 5640.0, 5680.0, 5720.0, 5760.0, 5800.0, 5840.0, 5880.0, 5920.0, 5960.0, 6000.0, 6050.0, 6100.0, 6150.0, 6200.0, 6250.0, 6300.0, 6350.0, 6400.0, 6450.0, 6500.0, 6550.0, 6600.0, 6650.0, 6700.0, 6750.0, 6800.0, 6850.0, 6900.0, 6950.0, 7000.0, 7050.0, 7100.0, 7150.0, 7200.0, 7250.0, 7300.0, 7350.0, 7400.0, 7450.0, 7500.0, 7550.0, 7600.0, 7650.0, 7700.0, 7750.0, 7800.0, 7850.0, 7900.0, 7950.0, 8000.0, 8050.0, 8100.0, 8150.0, 8200.0, 8250.0, 8300.0, 8350.0, 8400.0, 8450.0, 8500.0, 8550.0, 8600.0, 8650.0, 8700.0, 8750.0, 8800.0, 8850.0, 8900.0, 8950.0, 9000.0, 9050.0, 9100.0, 9150.0, 9200.0, 9250.0, 9300.0, 9350.0, 9400.0, 9450.0, 9500.0, 9550.0, 9600.0, 9650.0, 9700.0, 9750.0, 9800.0, 9850.0, 9900.0, 9950.0, 10000.0, 10100.0, 10200.0, 10300.0, 10400.0, 10500.0, 10600.0, 10700.0, 10800.0, 10900.0, 11000.0, 11100.0, 11200.0, 11300.0, 11400.0, 11500.0, 11600.0, 11700.0, 11800.0, 11900.0, 12000.0, 12100.0, 12200.0, 12300.0, 12400.0, 12500.0, 12600.0, 12700.0, 12800.0, 12900.0, 13000.0, 13100.0, 13200.0, 13300.0, 13400.0, 13500.0, 13600.0, 13700.0, 13800.0, 13900.0, 14000.0, 14100.0, 14200.0, 14300.0, 14400.0, 14500.0, 14600.0, 14700.0, 14800.0, 14900.0, 15000.0, 15200.0, 15400.0, 15600.0, 15800.0, 16000.0, 16200.0, 16400.0, 16600.0, 16800.0, 17000.0, 17200.0, 17400.0, 17600.0, 17800.0, 18000.0, 18200.0, 18400.0, 18600.0, 18800.0, 19000.0, 19200.0, 19400.0, 19600.0, 19800.0, 20000.0, 20200.0, 20400.0, 20600.0, 20800.0, 21000.0, 21200.0, 21400.0, 21600.0, 21800.0, 22000.0, 22200.0, 22400.0, 22600.0, 22800.0, 23000.0, 23200.0, 23400.0, 23600.0, 23800.0, 24000.0, 24200.0, 24400.0, 24600.0, 24800.0, 25000.0, 25200.0, 25400.0, 25600.0, 25800.0, 26000.0, 26200.0, 26400.0, 26600.0, 26800.0, 27000.0, 27200.0, 27400.0, 27600.0, 27800.0, 28000.0, 28200.0, 28400.0, 28600.0, 28800.0, 29000.0, 29200.0, 29400.0, 29600.0, 29800.0, 30000.0, 31000.0, 32000.0, 33000.0, 34000.0, 35000.0};

    float YS[] = {-20000.0, -19000.0, -18000.0, -17000.0, -16000.0, -15000.0, -14800.0, -14600.0, -14400.0, -14200.0, -14000.0, -13800.0, -13600.0, -13400.0, -13200.0, -13000.0, -12800.0, -12600.0, -12400.0, -12200.0, -12000.0, -11800.0, -11600.0, -11400.0, -11200.0, -11000.0, -10800.0, -10600.0, -10400.0, -10200.0, -10000.0, -9800.0, -9600.0, -9400.0, -9200.0, -9000.0, -8800.0, -8600.0, -8400.0, -8200.0, -8000.0, -7900.0, -7800.0, -7700.0, -7600.0, -7500.0, -7400.0, -7300.0, -7200.0, -7100.0, -7000.0, -6900.0, -6800.0, -6700.0, -6600.0, -6500.0, -6400.0, -6300.0, -6200.0, -6100.0, -6000.0, -5900.0, -5800.0, -5700.0, -5600.0, -5500.0, -5400.0, -5300.0, -5200.0, -5100.0, -5000.0, -4900.0, -4800.0, -4700.0, -4600.0, -4500.0, -4400.0, -4300.0, -4200.0, -4100.0, -4000.0, -3960.0, -3920.0, -3880.0, -3840.0, -3800.0, -3760.0, -3720.0, -3680.0, -3640.0, -3600.0, -3560.0, -3520.0, -3480.0, -3440.0, -3400.0, -3360.0, -3320.0, -3280.0, -3240.0, -3200.0, -3160.0, -3120.0, -3080.0, -3040.0, -3000.0, -2960.0, -2920.0, -2880.0, -2840.0, -2800.0, -2760.0, -2720.0, -2680.0, -2640.0, -2600.0, -2560.0, -2520.0, -2480.0, -2440.0, -2400.0, -2360.0, -2320.0, -2280.0, -2240.0, -2200.0, -2160.0, -2120.0, -2080.0, -2040.0, -2000.0, -1960.0, -1920.0, -1880.0, -1840.0, -1800.0, -1760.0, -1720.0, -1680.0, -1640.0, -1600.0, -1560.0, -1520.0, -1480.0, -1440.0, -1400.0, -1360.0, -1320.0, -1280.0, -1240.0, -1200.0, -1160.0, -1120.0, -1080.0, -1040.0, -1000.0, -980.0, -960.0, -940.0, -920.0, -900.0, -880.0, -860.0, -840.0, -820.0, -800.0, -780.0, -760.0, -740.0, -720.0, -700.0, -680.0, -660.0, -640.0, -620.0, -600.0, -580.0, -560.0, -540.0, -520.0, -500.0, -480.0, -460.0, -440.0, -420.0, -400.0, -380.0, -360.0, -340.0, -320.0, -300.0, -280.0, -260.0, -240.0, -220.0, -200.0, -180.0, -160.0, -140.0, -120.0, -100.0, -80.0, -60.0, -40.0, -20.0, 0.0, 20.0, 40.0, 60.0, 80.0, 100.0, 120.0, 140.0, 160.0, 180.0, 200.0, 220.0, 240.0, 260.0, 280.0, 300.0, 320.0, 340.0, 360.0, 380.0, 400.0, 420.0, 440.0, 460.0, 480.0, 500.0, 520.0, 540.0, 560.0, 580.0, 600.0, 620.0, 640.0, 660.0, 680.0, 700.0, 720.0, 740.0, 760.0, 780.0, 800.0, 820.0, 840.0, 860.0, 880.0, 900.0, 920.0, 940.0, 960.0, 980.0, 1000.0, 1040.0, 1080.0, 1120.0, 1160.0, 1200.0, 1240.0, 1280.0, 1320.0, 1360.0, 1400.0, 1440.0, 1480.0, 1520.0, 1560.0, 1600.0, 1640.0, 1680.0, 1720.0, 1760.0, 1800.0, 1840.0, 1880.0, 1920.0, 1960.0, 2000.0, 2040.0, 2080.0, 2120.0, 2160.0, 2200.0, 2240.0, 2280.0, 2320.0, 2360.0, 2400.0, 2440.0, 2480.0, 2520.0, 2560.0, 2600.0, 2640.0, 2680.0, 2720.0, 2760.0, 2800.0, 2840.0, 2880.0, 2920.0, 2960.0, 3000.0, 3040.0, 3080.0, 3120.0, 3160.0, 3200.0, 3240.0, 3280.0, 3320.0, 3360.0, 3400.0, 3440.0, 3480.0, 3520.0, 3560.0, 3600.0, 3640.0, 3680.0, 3720.0, 3760.0, 3800.0, 3840.0, 3880.0, 3920.0, 3960.0, 4000.0, 4100.0, 4200.0, 4300.0, 4400.0, 4500.0, 4600.0, 4700.0, 4800.0, 4900.0, 5000.0, 5100.0, 5200.0, 5300.0, 5400.0, 5500.0, 5600.0, 5700.0, 5800.0, 5900.0, 6000.0, 6100.0, 6200.0, 6300.0, 6400.0, 6500.0, 6600.0, 6700.0, 6800.0, 6900.0, 7000.0, 7100.0, 7200.0, 7300.0, 7400.0, 7500.0, 7600.0, 7700.0, 7800.0, 7900.0, 8000.0, 8200.0, 8400.0, 8600.0, 8800.0, 9000.0, 9200.0, 9400.0, 9600.0, 9800.0, 10000.0, 10200.0, 10400.0, 10600.0, 10800.0, 11000.0, 11200.0, 11400.0, 11600.0, 11800.0, 12000.0, 12200.0, 12400.0, 12600.0, 12800.0, 13000.0, 13200.0, 13400.0, 13600.0, 13800.0, 14000.0, 14200.0, 14400.0, 14600.0, 14800.0, 15000.0, 16000.0, 17000.0, 18000.0, 19000.0, 20000.0};
    
    float PSIS[] = {-3.14159, -3.12414, -3.10669, -3.08923, -3.07178, -3.05433, -3.03687, -3.01942, -3.00197, -2.98451, -2.96706, -2.94961, -2.93215, -2.9147, -2.89725, -2.87979, -2.86234, -2.84489, -2.82743, -2.80998, -2.79253, -2.77507, -2.75762, -2.74017, -2.72271, -2.70526, -2.68781, -2.67035, -2.6529, -2.63545, -2.61799, -2.60054, -2.58309, -2.56563, -2.54818, -2.53073, -2.51327, -2.49582, -2.47837, -2.46091, -2.44346, -2.42601, -2.40855, -2.3911, -2.37365, -2.35619, -2.33874, -2.32129, -2.30383, -2.28638, -2.26893, -2.25147, -2.23402, -2.21657, -2.19911, -2.18166, -2.16421, -2.14675, -2.1293, -2.11185, -2.0944, -2.07694, -2.05949, -2.04204, -2.02458, -2.00713, -1.98968, -1.97222, -1.95477, -1.93732, -1.91986, -1.90241, -1.88496, -1.8675, -1.85005, -1.8326, -1.81514, -1.79769, -1.78024, -1.76278, -1.74533, -1.72788, -1.71042, -1.69297, -1.67552, -1.65806, -1.64061, -1.62316, -1.6057, -1.58825, -1.5708, -1.55334, -1.53589, -1.51844, -1.50098, -1.48353, -1.46608, -1.44862, -1.43117, -1.41372, -1.39626, -1.37881, -1.36136, -1.3439, -1.32645, -1.309, -1.29154, -1.27409, -1.25664, -1.23918, -1.22173, -1.20428, -1.18682, -1.16937, -1.15192, -1.13446, -1.11701, -1.09956, -1.0821, -1.06465, -1.0472, -1.02974, -1.01229, -0.994838, -0.977384, -0.959931, -0.942478, -0.925025, -0.907571, -0.890118, -0.872665, -0.855211, -0.837758, -0.820305, -0.802851, -0.785398, -0.767945, -0.750492, -0.733038, -0.715585, -0.698132, -0.680678, -0.663225, -0.645772, -0.628319, -0.610865, -0.593412, -0.575959, -0.558505, -0.541052, -0.523599, -0.506145, -0.488692, -0.471239, -0.453786, -0.436332, -0.418879, -0.401426, -0.383972, -0.366519, -0.349066, -0.331613, -0.314159, -0.296706, -0.279253, -0.261799, -0.244346, -0.226893, -0.20944, -0.191986, -0.174533, -0.15708, -0.139626, -0.122173, -0.10472, -0.0872665, -0.0698132, -0.0523599, -0.0349066, -0.0174533, 0.0, 0.0174533, 0.0349066, 0.0523599, 0.0698132, 0.0872665, 0.10472, 0.122173, 0.139626, 0.15708, 0.174533, 0.191986, 0.20944, 0.226893, 0.244346, 0.261799, 0.279253, 0.296706, 0.314159, 0.331613, 0.349066, 0.366519, 0.383972, 0.401426, 0.418879, 0.436332, 0.453786, 0.471239, 0.488692, 0.506145, 0.523599, 0.541052, 0.558505, 0.575959, 0.593412, 0.610865, 0.628319, 0.645772, 0.663225, 0.680678, 0.698132, 0.715585, 0.733038, 0.750492, 0.767945, 0.785398, 0.802851, 0.820305, 0.837758, 0.855211, 0.872665, 0.890118, 0.907571, 0.925025, 0.942478, 0.959931, 0.977384, 0.994838, 1.01229, 1.02974, 1.0472, 1.06465, 1.0821, 1.09956, 1.11701, 1.13446, 1.15192, 1.16937, 1.18682, 1.20428, 1.22173, 1.23918, 1.25664, 1.27409, 1.29154, 1.309, 1.32645, 1.3439, 1.36136, 1.37881, 1.39626, 1.41372, 1.43117, 1.44862, 1.46608, 1.48353, 1.50098, 1.51844, 1.53589, 1.55334, 1.5708, 1.58825, 1.6057, 1.62316, 1.64061, 1.65806, 1.67552, 1.69297, 1.71042, 1.72788, 1.74533, 1.76278, 1.78024, 1.79769, 1.81514, 1.8326, 1.85005, 1.8675, 1.88496, 1.90241, 1.91986, 1.93732, 1.95477, 1.97222, 1.98968, 2.00713, 2.02458, 2.04204, 2.05949, 2.07694, 2.0944, 2.11185, 2.1293, 2.14675, 2.16421, 2.18166, 2.19911, 2.21657, 2.23402, 2.25147, 2.26893, 2.28638, 2.30383, 2.32129, 2.33874, 2.35619, 2.37365, 2.3911, 2.40855, 2.42601, 2.44346, 2.46091, 2.47837, 2.49582, 2.51327, 2.53073, 2.54818, 2.56563, 2.58309, 2.60054, 2.61799, 2.63545, 2.6529, 2.67035, 2.68781, 2.70526, 2.72271, 2.74017, 2.75762, 2.77507, 2.79253, 2.80998, 2.82743, 2.84489, 2.86234, 2.87979, 2.89725, 2.9147, 2.93215, 2.94961, 2.96706, 2.98451, 3.00197, 3.01942, 3.03687, 3.05433, 3.07178, 3.08923, 3.10669, 3.12414, 3.14159};
    
    int NUMX = 482;
    int NUMY = 410;
    int NUMP = 360;
    int NUMREGIONS = NUMX*NUMY*NUMP;
    
    // Loop through each RA to compute which regions could give the RA
    for (int target=0; target<5; target++){
        struct NNet* nnet = load_network(FULL_NET_PATH, target);
        
        printf("RA: %d\n",target);
        fflush(stdout); 

        gettimeofday(&start, NULL);
        
        // Query each region
        for (int reg=0; reg<NUMREGIONS; reg++){

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


            for (int i=0;i<inputSize;i++) {

                if (input_interval.upper_matrix.data[i] <\
                        input_interval.lower_matrix.data[i]) {
                    printf("wrong input!\n");
                    printf("Index: %d, Upper: %.5e, Lower: %.5e\n",i,input_interval.upper_matrix.data[i],input_interval.lower_matrix.data[i]);
                    exit(0);
                }

                if(input_interval.upper_matrix.data[i] !=\
                        input_interval.lower_matrix.data[i]){
                    n++;
                }

            }



            feature_range_length = n;
            int *feature_range = (int*)malloc(n*sizeof(int));

            for (int i=0, n=0;i<nnet->inputSize;i++) {
                if(input_interval.upper_matrix.data[i] !=\
                        input_interval.lower_matrix.data[i]){
                    feature_range[n] = i;
                    n++;
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
