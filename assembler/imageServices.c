
#include "data.h"



/*
	Builds the code image
	function gets a pointer to a 2-d array (which is the code/data image),
	a memory-word (20 bits long) to add to the image and the current words in memory count.
*/
int** buildImage(int **image, int *memWord, int totalWordsInMem)
{
	int i;

	image = realloc(image, (totalWordsInMem + 1) * sizeof(int *));
	image[totalWordsInMem] = (int *) malloc(WORD_SIZE * sizeof(int));

	for(i = 0; i < WORD_SIZE; i++)
		image[totalWordsInMem][i] = memWord[i];

	return image;
} /* end of buildImage */


/*
	Builds the data image
	function gets a pointer to a 2-d array (which is the code/data image),
	a memory-word (20 bits long) to add to the image and the current words in memory count.
*/
int** buildImage2(int **image, int *memWord, int totalWordsInMem)
{
	int i;

	image = realloc(image, (totalWordsInMem + 1) * sizeof(int *));
	image[totalWordsInMem] = (int *) malloc(WORD_SIZE * sizeof(int));

	for(i = 0; i < WORD_SIZE; i++)
		image[totalWordsInMem][i] = memWord[i];


	return image;
} /* end of buildImage2 */


/*
	function gets a pointer to a 2-d array (which is the code/data image),
	a memory-word (20 bits long) and an address location.
	the function replaces the old memory word in the given location with the new one.
	assuming address is correct.
*/
int** editImage (int **image, int *memWord, int address)
{

	int i;

	for(i = 0; i < WORD_SIZE; i++)
		image[address][i] = memWord[i];

	return image;
} /* end of editImage */


/*
	function gets a pointer to a 2-d array (which is the code/data image),
	and the total words in the memory. 
	it then prints it.
*/
void printImage(int **image, int totalWordsInMem)
{

	int i, j;


	for(i = 0; i < totalWordsInMem; i++)
	{
		for(j = 0; j < WORD_SIZE; j++)
		{
			printf("%d", image[i][j]);
		}
		printf("\n");
	}

} /* end of printImage */


/*
	function gets a pointer to a 2-d array (which is the code/data image).
	frees the memory allocated to it.
*/
void freeImage(int **image, int totalWordsInMem)
{

	int i;


	for(i = 0; i < totalWordsInMem; i++)
		free(image[i]);

	free(image);


} /* end of freeImage */



/*
	function gets a number of the funct or the register.
	gets a pointer to an array of size 4.
	fill the array according to the corresponding 4 bit representation of the given num.
*/
void convertToFourBit(int num, int *fourBit)
{
	
	int i;

	int bits[16][4] = {
						{0,0,0,0}, /* 0 */
 						{0,0,0,1}, /* 1 */
						{0,0,1,0}, /* 2 */
						{0,0,1,1}, /* 3 */
						{0,1,0,0}, /* 4 */
						{0,1,0,1}, /* 5 */
						{0,1,1,0}, /* 6 */
						{0,1,1,1}, /* 7 */
						{1,0,0,0}, /* 8 */
						{1,0,0,1}, /* 9 */
						{1,0,1,0}, /* 10 */
						{1,0,1,1}, /* 11 */
						{1,1,0,0}, /* 12 */
						{1,1,0,1}, /* 13 */
						{1,1,1,0}, /* 14 */
						{1,1,1,1} /* 15 */
						};

	
	for(i = 0; i < 4; i++)
		fourBit[i] = bits[num][i];

} /* end of convertToFourBit */


/*
	function gets the number of the addresing mode ( 0 - 3) .
	gets a pointer to an array of size 2.
	fill the array according to the corresponding 2 bit representation of the given num.
*/
void convertToTwoBit(int num, int *adrsModBit)
{
	
	int i;

	int bits[4][2] = {
						{0,0}, /* 0 */
 						{0,1}, /* 1 */
						{1,0}, /* 2 */
						{1,1} /* 3 */
						};

	for(i = 0; i < 2; i++)
		adrsModBit[i] = bits[num][i];

} /* end of convertToTwoBit */


