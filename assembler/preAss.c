/* pre assembler */

#include "data.h"



void preAss(char *fname, wordNode *head, macroNode *root)
{
	char *originalName = NULL;
	char *outPutName = NULL;

	originalName = (char *) malloc(MAX_FILE_NAME * sizeof(char));
	outPutName = (char *) malloc(MAX_FILE_NAME * sizeof(char));

	if(originalName == NULL)
		printf("\n ERROR: couldn't allocate memory for originalName in preAss->preAss.\n");
	else if(outPutName == NULL)
		printf("\n ERROR: couldn't allocate memory for outPutName in preAss->preAss.\n");
	else
	{
		sprintf(originalName, "%s%s", fname, ".as");
		readSource(originalName, head);
		defineMacro(head, root);
	
		sprintf(outPutName, "%s%s", fname, ".am");
		macroExpansion(outPutName, root, head);
	
		free(originalName);
		free(outPutName);

		originalName = NULL;
		outPutName = NULL;
	} /* end of else */

} /* end of preAss */

