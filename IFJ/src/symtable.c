/******************* sym_table.c *******************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/


#include "symtable.h"

#define DEBUGSYM 0


void logDebugSym(const char *str){
  if(DEBUGSYM == 1){
    printf("%s",str);
  }
}

int firstStringisGreater(char *a, char *b){
  return strcmp(a,b); //1 if greater
}



int initFuncTree(TFuncTree **tree){

  (*tree) = malloc(sizeof(TFuncTree *));
  if((*tree) == NULL){
    return -1;
  }
  (*tree)->RNode = NULL;
  (*tree)->LNode = NULL;
  (*tree)->data = NULL;
  return 0;
}


int initVarTree(TVarTree **tree){

  (*tree) = malloc(sizeof(TVarTree *));
  if((*tree) == NULL){
    return -1;
  }
  (*tree)->RNode = NULL;
  (*tree)->LNode = NULL;
  (*tree)->data = NULL;
  return 0;
}


int insertInFUNCTree(TFuncTree **tree, TFunc **data){
  if((*tree)->data == NULL){
    logDebugSym("bloy prvok\n");
    (*tree)->data = *data;
    return 0;
  }
  int result = firstStringisGreater((*data)->funcName,(*tree)->data->funcName);
  if(result > 0){
    if((*tree)->RNode == NULL){
      TFuncTree *new;
      initFuncTree(&new);
      if(new == NULL){
        return -1;
      }
      (*tree)->RNode = new;
    }
    return insertInFUNCTree(&(*tree)->RNode, data);
  }
  else if(result < 0){
    if((*tree)->LNode == NULL){
      TFuncTree *new;
      initFuncTree(&new);
      if(new == NULL){
        return -1;
      }
      (*tree)->LNode = new;
    }
    return insertInFUNCTree(&(*tree)->LNode, data);
  }
  else{
    if((*tree)->data != NULL){
      if((*tree)->data->funcName != NULL){
        free((*tree)->data->funcName);
        logDebugSym("freed func name!\n");
      }

      if(freeStack(&((*tree)->data->dependencies)) == -1){
        return -1;
      }
      if(freeStack(&((*tree)->data->parameters)) == -1){
        return -1;
      }
      logDebugSym("freed stacks!\n");
      TFunc *data = (*tree)->data;
      free(data);
      logDebugSym("freed data in node!\n");
    }
    logDebugSym("redefine prvok\n");
    (*tree)->data = *data;
    return 0;

  }
  return 0;
}

int insertInVARTree(TVarTree **tree, TVar **data){
  if((*tree)->data == NULL){
    logDebugSym("prvy prvok\n");
    (*tree)->data = *data;
    return 0;
  }
  int result = firstStringisGreater((*data)->varName,(*tree)->data->varName);
  if(result > 0){
    if((*tree)->RNode == NULL){
      TVarTree *new;
      initVarTree(&new);
      if(new == NULL){
        return -1;
      }
      (*tree)->RNode = new;
    }
    return insertInVARTree(&(*tree)->RNode, data);
  }
  else if(result < 0){
    if((*tree)->LNode == NULL){
      TVarTree *new;
      initVarTree(&new);
      if(new == NULL){
        return -1;
      }
      (*tree)->LNode = new;
    }
    return insertInVARTree(&(*tree)->LNode, data);
  }
  else{
    if((*tree)->data != NULL){
      if((*tree)->data->varName != NULL){
        free((*tree)->data->varName);
        logDebugSym("freed func name!\n");
      }

      logDebugSym("freed stacks!\n");
      TVar *data = (*tree)->data;
      free(data);
      logDebugSym("freed data in tree!\n");
    }
    logDebugSym("redefine prvok\n");
    (*tree)->data = *data;
  }
  return 0;
}

TFunc* findNodeFunc(TFuncTree *tree, char *key){
  if(!strcmp(tree->data->funcName,key)){
    return tree->data;
  }

  int result = firstStringisGreater(key,tree->data->funcName);
  if(result > 0){
    if(tree->RNode == NULL){
      return NULL;
    }
    return findNodeFunc(tree->RNode, key);
  }
  else if(result < 0){
    if(tree->LNode == NULL){
      return NULL;
    }
    return findNodeFunc(tree->LNode, key);
  }
  return NULL;
}

TVar* findNodeVar(TVarTree *tree, char *key){
  logDebugSym("je strom null?");
  if(tree == NULL){
    logDebugSym("strom je null");
    return NULL;
  }
  logDebugSym("hladame tuna");
  if(!strcmp(tree->data->varName,key)){
    return tree->data;
  }

  int result = firstStringisGreater(key,tree->data->varName);
  if(result > 0){
    if(tree->RNode == NULL){
      return NULL;
    }
    return findNodeVar(tree->RNode, key);
  }
  else if(result < 0){
    if(tree->LNode == NULL){
      return NULL;
    }
    return findNodeVar(tree->LNode, key);
  }
  return NULL;
}

void ARootBFunc(TFuncTree *tree){
  if(tree == NULL){
    return;
  }

  ARootBFunc(tree->LNode);
  printf("%s\n",tree->data->funcName);
  ARootBFunc(tree->RNode);
}

void ARootBVar(TVarTree *tree){
  if(tree == NULL){
    return;
  }

  ARootBVar(tree->LNode);
  printf("%s\n",tree->data->varName);
  ARootBVar(tree->RNode);
}

void freeFUNCTree(TFuncTree **tree){
  if((*tree) == NULL){
    return;
  }
  logDebugSym("freeing tree is not null\n");
  freeFUNCTree(&((*tree)->LNode));
  freeFUNCTree(&((*tree)->RNode));

  logDebugSym("freeing tree!\n");
  if((*tree)->data != NULL){
    if((*tree)->data->funcName != NULL){
      free((*tree)->data->funcName);
      logDebugSym("freed func name!\n");
    }

    freeStack(&((*tree)->data->dependencies));
    freeStack(&((*tree)->data->parameters));
    logDebugSym("freed stacks!\n");
    TFunc *data = (*tree)->data;
    free(data);
    logDebugSym("freed data in tree!\n");
  }

  free(*tree);
}

void freeVarTree(TVarTree **tree){
  if((*tree) == NULL){
    logDebugSym("Chyba nastala!");
    return;
  }
  logDebugSym("freeing tree is not null\n");
  freeVarTree(&(*tree)->LNode);
  freeVarTree(&(*tree)->RNode);

  logDebugSym("freeing tree!\n");
  if((*tree)->data != NULL){
    if((*tree)->data->varName != NULL){
      free((*tree)->data->varName);
      logDebugSym("freed func name!\n");
    }

    logDebugSym("freed stacks!\n");
    TVar *data = (*tree)->data;
    free(data);
    logDebugSym("freed data in tree!\n");
  }

  free(*tree);
}

#ifdef TEST_SYMTAB
int main(){


 TFuncTree *tree;
 initFuncTree(&tree);


 TFunc *new;
 int n = 500;
 for(int i=0;i<n;i++){
   new = malloc(sizeof(TFunc));
   new->typeVar = RT_INT;
   char *aa = malloc(sizeof(char)*20);
   aa[0] = 'f';
   aa[1] = '0'+(i / 100) % 100;
   aa[2] = '0' +(i / 10) % 10;
   aa[3] = 'c' + i % 10;
   aa[4] = '0' ;
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


 logDebugSym("going to pass tree\n");

 ARootBFunc(tree);
 logDebugSym("going to free tree\n");
 freeFUNCTree(&tree);


 TVarTree *treeV;
 initVarTree(&treeV);


 TVar *new1;
 int nn = 500;
 for(int i=0;i<nn;i++){
   new1 = malloc(sizeof(TVar));
   new1->typeVar = RT_INT;
   char *bb = malloc(sizeof(char)*20);
   bb[0] = 'f';
   bb[1] = '0'+(i / 100) % 100;
   bb[2] = '0' +(i / 10) % 10;
   bb[3] = 'c' + i % 10;
   bb[4] = '0' ;
   bb[5] = '\0';
   new1->varName = bb;
   new1->global = 0;
   if(insertInVARTree(&treeV,&new1)){
     printf("Chyba1\n");
     return 1;
   }
 }


 logDebugSym("going to pass treeV\n");

 ARootBVar(treeV);
 logDebugSym("going to free treeV\n");
 freeVarTree(&treeV);





}
#endif
