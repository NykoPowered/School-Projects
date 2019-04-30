/******************** expres.h *********.***********\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err_codes.h"


typedef enum
{
    LLTYPE_NIL, //to je ako null - podobne jak pascal "nil" //0

    LLTYPE_PLUS, //"+" 1
    LLTYPE_MINUS, //"-" 2
    LLTYPE_MUL, //"*"3
    LLTYPE_DIV, // "/"4
    LLTYPE_LESS, // "<"5
    LLTYPE_MORE, // ">"6
    LLTYPE_EQUAL, //"=="7
    LLTYPE_LESSEQUEL, //"<="8
    LLTYPE_MOREEQUAL, //">="9
    LLTYPE_NOTEQUAL, //"!="10


    LLTYPE_LBRACKET, // "("11
    LLTYPE_RBRACKET, // ")"12

    LLTYPE_COMMA,//13
    LLTYPE_INTEGER, // ak je to int//14
    LLTYPE_DECIMAL, //nejaky ten float//15
    LLTYPE_STRING, //ak je to string//16
    LLTYPE_ID//17

} LLType;

#define EXP_INT 0
#define EXP_DOUBLE 1
#define EXP_STRING 2
#define EXP_NIL 3
#define EXP_UNKNOWN 4


typedef struct LLitem{
		int intValue;
		char *stringValue;
		double doubleValue;
		LLType type;
    int varType;

		struct LLitem *next;
    struct LLitem *prev;
} LLitem;


typedef struct MyList{
    int size;
    int isActive;
		LLitem *first;
    LLitem *active;
} MyList;


int exprEval(LLitem **lLitem);
void freeItem(LLitem **item);
char *getUniqueVar();
int freeList(MyList **lList);
int pushBack(MyList **lList, int intValue, char *stringValue, double doubleValue, LLType switchType, int varType);
int activateEXPR(MyList *lList);
MyList *initList();
