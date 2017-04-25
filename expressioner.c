#include<stdio.h> // printf() scanf()
#include<stdlib.h>  // malloc()
#include<ctype.h> // isalnum()
#include<string.h> // strlen(), strtod()
#include<math.h>
#include<float.h>

/*
 * The linked list containing the symbols of the given expression.
 * This list is utilised by almost all the core methods, and the methods
 * return this type of list if needed.
 */
typedef struct Symbol {
	char *value;	// The string value
	struct Symbol *next; // Pointer to next symbol
} Symbol;

/*
 * The stack, used by most of the methods as an intermediate
 * data structure.
 */
typedef struct Stack {
	Symbol *top; // The stack pointer
	int count; // Count of nodes
} Stack;

/*
 * A one-liner to create a new Symbol *
 */
Symbol * newSymbol(){
	Symbol *symbol = (Symbol *)malloc(sizeof(Symbol)); // Allocate memory for a symbol
	symbol->value = NULL; // Clear the value
	symbol->next = NULL; // Clear the link
	return symbol;
}
/*
 * This method does the conversion of char->Symbol * when needed,
 * typically while storing an operator or a brace.
 */
Symbol * createCharacterSymbol(char c){
	Symbol *symbol = newSymbol(); //Allocate memory for the pointer
	symbol->value = (char *)malloc(sizeof(char)); //Allocate memory for the char
	(*symbol->value) = c; //Copy the character to value
	return symbol;
}

/*
 * This method does the conversion of char *->Symbol * when needed,
 * typically while handling a string
 */
Symbol * createStringSymbol(char *s){
	Symbol *symbol = newSymbol();
	symbol->value = strdup(s);
	return symbol;
}

/*
 * Adds the given character to the buffer. Since it modifies
 * the buffer itself, it returns the pointer in any case.
 * However, the pointer may not be reassigned to a new address
 * after calling realloc(). Also, there is no typical NULL
 * check after realloc, because it will practically never happen.
 * 
 * Arguments => buffer : The buffer to extend
 * 		bufferSize : A pointer to the present size of the buffer
 * 		add : The character to add
 * Returns   => The newly relocated buffer
 */
char * addToBuffer(char *buffer, size_t *bufferSize, char add){
	buffer = (char *)realloc(buffer, ++(*bufferSize)); //Call realloc to extend the buffer to bufferSize+1
	(*(buffer+(*bufferSize)-1)) = add; //Add the character to the newly available position
	return buffer;
}

/*
 * This method does all the hardwork needed to add
 * a symbol to a given list with head, and updates the
 * backup prev pointer. This method also returns the newly
 * created symbol.
 */
Symbol * addBufferToList(char *buffer, size_t *bufferSize, Symbol **head, Symbol **prev){
	buffer = addToBuffer(buffer, bufferSize, '\0'); // Terminate the string
	Symbol *symbol = createStringSymbol(buffer); // Create a new symbol with the string
	if(*prev==NULL){ // This is the first node
		*head = symbol;
	}
	else{
		(*prev)->next = symbol; // Link to the previous node
	}
	*prev = symbol; // The added string is the last symbol of the list
	return symbol;
}

/*
 * One-liner to create a new stack
 */
Stack * newStack(){
	Stack *stack = (Stack *)malloc(sizeof(Stack)); // Allocate memory
	stack->count = 0; // Reset count
	stack->top = NULL; // Reset link
	return stack;
}

/*
 * The typical push method for the stack, which pushes
 * a symbol in it. This method however, does not push the
 * exact argument pointer to the stack, because often they are part of
 * another list. It rather creates a new symbol, copies the value
 * of the argument symbol to the new one, and then inserts the new symbol
 * in the stack.
 */
void push(Stack *stack, Symbol *symbol){
	Symbol * insert = newSymbol(); //Create a new symbol with a dummy value
	insert->value = strdup(symbol->value); //Copy the given value to the new symbol
	insert->next = stack->top; //Adjust pointers
	stack->top = insert; //Insert the symbol at the top of the stack
	stack->count++; //Increment the value of count
}

