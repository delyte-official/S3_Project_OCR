#include "NeuralNetwork_XOR.h"

#define SIZE_INPUT_LAYER 2
#define SIZE_HIDDEN_LAYER 2
#define SIZE_OF_TEST 4
#define SIZE_OF_TRAIN 4
#define INPUT_LAYER_WEIGHTS 4
#define HIDDEN_LAYER_WEIGHTS 2

double _inputs[8] = {0, 0, 0, 1, 1, 0, 1, 1}; //value of test
double _answer_key[4] = {0, 1, 1, 0};//attempt result for test

double HIDDEN_LAYER[2]; //space for second layer results
double OUTPUT_LAYER[1]; //space for third layer results

double sig_HIDDEN_LAYER[2]; //sigmoid applicate to HIDDEN_LAYER
double sig_OUTPUT_LAYER[1]; //same for OUTPUT_LAYER

double L1_to_HIDDEN_LAYER_WEIGHTS[4]; //input weights
double HIDDEN_LAYER_to_OUTPUT_LAYER_weights[2]; //weights from first hidden layer (second overall) to next hidden layer
double *input_ptr = NULL; //pointer to set in input or test array

double learning_rate = 0.8; //learning rate

//vars used temporarily during backpropagation
double Der_OUPTUT_LAYER_O[1]; //output value derivatives
double Der_OUPTUT_LAYER_I[1]; //derivatives of the input to the final layer
double Der_W_Hidden_to_Output[2]; //derivatives of the weights from HIDDEN_LAYER to OUTPUT_LAYER
double HIDDEN_LAYER_suggested_weight_changes[2];
double Der_HIDDEN_LAYER_O[2];
double Der_HIDDEN_LAYER_I[2];
double Der_W_L1_to_HIDDEN[4]; //derivatives of the weights from L1 to HIDDEN_LAYER
double L1_suggested_weight_changes[4];

void test(void)
{
    input_ptr = _inputs;
    for (int i = 0; i < SIZE_OF_TEST; i++)
    {
        feed_forward();
        printf("%lf %lf\n", sig_OUTPUT_LAYER[0], _answer_key[i]);
        Reset_Neuron_Value();
        input_ptr += 2;
    }
}

void feed_forward(void) //matrix multiplication
{
    int i = 0;
    int j = 0;

    for (i = 0; i < SIZE_INPUT_LAYER; i++)
    {
        for (j = 0; j < SIZE_HIDDEN_LAYER; j++)
            HIDDEN_LAYER[i] += L1_to_HIDDEN_LAYER_WEIGHTS[(j * 2) + i] * input_ptr[j];

        sig_HIDDEN_LAYER[i] = sigmoid(HIDDEN_LAYER[i]);

    }

    for (i = 0; i < SIZE_HIDDEN_LAYER; i++)
	{
        OUTPUT_LAYER[0] += HIDDEN_LAYER_to_OUTPUT_LAYER_weights[i] * sig_HIDDEN_LAYER[i];
	}
    sig_OUTPUT_LAYER[0] = sigmoid(OUTPUT_LAYER[0]);
}

