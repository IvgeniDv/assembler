
#include "data.h"





/*
	head - start of label list 
	labelToInsert - self-explanatory
	address - the adress of the label- IC or DC. 0 if .extern
	ent - a flag to indicate if a label is an entry. 
	ext - a flag to indicate if a label is external. 
	cod - a flag to indicate that the label is defined in code.
	dat - a flag to indicate that the label is defined in data.
*/
void insertLabel(labelNode *head, char *labelToInsert, int address, int ent, int ext, int cod, int dat)
{
	
	labelNode *current = head;
	int  i;
	
	while (current->next_label != NULL) {
		current = current->next_label;
	} /* end of while */
	

	/* now we can add a new variable */
	current->next_label = (labelNode *) malloc(sizeof(labelNode));

	if (current->next_label == NULL) 
	{
		printf("Memory allocation for label failed in labelServices->insertLabel.\n");
	}
	else
	{
		
		current->next_label->name =  (char *) malloc(strlen(labelToInsert) * sizeof(char));

		for(i = 0; i < strlen(labelToInsert); i++)
			current->next_label->name[i] = labelToInsert[i];

		current->next_label->name[i] = '\0';
		
		current->next_label->lAddress = address;
		
		current->next_label->lBaseAdrs = address- (address % 16);
		
		current->next_label->lHist = address % 16;

		current->next_label->entry = ent;
		
		current->next_label->external = ext;

		current->next_label->code = cod;

		current->next_label->data = dat;

	} /* end  of else */
	

	current->next_label->next_label = NULL;
	
} /* end of inseretLabel */


/*
	function gets a pointer to the label list, the label name and address
	check if there is already a label with the same name and address.
	return 1 if found the same label
	return 0 if no label found with same name and address

*/
int checkSameExtern(labelNode *head, char *labelName, int address)
{

	int sameFlag = 0;
	labelNode *current = head;

	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		return sameFlag; 
	}  /* end of else */
	
	while(current != NULL)
	{
		if(strcmp(current->name, labelName) == 0)
		{
			if(current->lAddress == address)
				sameFlag = 1;

		} /* end of if */
		
		current = current->next_label;
	} /* end of while */

	
	return sameFlag;

} /* end of checkSameExtern*/


/*
	function gets a pionter to the start of the label list.
	it checks if there is a label that has been declared as an entry 
	but hasn't been define in the current file. 
	returns 0 if no such label.
	returns 1 if found such label.

*/
int checkEmptyEntry(labelNode *head)
{

	labelNode *current = head;
	int flag = 0;


	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		return flag; 
	}  /* end of else */
	

	while(current != NULL)
	{
		if(checkLblEntry(head, current->name) == 1) /* if label is declared as an entry */
		{
			if(getBaseAddress(head, current->name) == 0) /* label not defined in the file */
			{
				flag = 1;
				printf("\n ERROR - label <%s> set as entry but not defined.\n",current->name);
			} /* end of if */
				

		} /* end of if */
		
		current = current->next_label;
	} /* end of while */

	
	return flag;

} /* end of checkEmptyEntry */


/*
	function gets a pointer to start of label list.
	frees the list.
*/
void freelabels(labelNode *head)
{
	labelNode *current = head;
	labelNode *temp;

	if(current != NULL)
	{
		while (current->next_label != NULL)
		{
			temp = current->next_label;
			free(current);
			current = temp;
		} /* end of while */
	
		free(current);
	}

} /* end of freelabels */


/*
	updates an existing label.
	for values that we dont want to update we pass zeros.
	everything else will be updated to new values.
*/
void updateLabel(labelNode *head, char *label, int address, int ent, int ext, int cod, int dat)
{

	labelNode *current = head;

	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		return; 
	}  /* end of else */

	while(current != NULL)
	{
		if(strcmp(current->name, label) == 0)
		{
			if(address != 0)
			{
				current->lAddress = address;

				current->lBaseAdrs = address - (address % 16);
		
				current->lHist = address % 16;
		
			} /* end of if */

			if(ent != 0)
				current->entry = ent;
			
			if(ext != 0)
				current->external = ext;
			
			if(dat != 0)
				current->data = dat;

			if(cod != 0)
				current->code = cod;
		
			break;
		} /* end of if */

		current = current->next_label;

	} /* end of while */	

} /* end of updateLabel */


/*
	cheacks if a given lables is defined as external
	return -1 if label isnt defined . 
	0 if not defined as external.
	1 if label is external
*/
int checkLblExtrn(labelNode *head, char *label)
{
	
	labelNode *current = head;
	int externFlag = -1; /* no such label at all */
	
	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		return externFlag; 
	}  /* end of else */

	
	while(current != NULL)
	{
		if(strcmp(current->name, label) == 0)
		{
			externFlag = 0; /* label exists in list */
			if(current->external == 1)
				externFlag = 1; /* label defined as external */

			break;
		} /* end of if */
		
		current = current->next_label;

	} /* end of while */

	return externFlag;
} /* end of checkLblExtrn*/


