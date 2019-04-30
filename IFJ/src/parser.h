/******************** parser.h *********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/


#include "scanner.h"
#include "semantic.h"
#include "expres.h"

#define GLOBALSTATE 0
#define LOCALSTATE 1

TToken getNextToken();
