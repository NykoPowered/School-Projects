/******************* my_token.h ********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include <stdint.h>
#include <string.h>

typedef enum
{
    //keywords
    TK_DEF, //definicia funkcie "def"
    TK_DO, //ako ked mate do while atd oka? "do"
    TK_ELSE, //if else you know "else"
    TK_END, //neviem co to je ale bude to "end" - je to v zadani pri keywords
    TK_NIL, //to je ako null - podobne jak pascal "nil"
    TK_IF, //klasika "if"
    TK_NOT, //not
    TK_WHILE, //klasika "while"
    TK_THEN,  // "then"

    //operators
    TK_PLUS, //"+"
    TK_MINUS, //"-"
    TK_MUL, //"*"
    TK_DIV, // "/"
    TK_LESS, // "<"
    TK_MORE, // ">"
    TK_EQUAL, //"=="
    TK_LESSEQUEL, //"<="
    TK_MOREEQUAL, //">="
    TK_ASSIGN, //"="
    TK_NOTEQUAL, //"!"


    TK_COMMA,	 // ","
    TK_LBRACKET, // "("
    TK_RBRACKET, // ")"
    TK_EOL, // EOL
    TK_EOF, //"EOF" - end of file


    TK_INTEGER, // ak je to int
    TK_DECIMAL, //nejaky ten float
    TK_STRING, //ak je to string


    TK_ID, //to by malo byt nejaky identifikator
    TK_ERR

} TTokenState;
//to su vsetky stavy tokenu



//tu pridavam este aj hodnotu double, int alebo string
//este jedna vec, ked je TK_ERR nastavene tak potom je chybova hodnota v intValue
typedef struct
{
  TTokenState state;
  union {

      char* string;
      //to je lahke prehliadnut takze POZOR - cislo chyby sa vracia cez intValue!!!!
			int intValue;  //netreba zabudat ze cislo chyby sa vracia odtial!!!!!!!!!
			double doubleValue;

		};
} TToken;
//vytvorime to v scannery a potom vratime pre parser
