#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define LEARNING_RATE 0.1
#define EPOCHS 100000

//Sigmoid function
double sigmoid(double x) 
{
    return 1 / (1 + exp(-x));
}

//Derivative of the sigmoid function
double sigmoid_derivative(double x) {
    return x * (1 - x);
}

//Neuron structure
typedef struct 
{
    double *weights;
    double bias;
    double output;
    double delta;
} Neuron;

//Layer structure
typedef struct 
{
    Neuron *neurons;
    int num_neurons;
} Layer;

//NeuralNetwork structure
typedef struct {
    Layer *layers;
    int num_layers;
} NeuralNetwork;

//Initialization of a Neuron with a given number of weights 
void initialize_neuron(Neuron *neuron, int num_weights) 
{
    neuron->weights = (double *)malloc(num_weights * sizeof(double));
    for (int i = 0; i < num_weights; i++) 
    {
        neuron->weights[i] = ((double) rand() / RAND_MAX) * 2 - 1;
    }
    neuron->bias = ((double) rand() / RAND_MAX) * 2 - 1;
}

//Initialiszation of a layer with
//a given number of neurons and of weights per neuron
void initialize_layer(Layer *layer, int num_neurons, int num_weights) 
{
    layer->neurons = (Neuron *)malloc(num_neurons * sizeof(Neuron));
    layer->num_neurons = num_neurons;
    for (int i = 0; i < num_neurons; i++) 
    {
        initialize_neuron(&layer->neurons[i], num_weights);
    }
}

//Initialization of the network
NeuralNetwork initialize_network(int *layer_sizes, int num_layers) 
{
    NeuralNetwork network;
    network.num_layers = num_layers;
    network.layers = (Layer *)malloc(num_layers * sizeof(Layer));

    for (int i = 0; i < num_layers; i++) 
    {
        int num_inputs = (i == 0) ? layer_sizes[i] : layer_sizes[i - 1];
        initialize_layer(&network.layers[i], layer_sizes[i], num_inputs);
    }
    return network;
}

//Forward Propagation 
void forward_propagate(NeuralNetwork *network, double *inputs) 
{
    for (int i = 0; i < network->layers[0].num_neurons; i++) 
    {
        network->layers[0].neurons[i].output = inputs[i];
    }

    for (int i = 1; i < network->num_layers; i++) 
    {
        Layer *layer = &network->layers[i];
        Layer *prev_layer = &network->layers[i - 1];

        for (int j = 0; j < layer->num_neurons; j++) 
	{
            Neuron *neuron = &layer->neurons[j];
            double activation = neuron->bias;

            for (int k = 0; k < prev_layer->num_neurons; k++) 
	    {
                activation += prev_layer->neurons[k].output 
			* neuron->weights[k];
            }
            neuron->output = sigmoid(activation);
        }
    }
}

//Error computing and backward_propagation
void backward_propagate(NeuralNetwork *network, double *expected) 
{
    for (int i = network->num_layers - 1; i >= 0; i--) {
        Layer *layer = &network->layers[i];

        for (int j = 0; j < layer->num_neurons; j++) 
	{
            Neuron *neuron = &layer->neurons[j];

            if (i == network->num_layers - 1) 
	    {
                neuron->delta = (expected[j] - neuron->output) 
			* sigmoid_derivative(neuron->output);
            } 
	    else 
	    {
                Layer *next_layer = &network->layers[i + 1];
                double error = 0.0;

                for (int k = 0; k < next_layer->num_neurons; k++) 
		{
                    error += next_layer->neurons[k].weights[j] 
			    * next_layer->neurons[k].delta;
                }
                neuron->delta = error * sigmoid_derivative(neuron->output);
            }
        }
    }
}

//Update of the weights
void update_weights(NeuralNetwork *network) {
    for (int i = 1; i < network->num_layers; i++) {
        Layer *layer = &network->layers[i];
        Layer *prev_layer = &network->layers[i - 1];

        for (int j = 0; j < layer->num_neurons; j++) {
            Neuron *neuron = &layer->neurons[j];
            for (int k = 0; k < prev_layer->num_neurons; k++) {
                neuron->weights[k] += LEARNING_RATE * neuron->delta 
			* prev_layer->neurons[k].output;
            }
            neuron->bias += LEARNING_RATE * neuron->delta;
        }
    }
}

//Training of the network
void train(NeuralNetwork *network, double inputs[][2], 
		double outputs[][1], int num_samples) 
{
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int i = 0; i < num_samples; i++) {
            forward_propagate(network, inputs[i]);
            backward_propagate(network, outputs[i]);
            update_weights(network);
        }
    }
}

//Test of the network
float test(NeuralNetwork *network, double inputs[][2], 
		double outputs[][1], int num_samples) 
{
	float cvalid = 0;
    printf("Testing Results:\n");
    for (int i = 0; i < num_samples; i++) 
    {
        forward_propagate(network, inputs[i]);
        double result = network
		->layers[network->num_layers - 1].neurons[0].output;
        printf("Input: %.0f, %.0f, Expected: %.0f, Network Output: %.5f\n", 
			inputs[i][0], inputs[i][1], outputs[i][0], result);
	if(fabs(outputs[i][0]-result)<0.1)
		cvalid++;
    }
    printf("Number of valid tests %f\n", cvalid); 
    return (cvalid/num_samples)*100;
}

// Global test to check accuracy
float global_test(NeuralNetwork *network, double inputs[][2],
                double outputs[][1], int num_samples)
{
        float cvalid = 0;
    for (int i = 0; i < num_samples; i++)
    {
        forward_propagate(network, inputs[i]);
        double result = network
                ->layers[network->num_layers - 1].neurons[0].output;
        /*printf("Output; %f\n", result);
        int round = result < 0.5 ? 0 : 1;
        printf("Round: %d and (%d)(%f)\n", round, (int)outputs[i][0],
                outputs[i][0]);
        if(round == (int)outputs[i][0])
                cvalid++;*/
        if(fabs(outputs[i][0]-result)<0.01)
                cvalid++;
    }
    return cvalid;
}

int main(int argc, char*argv[]) 
{
    (void)argc;
    srand(time(0));

    //Network definition : 
    //an input layer of 1 neuron, 
    //an hidden layer of 2 neurons, 
    //an output layer of 1 neuron
    int layer_sizes[] = {2, 2, 1};
    NeuralNetwork network = initialize_network(layer_sizes, 3);

    //Dataset for Not(XOR) : {A, B} , {expected exit}
    double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double outputs[4][1] = {{1}, {0}, {0}, {1}};  

    //Training and test
    int nb_tests = atoi(argv[1]);
    if (nb_tests>1)
    {
	    float valid = 0;
	for(int i =0; i<nb_tests; i++)
	{
    		train(&network, inputs, outputs, 4);
    		valid += global_test(&network, inputs, outputs, 4);
	}
	printf("Number of valid tests = %f\n",valid);
	printf("Accuracy is : %f pourcent\n",(valid/(nb_tests*4))*100);
    }
    else if(nb_tests==1)
    {
	train(&network, inputs, outputs, 4);
	float percent = test(&network, inputs, outputs, 4); 
	printf("Accuracy is : %f pourcent\n",percent);
    }
    else
    {
	    printf("Invalid nb of tests\n");
    }
    return 0;
}
