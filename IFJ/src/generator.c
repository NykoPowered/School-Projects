/* ****************** generator.c *******************\
|****************************************************|
|Implementace překladače imperativního jazyka IFJ18  |
|                                                    |
|team: 053/1                                         |
|                                                    |
|authors: Marek Hrivňák    xhrivn02                  |
|         Nikolas Masica   xmasic00                  |
|         René Rešetár     xreset00                  |
|FIT VUT                                             |
|****************************************************|
|************************************************** */


#include "generator.h"

int counter = 0;

int stack_else[1000];
int *els = stack_else;


#define push(els, n) (*((els)++) = (n))
#define pop(els) (*--(els))

int stack_end[1000];
int *ende = stack_end;

char *replace(const char *s, char ch, const char *repl) {
    int count = 0;
    const char *t;
    for(t=s; *t; t++)
        count += (*t == ch);

    size_t rlen = strlen(repl);
    char *res = malloc(strlen(s) + (rlen-1)*count + 1);
    char *ptr = res;
    for(t=s; *t; t++) {
        if(*t == ch) {
            memcpy(ptr, repl, rlen);
            ptr += rlen;
        } else {
            *ptr++ = *t;
        }
    }
    *ptr = 0;
    return res;
}

char *Replacator(char *s){
    s = replace(s, ' ', "\\032");
    s = replace(s, '\n', "\\010");
    return s;
}

/* ************ ZASOBNIKOVE FUNKCIE *************  */

void SInitP (StackVar *S)

// Inicializujem zásobník.

{
    S->top = 0;
    S->var_numbers = 0;
}

void SPushP (StackVar *S, var_struct_ptr *ptr)
// vloženie hodnoty na vrchol zaspbniku

{
    // chyba v pripade pretečenia zasobnika
    if (S->top==MAXSTACK)
        printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->var_numbers++;
        S->a[S->top]=(*ptr);
    }
}

void gRET(var_struct_ptr *param){
    printf("MOVE ");
    gParam(*param);
    printf("TF@%%retval\n");
}

var_struct_ptr STopPopP (StackVar *S)
/*         --------
 ** Odstraní prvok z vrcholu zásobníku a súčasne vrátí jeho hodnotu.
 **/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        var_struct_ptr CHYBA;
        CHYBA.name = NULL;
        printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
        return (CHYBA);
    }
    else {
        S->var_numbers--;
        return (S->a[S->top--]);
    }
}

bool SEmptyP (StackVar *S)
/*   -------
 ** Je-li zásobník prázdný, vrátí hodnotu true.
 **/
{
    return(S->top==0);
}

/* ************ ZASOBNIKOVE FUNKCIE *************  */

bool gCallFunction(StackVar *S, char *function_name){
    
    printf("CREATEFRAME\n");    // vytvori ramec pre funkciu
    int abakus = 0; //pocitadlo
    var_struct_ptr  parameter;
    int param_numbers = S->var_numbers;
    while(S->var_numbers != 0){     // while pokial je pocet parametrov 0
        
        printf("DEFVAR TF@$%d\n",++abakus);
        parameter = STopPopP(S);
        if (parameter.frame == GF || parameter.frame == LF || parameter.frame == TF){    // ak to je premenna
            
            printf("MOVE TF@$%d ", abakus);
            gParam(parameter);
            printf("\n");
            printf("PUSHS TF@$%d\n", abakus);
            
        }
        
        else if (parameter.vartype == int_type || parameter.vartype == float_type || parameter.vartype == string_type || parameter.vartype == nil_type){ // ak je to hodnota
            
            printf("MOVE TF@$%d ", abakus);
            gParam(parameter);
            printf("\n");
            printf("PUSHS TF@$%d\n", abakus);
        
        }
        else {    // ak nebol zadany ani ramec ani typ hodnoty
            fprintf(stderr,"ERROR  ! Wrong value type or vyriable frame! ERROR\n");
            return 4;
        
            }
    }
    if(!strcmp(function_name, "print")){
        printf("PUSHS int@%d\n",param_numbers);
    }
        printf("CALL $%s", function_name );
        printf("\n");
    	
    // doplnit Return Value
    
    return true;
}

bool gDeclareVar(var_struct_ptr *assign){
    
    if (assign->frame == GF || assign->frame == LF || assign->frame == TF){
        printf("DEFVAR ");
        gParam((*assign));
        printf("\n");
        
    }
    else {
        fprintf(stderr," ! FATAL ERROR !  NOTE DECLARING VARIABLE (FRAME NOT STATED)    ! FATALNA ERROR !  \n");
        return 4;
    }
    
    return true;
}

