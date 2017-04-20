#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

typedef struct Symbol {
	char *value;
	struct Symbol *next;
} Symbol;

typedef struct Stack {
	Symbol *top;
	int count;
} Stack;

Symbol * createSymbol(char c){
	Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
	symbol->value = (char *)malloc(sizeof(char));
	(*symbol->value) = c;
	symbol->next = NULL;
	return symbol;
}

void push(Stack *stack, Symbol *symbol){
	Symbol * insert = createSymbol(' ');
	insert->value = symbol->value;
	insert->next = stack->top;
	stack->top = insert;
	stack->count++;
}

Symbol * pop(Stack *stack){
	Symbol *sym = stack->top;
	stack->top = stack->top->next;
	stack->count--;
	return sym;
}

int isOperator(char c){
	return c=='+' || c=='-' || c=='*' || c=='/' || c=='%' || c=='^' || c=='$';
}

int isBrace(char c){
	return c==']' || c=='[' || c=='}' || c=='{' || c==')' || c=='(';
}

char * addtoBuffer(char *buffer, size_t *bufferSize, char add){
	buffer = (char *)realloc(buffer, ++(*bufferSize));
	(*(buffer+(*bufferSize)-1)) = add;
	return buffer;
}

Symbol *tokenize(char *expression, size_t size){
	Symbol *head = NULL;
	Symbol *prev = NULL;
	size_t count = 0;
	char *buffer = NULL;
	size_t bufferSize = 0;
	size_t bracketCount = 0;
	while(count<size-1){
		char currentSymbol = *(expression+count);
#ifdef SHOW_STEPS
		printf("\nCurrent symbol : %c", currentSymbol);
#endif
		if(isBrace(currentSymbol) || isOperator(currentSymbol)){
#ifdef SHOW_STEPS
			printf("\nBrace or operator found : %c", currentSymbol);	
#endif
			if(isBrace(currentSymbol)){
				if(currentSymbol=='(' || currentSymbol=='{' || currentSymbol=='['){
					currentSymbol = '(';
					bracketCount++;
				}
				else if(currentSymbol==')' || currentSymbol=='}' || currentSymbol==']'){
					currentSymbol = ')';
					bracketCount--;
				}
			}
			

			Symbol *operator;

			operator = createSymbol(currentSymbol);
			operator->next = NULL;

			if(buffer!=NULL){
#ifdef SHOW_STEPS
				printf("\n\tPrevious buffer exists!");
#endif
				buffer = addtoBuffer(buffer, &bufferSize, '\0');
#ifdef SHOW_STEPS
				printf("\n\tValue of buffer : %s", buffer);
#endif
				Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
				symbol->value = buffer;
				if(currentSymbol!='(')
					symbol->next = operator;
				else{
					symbol->next = createSymbol('*');
					symbol->next->next = operator;
				}
				if(!head)
					head = symbol;
				else
					prev->next = symbol;
				prev = symbol;
				buffer = NULL;
				bufferSize = 0;
			}
			else{
				if(head==NULL)
					head = operator;
				else
					prev->next = operator;
			}
			if(isOperator(currentSymbol)){
				char nextSymbol = *(expression+count+1);
				char prevSymbol = *(prev->value);
				if((!isalnum(prevSymbol) && prevSymbol!=')') || (nextSymbol!='(' && !isalnum(nextSymbol))){
					printf("\nError : Unbalanced operator '%c' [previous symbol '%c', next symbol '%s']\n", currentSymbol, prevSymbol, nextSymbol=='\n'?"(newline)":&nextSymbol);
					exit(4);
				}
			}
			prev = operator;
		}
		else if(currentSymbol==' '){
#ifdef SHOW_STEPS
			printf("\nSpace found..");
#endif
			if(buffer!=NULL){
#ifdef SHOW_STEPS
				printf("\n\tPrevious buffer exists!");
#endif
				buffer = addtoBuffer(buffer, &bufferSize, '\0');
#ifdef SHOW_STEPS
				printf("\n\tValue of buffer : %s", buffer);
#endif
				Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
				symbol->value = buffer;
				symbol->next = NULL;
				if(!head)
					head = symbol;
				else
					prev->next = symbol;
				prev = symbol;
			}
			buffer = NULL;
			bufferSize = 0;
		}
		else if(isalnum(currentSymbol)){
#ifdef SHOW_STEPS
			printf("\nAdded to current buffer : %c", currentSymbol);
#endif
			buffer = addtoBuffer(buffer, &bufferSize, currentSymbol);
		}
		else{
			printf("\nError : Unknown operator or symbol '%c' !\n", currentSymbol);
			exit(2);
		}
		count++;
	}
	if(bracketCount!=0){
		printf("\nError : The given expression is not balanced in terms of paranthesis!\n");
		exit(3);
	}
	if(buffer!=NULL){
#ifdef SHOW_STEPS
		printf("\n\tPrevious buffer exists!");
#endif
		buffer = addtoBuffer(buffer, &bufferSize, '\0');
#ifdef SHOW_STEPS
		printf("\n\tValue of buffer : %s", buffer);
#endif
		Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
		symbol->value = buffer;
		symbol->next = NULL;
		if(!head)
			head = symbol;
		else
			prev->next = symbol;
	}
	return head;
}

