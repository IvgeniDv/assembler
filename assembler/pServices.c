/* pre assembler services */

#include "data.h"


/*
	this function is responsible for expanding calls for macro in the original code,
	into the full macro definition.
*/
void macroExpansion(char *fname, macroNode *root, wordNode *head)
{
	
	FILE *fpPostMacro = NULL;
	wordNode *currentWord = head;
	macroNode *currentMacro = root;

	char macro[] = "macro";
	char endm[] = "endm";
	int macroPrinted; /* a flag to indiacte if a macro has been printed before. 0 - not printed; 1 - printed */

	fpPostMacro = fopen(fname, "w");

	if(fpPostMacro == NULL)
		printf(" File %s can not be created.\n", fname); 
    	else 
        {
		while (currentWord->next_word != NULL)
		{
		
			macroPrinted = 0;
			currentMacro = root;

			if(strcmp(currentWord->next_word->data, macro) == 0) /* if its the defenition of the macro - move forward */
			{
				while( strcmp(currentWord->next_word->data, endm) != 0 )
				{
					currentWord = currentWord->next_word;
				} /* end of while */
				
				/*currentWord = currentWord->next_word;*/

			} /* end of if */
			else
			{
				/* check if word is a macro name; 0 if not a mcro */
				if(checkIfMacro(root, currentWord->next_word->data, currentWord->next_word->lineNum) != 0) 
				{
					while (currentMacro->next_macro != NULL && macroPrinted == 0)
					{
						if (strcmp(currentMacro->next_macro->data, macro) == 0)
						{
							if(strcmp(currentMacro->next_macro->next_macro->data, currentWord->next_word->data) == 0)
							{	
								currentMacro = currentMacro->next_macro->next_macro;
								macroPrinted = 1;

								while(strcmp(currentMacro->next_macro->data, endm) != 0)
								{
						
									if(currentMacro->next_macro->next_macro->lineNum > currentMacro->next_macro->lineNum)
									{
										fputs(currentMacro->next_macro->data, fpPostMacro);
										fputc('\n', fpPostMacro);
										
									} /*end of if */
									else 
									{
										fputs(currentMacro->next_macro->data, fpPostMacro);
										fputc(' ', fpPostMacro);
										
									} /* end of else */

									currentMacro = currentMacro->next_macro;

								} /* end of while */

							} /* end of if */
							else
							{
								currentMacro = currentMacro->next_macro;
							} /*end of else */

						} /* end of if */
						else
							currentMacro = currentMacro->next_macro;
		
					} /* end of while */	

				} /* end of if */
				else
				{
					if(currentWord->next_word->next_word != NULL)
					{
						if(currentWord->next_word->next_word->lineNum > currentWord->next_word->lineNum)
						{
							fputs(currentWord->next_word->data, fpPostMacro);
							fputc('\n', fpPostMacro);

						} /*end of if */
						else 
						{	
							fputs(currentWord->next_word->data, fpPostMacro);
							fputc(' ', fpPostMacro);
							
						} /* end of else */

					} /* end o f if */
					else 
					{
						if (currentWord != head) 
						{
							if(currentWord->next_word->lineNum > currentWord->lineNum)
							{
								fputs(currentWord->next_word->data, fpPostMacro);
								fputc('\n', fpPostMacro);

							} /*end of if */
							else 
							{	
								fputs(currentWord->next_word->data, fpPostMacro);
								fputc(' ', fpPostMacro);

							} /* end of else */	

						} /* end  of if */
						else
						{
							fputs(currentWord->next_word->data, fpPostMacro);
						} /* end of else */
	
					} /* end of else */

				} /* end of else*/

			} /* end of else */ 
			
			currentWord = currentWord->next_word;

		} /* end of while */

		fclose(fpPostMacro);

	} /* end o f else */
	
} /* end of macroExpansion */


