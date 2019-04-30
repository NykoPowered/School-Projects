/* ****************** generator.h *******************\
|****************************************************|
|team: 053/1                                         |
|                                                    |
|authors: Marek Hrivňák    xhrivn02                  |
|         Nikolas Masica   xmasic00                  |
|         René Rešetár     xreset00                  |
|FIT VUT                                             |
|****************************************************|
|************************************************** */

#ifndef generator_h
#define generator_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#define MAXSTACK 1000

/* ***** Ukazka ako pracovat s struct variables  *****
 *
 *   Nastavenie premennej
 *
 *   var_struct_ptr *var_a;                      <--- nastavime ukazatel
 *   var_a = malloc(sizeof(srtuct variables));   <--- alokujeme strukturu ak ju pouzivame 1. krat
 *   var_a->name = "Meno_premennej"              <--- nastavíme meno premennej
 *   var_a->vartype = int_type;                  <--- nastavíme o aký typ premennej sa jedna
 *   var_a->frame = LF;                          <--- nastavime ramec premennej ci je Lokalna/Globalna
 *
 *   Nastavenie hodnoty
 *
 *   var_struct_ptr *value_a;                    <--- nastavime ukazatel
 *   var_a = malloc(sizeof(srtuct variables));   <--- alokujeme strukturu
 *   var_a->vartype = int_type;                  <--- nastavíme o aký typ hodnoty sa jedna
 *   var_a->frame = undefined;    ! DOLEZITE !   <--- nastavime ramec(frame) na undefined pri hodnote
 *   value_a->int_value = 5;                     <--- nastavíme hodnotu do struktury
 *
 */


typedef enum    // enum, ktory vpripade premennej prida rámec a vprípade hodnoty undefined
{
    GF,
    LF,
    TF,
    undefined,
}frame_enum;

typedef enum
{
    int_type,
    float_type,
    string_type,
    nil_type
}vartype_enum;


typedef struct variables {
    char *name; // nazov Premennej
    vartype_enum vartype;  // typ Premennej
    frame_enum frame;
    int int_value;
    float float_value;
    char *str_value;
} var_struct_ptr;

typedef enum    // enum, pre IF -> relacny operator
{
    less,       // <
    lesseq,  // <=
    more,       // >
    moreeq,  // >=
    eq,      // ==
    noteq,   // !=
    other,   // iny ako relacny operator
    plus,    // +
    minus,   // -
    times,   // *
    divide,  // /
} relational_operator;

/* pomocný zásobník */
typedef struct    {
    var_struct_ptr a[MAXSTACK];
    int top;
    int var_numbers; // pocet parametrov v zasobniku
} StackVar;

/*  gRET
 *  funkcia ktoru zavolas v pripade ze do premennej mas priradit hodnotu ktoru vracia funkcia
 *  Napr. "a = inputs" bude vyzerat:
 *  gDeclareVar(a);
 *  gCallFunction(&zasobnik, "inputs");
 *  gRET(a);
 */

void gRET(var_struct_ptr *param);

/*  Replacator
 *  funkcia ktora sa musi pouzit vzdy s priradenim stringu Replacator("string s medzerou");
 *  nahradi medzeri a konce riadkov v stringu za hodnoty potrebne v IFJcode18
 *  parameter -> string
 */

char *Replacator(char *s);

/* replace
 * pomocna funckia pre Replacator
 */

char *replace(const char *s, char ch, const char *repl);

/* ************ ZASOBNIKOVE FUNKCIE *************  */

/*
 *  Zasobnikove funkcie
 *  pouzitie: pred volanim funkcie gCallFunction -> zavolame SPushP a pushneme potrebne argumenty na zasobnik
 *
 */

// inicializacia zasobnika
void SInitP (StackVar *S);
// Pushovanie do zasobnika
// argumenty: zasobnik, struktura(premenna,hodnota)
void SPushP (StackVar *S, var_struct_ptr *ptr);
// PoP zo zasobnika -> funkcia pouzivana v gCallFunction
var_struct_ptr STopPopP (StackVar *S);
// funkcia zistuje ci je zasobnik prazdny
bool SEmptyP (StackVar *S);

/* ********* ZASOBNIKOVE FUNKCIE KONIEC ********** */

/*      funkcia -> gCallFunction <-
 *      *@POUZITIE*  funckia sa vola vzdy pri volani funckie v jazyku IFJ18
 *      *@ARGUMENTY* 1. zásobnik do ktoreho sa napushovali argumenty pred zavolanim tejto funkcie
 *                   2. názov funckie
 */
bool gCallFunction(StackVar *S, char *function_name);


/*      funkcia -> gDeclareVar <-
 *      *@POUZITIE*  funckia sa vola vzdy pri deklerovani premennej v jazyku IFJ18
 *      *@ARGUMENTY* 1. struktura(premenna) ktoru deklarujeme
 */
bool gDeclareVar(var_struct_ptr *assign);


/*      funkcia -> gAssign <-
 *      *@POUZITIE*  funckia sa vola vzdy pri definicii premennej v jazyku IFJ18
 *      *@ARGUMENTY* 1. struktura(premenna), ktoru chceme definovat
 *                   2. struktura(premenna/hodnota) ktorou chceme 1. argument definovat
 */
