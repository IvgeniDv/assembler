
#include "data.h"




int main(int argc, char *argv[])
{

	int i;

	wordNode *wNodeRoot = NULL; /* start of word list */
	macroNode *macNodeRoot = NULL; /* start of macro list */
	labelNode *lNodeRoot = NULL; /* start of lable list */
	int **memoryImage = NULL; 

	int errorFlag = 0; /* will be turned to 1 if error is found */
	int cIS; /* total code image size */
	int dIS; /* total data image size */

	char *outPutName = NULL; /* will hold the name of the .am file */
	
	macNodeRoot = (macroNode *) malloc(sizeof(macroNode)); 
	macNodeRoot->next_macro = NULL;

	if(macNodeRoot == NULL)
		printf("\n ERROR: couldn't allocate memory for start of macro list.\n");
	else
	{
		for (i = 1; i < argc; i++)
		{
			errorFlag = 0;
			outPutName = (char *) malloc(MAX_FILE_NAME * sizeof(char));

			wNodeRoot = (wordNode *) malloc(sizeof(wordNode)); 
			wNodeRoot->next_word = NULL;

			lNodeRoot = (labelNode *) malloc(sizeof(labelNode)); 
			lNodeRoot->next_label = NULL;
	
			memoryImage = (int **) malloc(sizeof(int *));

			if(wNodeRoot == NULL)
				printf("\n ERROR: couldn't allocate memory for words list.\n");
			else if(macNodeRoot == NULL)
				printf("\n ERROR: couldn't allocate memory for macro list.\n");
			else if(lNodeRoot == NULL)
				printf("\n ERROR: couldn't allocate memory for label list.\n");
			else if(memoryImage == NULL)
				printf("\n ERROR: couldn't allocate memory for memory image.\n");
			else
			{
			 	preAss(argv[i], wNodeRoot, macNodeRoot); /* pre Assembler - macro expansion */

				/* finished macro expansion - now lets read from the output file */
				freeWords(wNodeRoot); /* free the old list of words */
				wNodeRoot = NULL;
				wNodeRoot = (wordNode *) malloc(sizeof(wordNode)); 
				wNodeRoot->next_word = NULL;

				sprintf(outPutName, "%s%s", argv[i], ".am");
				readSource(outPutName, wNodeRoot); /* read code from the .am file */

				/* first pass */
				memoryImage = parse(memoryImage, wNodeRoot, argv[i], lNodeRoot,  &cIS,  &dIS,  &errorFlag);
				if(errorFlag != 1) /* second pass */
					memoryImage = parse(memoryImage, wNodeRoot, argv[i], lNodeRoot,  &cIS,  &dIS,  &errorFlag);

				if(checkEmptyEntry(lNodeRoot) == 1) /* check if we have a label that is an entry + not defined */
					errorFlag = 1;

				if(errorFlag != 1) /* no errors found */
				{
					/* create object file */
					createObjFile(memoryImage, argv[i], cIS, dIS);
					/* create external and entry file */
					createLabelFiles(lNodeRoot, argv[i]);
			
				} /* end of if */

				freeWords(wNodeRoot);
				wNodeRoot = NULL;

				freelabels(lNodeRoot);
				lNodeRoot = NULL;

				freeImage(memoryImage, (cIS + dIS));
				memoryImage = NULL;

				free(outPutName);
				outPutName = NULL;

			}  /* end of else */
		} /* end of for */


		/* 
		freeMacros is happening outside of for loop because,
		in this method we can define a macro once, and use it in all files.
		macro definition is continuous. 
		*/
		freeMacros(macNodeRoot);
		macNodeRoot = NULL;

	} /* end of else */
	

	return 0;

} /* end of main */
