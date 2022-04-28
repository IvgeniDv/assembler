
#include "data.h"






/*
	*head = start of the word list
	*fname = name of the (argv[i]) original file name
*/
int ** parse(int **memoryImage, wordNode *head, char *fname, labelNode *lNodeRoot, int *cIS, int *dIS, int *errorFlag)
{  
	FILE *fpPostMacro;
	 
	char *tempLabel; /* will be used to hold a new label, without the colon or without the register index */

	char *stringHolder  = NULL; /* will be used to hold the defenition of strings */
	char *postMacroName; /* the name of the .am file */

	int **codeImage = NULL;
	int **dataImage = NULL;

	int currWord[WORD_SIZE]; /* stores the word that we will insert to the memory image */

	int fourBit[4];/* represents funct or register in 4 bits */

	int adrsModBit[2];/* represents addressing modes in 2 bits */
	
	 /* just some temp vars to hold some data */
	int i, n, num;
	char ch;
	
	int ic = 100; /* first code memory address */
	int codeImageCount = 0; /* total words in image */
	int dc = 1; /* first data memory address */
	int dataImageCount = 0; /* total words in image */
	/* we use secMemWrdAdrs in case we encounter an idndex addressing mode. we need to go back and change the registers */
	int secMemWrdAdrs; /* holds the address of the current second word of a given operation */

	int lineFlag = 0; /* a flag to know we are still on the same line */
	int k; /* used to count lines in the post macro expansion file - for error reports */
	
	int nextScenario; /* used in switch to know what to do next */

	wordNode *currentWord = head; /* start of words list  - start of user input code */


	postMacroName = (char *) malloc(MAX_FILE_NAME * sizeof(char));
	codeImage = (int **) malloc(sizeof(int *));
	dataImage = (int **) malloc(sizeof(int *));


	sprintf(postMacroName, "%s%s", fname, ".am");
	fpPostMacro = fopen(postMacroName, "r");

	if(fpPostMacro == NULL) 
	{
		printf(" ERROR: File %s does not exist or can not be opened.\n", postMacroName); 
		printf(" Check source file: %s\n", fname);
		*errorFlag = 1; /* found an error */
	} /* end of if */
	else if(codeImage == NULL)
	{
		printf(" ERROR: couldn't allocate memory for code image\n");
		*errorFlag = 1; /* found an error */
	} /* end of else if */
	else if(dataImage == NULL)
	{
		printf(" ERROR: couldn't allocate memory for data image\n");
		*errorFlag = 1; /* found an error */
	} /* end of else if */
	else 
	{
		
		k = 0;

		if(currentWord->next_word != NULL) 
		{	
			currentWord = currentWord->next_word;
			while((ic + dc) <= TOTAL_MEMORY)
			{
				lineFlag = currentWord->lineNum;  
				/* if lineNum changes, we know we dropped a line */

				k += 1; /* count lines in fpPostMacro */

				while (lineFlag == currentWord->lineNum) /* read one line at a time */
				{							
					/* check first word 
					   should be a label or an operation or a guidance */
					nextScenario = checkWordType(currentWord->data);
					
					switch (nextScenario)
					{
						case 1: /* mov */
							n = 0;
							n = checkTwoArgs(currentWord, lineFlag, k);
						
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */ 
								break;
							} /* end of if */

							/* correct number of argumnets for command */
							/* now we check for the right addressing modes and correct arguments */
							n = 0;
							n = checkTwoArgsAdrsMod(currentWord, k, nextScenario);
							
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* all addressing modes are correct and argumentrs are valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */
								
									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 1);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							
							currentWord = currentWord->next_word->next_word; /* skipping the comma */
							n = getAddressingMode(currentWord->data); /* destination-addressing mode. we know its valid */

							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 2: /* cmp */
							n = 0;
							n = checkTwoArgs(currentWord, lineFlag, k);
						
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							/* correct number of argumnets for command */
							/* now we check for the right addressing modes and correct arguments */
							n = 0;
							n = checkTwoArgsAdrsMod(currentWord, k, nextScenario);
							
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* all addressing modes are correct and argumentrs are valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 1);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							
							currentWord = currentWord->next_word->next_word; /* skipping the comma */
							n = getAddressingMode(currentWord->data); /* destination-addressing mode. we know its valid */

							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 3: /* add */
							n = 0;
							n = checkTwoArgs(currentWord, lineFlag, k);
						
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							/* correct number of argumnets for command */
							/* now we check for the right addressing modes and correct arguments */
							n = 0;
							n = checkTwoArgsAdrsMod(currentWord, k, nextScenario);
							
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* all addressing modes are correct and argumentrs are valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 1);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							
							currentWord = currentWord->next_word->next_word; /* skipping the comma */
							n = getAddressingMode(currentWord->data); /* destination-addressing mode. we know its valid */

							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 4: /* sub */
							n = 0;
							n = checkTwoArgs(currentWord, lineFlag, k);
						
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							/* correct number of argumnets for command */
							/* now we check for the right addressing modes and correct arguments */
							n = 0;
							n = checkTwoArgsAdrsMod(currentWord, k, nextScenario);
							
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* all addressing modes are correct and argumentrs are valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 1);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k); /* check if label is extern and entry */

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							
							currentWord = currentWord->next_word->next_word; /* skipping the comma */
							n = getAddressingMode(currentWord->data); /* destination-addressing mode. we know its valid */

							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 5: /* lea */
							n = 0;
							n = checkTwoArgs(currentWord, lineFlag, k);
						
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							/* correct number of argumnets for command */
							/* now we check for the right addressing modes and correct arguments */
							n = 0;
							n = checkTwoArgsAdrsMod(currentWord, k, nextScenario);
							
							if(n == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* all addressing modes are correct and argumentrs are valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 1);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							
							currentWord = currentWord->next_word->next_word; /* skipping the comma */
							n = getAddressingMode(currentWord->data); /* destination-addressing mode. we know its valid */

							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 6: /* clr */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 7: /* not */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 8: /* inc */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */

									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 9: /* dec */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 10: /* jmp */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
					
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 11: /* bne */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
			
									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 12: /* jsr */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */

									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 13: /* red */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */

									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 14: /* prn */
							/* check for correct number of argumnets for command */
							if(checkOneArg(currentWord, lineFlag, k) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							
							/* now we check for the right addressing mode and correct argument */
							if(checkOneArgAdrsMod(currentWord, k, nextScenario) == 1) /* found and error */
							{
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								lineFlag = 0;
								*errorFlag = 1; /* found an error */
								break;
							} /* end of if */

							 /* addressing mode is correct and argument is valid */
							/* here will insert the word to the code image */
							
							/* first word */
							codeImage = createFirstWord(codeImage, currWord, currentWord, &codeImageCount, &ic);

							/* second word */
							codeImage = createOneArgSecondWord(codeImage, currWord, fourBit, adrsModBit, currentWord, &codeImageCount, &ic, &secMemWrdAdrs);
							
							/* 
								so far we have in memory : 
								the minimum - two words
								0  A  opcode
								0  A  funct source-register source-addressing target-register target-addressing
							*/

							/* lets check if we have more words to build */
							
							currentWord = currentWord->next_word; /* we now hold the first operand */
							n = getAddressingMode(currentWord->data); /* source operand -addressing mode. we know its valid */
							switch (n)
							{
								case 0: /* immediate addressing mode */
									createImmediateAddrsWord(currWord, currentWord);

									codeImage = buildImage(codeImage, currWord, codeImageCount);
									codeImageCount += 1;
									ic += 1;
									break;
								case 1: /* direct addressing mode. the word is a label */
									n = 0;
									n = checkLabelDuplication(lNodeRoot, currentWord->data, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */

									if(checkLblExtrn(lNodeRoot, currentWord->data) == 1) /* label is external */
									{
										if(checkSameExtern(lNodeRoot, currentWord->data, ic) == 0)
											insertLabel(lNodeRoot, currentWord->data, ic, 0, 1, 0, 0);
									} /* end of if */

									codeImage = createDirectAddrsWord(currWord, lNodeRoot, currentWord, codeImage, &codeImageCount, &ic);

									break;
								case 2: /* index addressing mode */
									/* address of the second word in memory: secMemWrdAdrs */
									codeImage = insertIndexAddrs(codeImage, currWord, fourBit, secMemWrdAdrs, currentWord, 2);

									/* now we can get back to adding the extra two words. we need to isolate the label. */
									tempLabel =  (char *) malloc(MAX_LABEL_LENGTH * sizeof(char)); 
									/* extract the label from the index */
									tempLabel = extractLabel(tempLabel, currentWord);
									
									n = 0;
									n = checkLabelDuplication(lNodeRoot, tempLabel, k);

									if(n == 1) 
									{ /* label is both entry and extern */

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										lineFlag = 0;
										*errorFlag = 1; /* found an error */
										break;
									} /* end of if */
									
									codeImage = createLabelWord(codeImage, lNodeRoot, tempLabel, currWord, &codeImageCount, &ic);

									free(tempLabel);
									tempLabel  = NULL;

									break;
								case 3: /* register direct - already in word No2 */
									break;
								default: /* not really in use. we know only valid entries are possible */
									break;
							} /* end of switch */
							break;
						case 15: /* rts */
							if (currentWord->next_word != NULL)
							{
								if(lineFlag == currentWord->next_word->lineNum)
								{
									printf("\n ERROR: too many arguments for <stop> at line %d\n", k);
								
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
									break;
								} /* end of if */
								
							} /* end of if */

							/* creation and isert of memory word to image */
							/* reset the word holder befor use */
							resetHolder(currWord, WORD_SIZE);

							currWord[1] = 1; /* Coding classification A */
							currWord[WORD_SIZE - (getOpcode(currentWord->data)) -1] = 1; /* turning on the correct bit of the opcode */
							codeImage = buildImage(codeImage, currWord, codeImageCount);
							codeImageCount += 1;
							ic += 1;
							break;
						case 16: /* stop */
							if (currentWord->next_word != NULL)
							{
								if(lineFlag == currentWord->next_word->lineNum)
								{
									printf("\n ERROR: too many arguments for <stop> at line %d\n", k);
								
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
									break;
								} /* end of if */
								
							} /* end of if */

							/* creation and isert of memory word to image */
							/* reset the word holder befor use */
							resetHolder(currWord, WORD_SIZE);

							currWord[1] = 1; /* Coding classification A */
							currWord[WORD_SIZE - (getOpcode(currentWord->data)) -1] = 1; /* turning on the correct bit of the opcode */
							codeImage = buildImage(codeImage, currWord, codeImageCount);
							codeImageCount += 1;
							ic += 1;
							break;
						case 17: /* .data */
							if(currentWord->next_word == NULL)
							{
								*errorFlag = 1; /* found an error */
								printf("\n ERROR: too little arguments for <.data> at line %d\n", k);
							} /* end of if */
							else if (lineFlag != currentWord->next_word->lineNum)
							{
								*errorFlag = 1; /* found an error */
								printf("\n ERROR: too little arguments for <.data> at line %d\n", k);
							} /* end of else if */
							else 
							{	
								n = 1; /* a helping var to count total number of members in the array */
								
								while (currentWord->next_word != NULL)
								{
									
									/* while we are on the same line */
									while(lineFlag == currentWord->next_word->lineNum)
									{
										currentWord = currentWord->next_word; /* currently holding the first member of the array */

										/* not a number where a number should be */
										if(checkIfNum(currentWord) == 0 && n % 2 != 0)
										{
											printf("\n ERROR: unsupported data type for <.data> at line %d\n", k);
											/* move to next line */
											currentWord = moveLine(currentWord, lineFlag);
											*errorFlag = 1; /* found an error */
											lineFlag = 0;
											
											break;
										} /* end of if */
										/* no comma between two numbers */
										else if(checkIfNum(currentWord) != 0 && n % 2 == 0)
										{
											printf("\n ERROR: Syntax error for <.data> at line %d\n", k);
											/* move to next line */
											currentWord = moveLine(currentWord, lineFlag);
											*errorFlag = 1; /* found an error */
											lineFlag = 0;
											break;
										} /* end of else if */
										/* not a comma where a comma should be */
										else if(strcmp(currentWord->data, ",") != 0 && n % 2 == 0)
										{
											printf("\n ERROR: Syntax error for <.data> at line %d\n", k);
											/* move to next line */
											currentWord = moveLine(currentWord, lineFlag);
											*errorFlag = 1; /* found an error */
											lineFlag = 0;
											break;
										} /* end of else if */
										else /* every thing is in its place  - lets insert the numbers to the data image */
										/* data image memory counter - dc, dataImage - 2d array image , dataImageCount-total words in image*/
											n++; /* increment total member count */
	
											if(checkIfNum(currentWord) == 1)
											{
												num = atoi(currentWord->data);
								
												/* reset the word holder befor use */
												resetHolder(currWord, WORD_SIZE);

												twosCompliment(num, currWord);
												currWord[1] = 1; /* Coding classification A */
												dataImage = buildImage2(dataImage, currWord, dataImageCount);
												dc += 1;
												dataImageCount += 1;
											} /* end of if */
										
										if(currentWord->next_word == NULL)
											break;
									} /* end of while */
									break;
								}/* end of while */

							} /* end of else */
							break;
						case 18: /* .string */
							if(currentWord->next_word == NULL)
							{
								printf("\n ERROR: too little arguments for <.string> at line %d\n", k);
								*errorFlag = 1; /* found an error */
							} /* end of if */
							else if (lineFlag != currentWord->next_word->lineNum)
							{
								printf("\n ERROR: too little arguments for <.string> at line %d\n", k);
								*errorFlag = 1; /* found an error */
							} /* end of else if */
							/* if string does not start with a quotation mark*/
							else if(strcmp(currentWord->next_word->data, "\"") != 0)
							{
								
								printf("\n Syntax error for <.string> at line %d\n", k);
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								*errorFlag = 1; /* found an error */
								lineFlag = 0;
							} /* end of else if */
							else /* string starts with a quotation mark */
 							{
								/* skiped the first quotation mark */
								if(currentWord->next_word->next_word != NULL) 
								{
									currentWord = currentWord->next_word->next_word; 
									stringHolder = (char *) malloc(MAX_LINE_LENGTH * (sizeof(char)));  /* var to hold the string content */
									n = 0; /* falg: in a declaration of a string */
									stringHolder[0] = '\0';
									strcat(stringHolder, currentWord->data);

									while(currentWord->next_word != NULL)
									{
										if(lineFlag == currentWord->next_word->lineNum && n != 1)
										{
											currentWord = currentWord->next_word;

											if(strcmp(currentWord->data, "\"") != 0)
											{
												strcat(stringHolder, " ");
												strcat(stringHolder, currentWord->data);
											} /* end of if */						
											else
											{	
												n = 1; /* encountered the closing quotation mark */
												break;
											} /* end of else */

										} /* end of if */
										else
											break;
										
									} /* end of while */

									/* if no closing quotation mark */
									if( (currentWord->next_word == NULL) && n != 1)
									{
										printf("\n Syntax error for <.string> at line %d\n", k);
										*errorFlag = 1; /* found an error */
									}
									/* skipped to next line without closing quotation mark */
									else if((currentWord->next_word != NULL)  && n != 1)
									{
										printf("\n Syntax error for <.string> at line %d\n", k);
										*errorFlag = 1; /* found an error */
									} /* end of else if */
									else
									{
										if(currentWord->next_word != NULL) /*string continuous decleration ufter closing quotation mark */
										{
											if(lineFlag == currentWord->next_word->lineNum && n == 1)
											{
												printf("\n Syntax error for <.string> at line %d\n", k);
												*errorFlag = 1; /* found an error */
												break;
											} /* end of if */
											
										} /* end of if */

										/*insert string to memory */
										for(i = 0; i < strlen(stringHolder); i++)
										{
											ch = stringHolder[i];

											/* reset the word holder befor use */
											resetHolder(currWord, WORD_SIZE);

											twosCompliment(ch, currWord);
											currWord[1] = 1; /* Coding classification A */
											dataImage = buildImage2(dataImage, currWord, dataImageCount);
											dc += 1;
											dataImageCount += 1;
										} /* end of for */	

										/* inserting EOF to memory */
								
										/* reset the word holder befor use */
										resetHolder(currWord, WORD_SIZE);

										currWord[1] = 1; /* Coding classification A */

										dataImage = buildImage2(dataImage, currWord, dataImageCount);
										dc += 1;
										dataImageCount += 1;
										
									} /* end of else */	

									free(stringHolder);
									stringHolder = NULL;
								} /* end of if */
								else /* - nothing ufter quotation mark */
								{
									*errorFlag = 1; /* found an error */
									printf("\n Syntax error for <.string> at line %d\n", k);
								} /* end of else */

							} /* end of else */
							break;
						case 19: /* .entry */

							if(currentWord->next_word == NULL)
							{
								*errorFlag = 1; /* found an error */
								printf("\n ERROR: too little arguments for <.entry> at line %d\n", k);
								break;
							} /* end of if */

							if (lineFlag != currentWord->next_word->lineNum)
							{
								*errorFlag = 1; /* found an error */
								printf("\n ERROR: too little arguments for <.entry> at line %d\n", k);
								break;
							} /* end of if */

							if(currentWord->next_word->next_word != NULL)
							{
								if(lineFlag == currentWord->next_word->next_word->lineNum)
								{
									printf("\n ERROR: too many arguments for <.entry> at line %d\n", k);
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
								} /* end of if */

							} /* end of if*/
								
							currentWord = currentWord->next_word;
							/* 0 if not valid. 1 if valid */
							if(checkLabelValid(currentWord->data) == 1)
							{
								/* 0 if label have been defined before */ 
								if(checkLblExistence(lNodeRoot, currentWord->data) == 0) 
								{
									/* checking if the label already defined as external */
									 if(checkLblExtrn(lNodeRoot, currentWord->data) == 1)
									{
										printf("\n ERROR: <.entry> at line %d already defined as external\n", k);

										/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);
										*errorFlag = 1; /* found an error */
										lineFlag = 0;
									} /* end of if */
									else
									{
					/*updateLabel(labelNode *head, char *label, int address, int ent, int ext, int cod, int dat)*/
										updateLabel(lNodeRoot, currentWord->data, 0, 1, 0, 0, 0);
									} /* end of else */

								} /* end of if */
								else /* lable hasnt been defined yet */
								{
									/* insert the label with address 0. 
									   later we can check label list.
									   labels with address zero and entry flag-on were never defined.
									   we can frow an error. */
									insertLabel(lNodeRoot, currentWord->data, 0 , 1, 0, 0, 0);
								} /* end of else */
							} /* end of if */
							else /* lable is not valid */
							{
								printf("\n ERROR: <.entry> at line %d, label definition no valid.\n", k);
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								*errorFlag = 1; /* found an error */
								lineFlag = 0;

							} /* end of else */	
							
							break;
						case 20: /* .extern */
							if(currentWord->next_word == NULL)
							{
								*errorFlag = 1; /* found an error */
								printf("\n ERROR: too little arguments for <.extern> at line %d\n", k);
								break;
							}  /* end of if*/
								
							if (lineFlag != currentWord->next_word->lineNum)
							{
								*errorFlag = 1; /* found an error */
								printf("\n ERROR: too little arguments for <.extern> at line %d\n", k);
								break;
							}  /* end of if*/

							if(currentWord->next_word->next_word != NULL)
							{
								if(lineFlag == currentWord->next_word->next_word->lineNum)
								{
									printf("\n ERROR: too many arguments for <.extern> at line %d\n", k);
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
								} /* end of if*/
								
							} /* end of if*/

							currentWord = currentWord->next_word;
							/* 0 if not valid. 1 if valid */
							if(checkLabelValid(currentWord->data) == 1)
							{
								/* 0 if label have been defined before */ 
								if(checkLblEntry(lNodeRoot, currentWord->data) == 1) 
								{
									printf("\n ERROR: <.extern> at line %d label already defined in this scope.\n", k);
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
									
								} /* end of if */
								else /* lable hasnt been defined yet */
								{
									/*  insert the label with address 0. 
									 	this condition is used to prevent multiple insertions of the same label */
									if(checkLblExtrn(lNodeRoot, currentWord->data) == -1) 
										insertLabel(lNodeRoot, currentWord->data, 0 , 0, 1, 0, 0);
								} /* end of else */
							} /* end of if */
							else /* lable is not valid */
							{
								printf("\n ERROR: <.extern> at line %d, label definition no valid.\n", k);
								/* move to next line */
								currentWord = moveLine(currentWord, lineFlag);
								*errorFlag = 1; /* found an error */
								lineFlag = 0;
							} /* end of else */
							break;
						case 21: /* label - already checked for syntax */
							tempLabel = (char *) malloc( (strlen(currentWord->data)) * sizeof(char)); 

							/* (strlen(current) - 1) because we dont want the colon at the end */
							for(i = 0; i < strlen(currentWord->data) -1; i++)
								tempLabel[i] = currentWord->data[i];
							tempLabel[i] = '\0';


							if(checkLblExistence(lNodeRoot, tempLabel) == 0) /* label have been defined before */
							{
								if(checkLabelDuplication(lNodeRoot, tempLabel, k) != 0) /* label is both entry and extern */
								{
									/* move to next line */
										currentWord = moveLine(currentWord, lineFlag);

										lineFlag = 0;
										break;
								} /* end of if */

								if(checkLblExtrn(lNodeRoot, tempLabel) == 1) /* label already defined as external */
								{
									printf("\n ERROR: at line %d.\n\tLabel already defined as external at.\n", k);
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
									break;
								} /* end of if */

							} /* end of if */
							
							if(currentWord->next_word == NULL)
							{
								printf("\n ERROR: not a valid label definition at line %d\n\tlabel can't be empty\n", k);
								*errorFlag = 1; /* found an error */
							} /* end of if */
							else if (lineFlag != currentWord->next_word->lineNum)
							{
								printf("\n ERROR: not a valid label definition at line %d\n\tlabel can't be empty\n", k);
								*errorFlag = 1; /* found an error */
							} /* end of else if */
							else /* we have at least another word for label definition */
							{
								
								n = checkWordType(currentWord->next_word->data);	
					/*insertLabel(labelNode *head, char *labelToInsert, int address, int ent, int ext, int cod, int dat)*/
								if ( (1 <= n ) && (n <= 16)) /* next word is one of 16 legal operations */
								{
									if(checkLblEntry(lNodeRoot, tempLabel) != -1) /* label already defined  */
										updateLabel(lNodeRoot, tempLabel, ic, 0, 0, 1, 0);
									else
										insertLabel(lNodeRoot, tempLabel, ic , 0, 0, 1, 0);
									k -= 1; /* to balance out the line count */
								
								} /* end of if */
									
								else if(n == 17 || n == 18) /* next word is .data or .string*/
								{
									if(checkLblEntry(lNodeRoot, tempLabel) != -1) /* label defined as entry */
										updateLabel(lNodeRoot, tempLabel, dc, 0, 0, 0, 1);
									else
										insertLabel(lNodeRoot, tempLabel, dc , 0, 0, 0, 1);
									k -= 1; /* to balance out the line count */
								} /* end of else if */
								else if(n == 19 || n == 20) /* next word is .entry or .extern */
									printf("\n Warning: label defenition at line %d is redundant\n", k);
								else 
								{
									printf("\n ERROR: \n\tSyntax error at line %d: label defenition\n", k);	
									/* move to next line */
									currentWord = moveLine(currentWord, lineFlag);
									*errorFlag = 1; /* found an error */
									lineFlag = 0;
								} /*end of else */
							} /* end of else */
				
							free(tempLabel);
							tempLabel = NULL;
							break;
						default: /* not a valid word */
							printf("\n Syntax error at line %d\n -> %s\n", k, currentWord->data);
							/* move to next line */
							currentWord = moveLine(currentWord, lineFlag);
							*errorFlag = 1; /* found an error */
							lineFlag = 0;
							break;
					}  /* end of switch */

					lineFlag = 0;  /* indication that we finished with the current line */
					
				} /* end of while */
				
				if(currentWord->next_word != NULL)
					currentWord = currentWord->next_word; /* reading the next word */
				else
					break;
				
			} /* end of while */

		} /* end of  if */
		else
		{
			printf("ERROR: Source file: %s is empty.\nNothing to assemble\n", fname);
			*errorFlag = 1; /* found an error */
		} /* end of else */

		fclose(fpPostMacro);
	
	} /* end of else */

	
	free(postMacroName);
	
	/* updates labels to the correct address */
	updateLabelAddress(lNodeRoot, ic);

	/* unite both memory images to one single image - codeImage */
	/*codeImage = uniteCodeDataImage(codeImage, dataImage, codeImageCount, dataImageCount);*/
	
	memoryImage = uniteCodeDataImage(memoryImage, codeImage, 0, codeImageCount);
	memoryImage = uniteCodeDataImage(memoryImage, dataImage, codeImageCount, dataImageCount);

	*cIS = codeImageCount;
	*dIS = dataImageCount;

	freeImage(codeImage, codeImageCount);
	freeImage(dataImage, dataImageCount);
	

	return memoryImage;

} /* end of traverse */














