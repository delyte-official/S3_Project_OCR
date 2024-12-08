#ifndef NN_H
#define NN_H

#include <stdio.h>
#include <math.h>

struct Neural_Network
{
    int nbInput;
    int nbHidden;
    int nbOutput;

    //Arrays
    double OutputI[28*28];
    double Goal[52];
    //Weight Arrays
    double WeightIH[28*28][20];
    double WeightHO[20][52];
    //Bias Array
    double BiasH[20];
    //Bias output
    double BiasO[52];
    //Output Hidden
    double OutputH[20];
    //Output of output
    double OutputO[52];

    //delta weight
    double dWeightIH[28*28][20];
    double dWeightHO[20][52];
    //delta of output
    double dOutputO[52];
    //delta of hidden
    double dHidden[20];

    double MaxErrorRate;
    double ErrorRate;
    double eta;
    double alpha;

    char *str;
};

struct Neural_Network* InitializeNetwork();

double Random();
double Sigmoid(double x);
double der_sigmoid(double x);
void Feed_forward (struct Neural_Network *net);
void Backprop (struct Neural_Network *net);
void Weight_updater(struct Neural_Network *net);
void Biais_updater(struct Neural_Network *net);
void PrintState(struct Neural_Network *net);
void Neural_Network_OCR(struct Neural_Network *net, double *input, double *goal);
#endif