/*
 * Typical pop method for the stack, which returns a
 * symbol from the top of the stack. Right now, there
 * are no failsafe for this method, i.e. it'll allow
 * pop even if count==0, returning a NULL value and setting
 * count to -1. This is such because it is assumed that this
 * never is the case, will be modified later.
 */
Symbol * pop(Stack *stack){
	Symbol *sym = stack->top; //Obtain the top pointer
	stack->top = stack->top->next; //Shift the top pointer to the next node
	stack->count--; //Decrement count
	return sym;
}

/*
 * A method to check if the given character is a defined operator.
 * Note that it is assumed that the operator will always be a
 * single character, and for that right now there are no support
 * for compound operators as sin() cos() etc.
 */
int isOperator(char c){
	return c=='+' || c=='-' || c=='*' || c=='/' || c=='%' || c=='^' || c=='$';
}

/*
 * Checks whether the given character is a brace. It is safe to
 * assume that for a string to be a brace, the first character
 * of the string has to be a brace. Hence, passing a single letter is
 * truly justified here.
 */
int isBrace(char c){
	return c==']' || c=='[' || c=='}' || c=='{' || c==')' || c=='(';
}

//#define SHOW_STEPS //The debug switch

/*
 * This method perform tokenization over a given expression.
 * The tokenized expression is returned as a singly linked list,
 * whose each node contains exactly one symbol of the given
 * expression, left to right, whitespace trimmed. This method 
 * also permits expressions containing intermediate or extra spaces,
 * and it trims all of them. It scans the expression from left to right,
 * identifying any contiguous set of characters as a single symbol
 * of the given expression until a space, brace or operator is
 * found.
 * 
 * Arugments => expression : The string to be tokenized
 * 		size : Size of the size
 * Returns   => The given expression in tokenized form
 * 		in a singly linked list
 */
Symbol *tokenize(char *expression, size_t size){
	Symbol *head = NULL; // The head of resulting expression
	Symbol *prev = NULL; // A temporary pointer to the previous node of the resulting expression
	size_t count = 0; // The position of character that is scanned
	char *buffer = NULL; // The buffer containing a symbol, or part of a symbol, which later
			     //	gets added to the resulting list a Symbol *
	size_t bufferSize = 0; // A variable denoting the size of the given buffer

	printf("\n Tokenizing given expression..\n");

	while(count<size-1){	// Continue till the last character
		char currentSymbol = *(expression+count); // Get the present character
#ifdef SHOW_STEPS
		printf("\n Current symbol : %c", currentSymbol);
#endif
		if(isBrace(currentSymbol) || isOperator(currentSymbol)){ // Check if current operator is a brace or an operator
#ifdef SHOW_STEPS
			printf("\n Brace or operator found : %c", currentSymbol);	
#endif			
			Symbol *operator;

			operator = createCharacterSymbol(currentSymbol); // Create a symbol with the current operator
			operator->next = NULL; // Reset the next pointer

			if(buffer!=NULL){ // The buffer contains a set of characters, so it needs to be added before the
					  // present brace or operator
#ifdef SHOW_STEPS
				printf("\n\tPrevious buffer exists!");
#endif
				Symbol *symbol = addBufferToList(buffer, &bufferSize, &head, &prev);
#ifdef SHOW_STEPS
				printf("\n\tValue of buffer : %s", symbol->value);
#endif
				symbol->next = operator; // Make the brace or operator next member of the resulting expression
				buffer = NULL; // Reset the buffer
				bufferSize = 0; // Reset the size
			}
			else{ // Buffer is NULL, so we just need to add the brace or operator
				if(head==NULL) // Check the head
					head = operator; // Set it
				else
					prev->next = operator; // Link to the previous node
			}
			
			prev = operator; // At any case, the present brace or operator is going to be the last symbol of the list
		}
		else if(currentSymbol==' '){ // A space is found
#ifdef SHOW_STEPS
			printf("\n Space found..");
#endif
			if(buffer!=NULL){ // Buffer contains a string, so it needs to be added
#ifdef SHOW_STEPS
				printf("\n\tPrevious buffer exists!");
#endif
				Symbol * sy = addBufferToList(buffer, &bufferSize, &head, &prev); // Terminate the string
#ifdef SHOW_STEPS
				printf("\n\tValue of buffer : %s", sy->value);
#endif
			}
			buffer = NULL; // Reset buffer
			bufferSize = 0; // Reset size
		}
		else if(isalnum(currentSymbol) || currentSymbol=='.' || currentSymbol=='_'){
			// The character in hand is either a-z,A-Z,0-9,'.','_' which are the only
			// permissible set of characters in the expression
#ifdef SHOW_STEPS
			printf("\n Added to current buffer : %c", currentSymbol);
#endif
			buffer = addToBuffer(buffer, &bufferSize, currentSymbol); // Add the symbol in hand to the buffer
		}
		else{ // The character in hand should not be in the expression
			printf("\nError : Unknown operator or symbol '%c' !\n", currentSymbol);
			exit(2);
		}
		count++; // Proceed to the next character
	}
	
	if(buffer!=NULL){ // Check if we have the last symbol
#ifdef SHOW_STEPS
		printf("\n\tPrevious buffer exists!");
#endif
		Symbol * buf = addBufferToList(buffer, &bufferSize, &head, &prev); // Terminate the string
#ifdef SHOW_STEPS
		printf("\n\tValue of buffer : %s", buffer);
#endif
	}
		
	return head; // Return the head of the resulting tokenized expression
}