/*
	this function "reads" the source code and converts it into a list of words.
*/
void readSource(char *fname, wordNode *head)  
{
	
	int c;
	FILE *fpOriginal = NULL; /* Original file */
	
	int charInLine; /* Total chars in one line */
	int charCounter; /* total char in one word */
	int lineCounter;
	char *word = NULL;

	wordNode *wNodeRoot = head; /* start of word list */

	word = (char *) malloc(sizeof(char));

	fpOriginal = fopen(fname,"r");
		
	if(fpOriginal == NULL) 
		printf("\n File %s does not exist or can not be opened.\n", fname); 
	else if(word == NULL)
		printf("\n ERROR: couldn't allocate memory for word in pServices->readSource.\n");
	else 
	{
		
		c = fgetc(fpOriginal);
		lineCounter = 0 ;
		while( c != EOF)
		{
			lineCounter += 1;
			
			if (c != ';') /* if not a comment */
			{
				free(word);
				word = NULL;
				word = (char *) malloc(sizeof(char));
				charInLine = 0;
				
				while(c != '\n' && c != EOF) /* moving on line char by char */
				{	
					
					charCounter = 0;
					while(c == ' ' || c  == '\t') 
					{
						c = fgetc(fpOriginal);
					
					} /* end of while */
				
					if(charInLine == 0 && c == ';')	
					{
						while (c != '\n' && c != EOF)
							c = fgetc(fpOriginal);
							
					} /* end of if */
				
					
					if(c >= MIN_ASCII && c <= MAX_ASCII)
					{
						/* in a word */
						while (charCounter < MAX_WORD_LENGTH && charInLine < MAX_LINE_LENGTH && c >= MIN_ASCII && c <= MAX_ASCII)  

						{
						
							charCounter += 1;
							charInLine += 1;
							
							if(c == ',')
							{
								if( charCounter != 1)
								{
									/*word = (char *) realloc(word, ( charCounter + 1) * sizeof(char));*/
									word = (char *) realloc(word, ( charCounter) * sizeof(char));
									word[charCounter - 1] = '\0';
									insertWord(wNodeRoot, word, lineCounter);
									free(word);
									word = NULL;
									word = (char *) malloc(sizeof(char));
									/* 2 - one for the char and one for '\0' */
									word = (char *) realloc(word, 2 * sizeof(char)); 
									word[0] = c;
									word[1] = '\0';
									insertWord(wNodeRoot, word, lineCounter);
									free(word);
									word = NULL;
									word = (char *) malloc(sizeof(char));
									charCounter = 0;
									c = fgetc(fpOriginal);
								} /* end of if */
								else 
								{
									word = (char *) realloc(word, 2 * sizeof(char));
									word[0] = c;
									word[1] = '\0';
									insertWord(wNodeRoot, word, lineCounter);
									free(word);
									word = NULL;
									word = (char *) malloc(sizeof(char));
									charCounter = 0;
									c = fgetc(fpOriginal);
								} /* end of else */

							} /* end of if */
							else if (c == '"')
							{
								if( charCounter != 1)
								{
									word = (char *) realloc(word,  charCounter * sizeof(char));
									word[charCounter - 1] = '\0';
									insertWord(wNodeRoot, word, lineCounter);
									free(word);
									word = NULL;
									word = (char *) malloc(sizeof(char));
									word = (char *) realloc(word, 2 * sizeof(char));
									word[0] = c;
									word[1] = '\0';
									insertWord(wNodeRoot, word, lineCounter);
									free(word);
									word = NULL;
									word = (char *) malloc(sizeof(char));
									charCounter = 0;
									c = fgetc(fpOriginal);
								} /* end of if */
								else 
								{
									word = (char *) realloc(word, 2 * sizeof(char));
									word[0] = c;
									word[1] = '\0';
									insertWord(wNodeRoot, word, lineCounter);
									free(word);
									word = NULL;
									word = (char *) malloc(sizeof(char));
									charCounter = 0;
									c = fgetc(fpOriginal);
								} /* end of else */

							} /* end of else if */
							else
							{
								word = (char *) realloc(word, ( charCounter) * sizeof(char));
								word[charCounter -1] = c;
								c = fgetc(fpOriginal);
							} /* end of else */
							
						} /* end of while */

						if(charCounter != 0)
						{
							word = (char *) realloc(word, ( charCounter + 1) * sizeof(char));
							word[charCounter] = '\0';

							/*insert word to list */
							insertWord(wNodeRoot, word, lineCounter);

						} /* end of if */
						
					} /* end of if */

					if(charInLine >= MAX_LINE_LENGTH && c!= EOF)	
					{
						printf("\nWarning:\n"); 
						printf("File %s\n", fname); 
						printf("Line %d\n", lineCounter); 
						printf("Reached character limit.\nDumping excess characters\n"); 	
						while (c != '\n' && c != EOF) /* ignore all chars until the end of the line */
							c = fgetc(fpOriginal); 
					
						lineCounter += 1;
						
					} /* end of if */
					else
					{		
						if (c == ' ' || c  == '\t')
							c = fgetc(fpOriginal);
					} /* end of else */
					
					if (c != EOF && c == '\n')
					{
						c = fgetc(fpOriginal);
					
						if(charInLine < MAX_LINE_LENGTH)
							lineCounter += 1;
						charInLine = 0;

					} /* end  of if */
					
					free(word);
					word = NULL;
					word = (char *) malloc(sizeof(char));

				} /* end of while */
				
			} /* end of if */

			else /* a comment found in original file. ignoring line */
				while (c != '\n' && c != EOF)
					c = fgetc(fpOriginal);

			if (c != EOF)
				c = fgetc(fpOriginal);

		} /* end of while */
		
		printf("\nFinished %s\nTotal number of lines: %d\n", fname, (lineCounter - 1));
	
		fclose(fpOriginal);
	} /* end of else */

	free(word);

} /* end of readSource */




