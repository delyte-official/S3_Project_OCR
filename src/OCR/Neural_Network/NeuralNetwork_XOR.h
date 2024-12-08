#ifndef Neural_Network_H
#define Neural_Network_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


double rand_doubles(double min, double max);
void fill_hyperparams_with_rand(void);
void L1_weight_updater(void);
void HIDDEN_LAYER_weight_updater(void);
void feed_forward(void);
double sigmoid(double x);
void backprop(void);
void test(void);
void Reset_Neuron_Value(void);
void test_for_NN();

#endif
