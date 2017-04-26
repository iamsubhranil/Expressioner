# Expressioner
### A dead simple expression evaluator

Expressioner is a program to evaluate mathematical expressions, containing only basic mathematical operators (i.e. + for addition, - for subtraction, * for multiplication, / for quotient division, % for modulo division, and ^ or $ for power of operation), any number of variables, and constants.

**The program works in the following way :**
1. Ask user for input
2. Tokenize the given input string
3. Check the semantic correctness of the tokenized expression and add any missing symbols
4. Convert the expression to postfix
5. Evaluate the postfix expression
6. Display the result to the user

**The program can be divided into four modules, namely**

1. <code>tokenize()</code> : This module performs tokenization over the given expression. It just separates different elements of the expression and places them in a singly linked list. It supports multiletter variables, intermediate spaces, trailing and leading spaces, values starting with or containing '.' (i.e. .7), variable names starting with or containing '\_', and handles all of them. For example, if a user gives <code>variable1+   var2 +anotherVariable -   .7</code>, then after tokenization it becomes <code>variable1 -> + -> var2 -> anothervariable -> - -> .7</code> where <code>-></code> denotes the link of the next item in the generated singly linked list. However, it does not allow all characters as a valid item of the expression. It only allows an expression to contain <code>a-z,A-Z,0-9,'.','\_',+*-^$%/</code>. Any character other than the defined set is trapped as an illegal character and the tokenizar stops right there.

2. <code>checkSemantics()</code> : This module performs basic semantic checks over the tokenized expression. This module checks for the following situations and performs the actions as mentioned :

Situation|Action
---|---
Variable name contains '.'|Illegal semantic, trap
Two adjacent variables without an operator|Trap
A variable and an adjacent brace without an operator specified|Insert '*'(multiplication operator) between them
Variable name starts with a number|Trap
Unbalanced operator|Trap
Brace found|Convert to either '(' or ')' as required
Unbalanced paranthesis|Trap

3. <code>convertToPostfix()</code> : This module does the conversion of infix to postfix after the given expression is tokenized and said to be valid in terms of semantics. This module however does not know what to do with anything other than symbols starting with either '(' or ')',a-z,A-Z,'.','\_',0-9. So it is necessary for the expression to be perfectly tokenized. Also the method will most likely halt to a segfault if the given expression is semantically incorrect. For that, the semantic check is required before it too.

4. <code>evaluatePostfix()</code> : After the given expression has been converted to postfix, this method does the work of evaluation. This method checks for the variables in the expression using the submodule <code>getValues()</code>, and asks the user for the value of the variables present in the expression before evaluation. However, this method parses any constant silently, and directly uses the value of it while evaluation. After evaluation, this module shows the resulting value to the user.

Expressioner does not support common mathematical functions, and I do not intend to add support for it. I always want something very simple perform a rather complicated task, and hence this program will never support anything other than the basics of expression parsing and evaluation. However, I have plan to add prefix and prefix evaluation support recently.

Last but not least, this is a GPLv3 project. Do whatever you want with it. Just mention this repo in your project.
