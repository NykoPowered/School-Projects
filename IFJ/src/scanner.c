/******************** scanner.c ********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "err_codes.h"
#include "scanner.h"
//#include "my_token.h" - chyba v dependencies
//#include "my_string.h"

#define DEBUG 0
FILE *inputFile;
int inString = 0;
int inComment = 0;

typedef enum
{
    ST_START,
    ST_OPERATOR,
    ST_STRING,
    /*ST_ESCAPE,*/
    ST_NUMBER,
		ST_BACKSLASH,
    ST_KEYWORD_ID,
    /*ST_ID,*/
		ST_COMMENT_SHARP,
		ST_COMMENT_EQBEGIN,
		ST_COMMENT_EQEND,
		ST_EQUALS,
		ST_DECIMAL,
		ST_DECIMAL_EXP,
		ST_FUNC_ID,
		ST_NEW_LINE,
		ST_ZERO
} TPossibleState;

//funkcia loguje len v pripade ze debugujeme
//ze DEBGUG je nastaveny na nenulove cislo
void logDebug(char characterInput,TPossibleState State,TString *str){
  if(DEBUG == 0)
    return;
	if(State == ST_START){
		printf("State:ST_START, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_OPERATOR){
		printf("State:ST_OPERATOR, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_STRING){
		printf("State:ST_STRING, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_NUMBER){
		printf("State:ST_NUMBER, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_BACKSLASH){
		printf("State:ST_BACKSLASH, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_KEYWORD_ID){
		printf("State:ST_KEYWORD_ID, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_COMMENT_SHARP){
		printf("State:ST_COMMENT_SHARP, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_COMMENT_EQBEGIN){
		printf("State:ST_COMMENT_EQBEGIN, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_COMMENT_EQEND){
		printf("State:ST_COMMENT_EQEND, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_EQUALS){
		printf("State:ST_EQUALS, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_DECIMAL){
		printf("State:ST_DECIMAL, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_DECIMAL_EXP){
		printf("State:ST_DECIMAL_EXP, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_FUNC_ID){
		printf("State:ST_FUNC_ID, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_NEW_LINE){
		printf("State:ST_NEW_LINE, Char: %c, String: %s\n",characterInput,str->myString);
	}
	else if(State == ST_ZERO){
		printf("State:ST_ZERO, Char: %c, String: %s\n",characterInput,str->myString);
	}
}

static char deliminers[NUM_OF_DELIMINERS] = {
	' ',
	'\t',
	'*',
	'/',
	'+',
	'-',
	'=',
	'<',
	'>',
	'!',
	'(',
	')',
  ',',
	'\n'
};

static char oneCharOperators[NUM_OF_ONECHAROPERATORS] = {
	'=',
  '*',
  '/',
	'+',
	'-',
	'>',
	'<',
	'!'
};


int isHexadec(char characterInput){
  if((characterInput >= 'A' && characterInput <= 'F') ||
     (characterInput >= 'a' && characterInput <= 'f')){
    return 1;
  }
  return 0;
}

//funkcia rozhodne ci sa jedna o operator
//a o ktory operator sa jedna
//ak sa nejedna o operator vrati chybu TK_ERR
TTokenState findOperator(const char *strInput){

	//opat treba davat pozor aby niekto nezmenil (nepridal/neodobral oprator)
	//a pardon za magicke cislo :( inak som to nevedel
	if(NUM_OF_OPERATORS != 11){
		//toto by nikdy nemalo nastat
		fprintf(stderr, "Fatalna chyba - asi si zmenil pocet operatorov!!!\n");
	}

	if(!strcmp("*",strInput)){
		return TK_MUL;
	}
	else if(!strcmp("/",strInput)){
		return TK_DIV;
	}
	else if(!strcmp("=",strInput)){
		return TK_ASSIGN;
	}
	else if(!strcmp("==",strInput)){
		return TK_EQUAL;
	}
	else if(!strcmp("+",strInput)){
		return TK_PLUS;
	}
	else if(!strcmp("-",strInput)){
		return TK_MINUS;
	}
	else if(!strcmp("<",strInput)){
		return TK_LESS;
	}
	else if(!strcmp("<=",strInput)){
		return TK_LESSEQUEL;
	}
	else if(!strcmp(">",strInput)){
		return TK_MORE;
	}
	else if(!strcmp(">=",strInput)){
		return TK_MOREEQUAL;
	}
	else if(!strcmp("!=",strInput)){
		return TK_NOTEQUAL;
    }
	else{
		return TK_ERR;
	}
}

//funguje podobne ako funkcia vyssie
//zisti ci sa jedna o klucove slovo a ak ano tak vrati prislusny tokenstate
//inak vrati TK_ERR
TTokenState findKeyWord(const char *strInput)
{
	if(!strcmp("def",strInput)){
		return TK_DEF;
	}
	else if(!strcmp("do",strInput)){
		return TK_DO;
	}
	else if(!strcmp("else",strInput)){
		return TK_ELSE;
	}
	else if(!strcmp("end",strInput)){
		return TK_END;
	}
	else if(!strcmp("if",strInput)){
		return TK_IF;
	}
	else if(!strcmp("not",strInput)){
		return TK_NOT;
	}
	else if(!strcmp("nil",strInput)){
		return TK_NIL;
	}
	else if(!strcmp("then",strInput)){
		return TK_THEN;
	}
	else if(!strcmp("while",strInput)){
		return TK_WHILE;
	}
	else{
		return TK_ERR;
	}
}

//funckia rozhodne ci sa jedna o znak z operatora
int isOneCharOperator(const char characterInput)
{
	for(int i = 0; i < NUM_OF_ONECHAROPERATORS; i++){
		if (oneCharOperators[i] == characterInput)
			return TRUE;
	}
	return FALSE;
}

//funkcia rozhoduje ci sa jedna o oddelovac - deliminer (ale stringovy)
//vrati 0 ak sa nejedna inak nieco nenulove
int isDeliminer(const char characterInput){
	for(int i=0;i<NUM_OF_DELIMINERS;i++){
		if(deliminers[i] == characterInput){
			return TRUE;
		}
	}
	return FALSE;
}

//funkcia vrati TRUE ak znak je pismeno angl. abecedy
//inak FALSE
int isAlpha(char characterInput){
	if((characterInput >= 'A' && characterInput <= 'Z') || (characterInput >= 'a' && characterInput <= 'z')){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

//funkcia vrati TRUE ak charakter moze byt pismenkom v tele identifikatoru
//inak vrati FALSE
int isIDChar(char characterInput){
	if(isAlpha(characterInput) || isdigit(characterInput) || characterInput == '_'){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

//vrati TRUE ak string s pridanym znakom moze byt zaciatok komentaru
int canBeComment(TString *str, char characterInput){
	if((str->Len == 1 && characterInput == 'b') ||
		(str->Len == 2 && characterInput == 'e') ||
		(str->Len == 3 && characterInput == 'g') ||
		(str->Len == 4 && characterInput == 'i') ||
		(str->Len == 5 && characterInput == 'n')){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

//vrati TRUE ak string s pridanym znakom moze byt konca komentaru
int canBeCommentEnd(TString *str, char characterInput){
	if((str->Len == 1 && characterInput == 'e') ||
		(str->Len == 2 && characterInput == 'n') ||
		(str->Len == 3 && characterInput == 'd')){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

//POZOR funkcia vracia token
TToken scannerError(int inRow,int inColumn){

	TToken token;
	token.state = TK_ERR;
	token.intValue = SCNR_ERR;
  if(DEBUG != 0){
    fprintf(stderr, "Nastala chyba: riadok %d, stlpec %d: \n", inRow, inColumn);
  }

	return token;
}

TToken internalError(int inRow,int inColumn){

	TToken token;
	token.state = TK_ERR;
	token.intValue = INTERN_ERR;
	fprintf(stderr, "Nastala vazna interna chyba: riadok %d, stlpec %d: \n", inRow, inColumn);
	return token;
}

//funckia vrati ci znak moze nasledovat po operatore alebo nieco
//vrati TRUE ak moze a FALSE ak nie
int canBeOperatorDeliminer(char characterInput){
	if( characterInput == ' ' ||
	    characterInput == '\t' ||
		characterInput == '_' ||
		isLowerCase(characterInput) ||
		isdigit(characterInput) ||
		characterInput == '(' ||
        characterInput == ',' ||
		characterInput == ')'){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

TToken ungetcAndReturn(char characterInput,int inRow, int inColumn,TToken token){
	if(ungetc(characterInput, inputFile) == EOF){
		return internalError(inRow,inColumn);
	}
	else{
			return token;
	}
}
int inRow = 0,inColumn = 0;
//musi sa volat jedina kvoli inputFilu
TToken getToken(TString *str){
	TToken token;
	inputFile = stdin;

	TPossibleState State = ST_START;

	char characterInput;

	while ((characterInput = fgetc(inputFile))){

		logDebug(characterInput,State,str);
		if(characterInput != '\n'){
			inColumn++;
		}
		else{
			inRow++;
			inColumn = 0;
		}

		if(characterInput == EOF){
      if(inComment || inString){
        return scannerError(inRow, inColumn);
      }
			token.state = TK_EOF;
			return token;
		}

		switch (State) {
			//v pociatocnom stave
			case ST_START:
				//TODO - nemoze ostat
				if(!strcmp(str->myString,"\n")){
					State = ST_NEW_LINE;
          if(ungetc(characterInput, inputFile) == EOF){
        		return internalError(inRow,inColumn);
        	}
          cleanString(str);
				}
				//ak sa zacina string
				else if(characterInput == '"'){
					State = ST_STRING;
          inString = 1;
				}
				//ak je tam lava zatvroka
				else if(characterInput == '('){
					token.state = TK_LBRACKET;
					return token;
				}
				//ak je tam prava zatvroka
				else if(characterInput == ')'){
					token.state = TK_RBRACKET;
					return token;
				}
                // ak je tam ciarka
                else if(characterInput == ','){
                    token.state = TK_COMMA;
                    return token;
                }
				//ak sa moze jednat o operator
				else if(isOneCharOperator(characterInput)){
					State = ST_OPERATOR;
					pushBackString(str,characterInput);
				}
				//ak sa zacina komentar
				else if(characterInput == '#'){
					State = ST_COMMENT_SHARP;
				}
				//ak sa dostaneme do noveho riadku
				//v parsery nebudeme uvolnovat retazec ak sa rovna "\n"
				else if(characterInput == '\n'){
					pushBackString(str,characterInput);
					token.state = TK_EOL;
					return token;
				}
				//ak sa zacina cislo - dessatinne alebo cele - ale dekadicke
				else if(isdigit(characterInput) && characterInput != '0'){
					State = ST_NUMBER;
					pushBackString(str,characterInput);
				}
				//identifikator zacina iba s malym pismenom
				else if(isLowerCase(characterInput) || characterInput == '_'){
					State = ST_KEYWORD_ID;
					pushBackString(str,characterInput);
				}
				/*
				//identifikator moze zacinat '_'
				else if(characterInput == '_'){
				State = ST_ID;
				pushBackString(str,characterInput);
				}
				*/
				//ak je prva nula
				else if(characterInput == '0'){
					pushBackString(str,characterInput);
					State = ST_ZERO;
				}
				//ak je to medzera nic sa nedeje
				//HEY!!! MOJE buduce ja
				//ak toto budes chciet vymazat uvedom si
				//ze je to ok lebo to je ST_START
				else if(characterInput == ' '){
					continue;
				}
				else if(characterInput == '\t' ){
				    continue;
				}
				//ak je tam nieco ine
				else{
					return scannerError(inRow,inColumn);
				}
			break;
			//ak sme potencionalne v stave EOL - koniec riadka
			/* TREBA VYMAZAT
			case ST_EOL:
				if(strLength(str) == 1 && characterInput == 'O'){
					continue;
				}
				else if(strLength(str) == 2 && characterInput == 'L'){
					return scannerError(inRow,inColumn);
				}
				if(strLength(str) == 3 && characterInput == '\n'){
					token.state = TK_EOL;
					return token;
				}
				else{
					return scannerError(inRow,inColumn);
				}
				break;
				*/
			//Ak cislo zacina nulou tak bud je to nula alebo desatinne
			case ST_ZERO:
				//pokracujeme do stavu decimal napr 0.2 moze byt
				if(characterInput == '.'){
					pushBackString(str,characterInput);
					State = ST_DECIMAL;
				}
				//je to cista nula oddelena medzerou alebo inym oddelovacom (napr. operator)
				else if(isDeliminer(characterInput)){
					token.state = TK_INTEGER;
					//pardon ze tu davam natvrdo nulu
					token.intValue = ZERO;

					//treba vratit znak ktory sme precitali
					return ungetcAndReturn(characterInput, inRow, inColumn, token);
				}
				else{
					return scannerError(inRow,inColumn);
				}
			break;
			//
			case ST_OPERATOR:
				if(isOneCharOperator(characterInput)){
					pushBackString(str,characterInput);

					char *newString = getString(str);
					if(newString == NULL){
						return internalError(inRow,inColumn);
					}

					//ak je to operator (2-znakovy) musime najsť ktorý
					TTokenState operatorType = findOperator(newString);
					free(newString);
					if(operatorType == TK_ERR){
						//ked je sice dvojznakovy ale nie operator
						return scannerError(inRow,inColumn);
					}
					else{
						token.state = operatorType;
						return token;
					}
				}
				else if(canBeOperatorDeliminer(characterInput)){

					char *newString = getString(str);
					if(newString == NULL){
						return internalError(inRow,inColumn);
					}
					TTokenState operatorType = findOperator(newString);
					free(newString);

					//pozor!!! toto znamena zle chovanie programu
					//zavazna chyba
					if(operatorType == TK_ERR){
						fprintf(stderr, "Fatalna chyba - toto by sa nemalo stat :( je mi to luto)!!!\n");
						return scannerError(inRow,inColumn);
					}
          else{
            token.state = operatorType;
            return ungetcAndReturn(characterInput, inRow, inColumn, token);
          }
				}
				else{
					return scannerError(inRow,inColumn);
				}
			break;
			//ak sme v stave sharpoveho komentu tak do konca riadka nic neukladame
			case ST_COMMENT_SHARP:
				if(characterInput == '\n'){
          cleanString(str);
          pushBackString(str,'\n');
					State = ST_START;
				}
			break;

			case ST_KEYWORD_ID:
				if(isIDChar(characterInput)){
					pushBackString(str,characterInput);
				}
				else if(characterInput == '?' || characterInput == '!'){
					pushBackString(str,characterInput);
					State = ST_FUNC_ID;
				}
				else if(isDeliminer(characterInput)){
					char *newString = getString(str);
					if(newString == NULL){
						return internalError(inRow,inColumn);
					}
					TTokenState idType = findKeyWord(newString);

					if(idType != TK_ERR){
						token.state = idType;
						free(newString);
					}
					else{
						token.state = TK_ID;
						token.string = newString;
					}
					newString = NULL;
					//vratime znak ktory bude sucast dalsieho tokenu
					return ungetcAndReturn(characterInput, inRow, inColumn, token);
				}
				else{
					return scannerError(inRow,inColumn);
				}
			break;

			case ST_FUNC_ID:
				if(isDeliminer(characterInput) &&
          characterInput != '!' &&
          characterInput != '?' &&
          characterInput != '='){

          while(characterInput == ' '){
            characterInput = fgetc(inputFile);
          }
          if(characterInput == '!' ||
            characterInput == '?' ||
            characterInput == '='){
              return scannerError(inRow,inColumn);
          }

					char *newString = getString(str);
					if(newString == NULL){
						return internalError(inRow,inColumn);
					}
					token.state = TK_ID;
					token.string = newString;
					newString = NULL;

					//vratime znak spat do streamu
					return ungetcAndReturn(characterInput, inRow, inColumn, token);

				}
				else{
					return scannerError(inRow,inColumn);
				}
			break;

			case ST_NUMBER:
				if(isdigit(characterInput)){
					pushBackString(str,characterInput);
					State = ST_NUMBER; //stav sa nemeni
				}
				else if(isDeliminer(characterInput)){
					/*char *newString;
					if(!getString(str,newString)){
						return internalError(inRow,inColumn);
					}*/
					token.state = TK_INTEGER;
					token.intValue = strToIntSCANNER(str);
					if(token.intValue == FATAL){
						return internalError(inRow,inColumn);
					}
					/*free(newString);
					newString = NULL;*/
					//vratime znak spat do streamu
					return ungetcAndReturn(characterInput, inRow, inColumn, token);
				}
        else if(characterInput == 'e' || characterInput == 'E'){
					pushBackString(str,characterInput);
					State = ST_DECIMAL_EXP;
				}
        else if(characterInput == '.'){
					pushBackString(str,characterInput);
					State = ST_DECIMAL;
				}
				else{
					return scannerError(inRow,inColumn);
				}
			break;

			case ST_NEW_LINE:
				if(characterInput == '='){
					State = ST_EQUALS;
					cleanString(str);
					pushBackString(str,characterInput);
				}
				else{
					State = ST_START;
          if(ungetc(characterInput, inputFile) == EOF){
        		return internalError(inRow,inColumn);
        	}
					cleanString(str);
				}
			break;

			case ST_EQUALS:

				if(characterInput == ' ' || characterInput == '\n' || characterInput == '\t'){
					char *tempStr = getString(str);
					if(tempStr == NULL){
						return internalError(inRow,inColumn);
					}
					if(!strcmp(tempStr,"=begin")){
            if(ungetc(characterInput, inputFile) == EOF){
          		return internalError(inRow,inColumn);
          	}
						State = ST_COMMENT_EQBEGIN;
            inComment = 1;
						free(tempStr);
						cleanString(str);
					}
          else{
            free(tempStr);
  					return scannerError(inRow,inColumn);
  				}
				}
				else if(canBeComment(str,characterInput)){
					pushBackString(str,characterInput);
				}
				else{
					return scannerError(inRow,inColumn);
				}
			break;

			case ST_COMMENT_EQBEGIN:
				if(characterInput == '=' && str->myString[strLength(str)-1] == '\n'){
					State = ST_COMMENT_EQEND;
          cleanString(str);
					pushBackString(str,characterInput);
				}
				else{
					cleanString(str);
					pushBackString(str,characterInput);
				}
			break;

			case ST_COMMENT_EQEND:
				if(characterInput == ' ' || characterInput == '\n' || characterInput == '\t'){
					char *tempStr = getString(str);
					if(tempStr == NULL){
						return internalError(inRow,inColumn);
					}
					if(!strcmp(tempStr,"=end")){
            //v komentoch az do konca riadka neplati nic
            while (characterInput != '\n' && (characterInput = fgetc(inputFile))){
              if(characterInput == '\n' || characterInput == EOF){
                break;
              }
            }
						State = ST_START;
            inComment = 0;
            if(characterInput == EOF){
              free(tempStr);
              token.state = TK_EOF;
              return token;
            }
            if(ungetc(characterInput, inputFile) == EOF){
          		return internalError(inRow,inColumn);
          	}
						free(tempStr);
						cleanString(str);
					}
          else{
            State = ST_COMMENT_EQBEGIN;
						free(tempStr);
						cleanString(str);
          }
				}
				else if(canBeCommentEnd(str,characterInput)){
					pushBackString(str,characterInput);
				}
				else{
					State = ST_COMMENT_EQBEGIN;
				}
			break;

		case ST_DECIMAL:
			if(str->myString[strLength(str)-1] == '.'){
				if(isdigit(characterInput)){
					pushBackString(str,characterInput);
				}
				else{
					return scannerError(inRow,inColumn);
				}
			}
      else if(isdigit(characterInput)){
        pushBackString(str,characterInput);
      }
			else if(characterInput == 'e' || characterInput == 'E'){
				pushBackString(str,characterInput);
				State = ST_DECIMAL_EXP;
			}
			else if(isDeliminer(characterInput)){

				token.state = TK_DECIMAL;
				token.doubleValue = strToDoubleSCANNER(str);
				return ungetcAndReturn(characterInput, inRow, inColumn, token);
			}
			else{
				return scannerError(inRow,inColumn);
			}
		break;
		case ST_DECIMAL_EXP:
			if(str->myString[strLength(str)-1] == 'e' || str->myString[strLength(str)-1] == 'E'){
				if(characterInput == '+' ||
						characterInput == '-' ||
						isdigit(characterInput)){

					pushBackString(str,characterInput);
				}
				else{
					return scannerError(inRow,inColumn);
				}

			}
			else if(isdigit(characterInput)){
				pushBackString(str,characterInput);
			}
			else if(isDeliminer(characterInput) &&
							!(str->myString[strLength(str)-1] == 'e' || str->myString[strLength(str)-1] == 'E') &&
              !(str->myString[strLength(str)-1] == '-')
						){
				token.state = TK_DECIMAL;
				token.doubleValue = strToDoubleSCANNER(str);
				return ungetcAndReturn(characterInput, inRow, inColumn, token);
			}
			else{
				return scannerError(inRow,inColumn);
			}
		break;
		case ST_STRING:
			if(characterInput == '"'){
        inString = 0;
				token.state = TK_STRING;
        token.string = getString(str);
				if(token.string == NULL){
					return internalError(inRow,inColumn);
				}
				return token;
			}
			else if(characterInput == '\\'){
				State = ST_BACKSLASH;
			}
			else if(characterInput != '\n'){
				pushBackString(str,characterInput);
			}
			else{
				return scannerError(inRow,inColumn);
			}
		break;
		case ST_BACKSLASH:
			if(characterInput == 'n'){
				pushBackString(str,'\n');
				State = ST_STRING;
			}
			else if(characterInput == 't'){
				pushBackString(str,'\t');
				State = ST_STRING;
			}
			else if(characterInput == 's'){
				pushBackString(str,' ');
				State = ST_STRING;
			}
			else if(characterInput == '\\'){
				pushBackString(str,'\\');
				State = ST_STRING;
			}
			else if(characterInput == '"'){
				pushBackString(str,'"');
				State = ST_STRING;
			}
			else if(characterInput == 'x'){
				char newChar1 = fgetc(inputFile);
				if(!isdigit(newChar1)  && !isHexadec(newChar1)){
					return scannerError(inRow,inColumn);
				}
				char newChar2 = fgetc(inputFile);
				if(!isdigit(newChar2)  && !isHexadec(newChar2)){
          if(ungetc(newChar2, inputFile) == EOF){
        		return internalError(inRow,inColumn);
        	}
				}

				char stringToConvert[5];
				stringToConvert[0] = newChar1;
        char charToAppend;
        if(!isdigit(newChar2) && !isHexadec(newChar2)){
          stringToConvert[1] = '\0';
          charToAppend = hexToChar(stringToConvert);
  				if(charToAppend == FATAL){
  					printf("Nastala vazna chyba!! - first\n");
  					return internalError(inRow,inColumn);
  				}
        }
        else{
          stringToConvert[1] = newChar2;
          stringToConvert[2] = '\0';
          if((newChar1 == 'f' || newChar1 == 'F') ||
              (newChar1 == 'F' || newChar1 == 'F')){
              charToAppend = -1;
          }
          else{
            charToAppend = hexToChar(stringToConvert);
    				if(charToAppend == FATAL){
    					printf("Nastala vazna chyba!! -second\n");
    					return internalError(inRow,inColumn);
    				}
          }

        }

				pushBackString(str,charToAppend);
				State = ST_STRING;
			}
			else{
				pushBackString(str,characterInput);
				State = ST_STRING;
			}
		break;



		}
	logDebug(characterInput,State,str);
	}
	printf("Nastala vazna chyba podla mna\n");
	return token;
}


int isFirstTokenTest = 1;
int lastTokenIsEOLTest = 1;

TString str;

//
TToken getNextTokenSCANNER(){

  TToken token;

	if(isFirstTokenTest){
    if(initString(&str) != STR_OK){
			printf("Chyba pri alokacii pamati!\n");
			token.state = TK_ERR;
      token.intValue = INTERN_ERR;
			return token;
		}

		isFirstTokenTest = 0;
	}

	if(lastTokenIsEOLTest || isFirstTokenTest){
		setStringEOL(&str);
	}

	token = getToken(&str);
	if(token.state == TK_EOL){
		lastTokenIsEOLTest = 1;
	}
	else{
		lastTokenIsEOLTest = 0;
    cleanString(&str);
	}
  if(token.state == TK_EOF){
    freeString(&str);
  }
	return token;

}


#ifdef TEST_SCANNER
int main(){

  TToken token;

	inputFile = stdin;


	while(1){
		token = getNextTokenSCANNER();

		if(token.state == TK_EOF)
			break;
		char *tokenAsString = tokenToString(token);
    printf("Aktualny stav tokenu je: %s\n",tokenAsString);

		if(token.state == TK_ERR)
			return token.intValue;
	}
	return 0;
}
#endif
