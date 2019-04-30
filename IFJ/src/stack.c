/********************* stack.c *********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

#include "stack.h"

#define DEBUGSTACK 0

void logDebugStack(const char *string){
  if(DEBUGSTACK == 0){
    return;
  }
  printf("%s\n",string);
}

MyStack *initStack(){
  MyStack *stack = malloc(sizeof(MyStack));
  if(stack == NULL){
    return NULL;
  }
  stack->size = 0;
  stack->top = NULL;
  stack->active = NULL;
  stack->isActive = 0;
  return stack;
}


int pushTop(MyStack **stack, int intValue, char *stringValue, double doubleValue, int switchNum){
  stackItem *item =(stackItem *) malloc(sizeof(stackItem));
  if(item == NULL){
    return -1;
  }
  stackItem *temp = (*stack)->top;
  if(switchNum == SSTRING){

    item->stringValue = malloc(sizeof(char) * strlen(stringValue));
    if(item->stringValue == NULL){
      return -1;
    }
    strcpy(item->stringValue, stringValue);
    if(item->stringValue == NULL){
      return -1;
    }
    item->intValue = intValue; //kvoli dependenciam
    item->type = SSTRING;
  }
  else if(switchNum == SINT){
    item->intValue = intValue;
    item->type = SINT;
  }
  else if(switchNum == SDOUBLE){
    item->doubleValue = doubleValue;
    item->type = SDOUBLE;
  }
  else{
    return -1;
  }
  item->next = temp;
  (*stack)->top = item;
  (*stack)->size++;
  return 0;
}

int activate(MyStack *stack){
  if(stack == NULL){
    return -1;
  }
  stack->isActive = 1;
  stack->active = stack->top;
  return 0;
}

int next(MyStack *stack){
  if(stack == NULL || !(stack->isActive)){
    return -1;
  }
  stack->active = stack->active->next;
  return 0;
}

int popTop(MyStack **stack){
  stackItem *temp = (*stack)->top;
  if(activate((*stack)) == -1){
    return -1;
  }
  if(next((*stack)) == -1){
    return -1;
  }

  (*stack)->top = (*stack)->active;
  if(temp->type == SSTRING){
    logDebugStack(temp->stringValue);
    free(temp->stringValue);
    logDebugStack("freed string element from stack!");
  }
  free(temp);
  logDebugStack("freed stack node!");
  (*stack)->size--;
  return 0;
}


int freeStack(MyStack **stack){
  while((*stack)->size > 0){
    if(popTop(&(*stack)) != 0){
      return -1;
    }
    logDebugStack("poped from stack top!");
  }
  free((*stack));
  logDebugStack("Stack freed!");
  return 0;
}



int isEmpty(MyStack *stack){
  return (stack->size == 0);
}



#ifdef TEST_STACK
int main(){


  MyStack *myStackDyn;
  char *a;
  char *b;
  char *cc;



  myStackDyn = initStack();
  if(myStackDyn == NULL){
    return -1;
  }


  a = malloc(sizeof(char)*20);
  a[0] = 'a';
  a[1] = 'h';
  a[2] = 'o';
  a[3] = 'j';
  a[4] = 'j';
  a[5] = '\0';

  b = malloc(sizeof(char)*20);
  b[0] = 'b';
  b[1] = 'h';
  b[2] = 'o';
  b[3] = 'j';
  b[4] = 'j';
  b[5] = '\0';

  cc = malloc(sizeof(char)*20);
  cc[0] = 'b';
  cc[1] = 'j';
  cc[2] = 'j';
  cc[3] = 'j';
  cc[4] = 'j';
  cc[5] = '\0';

  pushTop(&myStackDyn, 0, a, 0, SSTRING);
  pushTop(&myStackDyn, 0, b, 0, SSTRING);
  pushTop(&myStackDyn, 0, cc, 0, SSTRING);

  activate(myStackDyn);
  if(isEmpty(myStackDyn)){
    printf("prazdny - error\n");
  }

  int testOrd = 0;

  while(myStackDyn->active != NULL){

    char *tempStr;
    if(testOrd == 0){
      tempStr = cc;
    }
    else if(testOrd == 1){
      tempStr = b;
    }
    else if(testOrd == 2){
      tempStr = a;
    }
    else{
      printf("Test failed!\n");
      return 1;
    }
    if(strcmp((char *)myStackDyn->active->stringValue,tempStr)){
      printf("Test failed!\n");
      return 1;
    }
    testOrd++;
    next(myStackDyn);
  }


  freeStack(&myStackDyn);
  free(a);
  free(b);
  free(cc);


  printf("All tests passed!\n");
  return 0;
}
#endif