/*  
	twos complement
	max number: +32767
	min number -32767

	gets a number, changes it to twos compliment binary.
	inserts it to the memory word in the correct place.
*/
void twosCompliment(int num, int *memWord)   
{  
	int i, j;  
	int temp;
	int binary[16]; /* 16 - number of bits */

	temp = abs(num);

	/* reseting the array */
	for(i = 0; i < 16; i++)
		binary[i] = 0;

	/* converting to binary */
	i = 0;
	while (temp != 0)
	{
		binary[i] = temp % 2;
		temp = temp / 2;
		i++;
	} /* end of wile */

	/* converting to twos compliment */
	if(num < 0)
	{
		for(i = 0; i < 16; i++)
		{
			if(binary[i] == 1)
				binary[i] = 0;
			else
				binary[i] = 1;
		} /* end of for */
	

		i = 0;
		while (i < 15) /* we dont want to change the last bit, which is the MSB */
		{
			if(binary[i] == 0)
			{
				binary[i] = 1;
				break;
			} /* end of if */
			else
			{
				binary[i] = 0;
				i++;
			} /* end of else */
		} /* end of while */	
	
	} /* end of if */


	j=4; /* we skip the first 4 bits of the given memory Word */
	i = 15; /* pointing to the end of the binary array */
	while(i >= 0 && j < WORD_SIZE) 
	{
		memWord[j] = binary[i];
		i--;
		j++;
	} /* end of while */


} /* end of twosCompliment */  


/*
	gets a pointer to a word
	if a word is a number: 1 is returned
	if a word in not a number: 0 is returned
*/
int checkIfNum(wordNode *current)
{
	int numFlag = 1; /* 1 - word is a number; 0 - word is not a number*/
	int i;

	if(current != NULL)
	{
		if(current->data[0] == '+' || current->data[0] == '-' )
		{
			/* we check that the number isnt to large to be represented in 16 bit twos compliment */
			if(strlen(current->data) <= 6 && strlen(current->data) > 1) 
			{
				
				for(i = 1; i < strlen(current->data); i++)	
				{
					if(isdigit(current->data[i]))
						;
					else
						numFlag = 0;
				} /* end of for */

			} /* end of if */
			else
			{
				
				printf("\nWarning: number might be out of working range.\n");
				numFlag = 0;
			} /* end of else */
	
		} /* end of if */
		else
		{
			if(isdigit(current->data[0]))
			{
				/* if(strlen(current->data) <= 6 && strlen(current->data) > 1) */
				if(strlen(current->data) <= 6)
				{	
					for(i = 1; i < strlen(current->data); i++)	
					{
						if(isdigit(current->data[i]))
							;
						else
							numFlag = 0;
					} /* end of for */
				} /* end of if */
				else
				{
					printf("\nWarning: number might be out of working range.\n");
					numFlag = 0;
				} /* end of else */
			} /* end of if */
			else
				numFlag = 0;
		} /* end of else */
			
	} /* end of if */
	else	
		numFlag = 0;

	return numFlag;

} /* end of checkIfNum */