/*
	cheacks if a given lables is defined as entry
	return -1 if label isnt defined.
	0 if not defined as entry.
	1 if label is entry
*/
int checkLblEntry(labelNode *head, char *label)
{
	
	labelNode *current = head;
	int entryFlag = -1;  /* no such label at all */

	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		return entryFlag; 
	}  /* end of else */

	while(current != NULL)
	{
		if(strcmp(current->name, label) == 0)
		{
			entryFlag = 0; /* label exists in list */
			if(current->entry == 1)
				entryFlag = 1; /* label defined as entry */

			break;
		} /* end of if */
		
		current = current->next_label;

	} /* end of while */

	return entryFlag;
} /* end of checkLblEntry*/


/*
	checks if a given label is already defined
	return 0 if label have been defined before. 
	return 1 if label is brand new.
*/
int checkLblExistence(labelNode *head, char *label)
{
	int labelFlag = 1;

	labelNode *current = head;

	while (current->next_label != NULL && labelFlag != 0)
	{
		if(strcmp(current->next_label->name, label) == 0)	
			labelFlag = 0;
	
		current = current->next_label;
	} /* end of while */

	return labelFlag;

} /* end of checkLblExistence */


/*
	gets a label name.
	returns its base address.
	we assume label exists 
	returns -1 if label doesn't exist
*/
int getBaseAddress(labelNode *head, char *label)
{
	
	labelNode *current = head;
	int baseAddress = 0; 

	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		baseAddress = -1; 
		return baseAddress;
	}  /* end of else */

	
	while(current != NULL)
	{
		if(strcmp(current->name, label) == 0)
		{
			baseAddress = current->lBaseAdrs;
			break;
		} /* end of if */
		
		current = current->next_label;

	} /* end of while */

	return baseAddress;

} /* end of getBaseAddress */

 
/*
	gets a label name.
	returns its hist address.
	we assume label exists 
*/
int getHistAddress(labelNode *head, char *label)
{
	
	labelNode *current = head;
	int histAddress = 0; 

	if(current->next_label != NULL)
		current = current->next_label;
	else 
	{
		histAddress = -1; 
		return histAddress;
	}  /* end of else */

	
	while(current != NULL)
	{
		if(strcmp(current->name, label) == 0)
		{
			histAddress = current->lHist;
			break;
		} /* end of if */
		
		current = current->next_label;

	} /* end of while */

	return histAddress;
} /* end of getHistAddress */


/*
	checks if a label has valid syntax according to page 39 in the course booklet
	returns 0 if not valid. 1 if valid
*/
int checkLabelValid(char *label)
{

	char operations[16][5] = {"mov", "cmp","add", "sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
	char guidance[4][10] = {".data", ".string", ".entry", ".extern"};
	char registers[16][5] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};



	int i;
	int labelSyntax = 1;

	if(isalpha(label[0]) == 0)
		labelSyntax = 0;
	else if(strlen(label) > MAX_LABEL_LENGTH) 
		labelSyntax = 0;
	else
	{
		
		for(i = 1; i < strlen(label); i++)
			if( ((48 <= label[i]) && (label[i] <= 57)) || ((65 <= label[i]) && (label[i] <= 90)) || ((97 <= label[i]) && (label[i] <= 122)))
				;
			else
				labelSyntax = 0;


		for(i = 0; i < OPERATIONS; i++)
			if(strcmp(label, operations[i]) == 0)
				labelSyntax = 0;

		for(i = 0; i < GUIDANCE; i++)
			if(strcmp(label, guidance[i]) == 0)
				labelSyntax = 0;

		for(i = 0; i < TOTAL_REGISTERS; i++)
			if(strcmp(label, registers[i]) == 0)
				labelSyntax = 0;
	
	} /* end of else */

	return labelSyntax;

} /*end of checkLabelValid */

/* not in use */
void printLabel(labelNode *head)
{

	printf("\nprinting labels:\n");
	while(head->next_label != NULL)
	{
		printf("->%s, base address: %d, hist address: %d, address: %d\n", head->next_label->name, head->next_label->lBaseAdrs, head->next_label->lHist, head->next_label->lAddress);
		printf("\texternal: %d, entry: %d, code: %d, data: %d\n", head->next_label->external, head->next_label->entry, head->next_label->code, head->next_label->data);
		head = head->next_label;
	}

} /* end of printLabel */


/*
	gets a pointer to the label list and the latest ic count
	updates labels that marked as data to the correct address.
*/
void updateLabelAddress(labelNode *head, int ic)
{
	int n;
	labelNode *current = head;

	while(current->next_label != NULL)
	{
		if(current->next_label->data == 1) /* label classified as data */
		{
			n = current->next_label->lAddress;
			updateLabel(head, current->next_label->name, ic + n -1 , 0, 0, 0, 0);
		} /* end of if */
	
		current = current->next_label;
	} /* end of while */

}/* end of updateLabelAddress */







