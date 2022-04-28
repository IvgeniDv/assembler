
#include "data.h"






void createObjFile(int **image, char *fname, int cIS, int dIS)
{
	int i;
	FILE *objFile = NULL;
	char *objName = NULL;
	char *line = NULL;
	int hex[5];

	objName = (char *) malloc(MAX_FILE_NAME * sizeof(char));
	line = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));

	if(objName == NULL)
		printf("\n ERROR: couldn't allocate memory for objName.\n");
	else if(line == NULL)
		printf("\n ERROR: couldn't allocate memory for line.\n");
	else
	{
		line[0] = '\0';

		sprintf(objName, "%s%s", fname, ".ob");
		objFile = fopen(objName, "w");

		if(objFile == NULL) 
			printf("\n File %s can't be created.\n", objName); 
		else
		{
			/* Headline */
			sprintf(line, "%s%d ", line, cIS);
			sprintf(line, "%s%d", line, dIS);
			fputs(line, objFile);
			fputc('\n', objFile);

			for(i = 0; i < (cIS + dIS); i++)
			{
				returnHex(hex, image, i);

				line[0] = '\0'; /* reset the string */

				/* insert the memory address */
				sprintf(line, "%s%04d ", line, (i + 100));
		
				sprintf(line, "%s%s", line, "A");
				sprintf(line, "%s%c", line, hex[0]);
				sprintf(line, "%s%s", line, "-");
				sprintf(line, "%s%s", line, "B");
				sprintf(line, "%s%c", line, hex[1]);
				sprintf(line, "%s%s", line, "-");
				sprintf(line, "%s%s", line, "C");
				sprintf(line, "%s%c", line, hex[2]);
				sprintf(line, "%s%s", line, "-");
				sprintf(line, "%s%s", line, "D");
				sprintf(line, "%s%c", line, hex[3]);
				sprintf(line, "%s%s", line, "-");
				sprintf(line, "%s%s", line, "E");
				sprintf(line, "%s%c", line, hex[4]);

				fputs(line, objFile);
				fputc('\n', objFile);

			} /* end of for */
	
			fclose(objFile);
			free(line);
			free(objName);
		} /* end of else */

	} /* end of else */
	
}/* end of createObjFile */


