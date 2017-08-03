class BinaryTree:
    def __init__(self, value):
        self.value = value
        self.children = []

    def getRightChild(self):
        return self.children[1]

    def getLeftChild(self):
        return self.children[0]

    def setLeftChild(self, child):
        self.children.insert(0, child)

    def setRightChild(self, child):
        self.children.insert(1, child)

    def getValue(self):
        return self.value

    def __str__(self, level=0):
        ret = "\t" * level + repr(self.value) + "\n"
        for child in self.children:
            ret += child.__str__(level + 1)
        return ret


def isOperator(x):
    return x == "+" or x == "-" or x == "*" or x == "/" or x == "^" or x == "%"


def priority(x):
    if x == "^":
        return 3
    if x == "*" or x == "/" or x == "%":
        return 2
    if x == "+" or x == "-":
        return 1
    else:
        return 0


def isClosingBrace(x):
    return x == ")" or x == "}" or x == "]"


def isOpeningBrace(x):
    return x == "(" or x == "{" or x == "["


def isBrace(x):
    return isClosingBrace(x) or isOpeningBrace(x)


def isVar(x):
    return x[0].isalpha() or x[0] == "_"


def isNumber(x):
    return x[0].isdigit() or x[0] == "."


def checkName(x):
    for i in range(1, len(x)):
        if x[i].isalnum() or x[i] == "_":
            pass
        else:
            raise ValueError


def tokenize(exp):
    tokenList = []
    buff = ""
    for i in range(0, len(exp)):
        if isOperator(exp[i]) or isBrace(exp[i]):
            if buff is not "":
                tokenList.append(buff)
                buff = ""
            if isBrace(exp[i]):
                if isOpeningBrace(exp[i]):
                    if i > 0 and (not (isOpeningBrace(exp[i - 1]) or isOperator(exp[i - 1]))):
                        tokenList.append("*")
                    tokenList.append("(")
                else:
                    tokenList.append(")")
                    if i < len(exp) - 1 and (not (isClosingBrace(exp[i + 1]) or isOperator(exp[i + 1]))):
                        tokenList.append("*")
            else:
                tokenList.append(exp[i])
        elif exp[i] == " ":
            if buff is not "":
                tokenList.append(buff)
                buff = ""
        else:
            buff += exp[i]
    if buff is not "":
        tokenList.append(buff)
    return tokenList


def validateTokens(tokenList):
    cont = 1
    for token in tokenList:
        if isVar(token):
            try:
                checkName(token)
            except ValueError:
                print("[ERROR] Bad variable name %s" % token)
                cont = 0
        elif isNumber(token):
            try:
                float(token)
            except ValueError:
                print("[ERROR] Bad constant %s" % token)
        elif isOperator(token) or isBrace(token):
            pass
        else:
            print("[ERROR] Bad token %s" % token)
    return cont


def topostfix(tokenList):
    postfixexp = []
    tempStack = []
    tokenList.append(")")
    tempStack.append("(")
    for token in tokenList:
        if isVar(token) or isNumber(token):
            postfixexp.append(token)
        elif isOpeningBrace(token):
            tempStack.append(token)
        elif isOperator(token):
            x = tempStack.pop()
            if priority(x) >= priority(token):
                postfixexp.append(x)
            else:
                tempStack.append(x)
            tempStack.append(token)
        else:
            x = tempStack.pop()
            while x is not "(":
                postfixexp.append(x)
                x = tempStack.pop()
    if len(tempStack) > 0:
        raise IndexError
    return postfixexp


def totree(postfixExp):
    tempStack = []
    for token in postfixExp:
        if isNumber(token) or isVar(token):
            tempStack.append(BinaryTree(token))
        else:
            x = tempStack.pop()
            y = tempStack.pop()
            z = BinaryTree(token)
            z.setLeftChild(x)
            z.setRightChild(y)
            tempStack.append(z)
    return tempStack.pop()


def getval(xstr):
    try:
        return float(xstr)
    except ValueError:
        try:
            x = float(input("[INPUT] Enter the value of %s : " % xstr))
            return x
        except ValueError:
            print("[ERROR] Enter a numeric value for %s!" % xstr)
            return getval(xstr)


def sumf(a, b):
    return a + b


def subf(a, b):
    return a - b


def mulf(a, b):
    return a * b


def divf(a, b):
    return a / b


def expf(a, b):
    return a ** b


def modf(a, b):
    return a % b


def evaluate(postfixExp):
    tempStack = []
    operations = {"+": sumf, "-": subf, "*": mulf, "/": divf, "^": expf, "%": modf}
    for token in postfixExp:
        if isOperator(token):
            y = getval(tempStack.pop())
            x = getval(tempStack.pop())
            res = operations[token](x, y)
            tempStack.append(str(res))
        else:
            tempStack.append(str(token))
    return getval(tempStack.pop())


exp = input("[INPUT] Enter the expression : ")
tokenList = tokenize(exp)
# print(tokenList)
cont = validateTokens(tokenList)
if not cont:
    print("[ERROR] Program terminated!")
else:
    try:
        tokenList = topostfix(tokenList)
        # print(tokenList)
        exptree = totree(tokenList)
        # print(exptree)
        res = evaluate(tokenList)
        print("[INFO] Result of expression : %g" % res)
    except IndexError as e:
        print("[ERROR] Given expression is invalid!")
    except ZeroDivisionError:
        print("[ERROR] Division by zero!")

