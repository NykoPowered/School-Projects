/******************* my_string.c *******************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/



#include "my_string.h"
#include "err_codes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define SMALL_A 'a'
#define SMALL_Z 'z'

#define DEBUGS 0


char *tokenToString(TToken token){

  if(token.state == TK_DEF)
    return "TK_DEF\n";
 else if(token.state == TK_COMMA)
    return "TK_COMMA\n";
  else if(token.state == TK_DO)
    return "TK_DO\n";
  else if(token.state == TK_ELSE)
    return "TK_ELSE\n";
  else if(token.state == TK_END)
    return "TK_END\n";
  else if(token.state == TK_NIL)
    return "TK_NIL\n";
  else if(token.state == TK_IF)
    return "TK_IF\n";
  else if(token.state == TK_NOT)
    return "TK_NOT\n";
  else if(token.state == TK_WHILE)
    return "TK_WHILE\n";
  else if(token.state == TK_THEN)
    return "TK_THEN\n";
  else if(token.state == TK_PLUS)
    return "TK_PLUS\n";
  else if(token.state == TK_MINUS)
    return "TK_MINUS\n";
  else if(token.state == TK_MUL)
    return "TK_MUL\n";
  else if(token.state == TK_DIV)
    return "TK_DIV\n";
  else if(token.state == TK_LESS)
    return "TK_LESS\n";
  else if(token.state == TK_MORE)
    return "TK_MORE\n";
  else if(token.state == TK_EQUAL)
    return "TK_EQUAL\n";
  else if(token.state == TK_LESSEQUEL)
    return "TK_LESSEQUEL\n";
  else if(token.state == TK_MOREEQUAL)
    return "TK_MOREEQUAL\n";
  else if(token.state == TK_ASSIGN)
    return "TK_ASSIGN\n";
  else if(token.state == TK_NOTEQUAL)
    return "TK_NOTEQUAL\n";
  else if(token.state == TK_LBRACKET)
    return "TK_LBRACKET\n";
  else if(token.state == TK_RBRACKET)
    return "TK_RBRACKET\n";
  else if(token.state == TK_EOL)
    return "TK_EOL\n";
  else if(token.state == TK_EOF)
    return "TK_EOF\n";
  else if(token.state == TK_INTEGER)
    return "TK_INTEGER\n";
  else if(token.state == TK_DECIMAL)
    return "TK_DECIMAL\n";
  else if(token.state == TK_STRING)
    return "TK_STRING\n";
  else if(token.state == TK_ID)
    return "TK_ID\n";
  else
    return "TK_ERR\n";

}

void logDebugRealloc(){
  if(DEBUGS == 0)
    return;
  printf("need to realloc\n");
}

//funkcia inicializuje string
int initString(TString *str){
  str->myString = malloc((INITIAL_STR_SIZE+1)*sizeof(char));
  if(str->myString == NULL){
    return INTERN_ERR;
  }
  str->Capacity = INITIAL_STR_SIZE;
  str->Len = 0;
  str->myString[0] = '\0';

  return STR_OK;
}

//uvolnenie pamati
void freeString(TString *str){
  free(str->myString);
}

//vycisti string - aby ho nebolo treba stale uvolnovat a initovat - urychli to
void cleanString(TString *str){
  str->Len = 0;
  str->myString[0] = '\0';
}

//vlozi charakter na koniec retazca
int pushBackString(TString *str,char newChar){
  if(str->Capacity > str->Len + 1){
    str->myString[str->Len] = newChar;
    str->myString[str->Len+1] = '\0';
    str->Len++;
    return STR_OK;
  }

  logDebugRealloc();
  str->myString = realloc(str->myString,sizeof(char)*(str->Capacity+SIZE_OF_REALLOC));
  if(str->myString == NULL){
    return INTERN_ERR;
  }
  str->myString[str->Len] = newChar;
  str->myString[str->Len+1] = '\0';
  str->Capacity+=SIZE_OF_REALLOC;
  str->Len++;
  return STR_OK;

}
//vymaze charakter z konca retazca -  nic neuvolnuje
void popBackString(TString *str){
  if(str->Len > 0){
    str->myString[str->Len - 1] = '\0';
    str->Len--;
  }
}

//vracia 1 ak je vstupny znak maly inak vracia 0
int isLowerCase(char characterInput){
  if(characterInput >= SMALL_A && characterInput <= SMALL_Z){
    return 1;
  }
  else{
    return 0;
  }

}

//nastavý hodnotu stringu na hodnotu ktora je v parametry
void setStringEOL(TString *str){
  str->myString[0] = '\n';
  str->myString[1] = '\0';
}

//vrati dlzku retazca
int strLength(TString *str){
  return str->Len;
}

//funkcia prijima ako parameter string obsahujuci hexadecimalne cislo
//v pripade uspechu vrati prevedeny znak
//inak vrati FATAL
//Tu by sa vsak nemala nikdy dostat
char hexToChar(char *hexString){
  unsigned char result;
  if(sscanf(hexString,"%hhx",&result)){
    return result;
  }
  else{
    return FATAL;
  }

}

/*
//Z oficialnej dokumentacie strcmp:
//if Return value < 0 then it indicates str1 is less than str2
//if Return value > 0 then it indicates str2 is less than str1
//if Return value = 0 then it indicates str1 is equal to str2
int strCmpSCANNER(TString *str1,TString *str2){
  return strcmp(str1->myString,str2->myString);
}

//otoci string
void reverseString(TString *str)
{
  int i, j;
  for (i = 0, j = strlen(str->myString)-1; i<j; i++, j--) {
      char c = str->myString[i];
      s[i] = str->myString[j];
      str->myString[j] = c;
  }
}

//prevedie integer na string
//ak v stringu nie je dost miesta tak vrati minus jedna a nastava chyba programu
//inak vrati STR_OK
int intToString(int n, TString *str)
{
  int i = 0;
  do {
      if(i == str->Capacity - 1){
        return -1;
      }
      str->myString[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  str->myString[i] = '\0';
  reverseString(str->myString);
  str->Len = i-1;

  return STR_OK;
}*/