bool gAssign(var_struct_ptr *assign, var_struct_ptr *param1){

    if (assign->frame == GF || assign->frame == LF || assign->frame == TF){

            printf("MOVE ");
            gParam((*assign));
            gParam((*param1));
            printf("\n");
        
    }
    else {
        fprintf(stderr," ! FATAL ERROR! Canno assign to value  ! FATAL ERROR !  \n");
        return 4;
    }
    
    return true;
}

/* Definicia funkcie */
bool gFunctionDef(char *function_name, int number_of_parameters){
    
    if (function_name == NULL){
        fprintf(stderr,"ERROR! Function without a name! ERROR!\nParameter function_name == NULL");
        return 4;
    }
    
    printf("\n");
    printf("JUMP $$%s$$\n", function_name);
    printf("LABEL $%s \n", function_name);   // dolar v Label -> začiatok funkcie
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%retval\n");
  //  printf("DEFVAR LF@p_number\n");
  //  printf("POPS LF@p_number\n");   // zo zasobnika vyberame pocet parametrov
    int param_counter = 1;
    while (number_of_parameters > 0 ){
        printf("DEFVAR LF@PARAM%d \n", param_counter);
        printf("POPS LF@PARAM%d \n", param_counter);
        number_of_parameters--;
        param_counter++;
    }
    return true;
}

/* Koniec definicie funkcie */
bool gEndFunctionDef(char *function_name, var_struct_ptr *param){

    if (param != NULL){
          printf("MOVE LF@%%retval ");
          gParam((*param));
          printf("\n");
          printf("POPFRAME \n");
          printf("RETURN \n");
          printf("LABEL $$%s$$", function_name);
          printf("\n");
    }
     else{
          printf("POPFRAME \n");
          printf("RETURN \n");
          printf("LABEL $$%s$$", function_name);
          printf("\n");
    }
    return true;

}

/*  Zaciatok if podmienky po ktorom nasleduje sekvencia prikazov            */
/*  Priklad: if (a > b)                                                     */
/*  gIF(a, frame_or_vartype , b, frame_or_vartype, relational_operator);    */
bool gIF(var_struct_ptr *param1, var_struct_ptr *param2, relational_operator RO){

    int CON;
    int DECIDER2 = 0;
    counter++;
    push(els, counter);
    push(ende, counter);
    printf("DEFVAR LF@V%d\n", counter);

    if(param1->vartype == param2->vartype){
        CON = 1;
    }
    if(param1->vartype != param2->vartype){

        if(param1->vartype == string_type || param2->vartype == string_type){
            if(param1->vartype == string_type) {
                fprintf(stderr,"ERROR! First parameter in if condition is string! ERROR!\n");
            }
            if(param2->vartype == string_type){
                fprintf(stderr,"ERROR! Second parameter in if condition is string! ERROR!\n");
            }
            fprintf(stderr,"Cannot compare 1 string and 1 other type!\n");
            return 4;
        }
        else if(param1->vartype == int_type)
        {
            if(param1->frame == GF || param1->frame == LF) {
                printf("INT2FLOAT ");
                gParam((*param1));
                gParam((*param1));
                printf("\n");
                CON = 1;
                DECIDER2 = 1;
            }
            else
            {
                CON = 2;
            }
        }
        else if(param2->vartype == int_type)
        {
            if(param2->frame == GF || param2->frame == LF) {
                printf("INT2FLOAT ");
                gParam((*param2));
                gParam((*param2));
                printf("\n");
                CON = 1;
                DECIDER2 = 2;
            }
            else
            {
                CON = 3;
            }
        }
    }
        switch (RO) {
            case less :
                if(CON == 1) {
                    printf("LT LF@V%d ", counter);
                    gParam((*param1));
                    gParam((*param2));
                    printf("\n");
                    gF2I(param1, param2, DECIDER2);
                }
                else if(CON == 2){
                    printf("LT LF@V%d float@%a ", counter, (float)param1->int_value);
                    gParam((*param2));
                    printf("\n");
                }
                else if(CON == 3){
                    printf("LT LF@V%d ", counter);
                    gParam((*param1));
                    printf("float@%a\n", (float)param2->int_value);
                }
                printf("JUMPIFNEQ else%d LF@V%d bool@true\n", counter, counter);
                break;
            case lesseq :
                if(CON == 1) {
                    printf("GT LF@V%d ", counter);
                    gParam((*param1));
                    gParam((*param2));
                    printf("\n");
                    gF2I(param1, param2, DECIDER2);
                }
                else if(CON == 2){
                    printf("GT LF@V%d float@%a ", counter, (float)param1->int_value);
                    gParam((*param2));
                    printf("\n");
                }
                else if(CON == 3){
                    printf("GT LF@V%d ", counter);
                    gParam((*param1));
                    printf("float@%a\n", (float)param2->int_value);
                }
                printf("JUMPIFEQ else%d LF@V%d bool@true\n", counter, counter);
                break;
            case more :
                if(CON == 1) {
                    printf("GT LF@V%d ", counter);
                    gParam((*param1));
                    gParam((*param2));
                    printf("\n");
                    gF2I(param1, param2, DECIDER2);
                }
                else if(CON == 2){
                    printf("GT LF@V%d float@%a ", counter, (float)param1->int_value);
                    gParam((*param2));
                    printf("\n");
                }
                else if(CON == 3){
                    printf("GT LF@V%d ", counter);
                    gParam((*param1));
                    printf("float@%a\n", (float)param2->int_value);
                }
                printf("JUMPIFNEQ else%d LF@V%d bool@true\n", counter, counter);
                break;
            case moreeq :
                if(CON == 1) {
                    printf("LT LF@V%d ", counter);
                    gParam((*param1));
                    gParam((*param2));
                    printf("\n");
                    gF2I(param1, param2, DECIDER2);
                }
                else if(CON == 2){
                    printf("LT LF@V%d float@%a ", counter, (float)param1->int_value);
                    gParam((*param2));
                    printf("\n");
                }
                else if(CON == 3){
                    printf("LT LF@V%d ", counter);
                    gParam((*param1));
                    printf("float@%a\n", (float)param2->int_value);
                }
                printf("JUMPIFEQ else%d LF@V%d bool@true\n", counter, counter);
                break;
            case eq :
                if(CON == 1) {
                    printf("EQ LF@V%d ", counter);
                    gParam((*param1));
                    gParam((*param2));
                    printf("\n");
                    gF2I(param1, param2, DECIDER2);
                }
                else if(CON == 2){
                    printf("EQ LF@V%d float@%a ", counter, (float)param1->int_value);
                    gParam((*param2));
                    printf("\n");
                }
                else if(CON == 3){
                    printf("EQ LF@V%d ", counter);
                    gParam((*param1));
                    printf("float@%a\n", (float)param2->int_value);
                }
                printf("JUMPIFEQ else%d LF@V%d bool@false\n", counter, counter);
                break;
            case noteq :
                if(CON == 1) {
                    printf("EQ LF@V%d ", counter);
                    gParam((*param1));
                    gParam((*param2));
                    printf("\n");
                    gF2I(param1, param2, DECIDER2);
                }
                else if(CON == 2){
                    printf("EQ LF@V%d float@%a ", counter, (float)param1->int_value);
                    gParam((*param2));
                    printf("\n");
                }
                else if(CON == 3){
                    printf("EQ LF@V%d ", counter);
                    gParam((*param1));
                    printf("float@%a\n", (float)param2->int_value);
                }
                printf("JUMPIFNEQ else%d LF@V%d bool@false\n", counter, counter);
                break;
            default :
                fprintf(stderr,"ERROR! Unknown relation operator in if condition! ERROR!\n");
                return 4;
        }
        printf("LABEL if%d\n", counter);
    return true;
}

