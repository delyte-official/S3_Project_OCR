#include "data.h"

void SaveData(struct Neural_Network *net)
{
	//Save weight Input --> Hidden.
	FILE *WeightInputHidden = fopen("WeightInputHidden.w", "w");

	for (int i = 0; i < net->nbInput; i++)
	{
		for (int j = 0; j < net->nbHidden; j++)
			fprintf(WeightInputHidden,"%f\n" , net->WeightIH[i][j]);
	}
	fclose(WeightInputHidden);


	//Save weight Hidden --> Output.
	FILE *WeightHiddenOutput = fopen("WeightHiddenOutput.w", "w");

	for (int i = 0; i < net->nbHidden; i++)
	{
		for (int j = 0; j < net->nbOutput; j++)
			fprintf(WeightHiddenOutput,"%f\n" , net->WeightHO[i][j]);
	}
	fclose(WeightHiddenOutput);


	//Save Bias Input --> Hidden.
	FILE *BiasHidden = fopen("BiasHidden.b", "w");
	for (int i = 0; i < net->nbHidden; i++)
		fprintf(BiasHidden,"%f\n" , net->BiasH[i]);
	fclose(BiasHidden);


	//Save Bias Hidden --> Output.
	FILE *BiasOutput = fopen("BiasOutput.b", "w");
	for (int i = 0; i < net->nbHidden; i++)
		fprintf(BiasOutput,"%f\n" , net->BiasO[i]);
	fclose(BiasOutput);

}

struct Neural_Network * RecupData()
{
	struct Neural_Network *net = malloc(sizeof(struct Neural_Network));

	net->nbInput = 28*28;
	net->nbHidden = 20;
	net->nbOutput = 52;

	char *line = calloc(15, sizeof(char));
	int MAXSIZE = 15;

//upload weight input hidden

	FILE *WeightInputHidden = fopen("WeightInputHidden.w", "r");
	for (int i = 0 ; i < net->nbInput; i++)
	{
		for (int j = 0; j < net->nbHidden; j++)
		{
			fgets(line, MAXSIZE, WeightInputHidden);
			strtok(line, "\n");
			net->WeightIH[i][j] = atof(line);
		}
	}
	fclose(WeightInputHidden); 

//upload weight hidden output

	FILE *WeightHiddenOutput = fopen("WeightHiddenOutput.w", "r");
	for (int i = 0 ; i < net->nbHidden; i++)
	{
		for (int j = 0; j < net->nbOutput; j++)
		{
			fgets(line, MAXSIZE, WeightHiddenOutput);
			strtok(line, "\n");
			net->WeightHO[i][j] = atof(line);
		}
	}
	fclose(WeightHiddenOutput); 

//upload bias hidden
	FILE *BiasHidden = fopen("BiasHidden.b", "r");
	for (int i = 0; i < net->nbHidden; i++)
	{
		fgets(line, MAXSIZE, BiasHidden);
		strtok(line, "\n");
		net->BiasH[i] = atof(line);
	}	
	fclose(BiasHidden);


//upload bias output.
	FILE *BiasOutput = fopen("BiasOutput.b", "r");
	for (int i = 0; i < net->nbOutput; i++)
	{
		fgets(line, MAXSIZE, BiasOutput);
		strtok(line, "\n");
		net->BiasO[i] = atof(line);
	}	
	fclose(BiasOutput);
	
	return net;
}
