/******************* semantic.h ********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include "symtable.h"


int isFuncDefined(TFuncTree *tree,char *funcName);
int checkParamsCount(TFuncTree *tree,char *funcName, MyStack *parameters);
int isVarDefined(TVarTree *tree,char *varName);
int addParams(TFuncTree **tree,char *funcName, char *paramName);
int addDependency(TFuncTree **tree,char *funcName, char *dependencyName, int paramsCount);
void ARootBFuncSEM(TFuncTree *tree);
DataTYPE getVarType(TVarTree *tree,char *varName);
int allDependenciesOK(TFuncTree *tree,char *funcName);
int checkParamsCountREAL(TFuncTree *tree,char *funcName, int paramsCount);
int allDependenciesPARAMSOK(TFuncTree *tree,char *funcName);
int addBuildIn(TFuncTree **tree);
DataTYPE getVarType(TVarTree *tree,char *varName);
