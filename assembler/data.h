#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>


#define TOTAL_MEMORY 8192
#define MAX_WORD_LENGTH 80
#define MAX_LINE_LENGTH 80
#define MAX_FILE_NAME 255
#define MIN_ASCII 33  /* minimal ascii code */
#define MAX_ASCII 126 /* maximal ascii code */
#define GUIDANCE 4
#define OPERATIONS 16
#define WORD_SIZE 20
#define REGISTER_SIZE 20
#define TOTAL_REGISTERS 16
#define MAX_LABEL_LENGTH 32 
#define OP_CODE_SIZE 16
#define FUNCT_SIZE 4
#define ADDRESSING_MODE 2
#define MAX_NUMBER 32767 /* to be represented in 16 bit twos compliment */
#define MIN_NUMBER -32767




typedef struct word_node{
	char *data; /* the actual word itself */
	int lineNum; /* line number of the word */
	struct word_node *next_word;
} wordNode;

typedef struct macro_node{
	char *data; 
	int lineNum; 
	struct macro_node *next_macro;
} macroNode;

typedef struct symbol_table{    
	char *name; /* the actual labe itself */
	int lAddress; /* the adress of the label- IC or DC. 0 if .extern*/
	int lBaseAdrs; /* (lAddress -1) - ((lAddress - 1) % 16) */
	int lHist; /* lHist = lAddress - lBaseAdrs */
	/*attributes: */
	int entry; /* o if not an entry. 1 if entry */
	int external; /* o if not external. 1 if external */  
	int code; /* o if label defined in data. 1 if label defined in code  */
	int data; /* o if label defined in code. 1 if label defined in data */ 
	struct symbol_table *next_label;
} labelNode;


/* 
------------------------
	pre-assembler:
------------------------
 
	------------------------------------------
		 wLServices.c: word list services
	------------------------------------------
*/
void insertWord(wordNode *head, char *wordToInsert, int lineNum);
void freeWords(wordNode *head);
void printWords(wordNode *head);


/* 
	------------------------------------------
		 pServices.c: pre assembler services
	------------------------------------------
*/
void macroExpansion(char *fname, macroNode *root, wordNode *head);
void readSource(char *fname, wordNode *head);


/* 
	-------------------------------------------
		 mLServices.c: macro list services
	-------------------------------------------
*/
void defineMacro(wordNode *head, macroNode *root);
int checkIfMacro(macroNode *root, char *data, int lineNum);
void insertMacro(macroNode *root, char *wordToInsert, int lineNum);
void printMacro(macroNode *head);
void freeMacros(macroNode *head);


/* 
	--------------------
		 preAss.c:
	--------------------
*/
void preAss(char *fname, wordNode *head, macroNode *root);


/* 
	----------------------
		 parse.c: 
	----------------------
*/
int ** parse(int **memoryImage, wordNode *head, char *fname, labelNode *lNodeRoot, int *cIS, int *dIS, int *errorFlag);


/* 
	----------------------
	imageServices.c: 
	----------------------
*/
int getAddressingMode(char *operand);
int getFunct(char *operation);
int getOpcode(char *operation);
int getNum(wordNode *current);
int checkIfNum(wordNode *current);
void twosCompliment(int num, int *memWord);
void convertToTwoBit(int num, int *adrsModBit);
void convertToFourBit(int num, int *fourBit);
void freeImage(int **image, int totalWordsInMem);
void printImage(int **image, int totalWordsInMem);
int** editImage (int **image, int *memWord, int address);
int** buildImage(int **image, int *memWord, int totalWordsInMem);
int** buildImage2(int **image, int *memWord, int totalWordsInMem);
int ** uniteCodeDataImage(int **codeImage, int **dataImage, int cIC, int dIC);


/* 
	----------------------
	labelServices.c: 
	----------------------
*/
int checkLabelValid(char *label);
int getHistAddress(labelNode *head, char *label);
int getBaseAddress(labelNode *head, char *label);
int checkLblExistence(labelNode *head, char *label);
int checkLblEntry(labelNode *head, char *label);
int checkLblExtrn(labelNode *head, char *label);
void updateLabel(labelNode *head, char *label, int address, int ent, int ext, int cod, int dat);
void freelabels(labelNode *head);
void insertLabel(labelNode *head, char *labelToInsert, int address, int ent, int ext, int cod, int dat);
int checkSameExtern(labelNode *head, char *labelName, int address);
void printLabel(labelNode *head);
void updateLabelAddress(labelNode *head, int ic);
int checkEmptyEntry(labelNode *head);


/* 
	------------------------------
		 parseServices.c: 
	------------------------------
*/
int checkWordType(char *current);
void resetHolder(int *holder, int size);
int checkTwoArgs(wordNode *current, int lFlag, int k);
int checkTwoArgsAdrsMod(wordNode *current, int k, int operationId);
int ** createFirstWord(int **image, int *memWord, wordNode *current, int *codeImageCount, int *ic);
int ** createSecondWord(int **image, int *memWord, int *fourBitWord, int *twoBitWord, wordNode *current, int *codeImageCount, int *ic, int *secMemWrdAdrs);
void createImmediateAddrsWord(int *memWord, wordNode *current);
wordNode * moveLine(wordNode *current, int lineFlag);
int ** createDirectAddrsWord(int *memWord, labelNode *labelRoot, wordNode *current, int **image, int *codeImageCount, int *ic);
int checkLabelDuplication(labelNode *labelRoot, char *label, int k);
int ** insertIndexAddrs(int **image, int *memWord, int *fourBitWord, int wordAddrs, wordNode *current, int argNum);
char * extractLabel(char *destination, wordNode *current);
int ** createLabelWord(int **image, labelNode *labelRoot, char *label, int *memWord, int *codeImageCount, int *ic);
int checkOneArg(wordNode *current, int lFlag, int k);
int checkOneArgAdrsMod(wordNode *current, int k, int operationId);
int ** createOneArgSecondWord(int **image, int *memWord, int *fourBitWord, int *twoBitWord, wordNode *current, int *codeImageCount, int *ic, int *secMemWrdAdrs);


/*
	------------------------------
		fileServices.c: 
	------------------------------

*/
void createLabelFiles(labelNode *head, char *fname);
void returnHex(int *hex, int **image, int address);
void createObjFile(int **image, char *fname, int cIS, int dIS);



