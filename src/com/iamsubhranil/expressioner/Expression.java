/*
    Created By : iamsubhranil
    Date : 19/4/17
    Time : 4:52 PM
    Package : com.iamsubhranil.expressioner
    Project : Expressioner
*/
package com.iamsubhranil.expressioner;

import java.util.ArrayList;

public class Expression {

    private String expression;
    private int currentPos = -1;
    private final ArrayList<String> tokens = new ArrayList<>();

    public Expression(String exp) {
        expression = exp;
        if (!isValid())
            throw new IllegalArgumentException("The given expression is not valid in terms of parentheses!");
        tokenize();
        //   System.out.println("Tokens =>");
        //  tokens.forEach(token-> System.out.println(" "+token));
    }

    private boolean isValid() {
        char[][] braces = {{'(', ')'}, {'{', '}'}, {'[', ']'}};
        for (char[] brace : braces) {
            if (count(brace[0]) != count(brace[1]))
                return false;
        }

        return true;
    }

    private char charAt(int pos) {
        return expression.charAt(pos);
    }

    private void tokenize() {
        if (expression.equals(""))
            return;
        String[] tempTok = expression.split(" ");
        for (String temp : tempTok) {
            //    System.out.println("Analyzing : "+temp);
            String[] subtokens = temp.split("");
            String temptoken = "";
            for (String subtoken : subtokens) {
                //       System.out.println("\tSubtoken : "+subtoken);
                if (isOperator(subtoken.charAt(0)) || isBrace(subtoken.charAt(0))) {
                    //          System.out.println("\t\tFound brace or oprator : "+subtoken);
                    if (!temptoken.equals("")) {
                        //            System.out.println("\t\ttempToken contains : "+temptoken);
                        tokens.add(temptoken);
                        //            System.out.println("\t\tAdded tempToken..");
                        temptoken = "";
                    }
                    tokens.add(subtoken);
                    //        System.out.println("\tAdded operator token..");
                } else if (isValidChar(subtoken.charAt(0)))
                    temptoken += subtoken;
                else
                    throw new IllegalArgumentException("Expression contains invalid character '" + subtoken + "' !");
            }
            if (!temptoken.equals("")) {
                //   System.out.println("\tAdding token : "+temptoken);
                tokens.add(temptoken);
            }
        }
    }

    public String scanNextSymbol() {
        return tokens.get(++currentPos);
    }

    public void add(String c) {
        tokens.add(c);
    }

    public static boolean isOperator(char c) {
        return c == '+' || c == '*' || c == '-' || c == '/' || c == '%' || c == '^' || c == '$';
    }

    public static boolean isBrace(char c) {
        return c == ']' || c == '[' || c == '}' || c == '{' || c == '(' || c == ')';
    }

    public static boolean isValidChar(char c) {
        return Character.isLetter(c) || Character.isDigit(c);
    }

    public int priority(String c) {
        if (c.equals("^") || c.equals("$"))
            return 3;
        if (c.equals("*") || c.equals("/"))
            return 2;
        if (c.equals("+") || c.equals("-"))
            return 1;
        if (c.equals("%"))
            return 0;
        return -1;
    }

    private int count(char c) {
        int i = 0, count = 0;
        if (!expression.contains(c + ""))
            return 0;
        while (i < expression.length()) {
            if (expression.charAt(i) == c)
                count++;
            i++;
        }
        return count;
    }

    public String toString() {
        return tokens.toString();
    }

}
