/******************** parser.c *********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>




const char *myTokens(TTokenState state){
	switch (state) {
		case TK_DEF:
			return "definicia funkcie";
		break;
		case TK_DO:
			return "klucove slovo do";
		break;
		case TK_ELSE:
			return "klucove slovo else";
		break;
		case TK_END:
			return "klucove slovo end";
		break;
		case TK_NIL:
			return  "hodnota nil";
		break;
		case TK_COMMA:
			return "ciarka";
		break;
		case TK_IF:
			return "klucove slovo if";
		break;
		case TK_NOT:
			return "klucove slovo not";
		break;
		case TK_WHILE:
			return  "klucove slovo while";
		break;
		case TK_THEN:
			return "klucove slovo then";
		break;
		case TK_PLUS:
			return "operator plus";
		break;
		case TK_MINUS:
			return "operator minus";
		break;
		case TK_MUL:
			return "operator krat";
		break;
		case TK_DIV:
			return "operator delene";
		break;
		case TK_LESS:
			return "operator mensi";
		break;
		case TK_MORE:
			return "operator vetsi";
		break;
		case TK_EQUAL:
			return "operator rovnaky";
		break;
		case TK_LESSEQUEL:
			return "operator mensi alebo rovny";
		break;
		case TK_MOREEQUAL:
			return "operator vetsi alebo rovny";
		break;
		case TK_ASSIGN:
			return "operator priradenia";
		break;
		case TK_NOTEQUAL:
			return "operator nerovnaky";
		break;
		case TK_LBRACKET:
			return "lava zatvorka";
		break;
		case TK_RBRACKET:
			return  "prava zatvorka";
		break;
		case TK_EOL:
			return "koniec riadka";
		break;
		case TK_EOF:
			return "koniec filu";
		break;
		case TK_INTEGER:
			return "typ integer";
		break;
		case TK_DECIMAL:
			return "typ decimal";
		break;
		case TK_STRING:
			return "typ string";
		break;
		case TK_ID:
			return "identifikator";
		break;
		case TK_ERR:
			return "chyba";
		break;
		default:
			return "Chyba default";
	}
}

//at the begening it is set to true
int isEOL = 1;
int isFirstToken = 1;
int lastTokenIsNewLine = 1;
DataTYPE lastExpress = -1;

TString globalString;
TToken globalToken;
TFuncTree *globalTree;
TVarTree *globalVarTree;
TVarTree *localVarTree;
int bracketDown = 0;


//global state je global = GLOBALSTATE and local = LOCALSTATE
int globalState = GLOBALSTATE;
char *inFunc;
char *inID;
char *inDep;
int givenParams = 0;
ReturnTYPE retType = RT_NIL;


int autoIncrementPars;

int getDigitsCountPAR(int num){
  if(num == 0){
    return 1;
  }
  int counter = 0;
  while(num > 0){
    num/=10;
    counter++;
  }
  return counter;
}

char *getUniqueVarPAR(){

  char *result = malloc((getDigitsCountPAR(autoIncrementPars)+5)*sizeof(char));
  if(result == NULL){
    return NULL;
  }

  autoIncrementPars++;

  result[0] = 'e';
  int i=1;
  int num = autoIncrementPars;
  while(num > 0){
    int pom = num % 10;
    result[i] = '0'+ pom;
    i++;
    num/=10;
  }
  result[i] =  '\0';
  return result;
}

//chalani nenavidim makra ale usetri to kopu riadkov
#define checkToken(tokenState) do { \
	if (globalToken.state != tokenState) { \
		fprintf(stderr, "(CheckToken)Chyba v parser: ocakavali sme '%s' , ale dostali sme '%s'.\n", myTokens(tokenState), myTokens(globalToken.state)); \
		return PRSR_ERR; \
	} \
} while (0);

#define checkEolEof() do { \
	if (globalToken.state != TK_EOF &&  globalToken.state != TK_EOL) { \
		fprintf(stderr, "Chyba v parser: ocakavali sme koniec riadka alebo filu , ale dostali sme '%s'.\n", myTokens(globalToken.state)); \
		return PRSR_ERR; \
	} \
} while (0);

#define callRule(Rule) do { \
	printRule(Rule,"IN");  \
	int returnValue = Rule(); \
	if (returnValue != OK) \
		return returnValue; \
} while (0);

#define exitRule(Rule,returnValue) do { \
	printRule(Rule,"OUT");  \
	return returnValue; \
} while (0);

//ak token v parametry moze byt hodnota vrati 1 inak 0
int tokenIsValue(TToken gToken){
	if(gToken.state == TK_ID ||
		 gToken.state == TK_INTEGER ||
	   gToken.state == TK_STRING ||
		 gToken.state == TK_NIL ||
	   gToken.state == TK_DECIMAL){
			 return 1;
	}
	else{
			return 0;
	}
}

#define DEBUGPRULE 1
#define DEBUGPTOKEN 0
#define DEBUGLOG 0


void logDebugParser(const char *string){
	if(DEBUGLOG == 0){
		return;
	}
	printf("state: %d string: %s\n",globalState,string);
}




//vrati 1 ak token je nastaveny na porovnavaci opeartor
int isOPCOM(){
	if(globalToken.state == TK_LESS ||
		globalToken.state == TK_MORE ||
		globalToken.state == TK_LESSEQUEL ||
		globalToken.state == TK_LESSEQUEL ||
		globalToken.state == TK_EQUAL ||
		globalToken.state == TK_NOTEQUAL){
			return 1;
		}
	return 0;
}

//podobne ako funkcia hore
int isOPMOD(){
	if(globalToken.state == TK_PLUS ||
		globalToken.state == TK_MINUS ||
		globalToken.state == TK_MUL ||
		globalToken.state == TK_DIV){
			return 1;
		}

		return 0;
}

//ak sa jedna o hodnotu tak vrati 1 inak 0
int isValue(){
	if(globalToken.state == TK_INTEGER ||
		 globalToken.state == TK_DECIMAL ||
	 	 globalToken.state == TK_STRING ||
	   globalToken.state == TK_ID){
			 return 1;
		 }

		 return 0;
}

//zasa to smutne makro :( mal som po celom kode to getNextToken
//tak som to proste musel tak definovat :(
//odkaz pre dalsie generacie - "robte projekt od septembra (zari)"
#define getNextToken() do { \
	getNextTokenFUNC();  \
	if(globalToken.state == TK_ERR) return globalToken.intValue; \
} while (0);

//funkcia riesi problem s novým riadkom - treba zakomponovať aj do testov takúto funkciu
//asi normálne okopirovat
void getNextTokenFUNC(){

	if(isFirstToken){

		isFirstToken = 0;
		if(initString(&globalString) != STR_OK){
			printf("Chyba pri alokacii pamati!\n");
			globalToken.state = TK_ERR;
			globalToken.intValue = INTERN_ERR;
		}


	}

	if(lastTokenIsNewLine || isFirstToken){
		setStringEOL(&globalString);
	}

	globalToken = getToken(&globalString);
	if(DEBUGPTOKEN == 1){
		char *tokenAsString = tokenToString(globalToken);
		printf("Aktualny stav tokenu je: %s\n",tokenAsString);
	}
	if(globalToken.state == TK_EOL){
		lastTokenIsNewLine = 1;
	}
	else if(globalToken.state == TK_ERR){
		return;
	}
	else{
		lastTokenIsNewLine = 0;
		cleanString(&globalString);
	}

}

//odfiltruje zvytocne tokeny
int cleanEOLToken(){
	while(globalToken.state == TK_EOL){
		getNextToken();
	}
	return OK;
}

//co moze ist za id funkcie
int isFuncSeparator(){
	if(globalToken.state == TK_LBRACKET || isValue() || globalToken.state == TK_EOL){
		return 1;
	}
	return 0;
}

int isAfterFuncCall(){
	if((bracketDown == 1 && globalToken.state == TK_RBRACKET) || globalToken.state == TK_EOL){
		return 1;
	}

	return 0;
}

int GP(); //global program
int ID(); //identifier
int EX(); //expression
int VALUE(); //value in expression
int OPMOD(); //operator (+,-,*,=)
int DEF(); // function definition
int FA(); //first argument
int FACOMMA(); //other arguments
int BODY(); //function body
int WHILE(); //while construction
int IF(); //if construction
int ASSIGN(); //assign construction
int CALLFUNC(); //call function construction
int CHECKARG(); //arguments checking
int CHECKMORE(); //checking if there are more arguments in calling



//funkcia vrati 1 ak sa funkcie rovnaju
//inak vrati 0
int compareFunc(int (*funcA)(),int (*funcB)()){
	if(funcA == funcB){
		return 1;
	}
	return 0;
}

void printRule(int (*rule)(),const char *inOut){

	if(DEBUGPRULE == 0){
		return;
	}

	if(compareFunc(rule,GP)){
		printf("(%s)Zavolane pravidlo je GP.\n",inOut);
	}
	else if(compareFunc(rule,ID)){
		printf("(%s)Zavolane pravidlo je ID.\n",inOut);
	}
	else if(compareFunc(rule,EX)){
		printf("(%s)Zavolane pravidlo je EX.\n",inOut);
	}
	else if(compareFunc(rule,VALUE)){
		printf("(%s)Zavolane pravidlo je VALUE.\n",inOut);
	}
	else if(compareFunc(rule,OPMOD)){
		printf("(%s)Zavolane pravidlo je OPMOD.\n",inOut);
	}
	else if(compareFunc(rule,FA)){
		printf("(%s)Zavolane pravidlo je FA.\n",inOut);
	}
	else if(compareFunc(rule,DEF)){
		printf("(%s)Zavolane pravidlo je DEF.\n",inOut);
	}
	else if(compareFunc(rule,FACOMMA)){
		printf("(%s)Zavolane pravidlo je FACOMMA.\n",inOut);
	}
	else if(compareFunc(rule,BODY)){
		printf("(%s)Zavolane pravidlo je BODY.\n",inOut);
	}
	else if(compareFunc(rule,CHECKARG)){
		printf("(%s)Zavolane pravidlo je CHECKARG.\n",inOut);
	}
	else if(compareFunc(rule,CHECKMORE)){
		printf("(%s)Zavolane pravidlo je CHECKMORE.\n",inOut);
	}
	else if(compareFunc(rule,WHILE)){
		printf("(%s)Zavolane pravidlo je WHILE.\n",inOut);
	}
	else if(compareFunc(rule,IF)){
		printf("(%s)Zavolane pravidlo je IF.\n",inOut);
	}
	else if(compareFunc(rule,ASSIGN)){
		printf("(%s)Zavolane pravidlo je ASSIGN.\n",inOut);
	}
	else if(compareFunc(rule,CALLFUNC)){
		printf("(%s)Zavolane pravidlo je CALLFUNC.\n",inOut);
	}
	else{
		printf("Mato zabudol doplnit pravidlo, kontaktujte ho.\n");
	}
}


int GP(){
	getNextToken();
	while(globalToken.state != TK_EOF){

		switch (globalToken.state) {
			case TK_ID:
				callRule(ID);
			break;
			case TK_DEF:
				callRule(DEF);
			break;
			case TK_IF:
				callRule(IF);
			break;
			case TK_WHILE:
				callRule(WHILE);
			break;
			case TK_EOL:
				getNextToken();
			break;
			case TK_EOF:
				exitRule(GP,OK);
			break;
			case TK_INTEGER:
			case TK_DECIMAL:
			case TK_STRING:
				callRule(EX);
			break;
			default:
				fprintf(stderr, "Chybna konstrukcia - GP.\n");
				exitRule(GP,PRSR_ERR);
		}
	}
	exitRule(GP,OK);

}

int DEF(){

	//definujeme funkciu - stav sa meni na lokalny
	globalState = LOCALSTATE;
	initVarTree(&localVarTree);
	getNextToken();

	//ocakavame ID funkcie
	checkToken(TK_ID);

	inFunc = globalToken.string;
	//vlozime ID do tabulky
	if(isFuncDefined(globalTree,globalToken.string)){
		logDebugParser("Redefinicia funkcie!\n");
		return SEM_ERR_UNDEF;
	}

	TFunc *new = malloc(sizeof(TFunc));
	new->typeVar = RT_INT;
	new->funcName = inFunc;
	new->paramsCount = 0;
	new->dependencies = initStack();
	new->parameters = initStack();
	if(insertInFUNCTree(&globalTree,&new)){
		printf("Chyba1\n");
		return INTERN_ERR;
	}

	// if(isFuncDefined(globalTree,globalToken.string)){
	// 	printf("Je!\n");
	// 	logDebugParser("Redefinicia funkcie!\n");
	// 	// return SEM_ERR_UNDEF;
	// }

	getNextToken();


	checkToken(TK_LBRACKET);

	callRule(FA);
	checkToken(TK_RBRACKET);

	callRule(BODY);
	checkToken(TK_END);
	getNextToken();
	globalState = GLOBALSTATE;
	freeVarTree(&localVarTree);
	exitRule(DEF,OK);
}

int BODY(){
	getNextToken();
	while(globalToken.state != TK_END){
		switch (globalToken.state) {
			case TK_ID:
				callRule(ID);
			break;
			case TK_IF:
				callRule(IF);
			break;
			case TK_ELSE:
				exitRule(BODY,OK);
			break;
			case TK_WHILE:
				callRule(WHILE);
			break;
			case TK_EOL:
				getNextToken();
			break;
			case TK_EOF:
				fprintf(stderr, "Chyba v parser: ocakavali sme koniec funkcie, ale dostali sme koniec suboru.\n");
				exitRule(BODY,PRSR_ERR);
			break;
			case TK_INTEGER:
			case TK_DECIMAL:
			case TK_STRING:
				callRule(EX);
			break;
			default:
				fprintf(stderr, "Chybna kosštrukcia - BODY.\n");
				exitRule(BODY,PRSR_ERR);
		}
	}
	exitRule(BODY,OK);

}

int WHILE(){
	getNextToken();
	cleanEOLToken();

	callRule(EX);
	cleanEOLToken();

	checkToken(TK_DO);
	cleanEOLToken();

	callRule(BODY);
	cleanEOLToken();

	checkToken(TK_END);
	getNextToken();
	exitRule(WHILE,OK);
}

int IF(){
	getNextToken();
	cleanEOLToken();

	callRule(EX);
	cleanEOLToken();

	checkToken(TK_THEN);
	cleanEOLToken();

	callRule(BODY);
	cleanEOLToken();

	if(globalToken.state == TK_ELSE){
		callRule(BODY);
		cleanEOLToken();
	}
	checkToken(TK_END);
	getNextToken();
	exitRule(IF,OK);
}

int FA(){
	getNextToken();
	if(globalToken.state == TK_RBRACKET){
		exitRule(FA,OK);
	}
	checkToken(TK_ID);
	addParams(&globalTree,inFunc,globalToken.string);

	TVar *newLocalParameter;
	newLocalParameter = malloc(sizeof(TVar));

	newLocalParameter->typeVar = T_PARAM;
	newLocalParameter->varName = globalToken.string;
	newLocalParameter->global = 0;
	if(insertInVARTree(&localVarTree,&newLocalParameter)){
		printf("Chyba1\n");
		return INTERN_ERR;
	}
	//free(globalToken.string);
	getNextToken();
	if(globalToken.state == TK_RBRACKET){
		exitRule(FA,OK);
	}
	else if(globalToken.state == TK_COMMA){
		callRule(FACOMMA);
	}
	else{
		fprintf(stderr, "Chyba v parser: ocakavali sme ciarku alebo pravu zatvorku, ale dostali sme '%s'.\n", myTokens(globalToken.state));
		exitRule(FA,PRSR_ERR);
	}

	exitRule(FA,OK);
}


int FACOMMA(){
	getNextToken();
	checkToken(TK_ID);
	addParams(&globalTree,inFunc,globalToken.string);
	TVar *newLocalParameter;
	newLocalParameter = malloc(sizeof(TVar));

	newLocalParameter->typeVar = T_PARAM;
	newLocalParameter->varName = globalToken.string;
	newLocalParameter->global = 0;
	if(insertInVARTree(&localVarTree,&newLocalParameter)){
		printf("Chyba1\n");
		return INTERN_ERR;
	}
	//free(globalToken.string);

	getNextToken();
	if(globalToken.state == TK_RBRACKET){
		exitRule(FACOMMA,OK);
	}
	else if(globalToken.state == TK_COMMA){
		callRule(FACOMMA);
	}
	else{
		fprintf(stderr, "Chyba v parser: ocakavali sme ciarku alebo pravu zatvorku, ale dostali sme '%s'.\n", myTokens(globalToken.state));
		exitRule(FACOMMA,PRSR_ERR);
	}
	exitRule(FACOMMA,OK);
}

int ID(){

	//moze to byt funkcia TODO
	inID = globalToken.string;
	getNextToken();
	if(globalToken.state == TK_ASSIGN){

		callRule(ASSIGN);
	}
	else if(isFuncSeparator()){

		if(globalToken.state == TK_LBRACKET){
			bracketDown = 1;
		}
		//to som kontroloval funkciu
		callRule(CALLFUNC);
		if(bracketDown){
			checkToken(TK_RBRACKET);
			bracketDown = 0;
		}
		free(inID);
		getNextToken();
	}
	else if(isOPCOM() || isOPMOD()){
		//TODO doplnit nejaky spajany zoznam zeby to islo - VYMAZAT KOMENT
		callRule(EX);
	}
	else if(globalToken.state == TK_EOL){
		if(globalState == GLOBALSTATE){
			if(!isVarDefined(globalVarTree,inID)){
				printf("Použitie nedefinovanej premennej!\n");
				return SEM_ERR_UNDEF;
			}
			else{
				;//generovat kod
			}
		}
		else if(globalState == LOCALSTATE){
			if(!isVarDefined(localVarTree,inID)){
				printf("Použitie nedefinovanej premennej!\n");
				return SEM_ERR_UNDEF;
			}
			else{
				retType = getVarType(localVarTree,inID);
				//generovat kod
			}
		}
	}
	else{
		fprintf(stderr, "Chyba v parser: ocakavali sme priradenie alebo funkciu, ale dostali sme '%s'.\n", myTokens(globalToken.state));
		exitRule(ID,PRSR_ERR);
	}


	// checkEolEof();

	exitRule(ID,OK);
}

int ASSIGN(){
	getNextToken();
	//cleanEOLToken(); - ziadne prazdne riadky!

	callRule(EX);
	if(globalState == GLOBALSTATE){
		if(isFuncDefined(globalTree,inID)){
			logDebugParser("Globalna funkcia sa vola rovnako ako premenna!\n");
			return SEM_ERR_UNDEF;
		}
		TVar *new;
		new = malloc(sizeof(TVar));

		new->typeVar = lastExpress; //TODOOOOOOOOO
		new->varName = inID;
		new->global = 0;
		if(insertInVARTree(&globalVarTree,&new)){
			printf("Chyba1\n");
			return INTERN_ERR;
		}
	}
	else if(globalState == LOCALSTATE){
		TVar *new;
		new = malloc(sizeof(TVar));
		new->typeVar = lastExpress; //TODOOOOOOOOO
		new->varName = inID;
		new->global = 0;
		if(insertInVARTree(&localVarTree,&new)){
			printf("Chyba1\n");
			return INTERN_ERR;
		}
	}
	exitRule(ASSIGN,OK);
}

int CALLFUNC(){
	callRule(CHECKARG);
	if(globalState == LOCALSTATE && isVarDefined(localVarTree,inID)){
		exitRule(CALLFUNC,OK);
	}
	else if(!isFuncDefined(globalTree,inID)){
		if(globalState == LOCALSTATE){
			addDependency(&globalTree, inFunc, inID, givenParams);
		}
		else{
			printf("Volanie nedefinovanej funkcie!\n");
			return SEM_ERR_UNDEF;
		}
	}
	else if(!allDependenciesOK(globalTree,inID)){
		//co ak nema dependencie oka
		printf("Volanie nedefinovanej funkcie!\n");
		return SEM_ERR_UNDEF;
	}
	else if(!checkParamsCountREAL(globalTree , inID, givenParams)){
		logDebugParser("Zly pocet parametrov! - real\n");
		exitRule(CALLFUNC,SEM_ERR_PARAM);
	}
	else if(!allDependenciesPARAMSOK(globalTree, inID)){
		logDebugParser("Zly pocet parametrov! - v depe\n");
		exitRule(CALLFUNC,SEM_ERR_PARAM);
	}

	givenParams = 0;
	exitRule(CALLFUNC,OK);
}

int CHECKARG(){
	if(bracketDown){
		getNextToken();
	}

	if(isAfterFuncCall()){
		logDebugParser("No arguments!\n");
		exitRule(CHECKARG,OK);
	}
	else if(tokenIsValue(globalToken)){
		logDebugParser("arguments!\n");
		givenParams++;
		//predame parameters

		if(globalToken.state == TK_ID){
			if(globalState == GLOBALSTATE){
				if(!isVarDefined(globalVarTree,globalToken.string)){
					logDebugParser("Parameter je nedefinovana premenna\n");
					return SEM_ERR_UNDEF;
				}
			}
			else if(globalState == LOCALSTATE){
				if(!isVarDefined(localVarTree,globalToken.string)){
					logDebugParser("Parameter je nedefinovana premenna\n");
					return SEM_ERR_UNDEF;
				}
			}
			free(globalToken.string);
		}
		getNextToken();
		if(isAfterFuncCall()){
			exitRule(CHECKARG,OK);
		}
		else if(globalToken.state == TK_COMMA){
			callRule(CHECKMORE);
		}
		else{
			fprintf(stderr, "Chyba v parser: ocakavali sme ciarku alebo pravu zatvorku, ale dostali sme '%s'.\n", myTokens(globalToken.state));
			exitRule(CHECKARG,PRSR_ERR);
		}
	}

	exitRule(CHECKARG,OK);
}

int CHECKMORE(){
	getNextToken();
	if(!tokenIsValue(globalToken)){
		fprintf(stderr, "Chyba v parser: ocakavali sme hodnotu, ale dostali sme '%s'.\n", myTokens(globalToken.state));
		exitRule(CHECKMORE,PRSR_ERR);
	}
	givenParams++;
	//TODO ulozit hodnotu


	if(globalToken.state == TK_ID){
		if(globalState == GLOBALSTATE){
			if(!isVarDefined(globalVarTree,globalToken.string)){
				logDebugParser("Parameter je nedefinovana premenna\n");
				return SEM_ERR_UNDEF;
			}
		}
		else if(globalState == LOCALSTATE){
			if(!isVarDefined(localVarTree,globalToken.string)){
				logDebugParser("Parameter je nedefinovana premenna\n");
				return SEM_ERR_UNDEF;
			}
		}
		free(globalToken.string);
	}
	getNextToken();
	if(isAfterFuncCall()){
	exitRule(CHECKMORE,OK);
	}
	else if(globalToken.state == TK_COMMA){
		callRule(CHECKMORE);///////////
	}
	else{
		fprintf(stderr, "Chyba v parser: ocakavali sme ciarku alebo pravu zatvorku, ale dostali sme '%s'.\n", myTokens(globalToken.state));
		exitRule(CHECKMORE,PRSR_ERR);
	}
	exitRule(CHECKMORE,OK);
}

int EX(){
	// callRule(VALUE);
	// if(globalToken.state == TK_EOL){
	// 	return OK;
	// }
	// else if(!isOPCOM()){
	// 	fprintf(stderr, "Chyba v parser: Ocakavali sme koniec riadka alebo porovnavaci operator.\n");
	// 	return PRSR_ERR;
	// }
	//
	// getNextToken();
	// callRule(VALUE);
	logDebugParser("vyraz zacina\n");
	MyList *lList = initList();
	logDebugParser("init list\n");

	while (globalToken.state == TK_PLUS ||
				 globalToken.state == TK_MUL ||
			 	 globalToken.state == TK_MINUS ||
				 globalToken.state == TK_DIV ||
				 globalToken.state == TK_INTEGER ||
				 globalToken.state == TK_EQUAL ||
				 globalToken.state == TK_NOTEQUAL ||
				 globalToken.state == TK_MORE ||
				 globalToken.state == TK_MOREEQUAL ||
				 globalToken.state == TK_LESS ||
				 globalToken.state == TK_LESSEQUEL ||
				 globalToken.state == TK_ID ||
				 globalToken.state == TK_COMMA ||
				 globalToken.state == TK_STRING ||
				 globalToken.state == TK_LBRACKET ||
				 globalToken.state == TK_RBRACKET ||
				 globalToken.state == TK_NIL ||
			 	 globalToken.state == TK_DECIMAL) {
					 if(globalToken.state == TK_PLUS){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_PLUS,0);
					 }
					 else if(globalToken.state == TK_MUL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_MUL,0);
					 }
					 else if(globalToken.state == TK_ID){

						 if(globalState == GLOBALSTATE && !isVarDefined(globalVarTree,globalToken.string) &&
					 			!isFuncDefined(globalTree,globalToken.string)){
									printf("toto1\n");
							  logDebugParser("not defined\n");
								exitRule(EX, SEM_ERR_UNDEF);
						 }
						 else if(globalState == LOCALSTATE && isVarDefined(localVarTree,globalToken.string)){
							 printf("%s\n",globalToken.string);
							 DataTYPE varT;
								if(globalState == GLOBALSTATE){
									logDebugParser("is global\n");
									varT = getVarType(globalVarTree,  globalToken.string);
								}
								else{
									logDebugParser("is local\n");
									varT = getVarType(localVarTree,  globalToken.string);
								}
								if(varT == T_INT){
									pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_INT);
								}
								else if(varT == T_STRING){
									pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_STRING);
								}
								else if(varT == T_DOUBLE){
									pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_DOUBLE);
								}
								else if(varT == T_PARAM){

									pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_UNKNOWN);
								}
								else{
									logDebugParser("neuviedol si parameter!\n");
									return 1;
								}
								getNextToken();
								continue;

						 }
						 else if(isFuncDefined(globalTree,globalToken.string)){
							 printf("tot2o\n");
							 printf("%s\n",globalToken.string);
							 inDep = globalToken.string;
							 getNextToken();
							 if(globalToken.state == TK_LBRACKET){
								 bracketDown = 1;
							 }
							 //to som kontroloval funkciu
							 callRule(CHECKARG);

							 if(!isFuncDefined(globalTree,inDep)){
								 if(globalState == LOCALSTATE){
									 addDependency(&globalTree, inFunc, inDep, givenParams);
								 }
								 else{
									 printf("Volanie nedefinovanej funkcie! - 1\n");
									 exitRule(EX, SEM_ERR_UNDEF);
								 }
							 }
							 else if(!allDependenciesOK(globalTree,inDep)){
								 //co ak nema dependencie oka
								 printf("Volanie nedefinovanej funkcie!- 2\n");
								 exitRule(EX, SEM_ERR_UNDEF);
							 }
							 else if(!checkParamsCountREAL(globalTree , inDep, givenParams)){
								 logDebugParser("Zly pocet parametrov! - real\n");
								 exitRule(EX, SEM_ERR_PARAM);
							 }
							 else if(!allDependenciesPARAMSOK(globalTree, inDep)){
								 logDebugParser("Zly pocet parametrov! - v depe\n");
								 exitRule(EX, SEM_ERR_PARAM);
							 }

							 givenParams = 0;
							 if(bracketDown){
								 checkToken(TK_RBRACKET);
								 bracketDown = 0;
							 }
							 free(inDep);

							 exitRule(EX, OK);
						 }
						 else if(globalState == LOCALSTATE && !isVarDefined(localVarTree,globalToken.string)){
							 	printf("toto3\n");
								inDep = globalToken.string;
							 	getNextToken();
								if(globalToken.state == TK_LBRACKET){
						 			bracketDown = 1;
						 		}
						 		//to som kontroloval funkciu
								callRule(CHECKARG);

								if(!isFuncDefined(globalTree,inDep)){
									if(globalState == LOCALSTATE){
										addDependency(&globalTree, inFunc, inDep, givenParams);
									}
									else{
										printf("Volanie nedefinovanej funkcie!\n");
										exitRule(EX, SEM_ERR_UNDEF);
									}
								}
								else if(!allDependenciesOK(globalTree,inDep)){
									//co ak nema dependencie oka
									printf("Volanie nedefinovanej funkcie!\n");
									exitRule(EX, SEM_ERR_UNDEF);
								}
								else if(!checkParamsCountREAL(globalTree , inDep, givenParams)){
									logDebugParser("Zly pocet parametrov! - real\n");
									exitRule(EX, SEM_ERR_PARAM);
								}
								else if(!allDependenciesPARAMSOK(globalTree, inDep)){
									logDebugParser("Zly pocet parametrov! - v depe\n");
									exitRule(EX, SEM_ERR_PARAM);
								}

								givenParams = 0;
						 		if(bracketDown){
						 			checkToken(TK_RBRACKET);
						 			bracketDown = 0;
						 		}
						 		free(inDep);

								exitRule(EX, OK);
						 }

						 else if(globalState != GLOBALSTATE && globalState != LOCALSTATE){
							 logDebugParser("nastala chyba\n");
							 return INTERN_ERR;
						 }
						 DataTYPE varT;
						 if(globalState == GLOBALSTATE){
							 logDebugParser("is global\n");
							 varT = getVarType(globalVarTree,  globalToken.string);
						 }
						 else{
							 logDebugParser("is local\n");
							 varT = getVarType(localVarTree,  globalToken.string);
						 }
						 if(varT == T_INT){
							 pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_INT);
						 }
					   else if(varT == T_STRING){
							 pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_STRING);
						 }
						 else if(varT == T_DOUBLE){
							 pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_DOUBLE);
						 }
						 else if(varT == T_PARAM){

							 pushBack(&lList, 0, globalToken.string, 0, LLTYPE_ID,EXP_UNKNOWN);
						 }
						 else{
							 logDebugParser("neuviedol si parameter!\n");
							 return 1;
						 }
						 free(globalToken.string);
					 }
					 else if(globalToken.state == TK_MINUS){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_MINUS,0);
					 }
					 else if(globalToken.state == TK_NOTEQUAL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_NOTEQUAL,0);
					 }
					 else if(globalToken.state == TK_NIL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_NIL,0);
					 }
					 else if(globalToken.state == TK_DIV){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_DIV,0);
					 }
					 else if(globalToken.state == TK_INTEGER){
						 logDebugParser("is integer\n");
						 pushBack(&lList, globalToken.intValue, NULL, 0, LLTYPE_INTEGER,0);
					 }
					 else if(globalToken.state == TK_EQUAL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_EQUAL,0);
					 }
					 else if(globalToken.state == TK_MORE){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_MORE,0);
					 }
					 else if(globalToken.state == TK_MOREEQUAL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_MOREEQUAL,0);
					 }
					 else if(globalToken.state == TK_LESS){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_LESS,0);
					 }
					 else if(globalToken.state == TK_LESSEQUEL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_LESSEQUEL,0);
					 }
					 else if(globalToken.state == TK_MUL){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_MUL,0);
					 }
					 else if(globalToken.state == TK_COMMA){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_COMMA,0);
					 }
					 else if(globalToken.state == TK_STRING){
						 pushBack(&lList, 0, globalToken.string, 0, LLTYPE_STRING,0);
					 }
					 else if(globalToken.state == TK_LBRACKET){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_LBRACKET,0);
					 }
					 else if(globalToken.state == TK_RBRACKET){
						 pushBack(&lList, 0, NULL, 0, LLTYPE_RBRACKET,0);
					 }
					 else if(globalToken.state == TK_DECIMAL){
						pushBack(&lList, 0, NULL, globalToken.doubleValue, LLTYPE_DECIMAL,0);
					}
					else{
						logDebugParser("Mato sa zabudol\n");
					}
					getNextToken();
	}
	logDebugParser("pushnuty term\n");
	pushBack(&lList, 0, NULL, 0, LLTYPE_RBRACKET,0);
	logDebugParser("pushnuty bracket\n");
	if(activateEXPR(lList) == -1){
    return -1;
  }
	int res = exprEval(&lList->active);
  if(res  != OK){
		return res;
	}


	// printf("matoooo %d\n",lList->active->type);
	logDebugParser("sem sa to dostalo\n");
	if(lList->active == NULL){
		logDebugParser("list je null\n");
	}
	logDebugParser("list nie je null\n");
	if(lList->active->varType == EXP_INT){
		lastExpress = T_INT;
	}
	else if(lList->active->varType == EXP_STRING){
		lastExpress = T_STRING;
	}
	else if(lList->active->varType == EXP_DOUBLE){
		lastExpress = T_DOUBLE;
	}
	else if(lList->active->varType == EXP_UNKNOWN){
		lastExpress = T_PARAM;
	}
	else if(lList->active->varType == EXP_NIL){
		lastExpress = T_NIL;
	}
	else{
		logDebugParser("Mato sa zabudol - errr\n");
	}
	//generate code
	// freeList(&lList);
	logDebugParser("list ma typ\n");
	exitRule(EX,OK);
}

int VALUE(){
	if(!isValue()){
		printf("toto by sa nemalo stat!\n");
		exitRule(VALUE,PRSR_ERR);
	}
	callRule(OPMOD);
	exitRule(VALUE,OK);
}

int OPMOD(){
	getNextToken();
	if(isOPMOD()){
		getNextToken();
		callRule(VALUE);
	}
	exitRule(OPMOD,OK);
}

int main(){

	initFuncTree(&globalTree);
	initVarTree(&globalVarTree);

	// addBuildIn(&globalTree);
	addBuildIn(&globalTree);
	callRule(GP);

	freeFUNCTree(&globalTree);
	freeVarTree(&globalVarTree);


	freeString(&globalString);
	return OK;
}
