/******************** expres.c *********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin GaĹo, xganom00                     |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include "expres.h"

#define DEBUGEX 0

int autoIncrement = 0;

void logDebugExpres(const char *string){
  if(DEBUGEX == 0){
    return;
  }
  printf("%s\n",string);
}

MyList *initList(){
  MyList *lList = malloc(sizeof(MyList));
  if(lList == NULL){
    return NULL;
  }
  lList->size = 0;
  lList->first = NULL;
  lList->active = NULL;
  lList->isActive = 0;
  return lList;
}

int activateEXPR(MyList *lList){
  if(lList == NULL){
    return -1;
  }
  lList->isActive = 1;
  lList->active = lList->first;
  return 0;
}

int nextEXPR(MyList *lList){
  if(lList == NULL || !(lList->isActive)){
    return -1;
  }
  lList->active = lList->active->next;
  return 0;
}

int prev(MyList *lList){
  if(lList == NULL || !(lList->isActive)){
    return -1;
  }
  lList->active = lList->active->prev;
  return 0;
}

//podla switchtype pristupujeme ku datam ktore potrebuejeme
//vrati 0 pri uspechu
int pushBack(MyList **lList, int intValue, char *stringValue, double doubleValue, LLType switchType, int varType){
  if((*lList) == NULL){
    return -1;
  }
  LLitem *item =(LLitem *) malloc(sizeof(LLitem));
  if(item == NULL){
    return -1;
  }
  item->type = switchType;
  item->varType = varType;
  if(switchType == LLTYPE_ID || switchType == LLTYPE_STRING){
    item->stringValue = malloc(sizeof(char) *strlen(stringValue));
    if(item->stringValue == NULL){
      return -1;
    }
    strcpy(item->stringValue, stringValue);
    if(item->stringValue == NULL){
      return -1;
    }
  }
  else if(switchType == LLTYPE_INTEGER){
    item->intValue = intValue;
  }
  else if(switchType == LLTYPE_DECIMAL){
    item->doubleValue = doubleValue;
  }
  item->next = NULL;

  if((*lList)->first == NULL){
    item->prev = NULL;
    (*lList)->first = item;
    return 0;
  }

  if(!(*lList)->isActive){
    if(activateEXPR((*lList)) == -1){
      return -1;
    }
  }

  while((*lList)->active->next != NULL){
    if(nextEXPR((*lList)) == -1){
      return -1;
    }
  }
  if((*lList)->active->next == NULL){
    item->prev = (*lList)->active;
    (*lList)->active->next = item;
  }
  else{
    logDebugExpres("Nastala velmi vazna chyba!\n");
    return -1;
  }


  return 0;
}


int freeList(MyList **lList){
  if((*lList) == NULL){
    return -1;
  }
  if(activateEXPR((*lList)) == -1){
    return -1;
  }
  while((*lList)->active != NULL){
    if((*lList)->active->type == LLTYPE_ID || (*lList)->active->type == LLTYPE_STRING){
      free((*lList)->active->stringValue);
    }
    LLitem *temp = (*lList)->active;
    if(nextEXPR((*lList)) == -1){
      return -1;
    }
    free(temp);
  }
  free((*lList));
  return 0;
}

int followValue(LLType nextType){
  if(nextType == LLTYPE_MUL ||
     nextType == LLTYPE_DIV ||
     nextType == LLTYPE_PLUS ||
     nextType == LLTYPE_MINUS ||
     nextType == LLTYPE_RBRACKET ||
     nextType == LLTYPE_MORE ||
     nextType == LLTYPE_MOREEQUAL ||
     nextType == LLTYPE_EQUAL ||
     nextType == LLTYPE_NOTEQUAL ||
     nextType == LLTYPE_LESS ||
     nextType == LLTYPE_LESSEQUEL){
       return 1;
  }

  return 0;

}

int followOperator(LLType nextType){
  if(nextType == LLTYPE_ID ||
     nextType == LLTYPE_NIL ||
     nextType == LLTYPE_INTEGER ||
     nextType == LLTYPE_DECIMAL ||
     nextType == LLTYPE_STRING ||
     nextType == LLTYPE_LBRACKET){
       return 1;
  }

  return 0;

}

int getDigitsCount(int num){
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

char *getUniqueVar(){

  char *result = malloc((getDigitsCount(autoIncrement)+5)*sizeof(char));
  if(result == NULL){
    return NULL;
  }

  autoIncrement++;

  result[0] = 'e';
  int i=1;
  int num = autoIncrement;
  while(num > 0){
    int pom = num % 10;
    result[i] = '0'+ pom;
    i++;
    num/=10;
  }
  result[i] =  '\0';
  return result;
}

void freeItem(LLitem **item){
  //uvolniĹĽ
  if((*item)->type == LLTYPE_ID || (*item)->type == LLTYPE_STRING){
    free((*item)->stringValue);
  }
  free((*item));
}



//list musi mat aktivny prvok
//posledny prvok musi byt zatvorka naviac
int exprEval(LLitem **lLitem){
  if((*lLitem) == NULL){
    return INTERN_ERR;
  }


  LLitem *act = (*lLitem);
  while(act != NULL && act->type != LLTYPE_RBRACKET){
    if(DEBUGEX != 0){
      printf("prev: ");
      if(act->prev == NULL){
        printf("NULL\n");
      }
      else{
        printf("%d\n", act->prev->type);
      }

      printf("%d\n", act->type);
      printf("post: ");
      if(act->next == NULL){
        printf("NULL\n");
      }
      else{
        printf("%d\n", act->next->type);
      }
    }



    if(act->type == LLTYPE_ID ||
     act->type == LLTYPE_INTEGER ||
     act->type == LLTYPE_DECIMAL ||
     act->type == LLTYPE_STRING ||
     act->type == LLTYPE_NIL){
       if(act->next != NULL && !followValue(act->next->type)){
         printf("ocakavali sme operator!\n");
         return PRSR_ERR;
       }
    }
    else if(act->type == LLTYPE_MUL){
      //generovat kod

      if(act->prev == NULL){
        return PRSR_ERR;
      }
      if((act->prev->type == LLTYPE_ID) && (act->prev->varType == EXP_UNKNOWN)){
        if(act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN){

          //tu asi nieco vygenerujeme
          LLitem *temp1 = act->next;
          LLitem *temp2 = act->prev;

          act->next = act->next->next;
          act->prev = act->prev->prev;
          if(act->next != NULL){
            act->next->prev = act;
          }
          if(act->prev != NULL){
            act->prev->next = act;
          }
          if((*lLitem) == act->prev){
            logDebugExpres("zaciatokk");
            (*lLitem) = act;
          }

          freeItem(&temp1);
          freeItem(&temp2);

          act->stringValue = getUniqueVar();

          //generovaĹĽ kod pomocou premennej

          act->type = LLTYPE_ID;
          act->varType = EXP_UNKNOWN;
        }
      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_INT) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }

            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
              (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_INT;

            }
            else if((act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                    (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_DECIMAL){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }

      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_DOUBLE) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN)||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }
            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER || act->next->type == LLTYPE_DECIMAL){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }


      }




    }
    else if(act->type == LLTYPE_DIV){
      //generovat kod
      if(act->prev == NULL){
        return PRSR_ERR;
      }

      if((act->prev->type == LLTYPE_ID) && (act->prev->varType == EXP_UNKNOWN)){
        if(act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN){
          //tu asi nieco vygenerujeme
          LLitem *temp1 = act->next;
          LLitem *temp2 = act->prev;

          act->next = act->next->next;
          act->prev = act->prev->prev;
          if(act->next != NULL){
            act->next->prev = act;
          }
          if(act->prev != NULL){
            act->prev->next = act;
          }
          if((*lLitem) == act->prev){
            logDebugExpres("zaciatokk");
            (*lLitem) = act;
          }
          freeItem(&temp1);
          freeItem(&temp2);
          act->stringValue = getUniqueVar();

          //generovaĹĽ kod pomocou premennej

          act->type = LLTYPE_ID;
          act->varType = EXP_UNKNOWN;
        }
      }

      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_INT) ||
      (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }

            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
              (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER){
                  //tu asi nieco vygenerujeme

                  if(act->next->type == LLTYPE_INTEGER &&
                     act->next->intValue == 0){
                       logDebugExpres("delenie 0\n");
                       return RUN_ERR_ZERODIV;
                  }

                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_INT;
                  logDebugExpres("bude to id");

            }
            else if((act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                    (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_DECIMAL){


                  if(act->next->type == LLTYPE_DECIMAL &&
                     act->next->doubleValue == 0){
                       logDebugExpres("delenie 0\n");
                       return RUN_ERR_ZERODIV;
                  }

                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }

      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_DOUBLE) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }
            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER || act->next->type == LLTYPE_DECIMAL){

                  if((act->next->type == LLTYPE_INTEGER &&
                     act->next->intValue == 0) ||
                    (act->next->type == LLTYPE_DECIMAL &&
                     act->next->doubleValue == 0)){
                       logDebugExpres("delenie 0\n");
                       return RUN_ERR_ZERODIV;
                  }

                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }


      }


    }
    else if(act->type == LLTYPE_LESS || act->type == LLTYPE_LESSEQUEL ||
            act->type == LLTYPE_MORE || act->type == LLTYPE_MOREEQUAL ||
            act->type == LLTYPE_PLUS || act->type == LLTYPE_MINUS ||
            act->type == LLTYPE_EQUAL || act->type == LLTYPE_NOTEQUAL){
              if(act->prev == NULL){
                return PRSR_ERR;
              }

              if(!followOperator(act->next->type)){
                printf("ocakavali sme hodnotu!\n");
                return PRSR_ERR;
              }

    }

    else if(act->type != LLTYPE_RBRACKET){
      printf("chyba zatvorky!\n");
      return SEM_ERR_ALL;
    }
    else if(act->type == LLTYPE_LBRACKET){
      LLitem *temp = act->next;
      exprEval(&temp);
    }
    else{
      printf("chyba -zz\n");
      logDebugExpres("chybny vyraz!");
      return SEM_ERR_ALL;
    }

    act = act->next;
  }
  logDebugExpres(".......................");

  while(act->prev != NULL){
    if(DEBUGEX != 0){
      printf("%d\n",act->type);
    }
    act = act->prev;
  }
  logDebugExpres(".......................");


  while(act != NULL && act->type != LLTYPE_RBRACKET){
    if(DEBUGEX != 0){
      printf("prev: ");
      if(act->prev == NULL){
        printf("NULL\n");
      }
      else{
        printf("%d\n", act->prev->type);
      }

      printf("%d\n", act->type);
      printf("post: ");
      if(act->next == NULL){
        printf("NULL\n");
      }
      else{
        printf("%d\n", act->next->type);
      }
    }
    if(act->type == LLTYPE_ID ||
       act->type == LLTYPE_INTEGER ||
       act->type == LLTYPE_DECIMAL ||
       act->type == LLTYPE_STRING ||
       act->type == LLTYPE_NIL){
         if(act->next != NULL && !followValue(act->next->type)){
           printf("ocakavali sme operator!\n");
           return PRSR_ERR;
         }
    }


    else if(act->type == LLTYPE_PLUS){
      //generovat kod

      if(act->prev == NULL){
        return PRSR_ERR;
      }
      if((act->prev->type == LLTYPE_ID) && (act->prev->varType == EXP_UNKNOWN)){
        logDebugExpres("here I am");
        if(act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN){
          //tu asi nieco vygenerujeme
          LLitem *temp1 = act->next;
          LLitem *temp2 = act->prev;

          act->next = act->next->next;
          act->prev = act->prev->prev;
          if(act->next != NULL){
            act->next->prev = act;
          }
          if(act->prev != NULL){
            act->prev->next = act;
          }
          if((*lLitem) == act->prev){
            logDebugExpres("zaciatokk");
            (*lLitem) = act;
          }
          freeItem(&temp1);
          freeItem(&temp2);
          act->stringValue = getUniqueVar();

          //generovaĹĽ kod pomocou premennej

          act->type = LLTYPE_ID;
          act->varType = EXP_UNKNOWN;
        }
      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_INT) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }

            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
              (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_INT;

            }
            else if((act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                    (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_DECIMAL){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }

      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_DOUBLE) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }
            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER || act->next->type == LLTYPE_DECIMAL){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }


      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_STRING) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_STRING){
            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }
            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_STRING) ||
              (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_STRING){
                  //tu asi nieco vygenerujeme
                  logDebugExpres("Praca so stringom!");
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_STRING;
                }

      }




    }
    else if(act->type == LLTYPE_MINUS){
      //generovat kod
      logDebugExpres("tuna som\n");
      if(act->prev == NULL){
        return PRSR_ERR;
      }
      if((act->prev->type == LLTYPE_ID) && (act->prev->varType == EXP_UNKNOWN) ){
        if(act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN){
          //tu asi nieco vygenerujeme
          LLitem *temp1 = act->next;
          LLitem *temp2 = act->prev;

          act->next = act->next->next;
          act->prev = act->prev->prev;
          if(act->next != NULL){
            act->next->prev = act;
          }
          if(act->prev != NULL){
            act->prev->next = act;
          }
          if((*lLitem) == act->prev){
            logDebugExpres("zaciatokk");
            (*lLitem) = act;
          }
          freeItem(&temp1);
          freeItem(&temp2);
          act->stringValue = getUniqueVar();

          //generovaĹĽ kod pomocou premennej

          act->type = LLTYPE_ID;
          act->varType = EXP_UNKNOWN;
        }
        else if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
            act->next->type == LLTYPE_INTEGER){
              //tu asi nieco vygenerujeme
              LLitem *temp1 = act->next;
              LLitem *temp2 = act->prev;

              act->next = act->next->next;
              act->prev = act->prev->prev;
              if(act->next != NULL){
                act->next->prev = act;
              }
              if(act->prev != NULL){
                act->prev->next = act;
              }
              if((*lLitem) == act->prev){
                logDebugExpres("zaciatokk");
                (*lLitem) = act;
              }
              freeItem(&temp1);
              freeItem(&temp2);
              act->stringValue = getUniqueVar();

              //generovaĹĽ kod pomocou premennej

              act->type = LLTYPE_ID;
              act->varType = EXP_INT;

        }
        else if((act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
            act->next->type == LLTYPE_DECIMAL){
              //tu asi nieco vygenerujeme
              LLitem *temp1 = act->next;
              LLitem *temp2 = act->prev;

              act->next = act->next->next;
              act->prev = act->prev->prev;
              if(act->next != NULL){
                act->next->prev = act;
              }
              if(act->prev != NULL){
                act->prev->next = act;
              }
              if((*lLitem) == act->prev){
                logDebugExpres("zaciatokk");
                (*lLitem) = act;
              }
              freeItem(&temp1);
              freeItem(&temp2);
              act->stringValue = getUniqueVar();

              //generovaĹĽ kod pomocou premennej

              act->type = LLTYPE_ID;
              act->varType = EXP_DOUBLE;

        }
        else{
          printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
          return SEM_ERR_TYPE;
        }
      }

      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_INT) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){
            logDebugExpres("tuna som\n");
            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }

            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
              (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_INT;

            }
            else if((act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                    (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_DECIMAL){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }

      }
      else if((act->prev->type == LLTYPE_ID && act->prev->varType == EXP_DOUBLE) ||
        (act->prev->type == LLTYPE_ID && act->prev->varType == EXP_UNKNOWN) ||
          act->prev->type == LLTYPE_INTEGER){

            if(act->next->type == LLTYPE_LBRACKET){
              LLitem *tempBracket = act->next;
              act->next = act->next->next;
              free(tempBracket);
              LLitem *temp = act->next->next;
              int a = exprEval(&temp);
              if(a != OK){
                return a;
              }
            }
            if((act->next->type == LLTYPE_ID && act->next->varType == EXP_INT) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_DOUBLE) ||
                (act->next->type == LLTYPE_ID && act->next->varType == EXP_UNKNOWN) ||
                act->next->type == LLTYPE_INTEGER || act->next->type == LLTYPE_DECIMAL){
                  //tu asi nieco vygenerujeme
                  LLitem *temp1 = act->next;
                  LLitem *temp2 = act->prev;

                  act->next = act->next->next;
                  act->prev = act->prev->prev;
                  if(act->next != NULL){
                    act->next->prev = act;
                  }
                  if(act->prev != NULL){
                    act->prev->next = act;
                  }
                  if((*lLitem) == act->prev){
                    logDebugExpres("zaciatokk");
                    (*lLitem) = act;
                  }
                  freeItem(&temp1);
                  freeItem(&temp2);
                  act->stringValue = getUniqueVar();

                  //generovaĹĽ kod pomocou premennej

                  act->type = LLTYPE_ID;
                  act->varType = EXP_DOUBLE;

            }
            else{
              printf("ocakavali sme int, float alebo vyraz v zatvorke\n");
              return SEM_ERR_TYPE;
            }


      }




    }

    else if(act->type == LLTYPE_LESS || act->type == LLTYPE_LESSEQUEL ||
            act->type == LLTYPE_MORE || act->type == LLTYPE_MOREEQUAL ||
            act->type == LLTYPE_MUL || act->type == LLTYPE_DIV ||
            act->type == LLTYPE_EQUAL || act->type == LLTYPE_NOTEQUAL){
              if(act->prev == NULL){
                return PRSR_ERR;
              }

              if(!followOperator(act->next->type)){
                printf("ocakavali sme hodnotu!\n");
                return PRSR_ERR;
              }

    }
    else{
      logDebugExpres("chybny vyraz -ten!");
      return SEM_ERR_ALL;
    }
    act = act->next;

  }
  if(DEBUGEX != 0){
    while(act->prev != NULL){
      if(DEBUGEX != 0){
        printf("%d\n",act->type);
      }
      act = act->prev;
    }
    logDebugExpres(".......................");
    while(act != NULL && act->type != LLTYPE_RBRACKET){
      if(DEBUGEX != 0){
        printf("prev: ");
        if(act->prev == NULL){
          printf("NULL\n");
        }
        else{
          printf("%d\n", act->prev->type);
        }

        printf("%d\n", act->type);
        printf("post: ");
        if(act->next == NULL){
          printf("NULL\n");
        }
        else{
          printf("%d\n", act->next->type);
        }
      }
      act = act->next;
    }
  }
  logDebugExpres(".......................");

  while(act->prev != NULL){
    printf("%d\n",act->type);
    act = act->prev;
  }
  printf("%d\n",act->type);
  logDebugExpres(".......................");
  if(act->type == LLTYPE_INTEGER ||
     act->type == LLTYPE_DECIMAL ||
     act->type == LLTYPE_STRING ||
     act->type == LLTYPE_ID){
       //printf("%d\n",act->next->type);
       if(act->next->type == LLTYPE_RBRACKET){
         //generuj kode
         return OK;
       }
       else if(act->next->type == LLTYPE_MORE ||
               act->next->type == LLTYPE_MOREEQUAL ||
               act->next->type == LLTYPE_LESS ||
               act->next->type == LLTYPE_LESSEQUEL ||
               act->next->type == LLTYPE_EQUAL ||
               act->next->type == LLTYPE_NOTEQUAL){

                 if(act->next->next->type == LLTYPE_INTEGER ||
                    act->next->next->type == LLTYPE_DECIMAL ||
                    act->next->next->type == LLTYPE_STRING ||
                    act->next->next->type == LLTYPE_ID){
                      //generuj kod
                      return OK;
                 }
                 else{
                   logDebugExpres("chyba here\n");
                   return SEM_ERR_TYPE;
                 }
        }
        else{
          logDebugExpres("chyba her2e\n");
          return SEM_ERR_TYPE;
        }

  }
  else{
    logDebugExpres("chyba here3\n");
    return SEM_ERR_TYPE;
  }

  while(act->next != NULL){
    act = act->next;
  }

  if(act->type != LLTYPE_RBRACKET){
    printf("zavora\n");
    return SEM_ERR_ALL;
  }

  LLitem *temp = act;
  act = act->prev;
  act->next = act->next->next;
  free(temp);
  return 0;

}


#ifdef TEST_EXPR
int main(){
  MyList *lList = initList();



  pushBack(&lList, 2, NULL, 0, LLTYPE_INTEGER,0);
  // pushBack(&lList, 0, NULL, 0, LLTYPE_MUL);
  // pushBack(&lList, 2, NULL, 0, LLTYPE_INTEGER);
  // pushBack(&lList, 0, NULL, 0, LLTYPE_RBRACKET);


  // pushBack(&lList, 2, "NULL1", 0, LLTYPE_STRING);
  // pushBack(&lList, 0, "NULL2", 0, LLTYPE_STRING);
  // pushBack(&lList, 0, "NULL3", 0, LLTYPE_STRING);
  // pushBack(&lList, 8, "NULL4", 0, LLTYPE_STRING);
  // pushBack(&lList, 0, "NULL5", 0, LLTYPE_STRING);
  // pushBack(&lList, 15, "NULL6", 0, LLTYPE_STRING);
  // pushBack(&lList, 0, "NULL7", 0, LLTYPE_STRING);
  // pushBack(&lList, 0, "NULL8", 0, LLTYPE_STRING);
  // pushBack(&lList, 20, "NULL9", 0, LLTYPE_STRING);
  // pushBack(&lList, 0, "NULL10", 0, LLTYPE_STRING);
  // pushBack(&lList, 2, "NULL11", 0, LLTYPE_STRING);


  if(activateEXPR(lList) == -1){
    return -1;
  }
  exprEval(&lList->active);

  printf("%d\n",lList->first->intValue);

  // while(lList->active->next != NULL){
  //   printf("hej\n");
  //   if(lList->active->type == LLTYPE_ID || lList->active->type == LLTYPE_STRING){
  //     printf("%s\n",lList->active->stringValue);
  //   }
  //   if(next(lList) == -1){
  //     return -1;
  //   }
  // }
  //
  // while(lList->active->prev != NULL){
  //   printf("hej\n");
  //   if(lList->active->type == LLTYPE_ID || lList->active->type == LLTYPE_STRING){
  //     printf("%s\n",lList->active->stringValue);
  //   }
  //   if(prev(lList) == -1){
  //     return -1;
  //   }
  // }



  freeList(&lList);


}
#endif
