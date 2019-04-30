/******************** scanner.h ********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/



#define NUM_OF_KEYWORDS 9
#define NUM_OF_OPERATORS 11
#define NUM_OF_ONECHAROPERATORS 8
#include "err_codes.h"
#include "my_string.h"

#define NUM_OF_DELIMINERS 14

#define ZERO 0
#define FALSE 0
#define TRUE 1

int isOperator(const char *string);
int isOneCharOperator(const char c);
int isKeyword(const char *string);
TToken getToken(TString *str);
char *tokenToString(TToken token);
TToken getNextTokenSCANNER();