/*
	this function is used only if immediate addressing mode is found.
	we use getAddressingMode(char *operand) to be sure its valid.
	the function extracts the number from the word and returns it.
*/
int getNum(wordNode *current)
{
	
	int i;
	char *num = NULL;
	int number;

	num = (char *) malloc(strlen(current->data) * sizeof(char));

	if(num == NULL)
		printf("\n ERROR: couldn't allocate memory for argument: num ,in function getNum - imageServices.\n");
	else
	{
		/* ignoring the # */
		if(current->data[1] == '+')
		{
			for(i = 2; i < strlen(current->data); i++)
				num[i - 2] = current->data[i];
		
			num[i-2] = '\0';
		} /* end of if */
		else
		{
			for(i = 1; i < strlen(current->data); i++)
				num[i - 1] = current->data[i];
		
			num[i-1] = '\0';

		} /* end of else */

		number = atoi(num);

	/*
		we need to check that number is in the limits of:
		MIN_NUMBER <= number <= MAX_NUMBER 
		if not, num will get the minimal or maximal value possible. 
	*/
		if( MIN_NUMBER  > number)
		{
			number = MIN_NUMBER;
			printf("\nWarning: number might be out of working range.\n\treverting to closest possible value.\n");
		} /* end of if */

		if(number > MAX_NUMBER )
		{
			number = MAX_NUMBER;
			printf("\nWarning: number might be out of working range.\n\treverting to closest possible value.\n");
		} /* end of if */


		free(num);
		num = NULL;
	
	} /* end of else */

	
	return number;  
	

} /* end of getNum */