/*
 * This method performs a standard semantic check over a tokenized, whitespace
 * trimmed expression. Because in many of the cases the method checks only
 * the first letter of a symbol to decide the step, it is necessary for the
 * given expression to be tokenized properly, otherwise the behaviour is undefined.
 * This method covers a handful of necessary semantic checks over an expression,
 * including but not limited to paranthesis balancing, operator balancing,
 * variable identifier validation and so on. At the end, this method converts
 * all type of braces to either '(' or ')', and adds any missing multipler operator
 * between a variable and an openning brace implicitly. This method is really
 * necessary before converting an expression to either of postfix or prefix,
 * otherwise the later method will most likely halt to a segfault if there is
 * any type of error in the expression.
 * 
 * Arguments => head : The start pointer of the tokenized expression
 */
void checkSemantics(Symbol *head){
	printf("\n Performing semantic analysis..\n");

	Symbol *prev = NULL; // Pointer to the previous symbol in the list
	size_t bracketCount = 0; // Count of brackets to perform paranthesis balancing check
	Symbol *temp = head; // Temporary pointer for traversal
	while(temp!=NULL){ // Continue until the last symbol
		Symbol *current = temp; // Copy the current symbol
		char cur = *(current->value); // Extract the first character of current symbol
		
		if(isalnum(cur) || cur=='.' || cur=='_'){ // The symbol is a variable or constant
			if(isalpha(cur) || cur=='_'){ // The symbol should be a variable as it starts with an alphabet or '_'
				if(strchr(current->value, '.')!=NULL){ // Variable identifer should not contain '.'
					printf("\n Error : Variable name '%s' contains '.' ! \n", current->value);
					exit(9);
				}
#ifdef SHOW_STEPS
				else
					printf("\n Found variable : %s \n",current->value);
#endif
			}
			else{ // The symbol should be a constant as it starts with a digit
				char *err; // Pointer to the remaining portion of the symbol after the last digit
				double d = strtod(current->value, &err); // Try to parse the symbol
				if(strlen(err)!=0){ // The symbol does contain some letters after one or more preceding digits
					printf("\n Error : Variable name '%s' precedes with a number! \n", current->value);
					exit(10);
				}
#ifdef SHOW_STEPS
				else
					printf("\n Found constant value : %s \n", current->value);
#endif
			}
			if(temp==head){ // If it is the first symbol, skip
				prev = temp; // Update previous node
				temp = temp->next; // Skip to next node
				continue;
			}
			char pre = *(prev->value); // Extract the previous character
			if(pre==')'){ // The previous character is a closing brace, so there must be a multiplier operator
				      // between previous and current symbol
				Symbol *mul = createCharacterSymbol('*'); // Create the multiplier symbol
				mul->next = prev->next; // Insert present symbol to the next node of the multiplier
				prev->next = mul; // Insert the operator symbol before the current symbol
#ifdef SHOW_STEPS
				printf("\n Alphanumeric operator : Inserted '*' between %s and %s \n", prev->value, current->value);
#endif
			}
			else if(isalnum(pre)){ // Previous symbol is also a variable!
				printf("\n Error : No operator specified between operands %s and %s !\n", prev->value, current->value);
				exit(5);
			}
			
		}
		else if(isOperator(cur)){ // Current symbol is an operator
			if(temp==head){ // But this is the start of the expression!
				printf("\n Error : An expression cannot start with an operator!\n");
				exit(6);
			}
			char pre = *(prev->value); // Extract the previous symbol
			char nextSymbol; // Variable to store the next symbol, if found
			if(current->next==NULL){ // There is no symbol after this
				nextSymbol = '\n'; // Assume the next symbol as a newline
			}
			else // There is a symbol after this
				nextSymbol = *(current->next->value); // Get the first character of it

			if(nextSymbol=='{' || nextSymbol=='[') // Represent any type of openning brace
				nextSymbol = '(';	       // as '(', we need not to care about the
							       // previous symbol as it has already been
							       // converted to either '(' or ')'

			/*
			 * An operator will only be said to be balanced if it is placed between
			 * 1. An operand and an openning brace
			 * 2. Two operands
			 * 3. A closing brace and an operand
			 * If all of these conditions are false, then the operator is definitely
			 * unbalanced. Moreover, an operand can start with an alphabet,
			 * or a '.'(i.e.: .7), or a '_'(i.e.: _value1).
			 */
			if((!isalnum(pre) && pre!=')' && pre!='.' && pre!='_') || (nextSymbol!='(' && nextSymbol!='.' && nextSymbol!='_' && !isalnum(nextSymbol))){ // All of the above conditions are false, the operator is unbalanced
				printf("\n Error : Unbalanced operator '%c' [previous symbol '%s', next symbol '%s'] !\n", cur, prev->value, nextSymbol=='\n'?"(newline)":current->next->value);
				exit(4);
			}
		}
		else if(isBrace(cur)){ // Current symbol is a brace
#ifdef SHOW_STEPS
			printf("\n Brace found : %c \n", cur);
#endif		
			if(cur=='(' || cur=='{' || cur=='['){ // It is an openning brace
				current->value = "("; // Convert any type of openning brace to '('
				cur = '('; // Set the current variable accordingly
				bracketCount++; // Increase the bracketCount to denote the openning
			}
			else if(cur==')' || cur=='}' || cur==']'){ // It is a closing brace
				if(temp==head){ // But we are at the start of the expression!
					printf("\n Error : An expression cannot start with a closing brace!\n");
					exit(8);
				}
				current->value = ")"; // Convert to ')'
				cur = ')'; // Set the current variable
				bracketCount--; // Decrease the bracketCount to denote the closing
			}

			if(prev!=NULL && cur=='(' && *prev->value!='(' && !isOperator(*prev->value)){ 
				// There is a previous symbol which is an operand or closing brace 
				// and the present is an openning brace, so insert '*' operator 
				// between them implicitly
				
				Symbol *mul = createCharacterSymbol('*'); // Create the operator
				mul->next = prev->next; // Insert the present brace next to it
				prev->next = mul; // Insert the operator before the present brace
#ifdef SHOW_STEPS
			printf("\n Brace operator : Inserted '*' between %s and %s \n", prev->value, current->value);
#endif		
			}
		}
		prev = temp; // Copy the present symbol
		temp = temp->next; // Proceed to the next symbol in the list
	}
	if(bracketCount!=0){ // Number of openning braces is not equal to the number of closing ones
		printf("\n Error : The given expression is not balanced in terms of parantheses!\n");
		exit(6);
	}
}

