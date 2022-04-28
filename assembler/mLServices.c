/* macro list services */

#include "data.h"



/* 
	function gets the root of the macro list, the word we want to insert to the list , and the line number.
	
*/
void insertMacro(macroNode *root, char *wordToInsert, int lineNum)
{
	
	macroNode *currentMacro = root;
	int i = 0;

	while (currentMacro->next_macro != NULL) {
		currentMacro = currentMacro->next_macro;
	} /* end of while */
	

	/* now we can add a new macro to the list */
	currentMacro->next_macro = (macroNode *) malloc(sizeof(macroNode));

	if (currentMacro->next_macro == NULL) 
	{
		printf("Memory allocation for macro failed in mLServices->insertMacro.\n");
	} 
	else
	{
		currentMacro->next_macro->data =  (char *) malloc(strlen(wordToInsert) * sizeof(char));

		while (wordToInsert[i] != '\0')
		{
			currentMacro->next_macro->data[i] = wordToInsert[i];
			i++;
		}/* end of while */
		
		currentMacro->next_macro->data[i] = '\0';

		currentMacro->next_macro->lineNum = lineNum;

	} /* end  of else */

	currentMacro->next_macro->next_macro = NULL;

} /* end of insertMacro */


/*
	functions runs on the list of words. 
	If encounters a macro definition - add to a list of macros.
*/
void defineMacro(wordNode *head, macroNode *root)
{
	int i;
	wordNode *currentWord = head;
	macroNode *currentMacro = root;
	char macro[] = "macro";
	char endm[] = "endm";

	
	char guidance[4][10] = {".data", ".string", ".entry", ".extern"};
	

	char operations[16][5] = {"mov", "cmp","add", "sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"}; 

	while (currentWord->next_word != NULL)
	{
		if(strcmp(currentWord->next_word->data, macro) == 0)
		{

			for (i = 0; i < OPERATIONS; i++)
				if(strcmp(currentWord->next_word->next_word->data, operations[i]) == 0)
					printf("\n Warning!\tmacro name is using a reserved system operation - %s\n", operations[i]);
			
			for (i = 0; i < GUIDANCE; i++)
				if(strcmp(currentWord->next_word->next_word->data, guidance[i]) == 0)
					printf("\n Warning!\tmacro name is using a reserved system guidance - %s\n", guidance[i]);

			while(strcmp(currentWord->next_word->data, endm) != 0)
			{
				insertMacro(currentMacro, currentWord->next_word->data, currentWord->next_word->lineNum);
				currentWord = currentWord->next_word;
			} /* end of while */			
			
			insertMacro(currentMacro, currentWord->next_word->data, currentWord->next_word->lineNum); /* insertion of endm */

		} /* end of if */
		
		currentWord = currentWord->next_word;

	} /* end  of while */

} /* end o f define macro */


/* 
	checks if a given word is a macro name 
	0 = not a macro; 1 = it is a mcaro name 
*/
int checkIfMacro(macroNode *root, char *data, int lineNum)
{
	
	int macroFlag = 0; 
	macroNode *currentMacro = root;
	char macro[] = "macro";

	while (currentMacro->next_macro != NULL)
	{
		if (strcmp(currentMacro->next_macro->data, macro) == 0)
		{
			if(currentMacro->next_macro->next_macro != NULL)
			{
				if(strcmp(currentMacro->next_macro->next_macro->data, data) == 0 && lineNum > currentMacro->next_macro->next_macro->lineNum)
				{
					macroFlag = 1;
					return macroFlag;
				} /* end of if */

			} /* end of if */
				
		} /* end of if */

		currentMacro = currentMacro->next_macro;
	} /* end of while */
		
	return macroFlag;
} /* end of check if macro */


/* not in use */
void printMacro(macroNode *head)
{
	printf("printing macros:\n");
	while(head->next_macro != NULL)
	{
		printf("->%s\n", head->next_macro->data);
		head = head->next_macro;
	}
	
} /* end of printMacro */


/*
	function gets a pointer to start of macro list.
	frees the list.
*/
void freeMacros(macroNode *head)
{
	macroNode *current = head;
	macroNode *temp;

	if(current != NULL)
	{
		while (current->next_macro != NULL)
		{
			temp = current->next_macro;
			free(current);
			current = temp;
		} /* end of while */

		free(current);

	} /* end of if */

} /* end of freeWMacro */