/* 
	gets an operation name,
	returns the opcode.
	-1 if operation name is not valid.
*/
int getOpcode(char *operation)
{

	char operations[16][5] = {"mov", "cmp","add", "sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
	int operValue[16] =   { 0,      1,    2,     2,   4,   5,     5,   5,    5,   9,   9,    9,   12,   13,   14,   15}; /* opcode */

	int i;
	int opCode = -1; 
	
	for(i = 0; i < OPERATIONS; i++)
	{
		if(strcmp(operation, operations[i]) == 0)
		{
			opCode = operValue[i];
			break;
		} /* end of if */	
	} /* end of for */

	return opCode;
		
} /* end of getOpCode */


/* 
	gets an operation name,
	returns the funct.
	assuming operation name is valid.
*/
int getFunct(char *operation)
{
	char operations[16][5] = {"mov", "cmp","add", "sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
	/* funct. 0 if no funct */
	int operValue[16] = {0, 0, 10, 11, 0, 10, 11, 12, 13, 10, 11, 12, 0, 0, 0, 0}; 

	int i;
	int funct = 0;

	for(i = 0; i < OPERATIONS; i++)
	{
		if(strcmp(operation, operations[i]) == 0)
		{
			funct = operValue[i];
			break;
		} /* end of if */	
	} /* end of for */

	return funct;

} /* end of getFunct */



/*
	gets a word from the word list. 
	assuming its an operand.
	checks its addressing mode.
	returns the addressing mode if found a legal addressing.
	returns -1 if addressing mode not legal.

*/
int getAddressingMode(char *operand)
{
	/* 
		legal addressing modes are: 
		(0) - immediate
		(1) - direct
		(2) - index
		(3) - register direct
	*/
	int i ,j;
	char *chr = NULL;
	int addressingMod = -1;

	char *tmpLabel;


	chr = (char *) malloc(2 * sizeof(char));

	if(chr == NULL)
		printf("\n ERROR: could not allocate memory for chr in imageServices->getAddressingMode.\n");
	else
	{	
		/* check if immediate */
		if(operand[0] == '#')
		{
			if(strlen(operand) > 1)
			{
				if(operand[1] == '-')
				{
				/* longest operand to handle is of 7 chars minus hashtag and the negative sign */
					if(strlen(operand) <= 7 && strlen(operand) > 2) 
					{ /* total of 5 digits because we need to represent it in 16 bit two compliment */
						i = 2;
						while ( i < strlen(operand))
						{
							if(isdigit(operand[i]) == 0)
							{
								addressingMod = -1;
							
								return addressingMod;
							} /* end of if */
							else
								addressingMod = 0;
							
							i++;
	
						} /* end of while */
					
					
						return addressingMod;
					} /* end of if */
					else /* number might be too large to handle or no number */
					{
						addressingMod = -1;
					
						return addressingMod;
					} /* end of else */
	
				}/* end of if */
				else if(operand[1] == '+')
				{
					if(strlen(operand) <= 7 && strlen(operand) > 2) 
					{
						i = 2;
						while ( i < strlen(operand))
						{
							if(isdigit(operand[i]) == 0)
							{
								addressingMod = -1;
							
								return addressingMod;
							} /* end of if */
							else
								addressingMod = 0;

							i++;
								
						} /* end of while */
				
					
						return addressingMod;

					} /* end of if */
					else /* number might be too large to handle or no number */
					{
						addressingMod = -1;
					
						return addressingMod;
					} /* end of else */

				} /* end of else if */
				else /* check if its a digit */
				{
					if(isdigit(operand[1]) == 0)
					{
						addressingMod = -1;
					
						return addressingMod;
					} /* end of if */
					else
					{

						if(strlen(operand) <= 6 && strlen(operand) > 1) 
						{
							i = 1;
							while ( i < strlen(operand))
							{
								if(isdigit(operand[i]) == 0)
								{
									addressingMod = -1;
									return addressingMod;
								} /* end of if */
								else
									addressingMod = 0;
						
								i++;
								
							} /* end of while */
	
					
						return addressingMod;

						} /* end of if */
						else /* number might be too large to handle or no number */
						{
							addressingMod = -1;
						
							return addressingMod;
						} /* end of else */
					
					} /* end of else */

				} /* end of else */
				
			} /* end of if */
			else /* not a vlid addressing mode */
			{
				addressingMod = -1;
			
				return addressingMod;
			} /* end of else */				
		} /* end of if */

		if(checkLabelValid(operand) == 1) /* check if direct*/
		{
			addressingMod = 1;
		
			return addressingMod;
		} /* end of if */	

		if(strlen(operand) >= 6) /* check if index */
		{ /* larger or equals to 6 because we have 5 chars for the index + at least one char for tha label name */
			if(operand[strlen(operand) -1] == ']' && operand[strlen(operand) - 5] == '[' && operand[strlen(operand) - 4] == 'r' && operand[strlen(operand) -3] == '1')
			{
			
				chr[0] = operand[strlen(operand) - 2];
				chr[1] = '\0';
				j = atoi(chr);

				free(chr);

				if(j >= 0 && j <= 5)
				{
					tmpLabel = (char *) malloc((strlen(operand) - 4) * sizeof(char)); 
					/* isolating the label from the index */
					for(i = 0; i < strlen(operand) - 5; i++)
						tmpLabel[i] = operand[i];

					tmpLabel[i] = '\0';


					if(checkLabelValid(tmpLabel) == 1)
					{
						addressingMod = 2;
						free(tmpLabel);
					
						return addressingMod;
					} /* end of if */
					else
					{
						addressingMod = -1;
						free(tmpLabel);
					
						return addressingMod;
					} /* end of else */
				} /* end of if */
				else
				{
					addressingMod = -1;
				
					return addressingMod;
				} /* end of else */	
			} /* end of if*/
			else
			{
				addressingMod = -1;
			
				return addressingMod;
			} /* end of else */	
		} /* end of if */	

		 /* check if register direct */
	
		if(checkWordType(operand) >= 30 && checkWordType(operand) <= 45) /* a valid return if a register is detected */
		{
			addressingMod = 3;
		
			return addressingMod;
		} /* end of if */
		else
		{
			addressingMod = -1;
		
			return addressingMod;
		} /* end of else */
	

	} /* end of else */

	return addressingMod;

} /* end of getAddressingMode */


/*
	function gets the code image and the data image.
	it units both to the code image.
	return back the code image.
*/
int ** uniteCodeDataImage(int **codeImage, int **dataImage, int cIC, int dIC)
{

	int i,j;
	int word[WORD_SIZE];

	int codeCounter =  cIC;

	for(i = 0; i < dIC  ; i++) 
	{		
		for(j = 0; j < WORD_SIZE; j++)
			word[j] = dataImage[i][j];

		codeImage = buildImage(codeImage, word, codeCounter);
		codeCounter += 1;
	} /* end of for */
	
	
	return codeImage;

} /* end of uniteCodeDataImage */















