/******************* semantic.c ********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/


//kontrolovat ci funkcie nemaju rovnake meno ako premenne

#include "semantic.h"

#define DEBUGSEM 0

void logDebugSemantic(const char *string){
  if(DEBUGSEM == 0){
    return;
  }
  printf("%s\n",string);
}


int firstStringisGreaterSEM(char *a, char *b){
  return strcmp(a,b); //1 if greater
}

//vrati 0 ak nie je definovana a 1 ak je
int isFuncDefined(TFuncTree *tree,char *funcName){
  if(tree->data == NULL){
    return 0;
  }
  TFunc* tempFunc = findNodeFunc(tree, funcName);
  if(tempFunc == NULL){
    return 0;
  }
  return 1;

}

//vrati 0 ak su argumenty zle a 1 ak su dobry pocet
int checkParamsCount(TFuncTree *tree,char *funcName, MyStack *parameters){
  if(tree->data == NULL){
    printf("toto by sa nemalo stat! - semantic\n");
    return 3;
  }
  TFunc* tempFunc = findNodeFunc(tree, funcName);
  if(tempFunc == NULL){
    printf("Vyskytla sa vazna chyba!\n");
    return 0;
  }

  if(tempFunc->paramsCount == parameters->size){
    return 1;
  }
  else{
    return 0;
  }
}


//vrati 0 ak su argumenty zle a 1 ak su dobry pocet
int checkParamsCountREAL(TFuncTree *tree,char *funcName, int paramsCount){
  if(tree->data == NULL){
    printf("toto by sa nemalo stat! - semantic\n");
    return 3;
  }
  if(!strcmp(funcName,"print") && paramsCount > 0){
    return 1;
  }
  else if(!strcmp(funcName,"print") && paramsCount == 0){
    return 0;
  }
  TFunc* tempFunc = findNodeFunc(tree, funcName);
  if(tempFunc == NULL){
    printf("Vyskytla sa vazna chyba!\n");
    return 0;
  }
  // printf("%d\n",tempFunc->paramsCount);
  // printf("%d\n",paramsCount);
  if(tempFunc->paramsCount == paramsCount){
    return 1;
  }
  else{
    return 0;
  }
}

//vrati 0 ak nie je definovana a 1 ak je
int isVarDefined(TVarTree *tree,char *varName){
  if(tree == NULL){
    logDebugSemantic("no tree!");
    return 0;
  }
  if(tree->data == NULL){
    logDebugSemantic("no data!");
    return 0;
  }

  logDebugSemantic("finding node!");
  TVar* tempVar = findNodeVar(tree, varName);
  logDebugSemantic("found node!");
  if(tempVar == NULL){
    return 0;
  }
    return 1;
}

DataTYPE getVarType(TVarTree *tree,char *varName){
  if(tree->data == NULL){
    return 0;
  }
  TVar* tempVar = findNodeVar(tree, varName);
  return tempVar->typeVar;
}

//vrati 0 ak niesu 1 ak su a 3 ked je zla chyba
int allDependenciesOK(TFuncTree *tree,char *funcName){
  if(tree->data == NULL){
    printf("toto by sa nemalo stat! - semantic\n");
    return 3;
  }
  TFunc* tempFunc = findNodeFunc(tree, funcName);
  if(tempFunc == NULL){
    printf("nastala chyba!\n");
    return 3;
  }

  if(activate(tempFunc->dependencies) == -1){
    return 3;
  }
  while(tempFunc->dependencies->active != NULL){
    if(!isFuncDefined(tree,tempFunc->dependencies->active->stringValue)){
      return 0;
    }
    if(next(tempFunc->dependencies) == -1){
      return 3;
    }
  }
  return 1;
}

//vrati 0 ak niesu 1 ak su a 3 ked je zla chyba
int allDependenciesPARAMSOK(TFuncTree *tree,char *funcName){
  if(tree->data == NULL){
    printf("toto by sa nemalo stat! - semantic\n");
    return 3;
  }
  TFunc* tempFunc = findNodeFunc(tree, funcName);
  if(tempFunc == NULL){
    printf("nastala chyba!\n");
    return 3;
  }

  if(activate(tempFunc->dependencies) == -1){
    return 3;
  }
  while(tempFunc->dependencies->active != NULL){
    if(!checkParamsCountREAL(tree,tempFunc->dependencies->active->stringValue,tempFunc->dependencies->active->intValue)){
      logDebugSemantic("pocet parametrov je zly!");

      return 0;
    }
    if(next(tempFunc->dependencies) == -1){
      return 3;
    }
  }
  return 1;
}



//prida parameter na top zasobnika
int addParams(TFuncTree **tree,char *funcName, char *paramName){
  if((*tree) == NULL){
    logDebugSemantic("chybny strom bol predany - addPArams!");
    return -1;
  }
  if((*tree)->data == NULL){
    logDebugSemantic("strom nema data - addPArams!");
    return -1;
  }
  logDebugSemantic("tree is ok!");
  TFuncTree *tempTree = *tree;
  while(strcmp((tempTree)->data->funcName,funcName)){
    if(firstStringisGreaterSEM((tempTree)->data->funcName,funcName) > 0){
      (tempTree) = (tempTree)->LNode;
      logDebugSemantic("to left!");
    }
    else{
      (tempTree) = (tempTree)->RNode;
      logDebugSemantic("to right!");
    }
    if((tempTree) == NULL || (tempTree)->data == NULL){
      logDebugSemantic("Nastala vazna chyba asi!");
      return -1;
    }
  }

  if(strcmp((tempTree)->data->funcName,funcName)){
    logDebugSemantic("program nerobi to co ma!");
    return -1;
  }
  if(pushTop(&((tempTree)->data->parameters), 0, paramName, 0, SSTRING)){
    logDebugSemantic("nastala chyba v zasobniku! - Add Params!") ;
    return -1;
  }
  tempTree->data->paramsCount++;
  return 0;
}

//prida dependency na top zasobnika
int addDependency(TFuncTree **tree,char *funcName, char *dependencyName, int paramsCount){
  if((*tree) == NULL){
    logDebugSemantic("chybny strom bol predany - addDependency!");
    return -1;
  }
  if((*tree)->data == NULL){
    logDebugSemantic("strom nema data - addDependency!");
    return -1;
  }
  logDebugSemantic("tree is ok!");
  TFuncTree *tempTree = *tree;
  while(strcmp((tempTree)->data->funcName,funcName)){
    if(firstStringisGreaterSEM((tempTree)->data->funcName,funcName) > 0){
      (tempTree) = (tempTree)->LNode;
      logDebugSemantic("to left!");
    }
    else{
      (tempTree) = (tempTree)->RNode;
      logDebugSemantic("to right!");
    }
    if((tempTree) == NULL || (tempTree)->data == NULL){
      logDebugSemantic("Nastala vazna chyba asi!");
      return -1;
    }
  }

  if(strcmp((tempTree)->data->funcName,funcName)){
    logDebugSemantic("program nerobi to co ma!");
    return -1;
  }
  //sledovat aj parametre
  if(pushTop(&((tempTree)->data->dependencies), paramsCount, dependencyName, 0, SSTRING)){
    logDebugSemantic("nastala chyba v zasobniku! - Add Params!") ;
    return -1;
  }
  return 0;
}

void ARootBFuncSEM(TFuncTree *tree){
  if(tree == NULL){
    return;
  }

  ARootBFuncSEM(tree->LNode);

  printf("func name: %s\n",tree->data->funcName);

  MyStack *params = tree->data->parameters;
  MyStack *depends = tree->data->dependencies;
  if(tree->data->parameters->size > 0){
    printf("*params*:\n");
    if(activate(params) == -1){
      printf("vyskytla sa chyba - activate - AROOTFuncSem!\n");
      return;
    }
    while(params->active != NULL){
      printf("%s ",params->active->stringValue);
      if(next(params) == -1){
        printf("vyskytla sa chyba - next - AROOTFuncSem!\n");
        return;
      }
    }
  }
  printf("\n");

  if(tree->data->dependencies->size > 0){
    printf("*depends*:\n");
    if(activate(depends) == -1){
      printf("vyskytla sa chyba - activate - AROOTFuncSem!\n");
      return;
    }
    while(depends->active != NULL){
      printf("%s ",depends->active->stringValue);
      if(next(depends) == -1){
        printf("vyskytla sa chyba - next - AROOTFuncSem!\n");
        return;
      }
    }
  }
  printf("\n");

  ARootBFuncSEM(tree->RNode);
}

//always while init table
int addBuildIn(TFuncTree **tree){

  TFunc *inputs = malloc(sizeof(TFunc));
  inputs->typeVar = RT_STRING;
  char *str_inputs = malloc(sizeof(char)*20);
  str_inputs[0] = 'i';
  str_inputs[1] = 'n';
  str_inputs[2] = 'p';
  str_inputs[3] = 'u';
  str_inputs[4] = 't';
  str_inputs[5] = 's';
  str_inputs[6] = '\0';
  inputs->funcName = str_inputs;
  inputs->paramsCount = 0;
  inputs->dependencies = initStack();
  inputs->parameters = initStack();
  if(insertInFUNCTree(tree,&inputs)){
    printf("Chyba inputs\n");
    return -1;
  }

  TFunc *inputi = malloc(sizeof(TFunc));
  inputi->typeVar = RT_INT;
  char *str_inputi = malloc(sizeof(char)*20);
  str_inputi[0] = 'i';
  str_inputi[1] = 'n';
  str_inputi[2] = 'p';
  str_inputi[3] = 'u';
  str_inputi[4] = 't';
  str_inputi[5] = 'i';
  str_inputi[6] = '\0';
  inputi->funcName = str_inputi;
  inputi->paramsCount = 0;
  inputi->dependencies = initStack();
  inputi->parameters = initStack();
  if(insertInFUNCTree(tree,&inputi)){
    printf("Chyba inputi\n");
    return -1;
  }

  TFunc *inputf = malloc(sizeof(TFunc));
  inputf->typeVar = RT_DOUBLE;
  char *str_inputf = malloc(sizeof(char)*20);
  str_inputf[0] = 'i';
  str_inputf[1] = 'n';
  str_inputf[2] = 'p';
  str_inputf[3] = 'u';
  str_inputf[4] = 't';
  str_inputf[5] = 'f';
  str_inputf[6] = '\0';
  inputf->funcName = str_inputf;
  inputf->paramsCount = 0;
  inputf->dependencies = initStack();
  inputf->parameters = initStack();
  if(insertInFUNCTree(tree,&inputf)){
    printf("Chyba inputf\n");
    return -1;
  }

  TFunc *print = malloc(sizeof(TFunc));
  print->typeVar = RT_NIL;
  char *str_print = malloc(sizeof(char)*20);
  str_print[0] = 'p';
  str_print[1] = 'r';
  str_print[2] = 'i';
  str_print[3] = 'n';
  str_print[4] = 't';
  str_print[5] = '\0';
  print->funcName = str_print;
  print->paramsCount = 0;
  print->dependencies = initStack();
  print->parameters = initStack();
  if(insertInFUNCTree(tree,&print)){
    printf("Chyba print\n");
    return -1;
  }

  TFunc *length = malloc(sizeof(TFunc));
  length->typeVar = RT_INT;
  char *str_length = malloc(sizeof(char)*20);
  str_length[0] = 'l';
  str_length[1] = 'e';
  str_length[2] = 'n';
  str_length[3] = 'g';
  str_length[4] = 't';
  str_length[5] = 'h';
  str_length[6] = '\0';
  length->funcName = str_length;
  length->paramsCount = 1;
  length->dependencies = initStack();
  length->parameters = initStack();
  if(insertInFUNCTree(tree,&length)){
    printf("Chyba length\n");
    return -1;
  }

  TFunc *substr = malloc(sizeof(TFunc));
  substr->typeVar = RT_STRING;
  char *str_substr = malloc(sizeof(char)*20);
  str_substr[0] = 's';
  str_substr[1] = 'u';
  str_substr[2] = 'b';
  str_substr[3] = 's';
  str_substr[4] = 't';
  str_substr[5] = 'r';
  str_substr[6] = '\0';
  substr->funcName = str_substr;
  substr->paramsCount = 3;
  substr->dependencies = initStack();
  substr->parameters = initStack();
  if(insertInFUNCTree(tree,&substr)){
    printf("Chyba substr\n");
    return -1;
  }

  TFunc *ord = malloc(sizeof(TFunc));
  ord->typeVar = RT_INT;
  char *str_ord = malloc(sizeof(char)*20);
  str_ord[0] = 'o';
  str_ord[1] = 'r';
  str_ord[2] = 'd';
  str_ord[3] = '\0';
  ord->funcName = str_ord;
  ord->paramsCount = 2;
  ord->dependencies = initStack();
  ord->parameters = initStack();
  if(insertInFUNCTree(tree,&ord)){
    printf("Chyba ord\n");
    return -1;
  }

  TFunc *chr = malloc(sizeof(TFunc));
  chr->typeVar = RT_STRING;
  char *str_chr = malloc(sizeof(char)*20);
  str_chr[0] = 'c';
  str_chr[1] = 'h';
  str_chr[2] = 'r';
  str_chr[3] = '\0';
  chr->funcName = str_chr;
  chr->paramsCount = 1;
  chr->dependencies = initStack();
  chr->parameters = initStack();
  if(insertInFUNCTree(tree,&chr)){
    printf("Chyba chr\n");
    return -1;
  }
  return 0;
}

#ifdef TEST_SEMANTIC
int main(){


  TFuncTree *tree;
  initFuncTree(&tree);

  TFunc *new;
  int n = 10;
  for(int i=0;i<n;i++){
    new = malloc(sizeof(TFunc));
    new->typeVar = RT_INT;
    char *aa = malloc(sizeof(char)*20);
    aa[0] = 'f';
    aa[1] = 'u';
    aa[2] = 'n';
    aa[3] = 'c';
    aa[4] = '0' + i;
    aa[5] = '\0';
    new->funcName = aa;
    new->paramsCount = 0;
    new->dependencies = initStack();
    new->parameters = initStack();
    if(insertInFUNCTree(&tree,&new)){
      printf("Chyba1\n");
      return 1;
    }
  }
  ARootBFuncSEM(tree);
  printf("*************\n");

  for(int i=0;i<n;i++){
    char *gg = malloc(sizeof(char)*20);
    gg[0] = 'f';
    gg[1] = 'u';
    gg[2] = 'n';
    gg[3] = 'c';
    gg[4] = '0' + i;
    gg[5] = '\0';
    for(int j=0;j<i+1;j++){
      char *pp = malloc(sizeof(char)*20);
      pp[0] = 'p';
      pp[1] = 'a';
      pp[2] = 'r';
      pp[3] = 'r';
      pp[4] = '0' + j;
      pp[5] = '\0';
      addParams(&tree,gg,pp);

      free(pp);
    }
    free(gg);
  }

  for(int i=0;i<n;i++){
    char *gg = malloc(sizeof(char)*20);
    gg[0] = 'f';
    gg[1] = 'u';
    gg[2] = 'n';
    gg[3] = 'c';
    gg[4] = '0' + i;
    gg[5] = '\0';
    for(int j=0;j<i+1;j++){
      char *pp = malloc(sizeof(char)*20);
      pp[0] = 'd';
      pp[1] = 'e';
      pp[2] = 'p';
      pp[3] = 'p';
      pp[4] = '0' + j;
      pp[5] = '\0';
      addDependency(&tree,gg,pp,0);

      free(pp);
    }
    free(gg);
  }

  ARootBFuncSEM(tree);


  freeFUNCTree(&tree);
}
#endif
