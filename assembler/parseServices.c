
#include "data.h"



/*
	gets a pointer to a word node
	check if a word is a: 
	valid operation - return 1(mov) to 16(stop) according to the operation
	valid guidance - return 17(data) to 20(extern) according to the guidance
	valid lable - return 21 
	valid register - return 30(r0) to 45(r15) according to the register
	else return 0;

*/
int checkWordType(char *current)
{
	int wordType = 0;	
	char label[MAX_LABEL_LENGTH];

	/* valid instruction operations table */
	char operations[16][5] = {"mov", "cmp","add", "sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};

	/* valid guidance operations table */
	char guidance[4][10] = {".data", ".string", ".entry", ".extern"};

	/* valid registers table */
	char registers[16][5] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
	
	int i;

	for(i = 0; i < OPERATIONS; i++)
	{
		if(strcmp(current, operations[i]) == 0)
		{
			wordType = i + 1;
	
			return wordType;
		} /* end of if */
	} /* end of for */		

	for(i = 0; i < GUIDANCE; i++)
	{
		if(strcmp(current, guidance[i]) == 0)
		{
			wordType = i + 17;
			
			return wordType;
		} /* end of if */
	} /* end of for */			

	for(i = 0; i < TOTAL_REGISTERS; i++)
	{
		if(strcmp(current, registers[i]) == 0)
		{
			wordType = i + 30;
			
			return wordType;
		} /* end of if */
	} /* end of for */			


	if(strlen(current) <= MAX_LABEL_LENGTH)
	{

		/* (strlen(current) - 1) because we dont want the colon at the end */

		for(i = 0; i < strlen(current) -1; i++)
			label[i] = current[i];

		label[i] = '\0';

		if(current[strlen(current) -1] == ':')
			if(checkLabelValid(label) != 0)
				wordType = 21;

	} /* end of if */

	return wordType;

} /* end of checkWordType */



/*
	function gets a pointer to an operation which takes two arguments.
	checks for correct number of arguments and a comma in between.
	return 0 if all ok. 
	return 1 if an error is found.

*/
int checkTwoArgs(wordNode *current, int lFlag, int k)
{

	int flag = 0;


	if(current->next_word != NULL)
	{
		if(current->next_word->next_word != NULL)
		{
			if(current->next_word->next_word->next_word != NULL)
			{
				if(current->next_word->next_word->next_word->next_word != NULL)
				{
					if(lFlag == current->next_word->next_word->next_word->next_word->lineNum)
					{
						printf("\n ERROR: too many arguments at line %d\n", k);
						flag = 1;
						return flag;
					} /* end of if */
				} /* end of if */
			} /* end of if */
		} /* end of if */
	} /* end of if */


	if (current->next_word == NULL) /* first argument is missing */
	{
		printf("\n ERROR: insufficient arguments at line %d\n", k);
		flag = 1;
		return flag;
	} /* end of if */


	/* why am i jumping 3 words?  because the scond "word" is a comma */
	if(current->next_word != NULL)
	{
		if(current->next_word->next_word != NULL)
		{
			if(current->next_word->next_word->next_word == NULL)
			{
				printf("\n ERROR: insufficient arguments at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
		} /* end of if */
	} /* end of if */


	if(lFlag != current->next_word->next_word->next_word->lineNum)
	{/* if arguments not on the same line */
		printf("\n ERROR: insufficient arguments at line %d\n", k);
		flag = 1;
		return flag;
	} /* end of if */

	if (strcmp(current->next_word->next_word->data, ",") != 0)
	{ /* we check if we have a comma between two arguments. ignoring spaces */
		printf("\n ERROR: syntax error at line %d. Missing comma.\n", k);
		flag = 1;
		return flag;
	} /* end of if */


	return flag;

} /* end of checkTwoArgs */


/*
	function gets a pointer to an operation which takes one argument.
	checks for correct number of arguments.
	return 0 if all ok. 
	return 1 if an error is found.

*/
int checkOneArg(wordNode *current, int lFlag, int k)
{
	int flag = 0;

	if(current->next_word != NULL)
	{
		if(current->next_word->next_word != NULL)
		{
			if(lFlag == current->next_word->next_word->lineNum)
			{
				printf("\n ERROR: too many arguments at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
		} /* end of if */
	} /* end of if */


	if (current->next_word == NULL) /* first argument is missing */
	{
		printf("\n ERROR: insufficient arguments at line %d\n", k);
		flag = 1;
		return flag;
	} /* end of if */
	
	if(lFlag != current->next_word->lineNum) /* argument not on the same line */
	{
		printf("\n ERROR: insufficient arguments at line %d\n", k);
		flag = 1;
		return flag;
	} /* end of if */


	return flag;

} /* end of checkOneArg */


/*
	function gets a pointer to an operation.
	checks valid addresing mode for argumnent.
	return  0 if all ok.
	return 1 if invalid addressing mode is used.

	operationId: "clr"- 6, "not"- 7, "inc"- 8, "dec"- 9, "jmp"- 10, "bne"- 11, "jsr"- 12, "red"- 13, "prn"- 14.
	
*/
int checkOneArgAdrsMod(wordNode *current, int k, int operationId)
{
	int temp;
	int flag = 0;
	int adrssCase;


	if((operationId >= 6 && operationId <= 9) || operationId == 13)	
		adrssCase = 1;
	else if (operationId >= 10 && operationId <= 12) 
		adrssCase = 2;
	else 
		adrssCase = 0;


	switch (adrssCase)
	{
		case 1: /* clr, not, inc, dec, red */
			temp = getAddressingMode(current->next_word->data);
			if(temp == -1 || temp == 0)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
		case 2: /* jmp, bne, jsr */
			temp = getAddressingMode(current->next_word->data);
			if(temp == -1 || temp == 0 || temp == 3)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
		default: /* prn */ 
			if(getAddressingMode(current->next_word->data) == -1)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
	} /* end of switch */

	return flag;

} /* end of checkOneArgAdrsMod */


/*
	function gets a pointer to an operation.
	checks valid addresing mode for both argumnents.
	return  0 if all ok.
	return 1 if invalid addressing mode is used.

	operationId: "mov"- 1, "cmp"- 2, "add"- 3, "sub"- 4, "lea"- 5
	
*/
int checkTwoArgsAdrsMod(wordNode *current, int k, int operationId)
{
	int temp;
	int flag = 0;

	switch (operationId) /* first argument addressing mode */
	{
		case 5: /* for operation lea */
			temp = getAddressingMode(current->next_word->data);
			if(temp == -1 || temp == 0 || temp == 3)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
		default: /* for operations mov, cmp, add, sub */
			if(getAddressingMode(current->next_word->data) == -1)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
	} /* end of switch */

	switch (operationId) /* second argument addressing mode */
	{
		case 2: /* for operation cmp */
			if(getAddressingMode(current->next_word->next_word->next_word->data) == -1)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
		default: /* for operations mov, cmp, add, sub */
			temp = getAddressingMode(current->next_word->next_word->next_word->data);
			if(temp == -1 || temp == 0)
			{
				printf("\n ERROR: incorrect addressing mode for argument at line %d\n", k);
				flag = 1;
				return flag;
			} /* end of if */
			break;
	} /* end of switch */

	return flag;

} /* end of checkTwoArgsAdrsMod */


/*
	function checks that a given label is both extern and entry.
	return 0 of all ok.
	return 1 if label is both.
*/
int checkLabelDuplication(labelNode *labelRoot, char *label, int k)
{

	int flag = 0;

	if(checkLblEntry(labelRoot, label) == 1 && checkLblExtrn(labelRoot, label) == 1) 
	{ /* label is both entry and extern */

		printf("\n ERROR: label <%s> at line %d\n",label, k);
		printf("\t defined as both entry and extern.\n");
		flag = 1;
	} /* end of if */

	return flag;

} /* end of checkLabelDuplication */


/*
	function gets an int array
	resets it to 0 
*/
void resetHolder(int *holder, int size)
{
	int i;

	for(i = 0; i < size; i++) 
		holder[i] = 0;

} /* end of resetWordHolder */


/*
	function creates the first memory word of a given operation.
*/
int ** createFirstWord(int **image, int *memWord, wordNode *current, int *codeImageCount, int *ic)
{
	/* reset the word holder befor use */
	resetHolder(memWord, WORD_SIZE);

	memWord[1] = 1; /* Coding classification A */
	memWord[WORD_SIZE - (getOpcode(current->data)) -1] = 1; /* turning on the correct bit of the opcode */

	image = buildImage(image, memWord, *codeImageCount); /* inserting the word to the code image */
	*codeImageCount += 1;
	*ic += 1;
	
	return image;
} /* end of createFirstWord */


/*
	this function will work only for operations with one argument.
	second memory word creator for one-argument operations.
*/
int ** createOneArgSecondWord(int **image, int *memWord, int *fourBitWord, int *twoBitWord, wordNode *current, int *codeImageCount, int *ic, int *secMemWrdAdrs)
{

	int n, i;
	
	/* reset the word holder befor use */
	resetHolder(memWord, WORD_SIZE);

	/* reset bit representation of funct and registers */
	resetHolder(fourBitWord, 4);

	memWord[1] = 1; /* Coding classification A */
	n = getFunct(current->data);
	convertToFourBit(n, fourBitWord);
	/* copy funct to word */
	for(i = 0; i < 4; i++)
			memWord[4 + i] = fourBitWord[i];

	/* 
		values in this if are according to the return values of the function 
		30 = r0, 45 = r15
	*/
	n = checkWordType(current->next_word->data);
	if(n >= 30 && n <= 45)
	{
		n -= 30;
		convertToFourBit(n, fourBitWord);

		for(i = 0; i < 4; i++)
			memWord[14 + i] = fourBitWord[i];
		
	} /* end of if */

	n = getAddressingMode(current->next_word->data);

	convertToTwoBit(n, twoBitWord);
	for(i = 0; i < 2; i++)
			memWord[18 + i] = twoBitWord[i];

	image = buildImage(image, memWord, *codeImageCount);
	/* wi'll keep the address of the second word in case we have index addresing mode. */
	*secMemWrdAdrs = *codeImageCount; 
	*codeImageCount += 1;
	*ic += 1;
	
	return image;

} /* end of createOneArgSecondWord */


/*
	this function will work only for operations with two arguments
	second memory word creator for two-argument operations.
*/
int ** createSecondWord(int **image, int *memWord, int *fourBitWord, int *twoBitWord, wordNode *current, int *codeImageCount, int *ic, int *secMemWrdAdrs)
{
	int n, i;

	/* reset the word holder befor use */
	resetHolder(memWord, WORD_SIZE);

	/* reset bit representation of funct and registers */
	resetHolder(fourBitWord, 4);

	memWord[1] = 1; /* Coding classification A */
	n = getFunct(current->data);
	convertToFourBit(n, fourBitWord);
	/* copy funct to word */
	for(i = 0; i < 4; i++)
			memWord[4 + i] = fourBitWord[i];

	/* 
		values in this if are according to the return values of the function 
		30 = r0, 45 = r15
	*/
	n = checkWordType(current->next_word->data);
	if(n >= 30 && n <= 45)
	{
		n -= 30;
		convertToFourBit(n, fourBitWord);

		for(i = 0; i < 4; i++)
			memWord[8 + i] = fourBitWord[i];
		
	} /* end of if */

	n = getAddressingMode(current->next_word->data);

	convertToTwoBit(n, twoBitWord);
	for(i = 0; i < 2; i++)
			memWord[12 + i] = twoBitWord[i];

	/* reset bit representation of funct and registers */
	resetHolder(fourBitWord, 4);

	/* checking if the second argument is a register.  we jump over the comma "word" */
	n = checkWordType(current->next_word->next_word->next_word->data);
	if(n >= 30 && n <= 45) 
	{
		convertToFourBit((n - 30), fourBitWord);
		for(i = 0; i < 4; i++)
			memWord[14 + i] = fourBitWord[i];

	} /* end of if */

	n = getAddressingMode(current->next_word->next_word->next_word->data);  /* we jump over the comma "word" */
	convertToTwoBit(n, twoBitWord);
	for(i = 0; i < 2; i++)
			memWord[18 + i] = twoBitWord[i];


	
	image = buildImage(image, memWord, *codeImageCount);
	/* wi'll keep the address of the second word in case we have index addresing mode. */
	*secMemWrdAdrs = *codeImageCount; 
	*codeImageCount += 1;
	*ic += 1;
	
	return image;

} /* end of createSecondWord */


/*
	function creates a memory word out of an immediate addressing mode argument
*/
void createImmediateAddrsWord(int *memWord, wordNode *current)
{

	int n;

	/* reset the word holder befor use */
	resetHolder(memWord, WORD_SIZE);

	memWord[1] = 1; /* Coding classification A */

	/* retrieving number from word */
	n = getNum(current);
	/* conversion to twos-Compliment 16 bit from the given int */
	twosCompliment(n, memWord);

} /* end of createImmediateAddrsWord */ 


/*
	function creates a memory word out of a direct addressing mode argument
*/
int ** createDirectAddrsWord(int *memWord, labelNode *labelRoot, wordNode *current, int **image, int *codeImageCount, int *ic)
{

	int n;
	
	if(checkLblExtrn(labelRoot, current->data) == 1)
	{ /* label is external */
		/* reset the word holder befor use */	
		resetHolder(memWord, WORD_SIZE);

		memWord[3] = 1; /* Coding classification E */
		
		/* insert two empty words */
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

	} /* end of else if */
	else if(checkLblEntry(labelRoot, current->data) == 1 || checkLblEntry(labelRoot, current->data) == 0)
	{ /* label is entry or not entry and not external */
		/* reset the word holder befor use */
		resetHolder(memWord, WORD_SIZE);

		memWord[2] = 1; /* Coding classification R */
		/* insert base address */
		n = getBaseAddress(labelRoot, current->data);
		twosCompliment(abs(n), memWord);
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

		/* reset the word holder befor use */
		resetHolder(memWord, WORD_SIZE);

		memWord[2] = 1; /* Coding classification R */
		/* insert hist address */
		n = getHistAddress(labelRoot, current->data);
		twosCompliment(abs(n), memWord);
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;
	} /* end of else if */
	else /* no label defined yet */
	{
		/* insert two words as a flag will need to comeback to it in the second pass */
		/* reset the word holder befor use */
		resetHolder(memWord, WORD_SIZE);

		memWord[2] = 1; /* Coding classification R */

		/* insert flags to word. flag will be represented using the int - 9 */
		for(n = 4; n < WORD_SIZE; n++) 
			memWord[n] = 9;
	
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;
		
	} /* end of else */

	return image;

} /* end of createDirectAddrsWord */


int ** createLabelWord(int **image, labelNode *labelRoot, char *label, int *memWord, int *codeImageCount, int *ic)
{

	int n;

	if(checkLblExtrn(labelRoot, label) == 1)
	{ /* label is external */
		/* reset the word holder befor use */
		resetHolder(memWord, WORD_SIZE);

		memWord[3] = 1; /* Coding classification E */
		
		/* insert two empty words */
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

	} /* end of else if */
	else if(checkLblEntry(labelRoot, label) == 1 || checkLblEntry(labelRoot, label) == 0)
	{ /* label is entry or not entry and not external */
		/* reset the word holder befor use */
		resetHolder(memWord, WORD_SIZE);

		memWord[2] = 1; /* Coding classification R */
		/* insert base address */
		n = getBaseAddress(labelRoot, label);
		twosCompliment(abs(n), memWord);
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

		for(n = 0; n < WORD_SIZE; n++) /* reset the word holder befor use */
			memWord[n] = 0;

		memWord[2] = 1; /* Coding classification R */
		/* insert hist address */
		n = getHistAddress(labelRoot, label);
		twosCompliment(abs(n), memWord);
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;
	} /* end of else if */
	else /* no label defined yet */
	{
		/* insert two words as a flag will need to comeback to it in the second pass */
		/* reset the word holder befor use */
		resetHolder(memWord, WORD_SIZE);

		memWord[2] = 1; /* Coding classification R */

		/* insert flags to word. flag will be represented using the int 9. */
		for(n = 4; n < WORD_SIZE; n++) 
			memWord[n] = 9;
	
		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;

		image = buildImage(image, memWord, *codeImageCount);
		*codeImageCount += 1;
		*ic += 1;
		
	} /* end of else */

	return image;

} /* end of createLabelWord*/


/*
	the function edits the second word. 
	inserting the register number that is being used in the index addressing mode.
	int argNum used to indicate what argument is being used.
	argNum allows us to insert the index to the correct place in the second word.
*/
int ** insertIndexAddrs(int **image, int *memWord, int *fourBitWord, int wordAddrs, wordNode *current, int argNum)
{
	int n, i;
	char c[2];
	
	/* reset the word holder befor use */
	resetHolder(memWord, WORD_SIZE);

	/* reset bit representation of funct and registers */
	resetHolder(fourBitWord, 4);

	/* we want the last digit of the register description. we know its valid by now. */
	c[0] = current->data[strlen(current->data) - 2];
	c[1] = '\0';
	n = atoi(c); 
	
	/* we add 10 to n because we got n to be the units digit */
	convertToFourBit( n + 10, fourBitWord);

	for(n = 0; n < WORD_SIZE; n++)/* get the second word */
	{
		memWord[n] = image[wordAddrs][n];
	} /* end of for */

	/* edit the second word with the correct register number */
	switch (argNum)
	{
		case 1: /* first argument is of index addressing mode */
			for(i = 0; i < 4; i++)
				memWord[8 + i] = fourBitWord[i];
			break;
		default: /* second argument is of index addressing mode */
			for(i = 0; i < 4; i++)
				memWord[14 + i] = fourBitWord[i];
			break;
	} /* end of switch */
	
	image = editImage(image, memWord, wordAddrs); /* insert back the second word */

	return image;

} /* end of insertIndexAddrs */


/*
	function moves to next line in a file.
*/
wordNode * moveLine(wordNode *current, int lineFlag)
{

	while(current->next_word != NULL)
	{
		if(lineFlag == current->next_word->lineNum)
			current = current->next_word;
		else
			break;
	} /* end of while */


	return current;

} /* end of moveLine */



/*
	function gets a pointer to a temp label holder and the current word,
	holding the index addressing mode.
	the function extracts the label from the index.
*/
char * extractLabel(char *destination, wordNode *current)
{
	int n;

	/* -5 te remove the chars of the index [rxx] */
	for(n = 0; n < strlen(current->data) - 5; n++)
		destination[n] = current->data[n];
	
	destination[n] = '\0';

	
	return destination;

} /* end of  extractLabel */


