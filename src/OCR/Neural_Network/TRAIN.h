#ifndef TRAIN_H
#define TRAIN_H

#include <stdlib.h>
#include <time.h>
#include "NN.h"
#include "data.h"
double *matrixFromFile(char *filename);
double *goalArray(char letter);
double **goalMatrix();
double **lettersMatrix();

int RetrievePos(struct Neural_Network *net);
void SquaredError(struct Neural_Network *net);
int PosGoal(double *goal);
char RetrieveChar(int val);
int trainNN();

#endif