void returnHex(int *hex, int **image, int address)
{
	int i,j,k;
	int flag;
	int word1[4];
	int word2[4];
	int word3[4];
	int word4[4];
	int word5[4];
	
	int binary[16][4] = {
							{0,0,0,0},
							{0,0,0,1},
							{0,0,1,0},
							{0,0,1,1},
							{0,1,0,0},
							{0,1,0,1},
							{0,1,1,0},
							{0,1,1,1},
							{1,0,0,0},
							{1,0,0,1},
							{1,0,1,0},
							{1,0,1,1},
							{1,1,0,0},
							{1,1,0,1},
							{1,1,1,0},
							{1,1,1,1},
						};
	char hexadec[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

	/* break the word into 5 arrays */
	for(k = 0; k < 4; k++)
	{			
		word1[k] = image[address][k];
		word2[k] = image[address][k + 4];
		word3[k] = image[address][k + 8];
		word4[k] = image[address][k + 12];
		word5[k] = image[address][k + 16];

	} /* end of for */

	/* 1st word */
	for(i = 0; i < 16; i++) 
	{
		flag = 0;

		for(j = 0; j < 4; j++)
		{
			if(word1[j] != binary[i][j])
				flag = 1;

		} /* end of for */

		if (flag == 0)
		{
			hex[0] = hexadec[i];

		} /* end of if */
	} /* end of for */
	
	/* 2nd word */
	for(i = 0; i < 16; i++) 
	{
		flag = 0;

		for(j = 0; j < 4; j++)
		{
			if(word2[j] != binary[i][j])
				flag = 1;

		} /* end of for */

		if (flag == 0)
		{
			hex[1] = hexadec[i];

		} /* end of if */
	} /* end of for */

	/* 3nd word */
	for(i = 0; i < 16; i++) 
	{
		flag = 0;

		for(j = 0; j < 4; j++)
		{
			if(word3[j] != binary[i][j])
				flag = 1;

		} /* end of for */

		if (flag == 0)
		{
			hex[2] = hexadec[i];

		} /* end of if */
	} /* end of for */

	/* 4th word */
	for(i = 0; i < 16; i++) 
	{
		flag = 0;

		for(j = 0; j < 4; j++)
		{
			if(word4[j] != binary[i][j])
				flag = 1;

		} /* end of for */

		if (flag == 0)
		{
			hex[3] = hexadec[i];

		} /* end of if */
	} /* end of for */

	/* 5th word */
	for(i = 0; i < 16; i++) 
	{
		flag = 0;

		for(j = 0; j < 4; j++)
		{
			if(word5[j] != binary[i][j])
				flag = 1;

		} /* end of for */

		if (flag == 0)
		{
			hex[4] = hexadec[i];

		} /* end of if */
	} /* end of for */



} /* end of returnHex */


void createLabelFiles(labelNode *head, char *fname)
{
	
	labelNode *current = head;

	FILE *entrFile = NULL;
	FILE *externFile = NULL;

	char *entryName = NULL; /* will hold the name of the .ent file */
	char *externalName = NULL; /* will hold the name of the .ext file */
	char *line = NULL;	

	int entFlag = 0;
	int extFlag = 0;

	entryName = (char *) malloc(MAX_FILE_NAME * sizeof(char));
	externalName = (char *) malloc(MAX_FILE_NAME * sizeof(char));
	line = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
	
	if(entryName == NULL)
		printf("\n ERROR: couldn't allocate memory for entryName.\n");
	else if(externalName == NULL)
		printf("\n ERROR: couldn't allocate memory for externalName.\n");
	else if(line == NULL)
		printf("\n ERROR: couldn't allocate memory for line.\n");
	else
	{
		sprintf(entryName, "%s%s", fname, ".ent");
		sprintf(externalName, "%s%s", fname, ".ext");

		if(current != NULL)
		{
			while(current->next_label != NULL)
			{
				if(current->next_label->entry == 1)	
					entFlag = 1;
				if(current->next_label->external == 1)
					extFlag = 1;

				current = current->next_label;
			} /* end of while */
		} /* end of if */
	
	
		if(entFlag == 1)
		{
			current = head;
			entrFile = fopen(entryName, "w");

			if(entrFile == NULL)
				printf("\n File %s can't be created.\n", entryName); 
			else
			{
				while(current->next_label != NULL)
				{
					if(current->next_label->entry == 1)	
					{
				
						line[0] = '\0';
			
						sprintf(line, "%s%s, ", line, current->next_label->name);
						sprintf(line, "%s%04d, ", line, current->next_label->lBaseAdrs);
						sprintf(line, "%s%04d", line, current->next_label->lHist);
			
						fputs(line, entrFile);
						fputc('\n', entrFile);
		
					} /* end of if */
					current = current->next_label;
				} /* end of while */

				fclose(entrFile);
	
			} /* end of else */
			
		} /* end of if */

		if(extFlag == 1)
		{
			current = head;
			externFile = fopen(externalName, "w");

			if(entrFile == NULL)
				printf("\n File %s can't be created.\n", externalName); 
			else
			{
				while(current->next_label != NULL)
				{
					if(current->next_label->external == 1 && current->next_label->lAddress == 0)	
					{
						line[0] = '\0';

						sprintf(line, "%s%s ", line, current->next_label->name);
						sprintf(line, "%s%s ", line, "BASE");
						sprintf(line, "%s%04d ", line, current->next_label->lAddress);
				
						fputs(line, externFile);
						fputc('\n', externFile);

						line[0] = '\0';
			
						sprintf(line, "%s%s ", line, current->next_label->name);
						sprintf(line, "%s%s ", line, "OFFSET");
						sprintf(line, "%s%04d", line, (current->next_label->lAddress + 1));
				
						fputs(line, externFile);
						fputc('\n', externFile);
						fputc('\n', externFile);
				
					} /* end of if */
					current = current->next_label;
				} /* end of while */
	
				fclose(externFile);

			} /* end of else */
		
		} /* end of if */

		free(line);
		line = NULL;
		free(entryName);
		entryName = NULL;
		free(externalName);
		externalName = NULL;
	} /* end of else */

} /* end of createLabelFiles*/