/*
TString *intToStrSCANNER(int integer){

  TString *str;
  initString(str);

  if(intToString(INT_MAX,str) == STR_OK){
    return str;
  }


}*/





//konvertuje string na double ktory vrati
//pri neuspechu vrati -1
//inak konvertovanu hodnotu
double strToDoubleSCANNER(TString *str){
  char *szOrbits = str->myString;
  char* pEnd;
  double f1 = (double) strtof (szOrbits, &pEnd);
  if(f1 < 0.00000001){
    return -1;
  }
  else{
    return f1;
  }
}

//Pozor - funkciu nikdy nevolat ak cakate ze cislo bude 0!!
//program je postaveny tak ze tato funkcia prevadza len na nezaporne cele cisla
//inak by to mohlo viesť k nespecifikovanému chovaniu
//robim to preto aby sme si ujasnili naco tato funkcia je, co je jej logika a aby sme nevniesli nejake chyby
//pouzivat u scannera
int strToIntSCANNER(TString *str){


  int result = atoi(str->myString);
  if(result <= 0){
    //do toho stavu by sa nemala funkcia nikdy dostať!!!
    fprintf(stderr, "Pozor, pozor!!! Zavazna chyba vo funkcii strToIntSCANNER\n");
    return FATAL;
  }

  //ak je vysledok oki
  return result;
}

//prekopiruje string z typu TString do stringToReturn
//vrati vysledok ak je to uspesne
//pri neuspechu vrati NULL
char *getString(TString *str){
  char *stringToReturn = malloc((str->Len+1)*sizeof(char));
  if(stringToReturn == NULL){
    return NULL;
  }

  for(int i=0;i<str->Len;i++){
    stringToReturn[i] = str->myString[i];
  }
  stringToReturn[str->Len] = '\0';
  return stringToReturn;
}

//vezme char a urobi z neho string ktory vracia ako ukazatel
//vrati nenulovu hodnotu ak je to uspesne
//pri neuspechu vrati nulu
int charToString(char characterInput,char *stringToReturn){
  stringToReturn = malloc(2*sizeof(char));
  if(stringToReturn == NULL){
    return 0;
  }
  stringToReturn[0] = characterInput;
  stringToReturn[1] = '\0';
  return 1;
}
