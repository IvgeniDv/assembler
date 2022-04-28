/* word list services */

#include "data.h"


/*
	function gets a pointer to a word list.
	function frees the list.
*/
void freeWords(wordNode *head)
{
	wordNode *current = head;
	wordNode *temp;
	
	if(current != NULL)
	{
		while (current->next_word != NULL)
		{
			temp = current->next_word;
			free(current);
			current = temp;
		} /* end of while */
	
		free(current);

	} /* end of if */

} /* end of freeWords */


/*
	function gets a pointer to the start of a word list, the wotd itself and the line number.
	function inserts the given word to the end of the list.
*/
void insertWord(wordNode *head, char *wordToInsert, int lineNum) {
	
	wordNode *current = head;
	int  i = 0;

	if(head != NULL)
	{
		while (current->next_word != NULL) 
		{
			current = current->next_word;
		} /* end of while */
	

		/* now we can add a new word */
		current->next_word = (wordNode *) malloc(sizeof(wordNode));

		if (current->next_word == NULL) 
		{
			printf("Memory allocation for word failed in wLServices->insertWord.\n");
		}
		else
		{
			current->next_word->data =  (char *) malloc(strlen(wordToInsert) * sizeof(char));

			while (wordToInsert[i] != '\0')
			{
				current->next_word->data[i] = wordToInsert[i];
				i++;
			}/* end of while */
		
			current->next_word->data[i] = '\0';

			current->next_word->lineNum = lineNum;

		} /* end  of else */

		current->next_word->next_word = NULL;
	} /* end of if */
	
} /* end of inseretWord */


/* printing the list of the words - not in use */
void printWords(wordNode *head)
{
	printf("printing words:\n");

	if(head != NULL)
	{
		while(head->next_word != NULL)
		{
			printf("->%s\n", head->next_word->data);
			head = head->next_word;
		}
	} /* end of if */

} /* end of printWords */





