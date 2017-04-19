package com.iamsubhranil.expressioner;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.DecimalFormat;
import java.util.HashMap;
import java.util.Stack;

public class Main {

    public static void main(String[] args) {

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        System.out.println("Enter the expression : ");
        try {
            Expression outputExpression = toPostFix(new Expression(reader.readLine()));
            System.out.println("Converted postfix expression : " + outputExpression.toString());
            double res = evaluate(outputExpression);
            System.out.println("\nEvaluated value : " + new DecimalFormat().format(res));
        } catch (IOException e) {
            System.err.println("Unable to read the expression");
            e.printStackTrace();
        }

    }

    private static double evaluate(Expression expression) {
        double ret;
        expression.add(")");
        Stack<String> tempStack = new Stack<>();
        String item = expression.scanNextSymbol();
        HashMap<String, Double> valueMap = new HashMap<>();
        while (!item.equals(")")) {
            if (Expression.isOperator(item.charAt(0))) {
                String Y = tempStack.pop();
                String X = tempStack.pop();
                double x, y;
                if (valueMap.containsKey(Y))
                    y = valueMap.get(Y);
                else {
                    y = resolveValue(Y, null);
                    valueMap.put(Y, y);
                }
                if (valueMap.containsKey(X))
                    x = valueMap.get(X);
                else {
                    x = resolveValue(X, null);
                    valueMap.put(X, x);
                }
                double res = Double.NaN;
                switch (item.charAt(0)) {
                    case '+':
                        res = x + y;
                        break;
                    case '-':
                        res = x - y;
                        break;
                    case '*':
                        res = x * y;
                        break;
                    case '/':
                        res = x / y;
                        break;
                    case '%':
                        res = x % y;
                        break;
                    case '^':
                    case '$':
                        res = Math.pow(x, y);
                        break;
                }
                tempStack.push(res + "");
            } else
                tempStack.push(item);

            item = expression.scanNextSymbol();
        }
        ret = Double.parseDouble(tempStack.pop());
        return ret;
    }

    private static double resolveValue(String item, BufferedReader reader) {
        try {
            double value = Double.parseDouble(item);
            return value;
        } catch (NumberFormatException nfe) {
            System.out.println("Enter the value of " + item + " : ");
            if (reader == null)
                reader = new BufferedReader(new InputStreamReader(System.in));
            try {
                double value = Double.parseDouble(reader.readLine());
                return value;
            } catch (NumberFormatException nfe2) {
                System.err.println("Oops! Please insert a numeric value!");
                return resolveValue(item, reader);
            } catch (IOException e) {
                e.printStackTrace();
                return Double.NaN;
            }
        }
    }

    private static Expression toPostFix(Expression expression) {
        Stack<String> tempStack = new Stack<>();
        Expression outputExpression = new Expression("");
        tempStack.push("(");
        expression.add(")");
        while (!tempStack.isEmpty()) {
            String item = expression.scanNextSymbol();
            if (Expression.isOperator(item.charAt(0)) || item.equals("(")) {
                String temp = tempStack.pop();
                if (Expression.isOperator(item.charAt(0))) {
                    if (expression.priority(temp) >= expression.priority(item)) {
                        outputExpression.add(temp);
                        tempStack.push(item);
                    } else {
                        tempStack.push(temp);
                        tempStack.push(item);
                    }
                }
                if (item.equals("(")) {
                    tempStack.push(temp);
                    tempStack.push(item);
                }
            } else if (item.equals(")")) {
                while (!item.equals("(")) {
                    item = tempStack.pop();
                    if (!item.equals("("))
                        outputExpression.add(item);
                }
            } else {
                outputExpression.add(item);
            }
        }
        return outputExpression;
    }
}