void backprop(void) //aka chain rule time boiz
{
    int i = 0;
    int j = 0;

    for (int epochs = 0; epochs < 100000; epochs++)
    {
        input_ptr = _inputs;

        for (int train_number = 0; train_number < SIZE_OF_TRAIN; train_number++)
        {
            feed_forward(); //run through the network

            //find derivatives of final output, (output - expected = cost)
            Der_OUPTUT_LAYER_O[0] = sig_OUTPUT_LAYER[0] - _answer_key[train_number];

            //find derivatives of the input to the final layer
            Der_OUPTUT_LAYER_I[0] = (sig_OUTPUT_LAYER[0] * (1 - sig_OUTPUT_LAYER[0])) *
                                  Der_OUPTUT_LAYER_O[0];
                                  //sig_OUTPUT_LAYER[0]*(1-sig_OUTPUT_LAYER[0])) is derivative of the sigmoid

            //derivative of weights from HIDDEN_LAYER to OUTPUT_LAYER
            for (i = 0; i < SIZE_HIDDEN_LAYER; i++)
                Der_W_Hidden_to_Output[i] = sig_HIDDEN_LAYER[i] * Der_OUPTUT_LAYER_I[0];

            HIDDEN_LAYER_weight_updater();

            //derivative of the output of second layer
            for (i = 0; i < SIZE_HIDDEN_LAYER; i++)
                Der_HIDDEN_LAYER_O[i] = HIDDEN_LAYER_to_OUTPUT_LAYER_weights[i] * Der_OUPTUT_LAYER_I[0];

            //derivative of the input to the third layer
            for (i = 0; i < SIZE_HIDDEN_LAYER; i++)
                Der_HIDDEN_LAYER_I[i] = (sig_HIDDEN_LAYER[i] * (1 - sig_HIDDEN_LAYER[i])) * Der_HIDDEN_LAYER_O[i];

            //derivative of the weights from L1 to HIDDEN_LAYER
            for (i = 0; i < SIZE_INPUT_LAYER; i++)
                for (j = 0; j < SIZE_HIDDEN_LAYER; j++)
                    Der_W_L1_to_HIDDEN[(i * 2) + j] = input_ptr[i] * Der_HIDDEN_LAYER_I[j];

            L1_weight_updater();

            input_ptr += 2; //move the pointer to the next set

            for (i = 0; i < INPUT_LAYER_WEIGHTS; i++)
            {
                Der_W_L1_to_HIDDEN[i] = 0;

                if (i < HIDDEN_LAYER_WEIGHTS)
                {
                    Der_HIDDEN_LAYER_O[i] = 0;
                    Der_HIDDEN_LAYER_I[i] = 0;
                    Der_W_Hidden_to_Output[i] = 0;
                }
            }
            Der_OUPTUT_LAYER_O[0] = 0;
            Der_OUPTUT_LAYER_I[0] = 0;
            Reset_Neuron_Value();
        }

        for (i = 0; i < INPUT_LAYER_WEIGHTS; i++)
        {
            L1_to_HIDDEN_LAYER_WEIGHTS[i] += L1_suggested_weight_changes[i];
            L1_suggested_weight_changes[i] = 0;

            if (i < HIDDEN_LAYER_WEIGHTS)
            {
                HIDDEN_LAYER_to_OUTPUT_LAYER_weights[i] += HIDDEN_LAYER_suggested_weight_changes[i];
                HIDDEN_LAYER_suggested_weight_changes[i] = 0;
            }
        }
    }
}

void L1_weight_updater(void)
{
    for (int i = 0; i < INPUT_LAYER_WEIGHTS; i++)
        L1_suggested_weight_changes[i] += -1 * learning_rate * Der_W_L1_to_HIDDEN[i];
}

void HIDDEN_LAYER_weight_updater(void)
{
    for (int i = 0; i < HIDDEN_LAYER_WEIGHTS; i++)
        HIDDEN_LAYER_suggested_weight_changes[i] += -1 * learning_rate * Der_W_Hidden_to_Output[i];
}

double sigmoid(double x)
{
    return 1 / (1 + exp(-5 * x));
}

double rand_doubles(const double min, const double max)
{
    if (min < max)
        return (max - min) * ((double) rand() / RAND_MAX) + min;

    // return 0 if min > max or min = max, which shouldn't ever happen as we control the args
    return 0;
}

void fill_hyperparams_with_rand(void) //this works so DON'T TOUCH
{
    for (int i = 0; i < INPUT_LAYER_WEIGHTS; i++)
        L1_to_HIDDEN_LAYER_WEIGHTS[i] = rand_doubles(-0.5, 0.5);

    for (int i = 0; i < HIDDEN_LAYER_WEIGHTS; i++)
        HIDDEN_LAYER_to_OUTPUT_LAYER_weights[i] = rand_doubles(-0.5, 0.5);
}

void Reset_Neuron_Value(void) //Reset_Neuron_Values
{
    for(int i = 0; i < HIDDEN_LAYER_WEIGHTS; i++)
    {
      HIDDEN_LAYER[i] = 0;
    }
    OUTPUT_LAYER[0] = 0;
}

void test_for_NN()
{
    //randomizer
    srand((unsigned) 0); //seed with 0 for consistency
    fill_hyperparams_with_rand();
    backprop();
    test();

    putchar('\n');
    for(int i = 0; i < INPUT_LAYER_WEIGHTS; i++)
    {
      printf("L1_to_HIDDEN_LAYER_WEIGHTS[%i] = %lf\n",i, L1_to_HIDDEN_LAYER_WEIGHTS[i]);
    }
    putchar('\n');
    for(int i = 0; i < HIDDEN_LAYER_WEIGHTS; i++)
    {
      printf("HIDDEN_LAYER_to_OUTPUT_LAYER_weights[%i] = %lf\n",i , HIDDEN_LAYER_to_OUTPUT_LAYER_weights[i]);
    }
}