/*
 * A simple display method to display a singly linked list
 */
void display(Symbol *head){
	if(head==NULL) // Nothing to print
		return;
	Symbol *temp = head; // Copy the pointer for traversal
	printf(" [ %s", temp->value); // Print the first value
	temp = temp->next; // Proceed to next
	while(temp!=NULL){ // Continue until the last node
		printf(", %s", temp->value); // Print the value of present node
		temp = temp->next; // Proceed to next node
	}
	printf(" ] ");
}

/*
 * A method to check the priority of an operator.
 * Since this method returns a relative priority between
 * the operators, all operators in comparison must be
 * used with this method only.
 */
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

/*
 * Implementation of the algorithm of postfix conversion.
 * This method expects each node of the given list to contain
 * a single symbol of the expression, and it does not know
 * what to do otherwise. It also does not know what to do
 * with an unbalanced expression, and for that reason,
 * it is recommended to pass the given expression first
 * with the method tokenize(), then checkSemantics(), and then
 * to this method to ensure proper conversion.
 * 
 * Arguments => head : The first symbol of the a semantically correct
 * 			expression in tokenized form
 * Returns   => A pointer to the head of the resulting postfix expression
 */
Symbol * convertToPostFix(Symbol *head){
	Stack *stack = newStack();

	Symbol * output = NULL;
	Symbol * prevOutput = NULL;

	push(stack, createCharacterSymbol('('));

	Symbol *temp = head;
	while(temp->next!=NULL){
		temp = temp->next;
	}
	temp->next = createCharacterSymbol(')');
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

/*
 * An intermediate structure to map
 * variable names to their values
 */
typedef struct Variable{
	char *name; // Name of the variable
	double value; // Value of the variable
	struct Variable *next; // Link to the next variable
} Variable;

/*
 * This is basically a search method for
 * the link list starting with 'head'. This
 * method traverses the whole list for the
 * variable name of 's', if found return the
 * value member of the variable. If no such
 * variable found in the list, this method returns
 * NAN to indicate a failure.
 */
double getValue(Variable *head, char *s){
	Variable *temp = head; // Create a temporary pointer
#ifdef SHOW_STEPS
	printf("\n Searching values for %s \n", s);
#endif
	while(temp!=NULL){ // Traverse till the last node of the list
#ifdef SHOW_STEPS
		printf("\n\t Current variable : %s \n", temp->name);
#endif
		if(strcmp(temp->name, s)==0){ // Variable name 's' found
			return temp->value; // Return the value of the variable
		}
		temp = temp->next; // Switch to next variable
	}
#ifdef SHOW_STEPS
	printf("\n\tValue not found! \n");
#endif
	return NAN; // Variable 's' not found in the list!
}

/*
 * This method basically generates a Variable list
 * from a Symbol list. In the resulting Variable list,
 * only the variables with their values are added, 
 * all the constants and operators are kept as it is in 
 * the argument Symbol list. This method does not know 
 * what to do with braces, so it must be called after 
 * convertTo*Fix().
 */
Variable * getValues(Symbol *start){
	Symbol *temp = start; // Create temp pointers
	Variable *head = NULL, *prev = NULL; // Pointers to keep track of the new list
#ifdef SHOW_STEPS
	printf("\n Getting values for variables \n");
#endif
	while(temp!=NULL){ // Traverse till the last node
		char cur = *temp->value; // Extract the first character of present symbol
		if(isalpha(cur) || cur=='_'){ // Present symbol is a variable
#ifdef SHOW_STEPS
			printf("\n Value for variable %s \n", temp->value);
#endif
			if(isnan(getValue(head, temp->value))){ // There is no value for temp yet in the
								// Variable list
#ifdef SHOW_STEPS
				printf("\n\tValue not found in index!\n");
#endif
				Variable *var = (Variable *)malloc(sizeof(Variable)); // Allocate memory
				var->name  = strdup(temp->value); // Assign the variable name
				printf("\n Enter the value of '%s' : ", temp->value); // Ask for the value
				scanf("%lf", &var->value); // Load the value in the variable
	
				if(head==NULL) // This is the first variable
					head = var;
				else // There is another variable
					prev->next = var;
				prev = var; // Obviously this is the last variable
			}	
#ifdef SHOW_STEPS
			else
				printf("\n Value found in index %g \n", getValue(head, temp->value));
#endif
		}
		
		temp = temp->next; // Switch to next Symbol
	}
	return head;
}

/*
 * This method evalutes a postfix expression
 * whose first symbol is passed as the argument.
 * Internally, this method first populates a Variable
 * list for each unknown variable, starts evaluation,
 * in the way retrieves the value of any unknown variables
 * from the newly created Variable list.
 */
void evaluatePostfix(Symbol *start){
	Stack *stack = newStack(); // Create a new stack as an intermediate data strcuture
	Symbol *temp = start; // Create a temporary pointer
	printf("\n Evaluating the expression.. \n");
	Variable *values = getValues(start); // Populate the variable list

	while(temp!=NULL){ // Traverse to the last node
		char cur = *temp->value; // Extract the first character of present Symbol
		if(isalnum(cur) || cur=='_' || cur=='.'){ // The symbol is an operand
#ifdef SHOW_STEPS
			printf("\n Pushed to stack %s \n", temp->value);
#endif
			push(stack, createStringSymbol(temp->value)); // Push it to the stack
		}
		else{ // Current symbol is an operator
			char *s1 = pop(stack)->value; // Pop second operand
			char *s2 = pop(stack)->value; // Pop the first operand
			char *err; // Error pointer
			double x = strtod(s2, &err); // Try to parse first operand as a constant double
			if(*err!=0){ // First operand does contain an alphabet, it's a variable
				x = getValue(values, s2); // Retrieve the value from the Variable list
			}
			err = NULL; // Reset the error pointer
			double y = strtod(s1, &err); // Try to parse the second operand
			if(*err!=0){ // Second operand is a variable
				y = getValue(values, s1); // Retrieve the value from the Variable list
			}
#ifdef SHOW_STEPS
			printf("\n %s : %g %s : %g \n", s2, x, s1, y);
#endif
			double res; // Temporary result variable
			switch(cur){ // Evaluate present subexpression as 'res = x cur y'
				case '+' : res = x + y;
					   break;
				case '-' : res = x-y;
					   break;
				case '*' : res = x * y;
					   break;
				case '/' : res = x / y;
					   break;
				case '^' : 
				case '$' : res = pow(x, y);
					   break;
				case '%' : res = (int)x % (int)y;
					   break;
			}
#ifdef SHOW_STEPS
			printf("\n\t Result : %g \n", res);
#endif
			int extra = 1 + (res<0);
			int s = (int)((ceil(log10(fabs(res)))+extra)*sizeof(char)); // Find the number of digits in 'res'
			if(res==0)
				s = 1;
			char *r = (char *)malloc(s); // Allocate memory for 'res'
#ifdef SHOW_STEPS
			printf("\n Malloced %d bytes \n",s);
#endif
			sprintf(r, "%g", res); // Convert 'res' to string
#ifdef SHOW_STEPS
			printf("\n\t Pushing result back to the stack \n");
#endif
			push(stack, createStringSymbol(r)); // Push 'res' back in the stack
		}
		temp = temp->next; // Switch to the next symbol
	}
	char *err; // Error pointer, it should not be needed here
	double res = strtod(pop(stack)->value, &err); // Pop the remaining item from stack
							// and parse it to double, it is our
							// final result
	printf("\n Final result : %g\n", res);
}

/*
 * The driver of the program
 */
int main()
{
    char *line = NULL; // A pointer to contain the user given string
    size_t temp, size, end = -1; // Variables for various use
    printf(" Enter the expression : ");
    size = getline(&line, &temp, stdin); // Get the expression
    if(size==end){
    	printf(" Enter an expression to continue!");
	return 1;
    }
    Symbol *head = tokenize(line, size); // Tokenize the given expression
    printf("\n Tokenized expression : ");
    display(head); // Display the tokenized expression
    printf("\n");
    checkSemantics(head); // Perform semantic check over the tokenized expression
    printf("\n Corrected expression : ");
    display(head); // Print the corrected expression
    printf("\n");
    Symbol *output = convertToPostFix(head); // Convert the expression to postfix
    printf("\n Postfix expression : ");
    display(output); // Print the postfix expression
    printf("\n");
    evaluatePostfix(output);
    return 0;
}
