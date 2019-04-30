/******************* sym_table.h *******************\
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
#include "stack.h"

typedef enum
{
  RT_INT,
  RT_DOUBLE,
  RT_STRING,
  RT_NIL,
  RT_UNKNOWN,
} ReturnTYPE;

typedef enum
{
  T_INT,
  T_DOUBLE,
  T_STRING,
  T_PARAM,
  T_NIL,
} DataTYPE;

#define GLOBAL 1
#define LOCAL 0

typedef struct{
  DataTYPE typeVar;
  int global; //LOCAL if local GLOBAL if global
  char* varName;
} TVar;

typedef struct{
  ReturnTYPE typeVar;
  char *funcName;
  int paramsCount;
  MyStack *parameters;
  MyStack *dependencies;
} TFunc;


typedef struct TFuncTree{
  TFunc *data;
  struct TFuncTree *LNode;
  struct TFuncTree *RNode;
}TFuncTree;

typedef struct TVarTree{
  TVar *data;
  struct TVarTree *LNode;
  struct TVarTree *RNode;
}TVarTree;


int initFuncTree(TFuncTree **tree);
int initVarTree(TVarTree **tree);
int insertInFUNCTree(TFuncTree **tree, TFunc **data);
int insertInVARTree(TVarTree **tree, TVar **data);
TFunc* findNodeFunc(TFuncTree *tree, char *key);
TVar* findNodeVar(TVarTree *tree, char *key);
void freeVarTree(TVarTree **tree);
void freeFUNCTree(TFuncTree **tree);