void display(Symbol *head){
	if(head==NULL)
		return;
	Symbol *temp = head;
	printf(" [ %s", temp->value);
	temp = temp->next;
	while(temp!=NULL){
		printf(", %s", temp->value);
		temp = temp->next;
	}
	printf(" ] ");
}

int priority(char c){
	if(c=='(')
		return 4;
	if(c=='^' || c=='$')
		return 3;
	if(c=='*' || c=='/')
		return 2;
	if(c=='+' || c=='-')
		return 1;
	if(c=='%')
		return 0;
	return -1;
}

Symbol * convertToPostFix(Symbol *head){
	Stack *stack = (Stack *)malloc(sizeof(Stack));
	stack->top = NULL;
	stack->count = 0;

	Symbol * output = NULL;
	Symbol * prevOutput = NULL;

	push(stack, createSymbol('('));

	Symbol *temp = head;
	while(temp->next!=NULL){
		temp = temp->next;
	}
	temp->next = createSymbol(')');
	temp = head;

	while(stack->count!=0){
		char *item = temp->value;
#ifdef SHOW_STEPS		
		printf("\nItem : %s ", item);
#endif
		if(isOperator(*item) || (*item)=='('){
			Symbol *sym = pop(stack);
			char *x = sym->value;
			sym->next = NULL;
			if(isOperator((*x))){
#ifdef SHOW_STEPS
				printf(" X_Type : Operator ");
#endif
				if(priority(*x)>=priority(*item)){
#ifdef SHOW_STEPS
					printf(" Action : Add->Push ");
#endif
					if(!output)
						output = sym;
					else
						prevOutput->next = sym;
					prevOutput = sym;

					push(stack, temp);
				}
				else{
#ifdef SHOW_STEPS
					printf(" Action : Push->Push");
#endif
					push(stack, sym);
					push(stack, temp);
				}
			}
			else if((*x)=='('){
#ifdef SHOW_STEPS
				printf(" X_Type : ( Action : Push->Push");
#endif
				push(stack, sym);
				push(stack, temp);
			}
		}
		else if(*item == ')'){
#ifdef SHOW_STEPS
			printf(" Type : ) Action : Continue pop ");
#endif
			while(*item != '('){
				Symbol * temp2 = pop(stack);
				item = temp2->value;
				temp2->next = NULL;
				if(*item!='('){
					if(!output)
						output = temp2;
					else
						prevOutput->next = temp2;
					prevOutput = temp2;
				}
			}
		}
		else{
#ifdef SHOW_STEPS
			printf(" Type : Operand Action : Add");
#endif
			if(!output)
				output = temp;
			else
				prevOutput->next = temp;
			prevOutput = temp;
		}
#ifdef SHOW_STEPS
		printf(" StackSize : %d Stack : ", stack->count);
		display(stack->top);
		printf("\n");
#endif
		temp = temp->next;
	}
	prevOutput->next = NULL;
	return output;
}

int main()
{
    char *line = NULL;
    size_t temp, size, end = -1;
    printf("Enter the expression : ");
    size = getline(&line, &temp, stdin);
    if(size==end){
    	printf("Enter an expression to continue!");
	return 1;
    }
    Symbol *head = tokenize(line, size);
    printf("\n Tokenized expression : ");
    display(head);
    printf("\n");
    Symbol *output = convertToPostFix(head);
    printf("\n Postfix expression : ");
    display(output);
    printf("\n");
    return 0;
}