bool gAssign(var_struct_ptr *assign, var_struct_ptr *param1);


/*      funkcia -> gFunctionDef <-
 *      *@POUZITIE*  funckia sa vola vzdy pri definicii funkcie v jazyku IFJ18
 *      *@ARGUMENTY* 1. názov funckie (string)
 *                   2. pocet argumentov funckie
 */
bool gFunctionDef(char *function_name, int number_of_parameters);


/*      funkcia -> gEndFunctionDef <-
 *      *@POUZITIE*  funckia sa vola vzdy pri ukonceni definicii funkcie v jazyku IFJ18
 *      *@ARGUMENTY* 1. názov funckie (string)
 *                   2. struktura(premenna) ktoru funckia vrati - v pripade ze funckia nevracia hodnotu sa priradi NULL
 */
bool gEndFunctionDef(char *function_name, var_struct_ptr *param);


/*      funkcia -> gWHILE <-
 *      *@POUZITIE*  funckia sa vola vzdy pri vykonavani WHILE cyklu v jazyku IFJ18
 *      *@ARGUMENTY*    while a > b
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 *                   3. > -> relacny operator viz. enum relational_operator
 */
bool gWHILE(var_struct_ptr *param1, var_struct_ptr *param2, relational_operator RO);


/*      funkcia -> gWHILE_END <-
 *      *@POUZITIE*  funckia sa vola vzdy pri ukonceni WHILE cyklu v jazyku IFJ18
 *      *@ARGUMENTY* while a > b
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 */
void gWHILE_END(var_struct_ptr *param1, var_struct_ptr *param2);


/*      funkcia -> gIF <-
 *      *@POUZITIE*  funckia sa vola vzdy na zaciatku pri vykonavani IF podmienky v jazyku IFJ18
 *      *@ARGUMENTY*    if a > b
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 *                   3. > -> relacny operator viz. enum relational_operator
 */
bool gIF(var_struct_ptr *param1, var_struct_ptr *param2, relational_operator RO);


/*      pomocna funkcia -> gF2I <-
 *      *@POUZITIE*  pomocna funckia, ktora prevedie float na integer
 */
void gF2I(var_struct_ptr *param1, var_struct_ptr *param2, int N);


/*      funkcia -> gELSE <-
 *      *@POUZITIE*  funckia sa vola ked narazime na ELSE v jazyku IFJ18
 */
void gELSE(void);


/*      funkcia -> gIF_END <-
 *      *@POUZITIE* funckia sa vola pri ukonceni podmienky IF-ELSE nakonci v jazyku IFJ18
 */
void gIF_END(void);


/*      funkcia -> gMainStart <-
 *      *@POUZITIE* funckia sa vola pri zacati jazyka IFJ18
 */
bool gMainStart();


/*      funkcia -> gMainEnd <-
 *      *@POUZITIE* funckia sa vola pri ukonceni jazyka IFJ18
 */
bool gMainEnd();


/*      pomocna funkcia -> gParam <-
 *      *@POUZITIE*  funckia sa vola pri vypise parametra
 *      *@ARGUMENTY* 1. struktura(premenna) ktoru vypisujeme
 */
bool gParam(var_struct_ptr param);


/*      funkcia -> gAddFunction <-
 *      *@POUZITIE*  funckia sa vola vzdy pri scitani v jazyku IFJ18
 *      *@ARGUMENTY*    a = b + c
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 *                   3. c -> struktura(premenna/hodnota)
 */
bool gADDFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2);


/*      funkcia -> gSUBFunction <-
 *      *@POUZITIE*  funckia sa vola vzdy pri odcitani v jazyku IFJ18
 *      *@ARGUMENTY*    a = b - c
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 *                   3. c -> struktura(premenna/hodnota)
 */
bool gSUBFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2);


/*      funkcia -> gMULFunction <-
 *      *@POUZITIE*  funckia sa vola vzdy pri nasobeni v jazyku IFJ18
 *      *@ARGUMENTY*    a = b * c
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 *                   3. c -> struktura(premenna/hodnota)
 */
bool gMULFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2);


/*      funkcia -> gDIVFunction <-
 *      *@POUZITIE*  funckia sa vola vzdy pri deleni v jazyku IFJ18
 *      *@ARGUMENTY*    a = b / c
 *                   1. a -> struktura(premenna/hodnota)
 *                   2. b -> struktura(premenna/hodnota)
 *                   3. c -> struktura(premenna/hodnota)
 */
bool gDIVFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2);




// funkcia sa vola automaticky pri zacati
void gFileHeader(void);

/* *************** BUILD-IN FUNKCIE ****************** */
// tieto funkcie sa volaju automaticky pri zacati
void gBuildInFunctions(void);

void gINPUTS(void);

void gINPUTI(void);

void gINPUTF(void);

void gPRINT(void);

void gORD(void);

void gLENGTH(void);

void gCHR(void);

void gSUBSTR(void);

/* *************** BUILD-IN FUNKCIE ****************** */

#endif /* generator_h */