void gF2I(var_struct_ptr *param1, var_struct_ptr *param2, int N){
    if(N == 1) {
        printf("FLOAT2INT ");
        gParam((*param1));
        gParam((*param1));
        printf("\n");
    }else if(N == 2){
        printf("FLOAT2INT ");
        gParam((*param2));
        gParam((*param2));
        printf("\n");
    }
    else if(N != 0){
        fprintf(stderr,"ERROR! Calling function gF2I with wrong argument N! ERROR!\n");
        return 4;
    }
}

/* Zaciatok else podmienky po ktorej nasleduje sekvencia prikazov */
/* Priklad: else "sekvencia"                                      */
void gELSE(){

    int x = pop(els);
    printf("JUMP end%d\n",x );
    printf("LABEL else%d\n", x);

}

/* Koniec if podmienky po ktorej pokracujeme v kode               */
void gIF_END(void){

    int y = pop(ende);
    printf("LABEL end%d\n", y);

}

bool gWHILE(var_struct_ptr *param1, var_struct_ptr *param2, relational_operator RO){

    int CON2;
    int DECIDER = 0;
    counter++;
    push(els, counter);
    printf("DEFVAR LF@V%d\n", counter);
    printf("LABEL while%d\n", counter);
    if(param1->vartype == param2->vartype){
        CON2 = 1;
    }
    else if(param1->vartype != param2->vartype) {

        if(param1->vartype == string_type || param2->vartype == string_type){
            if(param1->vartype == string_type) {
                fprintf(stderr,"ERROR! First parameter in while condition is string! ERROR!\n");
            }
            if(param2->vartype == string_type){
                fprintf(stderr,"ERROR! Second parameter in while condition is string! ERROR!\n");
            }
            fprintf(stderr,"Cannot compare 1 string and 1 other type!\n");
            return 4;
        }

        else if(param1->vartype == int_type){
            if(param1->frame == GF || param1->frame == LF ){
                printf("INT2FLOAT ");
                gParam((*param1));
                gParam((*param1));
                printf("\n");
                CON2 = 1;
                DECIDER = 1;
            }
            else{
                CON2 = 2;
            }
        }
        else if(param2->vartype == int_type){
            if(param2->frame == GF || param2->frame == LF ){
                printf("INT2FLOAT ");
                gParam((*param2));
                gParam((*param2));
                printf("\n");
                CON2 = 1;
                DECIDER = 2;
            }
            else{
                CON2 = 3;
            }
        }
    }

    switch(RO) {
        case less :
            if(CON2 == 1){
                printf("LT LF@V%d ", counter);
                gParam((*param1));
                gParam((*param2));
                printf("\n");
                gF2I(param1, param2, DECIDER);
                printf("JUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 2){
                printf("LT LF@V%d flaot@%a ", counter, (float)param1->int_value);
                gParam((*param2));
                printf("\nJUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 3) {
                printf("LT LF@V%d ", counter);
                gParam((*param1));
                printf("float@%a\n", (float) param2->int_value);
                printf("JUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            break;
        case lesseq :
            if(CON2 == 1){
                printf("GT LF@V%d ", counter);
                gParam((*param1));
                gParam((*param2));
                printf("\n");
                gF2I(param1, param2, DECIDER);
                printf("JUMPIFEQ end_while%d LF@V%d bool@true\n", counter, counter);
            }
            else if(CON2 == 2){
                printf("GT LF@V%d float@%a ", counter, (float)param1->int_value);
                gParam((*param2));
                printf("\nJUMPIFEQ end_while%d LF@V%d bool@true\n", counter, counter);
            }
            else if(CON2 == 3){
                printf("GT LF@V%d ", counter);
                gParam((*param1));
                printf("float@%a\n", (float)param2->int_value);
                printf("JUMPIFEQ end_while%d LF@V%d bool@true\n", counter, counter);
            }
            break;
        case more :
            if(CON2 == 1){
                printf("GT LF@V%d ", counter);
                gParam((*param1));
                gParam((*param2));
                printf("\n");
                gF2I(param1, param2, DECIDER);
                printf("JUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 2){
                printf("GT LF@V%d flaot@%a ", counter, (float)param1->int_value);
                gParam((*param2));
                printf("\nJUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 3){
                printf("GT LF@V%d ",counter);
                gParam((*param1));
                printf("float@%a\n", (float)param2->int_value);
                printf("JUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            break;
        case moreeq :
            if(CON2 == 1){
                printf("LT LF@V%d ", counter);
                gParam((*param1));
                gParam((*param2));
                printf("\n");
                gF2I(param1, param2, DECIDER);
                printf("JUMPIFNEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 2){
                printf("LT LF@V%d float@%a ", counter, (float)param1->int_value);
                gParam((*param2));
                printf("\nJUMPIFNEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 3){
                printf("GT LF@V%d ", counter);
                gParam((*param1));
                printf("float@%a\n", (float)param2->int_value);
                printf("JUMPIFNEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            break;
        case eq :
            if(CON2 == 1) {
                printf("EQ LF@V%d ", counter);
                gParam((*param1));
                gParam((*param2));
                printf("\n");
                gF2I(param1, param2, DECIDER);
                printf("JUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 2){
                printf("EQ LF@V%d float@%a ", counter, (float)param1->int_value);
                gParam((*param2));
                printf("\nJUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            else if(CON2 == 3){
                printf("EQ LF@V%d ", counter);
                gParam((*param1));
                printf("float@%a\n", (float)param2->int_value);
                printf("JUMPIFEQ end_while%d LF@V%d bool@false\n", counter, counter);
            }
            break;
        case noteq :
            if(CON2 == 1) {
                printf("EQ LF@V%d ", counter);
                gParam((*param1));
                gParam((*param2));
                printf("\n");
                gF2I(param1, param2, DECIDER);
                printf("JUMPIFEQ end_while%d LF@V%d bool@true\n", counter, counter);
            }
            else if(CON2 == 2){
                printf("EQ LF@V%d float@%a ", counter, (float)param1->int_value);
                gParam((*param2));
                printf("\nJUMPIFEQ end_while%d LF@V%d bool@true\n", counter, counter);
            }
            else if(CON2 == 3){
                printf("EQ LF@V%d ", counter);
                gParam((*param1));
                printf("float@%a\n", (float)param2->int_value);
                printf("JUMPIFEQ end_while%d LF@V%d bool@true\n", counter, counter);
            }
            break;
        default :
            fprintf(stderr,"ERROR! Unknown relation operator in while condition! ERROR!\n");
            return 4;
    }
    return true;
}

void gWHILE_END(var_struct_ptr *param1, var_struct_ptr *param2){

    int z = pop(els);
    printf("JUMP while%d\n", z);
    printf("LABEL end_while%d\n", z);
    if(param1->vartype != param2->vartype) {
        if (param1->vartype == float_type)
            gF2I(param1, param2, 1);
        else if (param2->vartype == float_type)
            gF2I(param1, param2, 2);
    }
}


bool gMainStart()
{
	printf("LABEL $$main\n");
	printf("CREATEFRAME\n");
	printf("PUSHFRAME\n");

	return true;
}

bool gMainEnd()
{

	printf("POPFRAME\n");
	printf("CLEARS\n");

	return true;
}


bool gParam(var_struct_ptr param){
    
    char *type;
    
    switch(param.frame) {
        case GF :                                //case 0: Globalna premenna
            type = "GF";
            printf("%s@%s ",type, param.name);
            break;
        case LF :                                //case 1: Lokalna premenna
            type = "LF";
            printf("%s@%s ",type, param.name);
            break;
        case TF :
            type = "TF";
            printf("%s@%s ",type, param.name);
            break;
        case undefined :
            switch (param.vartype) {
                case int_type :
                    type = "int";                               //case 3: Hodnota typu integer
                    printf("%s@%d ", type, param.int_value);
                    break;
                case float_type :                                //case 4: Hodnota tyu float
                    type = "float";
                    //arg1 = atof((*premenna1)->float_value);
                    printf("%s@%a ", type, param.float_value);
                    break;
                case string_type :                                //case 5: Hodnota typu string
                    type = "string";
                    printf("%s@%s ", type, param.str_value);
                    break;
                case nil_type :                                   //case 6: Hodnota typu nil
                    type = "nil";
                    printf("%s@%s ", type, type);
                    break;
                default:
                    fprintf(stderr,"ERROR! Unknown type of value! ERROR!\n");
                    return 4;
            }
        break;
        default :                               //default: Hodnota nespravneho typu
            fprintf(stderr,"ERROR! Unknown type of variable! ERROR!\n");
            return 4;
    }
    return true;
}

/*
 *  a = b + c
 *  gADDFunction(->a, typ hodnoty/frame premennej a, ->b, typ hodnoty/frame premennej b, ->c, typ hodnoty/frame premennej c, )
 */
bool gADDFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2){

    if (param1->vartype == param2->vartype) {                   // Oba parametry su rovnake, netreba robit prevod na iny typ
    
        if (param1->vartype != string_type) {                   // vpriprade ze sa nejedna o string
            printf("ADD ");                                     // klasicke ADD
            gParam((*assign));
            gParam((*param1));
            gParam((*param2));
            printf("\n");
        }
        else {                                                  // vpripade ze sa jedna o string
            printf("CONCAT ");                                  // Volame instrukciu CONCAT
            gParam((*assign));
            gParam((*param1));
            gParam((*param2));
            printf("\n");
        }
    }
    else {                                                      // Ak su rozdielneho typu
        if (param1->vartype == string_type) {
            fprintf(stderr,"ERROR! Function gADDFunction is called with first parameter of string type! ERROR!\n");
            return 4;
        } else if (param2->vartype == string_type) {
            fprintf(stderr,"ERROR! Function gADDFunction is called with second parameter of string type! ERROR!\n");
            return 4;
        }

        else if (param1->vartype == float_type){                     // Tak ak jeden je float druhy musi byt integer
                                                                // Preto budeme pracovat s 2 parametrom
    		if(param2->frame == GF || param2->frame == LF){     // Ak je to premenna tak volame instrukciu INT2FLOAT
				printf("INT2FLOAT ");
            	gParam(*param2);
            	gParam(*param2);
            	printf("\n");
            	printf("ADD ");
            	gParam(*assign);
            	gParam(*param1);
            	gParam(*param2);
            	printf("\n");
            	gF2I(param1, param2, 2);
            }
            else{                                               // Pokial sa jedna o hodnotu, pre jednoduchost urobime prevod u nas v generatore
            	printf("ADD ");
            	gParam(*assign);
            	gParam(*param1);
            	printf("float@%a\n",(float)param2->int_value);
            }
        }
        else{
                                                                // Prvy param nebol float, takze druhy param musi byt float
        	if(param1->frame == GF || param1->frame == LF){                         // Preto pracujeme s prvym parametrom
            	printf("INT2FLOAT ");                           // Ak je to premenna opat volame instrukciu INT2FLOAT
            	gParam(*param1);
            	gParam(*param1);
            	printf("\n");
            	printf("ADD ");
            	gParam(*assign);
            	gParam(*param1);
            	gParam(*param2);
            	printf("\n");
                gF2I(param1, param2, 1);
            }
            else{                                               //Ak sa jedna o hodnotu, urobime prevod v generatore
            	printf("ADD ");
            	gParam(*assign);
            	printf("float@%a ",(float)param1->int_value);
            	gParam(*param2);
            	printf("\n");
            }
        }
    }
 return true;
}

/*
 *  a = b - c
 *  gSUBFunction(->a, typ hodnoty/frame premennej a, ->b, typ hodnoty/frame premennej b, ->c, typ hodnoty/frame premennej c, )
 */
bool gSUBFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2){

    if (param1->vartype == param2->vartype) {                   // V pripade ze su obidve argumenty rovnake
        
        if (param1->vartype != string_type){                    // V pripade ze parametre niesu string
                printf("SUB ");
                gParam((*assign));
                gParam((*param1));
                gParam((*param2));
                printf("\n");
        }
        else{                                                   // Ak to su stringy, ERROR
            fprintf(stderr,"ERROR! Function gSUBFunction is called with both string parameters! ERROR!\n");
            return 4;
        }
    }
    else{                                                                               // Ak argumenty nie su rovnaekho typu
        if(param1->vartype == string_type) {
            fprintf(stderr,"ERROR! Function gSUBFunction is called with first parameter of string type! ERROR!\n");
            return 4;
        }
        else if(param2->vartype == string_type) {
            fprintf(stderr,"ERROR! Function gSUBFunction is called with first parameter of string type! ERROR!\n");
            return 4;
        }
        else if(param1->vartype == float_type) {                                       // A prvy argument je typu float
            // Pracujeme s druhym argumentom
            if (param2->frame == GF || param2->frame == LF) {                          // Ak je to premenna
                printf("INT2FLOAT ");                                                  // Volame instrukciu INT2FLOAT
                gParam(*param2);
                gParam(*param2);
                printf("\n");
                printf("SUB ");
                gParam(*assign);
                gParam(*param1);
                gParam(*param2);
                printf("\n");
                gF2I(param1, param2, 2);
            } else {                                                                      // Ak je to hodnota prevod urobime v generatore
                printf("SUB ");
                gParam(*assign);
                gParam(*param1);
                printf("float@%a\n", (float) param2->int_value);
            }
        }
        else {                                                                               // Prvy argument nie je float, takze druhy je
            if (param1->frame == GF || param1->frame == LF) {                                                     // Ak prvy je premenna
                printf("INT2FLOAT ");                                                       // Volame instrukciu INT2FLOAT
                gParam(*param1);
                gParam(*param1);
                printf("\n");
                printf("SUB ");
                gParam(*assign);
                gParam(*param1);
                gParam(*param2);
                printf("\n");
                gF2I(param1, param2, 1);
            } else {                                                                           // Ak je to hodnota
                printf("SUB ");                                                             // Prevod robime v generatore
                gParam(*assign);
                printf("float@%a ", (float) param1->int_value);
                gParam(*param2);
                printf("\n");
            }
        }
    }
    return true;
}

/*
 *  a = b * c
 *  gMULFunction(->a, typ hodnoty/frame premennej a, ->b, typ hodnoty/frame premennej b, ->c, typ hodnoty/frame premennej c, )
 */
bool gMULFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2){

	if (param1->vartype == param2->vartype) {                                               // V pripade ze su obidva argumenty rovnake
        
        if ( param1->vartype != string_type ){                                              // V pripade ze parametre niesu string
                printf("MUL ");                                                             // Staci nam zavolat MUL     
                gParam((*assign));
                gParam((*param1));
                gParam((*param2));
                printf("\n");
        }
        else{
            fprintf(stderr,"ERROR! Function gMULFunction is called with both string parameters! ERROR!\n"); // Ak su to stringy, vraciame ERROR
            return 4;
        }
    }
    else {                                                                                  // Argumenty su ineho typu
        if (param1->vartype == string_type) {
            fprintf(stderr,"ERROR! Function gMULFunction is called with first parameter of string type! ERROR!\n");
            return 4;
        } else if (param2->vartype == string_type) {
            fprintf(stderr,"ERROR! Function gMULFunction is called with first parameter of string type! ERROR!\n");
            return 4;
        } else if (param1->vartype ==
                   float_type) {                                             // Ak je prvy argument typu float, pracujeme z druhym argumentom

            if (param2->frame == GF || param2->frame ==
                                       LF) {                                                     // Ak je druhy argument premenna
                printf("INT2FLOAT ");                                                       // Volame instrukciu INT2FLOAT
                gParam(*param2);
                gParam(*param2);
                printf("\n");
                printf("MUL ");
                gParam(*assign);
                gParam(*param1);
                gParam(*param2);
                printf("\n");
                gF2I(param1, param2, 2);
            } else {                                                                           // Ak je druhy argument hodnota prevod urobime v generatore
                printf("MUL ");
                gParam(*assign);
                gParam(*param1);
                printf("float@%a\n", (float) param2->int_value);
            }
        } else {                                                                              // Ak prvy argument nie je typu float, druhy je typu flaot
            if (param1->frame == GF || param1->frame ==
                                       LF) {                                                     // Pracujeme s prvym, ktory ak je premenna
                printf("INT2FLOAT ");                                                       // Volame instrukciu INT2FLOAT
                gParam(*param1);
                gParam(*param1);
                printf("\n");
                printf("MUL ");
                gParam(*assign);
                gParam(*param1);
                gParam(*param2);
                printf("\n");
                gF2I(param1, param2, 1);
            } else {                                                                           // Ak je to hodnota
                printf("MUL ");                                                             // Prevod urobime v generatore
                gParam(*assign);
                printf("float@%a ", (float) param1->int_value);
                gParam(*param2);
                printf("\n");
            }
        }
    }
    return true;
}

/*
 *  a = b / c
 *  gDIVFunction(->a, typ hodnoty/frame premennej a, ->b, typ hodnoty/frame premennej b, ->c, typ hodnoty/frame premennej c, )
 */

bool gDIVFunction(var_struct_ptr *assign, var_struct_ptr *param1, var_struct_ptr *param2){

    if ((param2->float_value == 0.0) && (param2->int_value == 0)){                          // Ak je delitel float 0.0 alebo int 0
        fprintf(stderr,"ERROR! Function gDIVFunction is called with value 0 or 0.0 in divisor! ERROR!\n");
    	return 9;                                                                       // ERROR
  	}

    if (param1->vartype == param2->vartype) {                                               // Delitel aj Delenec su rovnakeho typu

        if(param1->vartype == string_type){
            fprintf(stderr,"ERROR! Function gDIVFunction is called with both divisor and dividend in string_type! ERROR!\n");
            return 4;
        }
        else if(assign->vartype != param1->vartype) {
                                                                                                // Premenna priradenie je ineho typu
            if (param1->vartype == int_type) {                                              // Premenna priradenia je typu float a potrebujeme ju int
                printf("FLOAT2INT ");
                gParam(*assign);
                gParam(*assign);
                printf("\n");
            }
            else if (param1->vartype == float_type) {                                            // Premenna priradenia je typu int a potrebujeme ju float
                printf("INT2FLOAT ");
                gParam(*assign);
                gParam(*assign);
                printf("\n");
            }
        }

        else if (param1->vartype == float_type) {                                                 // Delitel, Delenec ani premenna priradenia nie su typu string ale float

            printf("DIV ");
            gParam((*assign));
            gParam((*param1));
            gParam((*param2));
            printf("\n");

        }
        else if (param1->vartype == int_type) {                                             // Delitel, Delenec ani premenna priradenia nie su typu string ale int

            printf("IDIV ");
            gParam((*assign));
            gParam((*param1));
            gParam((*param2));
            printf("\n");

        }
    }
    else{                                                                                   // Delitel a Delenec su ineho typu

        if (param1->vartype == string_type){                                                // Ak je aspon jeden argument typu string
            fprintf(stderr,"ERROR! Function gDIVFunction is called with dividend of string type! ERROR!\n");
            return 4;
        }
        else if(param2->vartype == string_type){
            fprintf(stderr,"ERROR! Function gDIVFunction is called with divisor of string type! ERROR!\n");
            return 4;
        }

        else if(assign->vartype == int_type){                                               // Premenna priradenia je typu int
    			printf("INT2FLOAT ");                                                       // Prevedieme ju na float typ                   
            	gParam(*assign);
            	gParam(*assign);
            	printf("\n");
    	}

    	else if (param1->vartype == float_type){                                            // Delenec je typu float, takze Delitel je int
                                                                                            // Pracujeme s Delitelom
    		if(param2->frame == GF || param2->frame == LF){                                 // Ak je Delitel premenna volame instrukciu INT2FLOAT
				printf("INT2FLOAT ");
            	gParam(*param2);
            	gParam(*param2);
            	printf("\n");
            	printf("DIV ");
            	gParam(*assign);
            	gParam(*param1);
            	gParam(*param2);
            	printf("\n");
                gF2I(param1, param2, 2);
            }
            else                                                                            // Ak je Delitel hodnota, prevedieme ju v generatore
            {
            	printf("DIV ");
            	gParam(*assign);
            	gParam(*param1);
            	printf("float@%a\n",(float)param2->int_value);
            }
        }
        else{                                                                               // Ak Delenec nie je typu float, tak je typu int
        	if(param1->frame == GF || param1->frame == LF){                                                     // Pracujeme s Delencom
            	printf("INT2FLOAT ");                                                       // Ak je Denelec premenna, volame instrukciu INT2FLOAT
            	gParam(*param1);
            	gParam(*param1);
            	printf("\n");
            	printf("DIV ");
            	gParam(*assign);
            	gParam(*param1);
            	gParam(*param2);
            	printf("\n");
                gF2I(param1, param2, 1);
            }
            else                                                                            // Ak je Delenec hodnota, prevedieme ju v generatore    
            {
            	printf("DIV ");
            	gParam(*assign);
            	printf("float@%a ",(float)param1->int_value);
            	gParam(*param2);
            	printf("\n");
            }
        }
    }
 	return true;
}

void gFileHeader()
{

	printf(".IFJcode18\n");
	printf("JUMP $$main\n");
	
}

void gBuildInFunctions(void){
	
	gINPUTF();
	gINPUTI();
	gINPUTS();
    gORD();
    gLENGTH();
    gCHR();
    gSUBSTR();
    gPRINT();
    
}

void gINPUTI(){
	
	/**
	 *Inputi
	 */

	gFunctionDef("inputi", 0);
	printf("READ LF@%%retval int\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$", "inputi");
    printf("\n");

	
}

void gINPUTF(){
	
	/**
	 *Inputf
	 */
	gFunctionDef("inputf", 0);
	printf("READ LF@%%retval float\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$", "inputf");
    printf("\n");

	
}

void gINPUTS(){
	
	/**
	 *Inputs
	 */
	gFunctionDef("inputs", 0);
	printf("READ LF@%%retval string\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$", "inputs");
    printf("\n");
	
}

void gLENGTH(){
    // zisti dlzku retazca
	gFunctionDef("length", 1);
    printf("STRLEN LF@%%retval LF@PARAM1\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$", "length");
    printf("\n");
    
}

void gORD(){
     //# vrati ASCII hodnotu znaku zo str
    gFunctionDef("ord", 2);
	printf("STRI2INT LF@%%retval LF@PARAM1 LF@PARAM2\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$", "ord");
    printf("\n");
    
}

void gCHR(){
    // prevedie ASCII na znak
	gFunctionDef("chr", 1);
    printf("INT2CHAR LF@%%retval LF@PARAM1\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$", "chr");
    printf("\n");
    
}


void gSUBSTR(void){
    // vypise podretazec zo stringu
    gFunctionDef("substr", 3);
    printf("MOVE LF@%%retval string@ \n");
    printf("DEFVAR LF@help\n");
	printf("DEFVAR LF@length\n");
	printf("DEFVAR LF@condition\n");
	printf("DEFVAR LF@difference\n");
	
	printf("STRLEN LF@length LF@PARAM1\n");
	printf("GT LF@condition LF@length LF@PARAM2\n");
	printf("JUMPIFEQ err LF@condition bool@false\n");
	
	printf("GT LF@condition LF@PARAM2 int@-1\n");
	printf("JUMPIFEQ err LF@condition bool@false\n");
	
	printf("SUB LF@difference LF@length LF@PARAM2\n");
	printf("LT LF@condition LF@difference LF@PARAM3\n");
	printf("JUMPIFEQ exception LF@condition bool@true\n");
	
    printf("LABEL repeat\n");
    printf("GETCHAR LF@help LF@PARAM1 LF@PARAM2\n");
    printf("CONCAT LF@%%retval LF@%%retval LF@help\n");
    printf("SUB LF@PARAM3 LF@PARAM3 int@1\n");
    printf("ADD LF@PARAM2 LF@PARAM2 int@1\n");
    printf("JUMPIFNEQ repeat LF@PARAM3 int@0\n");
	
	printf("JUMP err\n");
	
	printf("LABEL exception\n");
	printf("LABEL repeat_2\n");
	printf("GETCHAR LF@help LF@PARAM1 LF@PARAM2\n");
    printf("CONCAT LF@%%retval LF@%%retval LF@help\n");
	printf("ADD LF@PARAM2 LF@PARAM2 int@1\n");
	printf("JUMPIFNEQ repeat_2 LF@PARAM2 LF@length\n");
	
	printf("LABEL err\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$\n", "substr");
    printf("\n");

}


void gPRINT(void){

    /**
     * Print
     */
    printf("\n");
    printf("JUMP $$%s$$\n", "print");
	printf("LABEL $print\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@number\n");
    printf("POPS LF@number\n");
    printf("DEFVAR LF@retval\n");
    printf("LABEL $for\n");
    printf("POPS LF@retval\n");
    printf("WRITE LF@retval\n");
    printf("SUB LF@number LF@number int@1\n");
    printf("JUMPIFNEQ $for LF@number int@0\n");
    printf("MOVE LF@retval nil@nil\n");
    printf("POPFRAME \n");
    printf("RETURN \n");
    printf("LABEL $$%s$$\n", "print");

	
}




