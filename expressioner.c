#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

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

//#define SHOW_STEPS

Symbol *tokenize(char *expression, size_t size){
	Symbol *head = NULL;
	Symbol *prev = NULL;
	size_t count = 0;
	char *buffer = NULL;
	size_t bufferSize = 0;

	printf("\n Tokenizing given expression..\n");

	while(count<size-1){
		char currentSymbol = *(expression+count);
#ifdef SHOW_STEPS
		printf("\n Current symbol : %c", currentSymbol);
#endif
		if(isBrace(currentSymbol) || isOperator(currentSymbol)){
#ifdef SHOW_STEPS
			printf("\n Brace or operator found : %c", currentSymbol);	
#endif			
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
				symbol->next = operator;
				
				if(!head)
					head = symbol;
				else
					prev->next = symbol;

				buffer = NULL;
				bufferSize = 0;
			}
			else{
				if(head==NULL)
					head = operator;
				else
					prev->next = operator;
			}
			
			prev = operator;
		}
		else if(currentSymbol==' '){
#ifdef SHOW_STEPS
			printf("\n Space found..");
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
		else if(isalnum(currentSymbol) || currentSymbol=='.' || currentSymbol=='_'){
#ifdef SHOW_STEPS
			printf("\n Added to current buffer : %c", currentSymbol);
#endif
			buffer = addtoBuffer(buffer, &bufferSize, currentSymbol);
		}
		else{
			printf("\nError : Unknown operator or symbol '%c' !\n", currentSymbol);
			exit(2);
		}
		count++;
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

void checkSemantics(Symbol *head){
	printf("\n Performing semantic analysis..\n");

	Symbol *prev = NULL;
	size_t bracketCount = 0;
	Symbol *temp = head;
	while(temp!=NULL){
		Symbol *current = temp;
		char cur = *(current->value);
		
		if(isalnum(cur) || cur=='.' || cur=='_'){
			if(isalpha(cur) || cur=='_'){
				if(strchr(current->value, '.')!=NULL){
					printf("\n Error : Variable name '%s' contains '.' ! \n", current->value);
					exit(9);
				}
#ifdef SHOW_STEPS
				else
					printf("\n Found variable : %s \n",current->value);
#endif
			}
			else{
				char *err;
				double d = strtod(current->value, &err);
				if(strlen(err)!=0){
					printf("\n Error : Variable name '%s' precedes with a number! \n", current->value);
					exit(10);
				}
#ifdef SHOW_STEPS
				else
					printf("\n Found constant value : %s \n", current->value);
#endif
			}
			if(temp==head){
				prev = temp;
				temp = temp->next;
				continue;
			}
			char pre = *(prev->value);
			if(pre==')'){
				Symbol *mul = createSymbol('*');
				mul->next = prev->next;
				prev->next = mul;
#ifdef SHOW_STEPS
				printf("\n Alphanumeric operator : Inserted '*' between %s and %s \n", prev->value, current->value);
#endif
			}
			else if(isalnum(pre)){
				printf("\n Error : No operator specified between operands %s and %s !\n", prev->value, current->value);
				exit(5);
			}
			
		}
		else if(isOperator(cur)){
			if(temp==head){
				printf("\n Error : An expression cannot start with an operator!\n");
				exit(6);
			}
			char pre = *(prev->value);
			char nextSymbol;
			if(current->next==NULL){
				nextSymbol = '\n';
			}
			else
				nextSymbol = *(current->next->value);

			if(nextSymbol=='{' || nextSymbol=='[')
				nextSymbol = '(';

			if((!isalnum(pre) && pre!=')' && pre!='.' && pre!='_') || (nextSymbol!='(' && nextSymbol!='.' && nextSymbol!='_' && !isalnum(nextSymbol))){
				printf("\n Error : Unbalanced operator '%c' [previous symbol '%c', next symbol '%s'] !\n", cur, pre, nextSymbol=='\n'?"(newline)":&nextSymbol);
				exit(4);
			}
		}
		else if(isBrace(cur)){
#ifdef SHOW_STEPS
			printf("\n Brace found : %c \n", cur);
#endif		
			if(cur=='(' || cur=='{' || cur=='['){
				current->value = "(";
				cur = '(';
				bracketCount++;
			}
			else if(cur==')' || cur=='}' || cur==']'){
				if(temp==head){
					printf("\n Error : An expression cannot start with a closing brace!\n");
					exit(8);
				}
				current->value = ")";
				cur = ')';
				bracketCount--;
			}

			if(prev!=NULL && cur=='(' && !isOperator(*prev->value)){
				Symbol *mul = createSymbol('*');
				mul->next = prev->next;
				prev->next = mul;
#ifdef SHOW_STEPS
			printf("\n Brace operator : Inserted '*' between %s and %s \n", prev->value, current->value);
#endif		
			}
		}
		prev = temp;
		temp = temp->next;
	}
	if(bracketCount!=0){
		printf("\n Error : The given expression is not balanced in terms of parantheses!\n");
		exit(6);
	}
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
		printf("\n Item : %s ", item);
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
    printf(" Enter the expression : ");
    size = getline(&line, &temp, stdin);
    if(size==end){
    	printf(" Enter an expression to continue!");
	return 1;
    }
    Symbol *head = tokenize(line, size);
    printf("\n Tokenized expression : ");
    display(head);
    printf("\n");
    checkSemantics(head);
    printf("\n Corrected expression : ");
    display(head);
    printf("\n");
    Symbol *output = convertToPostFix(head);
    printf("\n Postfix expression : ");
    display(output);
    printf("\n");
    return 0;
}
