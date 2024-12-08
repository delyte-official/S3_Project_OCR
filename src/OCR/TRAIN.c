#include "TRAIN.h"


//Create a matrix from a .txt file.
double *matrixFromFile(char *filename)
{
  double *matrix = malloc(sizeof(double) * 28 * 28);
  FILE *file = fopen(filename,"r");

  if(file == NULL)
    printf("File is NULL \n");
  for(int i = 0; i <= 28; i++)
  {
    for(int j = 0; j <= 28; j++)
    {
      int c = fgetc(file);
      if(c == 49)
        matrix[j+i*28] = 1;
      if(c == 48)
        matrix[j+i*28] = 0;
    }
  }
  fclose(file);
  return matrix;
}

//Create & return the double* goals pixels values from filename
double *goalArray(char letter)
{
  double *goal = calloc(52, sizeof(double));

  if(letter >= 'A' && letter <= 'Z')
    goal[(int)(letter) - 65] = 1;

  else if(letter >= 'a' && letter <= 'z')
    goal[((int)(letter) - 97) + 26] = 1;

  return goal;
}
//Create & return all the goals matrixes (for all letters)
double **goalMatrix()
{
  double **goalMatrix = malloc(sizeof(double*) *  52);
  char maj = 'A';
  char min = 'a';
  for(int i = 0; i < 52; i++)
  {
    if(i < 26)
    {
      goalMatrix[i] = goalArray(maj);
	  maj++;
    }
    else
    {
      goalMatrix[i] = goalArray(min);
      min++;
    }
  }
  return goalMatrix;
}

//Create & return all the letters matrixes (for all letters)
double **lettersMatrix()
{
  //Variables
  //char majs_path[17] = "../majs/0/04.txt\0";
  //char mins_path[17] = "../mins/0/03.txt\0";
  char majs_path[17+9] = "../dataset3/majs/0/04.txt\0";
  char mins_path[17+9] = "../dataset3/mins/0/03.txt\0";
  double **lettersMatrix = malloc(sizeof(double *) * 52);
  char maj = 'A';
  char min = 'a';

  for(int i = 0; i < 52; i++)
  {

    if(i < 26 * 1)
    {
      majs_path[5+3+9] = maj;
	  majs_path[7+3+9] = maj;
      lettersMatrix[i] = matrixFromFile(majs_path);
      maj++;
    }

    else if(i >= 26*1)
    {
      mins_path[5+3+9] = min;
      mins_path[7+3+9] = min;
      lettersMatrix[i] = matrixFromFile(mins_path);
      min++;

    }
  }
  return lettersMatrix;
}

//Return the position of the output with the greatest sigmoid result
int RetrievePos(struct Neural_Network *net)
{
  double max = 0;
  int posMax = 0;

  for (int o = 0; o < net -> nbOutput; o++)
  {
    if (max < net -> OutputO[o])
    {
      posMax = o;
      max = net -> OutputO[o];
    }
  }
  return posMax;
}

//Calculates the Squared error
void SquaredError(struct Neural_Network *net)
{
  double max = 0;
  double sum = 0;

  for (int o = 0; o < net -> nbOutput; o++)
  {
    if (max < net -> OutputO[o])
    {
      max = net -> OutputO[o];
    }
    sum += (net -> Goal[o] - net -> OutputO[o]) *
                      (net -> Goal[o] - net -> OutputO[o]);
  }
  net -> ErrorRate += 0.5 * sum;
}

//Retrive the position of the char in the goal tab
int PosGoal(double *goal)
{
  int count = 0;

  while(goal[count] != 1.0)
    count++;

  return count;
}

//Retrive char from value val
char RetrieveChar(int val)
{
  char c;

  if(val <= 25)
  {
    c = val + 65;
  }
  else if(val > 25 && val <= 51)
  {
    c = (val + 97 - 26);
  }
  else if(val > 51 && val <= 61)
  {
    c = val + 48 - 52;
  }
  else
  {
    switch(val)
    {
      case 62:
        c = ';';
        break;
      case 63:
        c = '\'';
        break;
      case 64:
        c = ':';
        break;
      case 65:
        c = '-';
        break;
      case 66:
        c = '.';
        break;
      case 67:
        c = '!';
        break;
      case 68:
        c = '?';
        break;
      case 69:
        c = '(';
        break;
      case 70:
        c = '\"';
        break;
      case 71:
        c = ')';
        break;
      default:
        exit(1);
        break;
    }
  }
  return c;
}

#define KRED "\x1b[31m"
#define KGRN "\x1b[32m"
#define KWHT "\x1b[37m"

int trainNN()
{
	//Variables
	int nbEpoch = 2000;
	int nbLetters = 26 * 1 + 26 * 1; //5 fonts for uppers & 4 for lowers
	int currentChar = 0;
	srand(time(NULL));

	//Intialize network
	struct Neural_Network *net = InitializeNetwork();

	//Initialize all goals & letters
	double **goal = goalMatrix();
	double **letters = lettersMatrix();

	for (int epoch = 0; epoch <=  nbEpoch; epoch++)
	{
			currentChar = 0;
			for (int l = 0; l < nbLetters; l++)
			{

					Neural_Network_OCR(net, letters[l], goal[currentChar]);
          currentChar++;

					if (epoch % 100 == 0)
					{
							PrintState(net);
					}
			}
			//== PRINT ERROR EVERY 100 EPOCHs ==//
			if (epoch % 100 == 0)
			{
					if(net -> MaxErrorRate > 0.005)
							printf("Epoch %-5d | MaxErrorRate = %s %f \n",
                    epoch,KRED,net->MaxErrorRate);
					else
					{
							printf("Epoch %-5d | MaxErrorRate = %s %f \n",
                      epoch,KGRN,net->MaxErrorRate);
					}
					printf("%s",KWHT);
			}
      if(net->MaxErrorRate<0.005 && net->MaxErrorRate != 0.0)
      {
         break;
      }
			net -> MaxErrorRate = 0.0;
	}
  printf("Save data...\n");
  SaveData(net);
  printf("Learn finish\n");
	return EXIT_SUCCESS;
}
