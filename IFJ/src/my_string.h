/******************* my_string.h *******************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include "my_token.h"

typedef struct
{

  int Capacity;
  int Len;
  char *myString;
} TString;

#define INITIAL_STR_SIZE 15
#define SIZE_OF_REALLOC 10
#define STR_OK 0
#define FATAL -1
#define CONVERSION_ERROR -2


int initString(TString *str);
void freeString(TString *str);
void cleanString(TString *str);
int pushBackString(TString *str,char newChar);
void popBackString(TString *str);
int isLowerCase(char characterInput);
int strLength(TString *str);
char hexToChar(char *hexString);
double strToDoubleSCANNER(TString *str);
int strToIntSCANNER(TString *str);
char *getString(TString *str);
int charToString(char characterInput,char *stringToReturn);
void setStringEOL(TString *str);
char *tokenToString(TToken token);
