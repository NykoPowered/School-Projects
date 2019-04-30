/********************* stack.h *********************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SSTRING 150
#define SINT 155
#define SDOUBLE 158
#define MAX_STACK_SIZE 200

typedef struct stackItem{
		int intValue;
		char *stringValue;
		double doubleValue;
		int type;
		struct stackItem *next;
} stackItem;

typedef struct MyStack{
    int size;
    int isActive;
		stackItem *top;
    stackItem *active;
} MyStack;

MyStack *initStack();
int activate(MyStack *stack);
int pushTop(MyStack **stack, int intValue, char *stringValue, double doubleValue, int switchNum);
int popTop(MyStack **stack);
int isEmpty(MyStack *stack);
int freeStack(MyStack **stack);
int activate(MyStack *stack);
int next(MyStack *stack);
