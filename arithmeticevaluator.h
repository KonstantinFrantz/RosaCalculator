#ifndef ARITHMETICEVALUATOR_H
#define ARITHMETICEVALUATOR_H

#include "parser.h"

class ArithmeticEvaluator
{
public:
    static double evaluate(const QString &expression, bool *ok = nullptr);

private:
    static double evaluatePostfix(const QList<Parser::Token> &tokens);
    static double applyOperator(const QString &op, double left, double right);
    static double applyFunction(const QString &func, double arg);
    static double factorial(double n);
};

#endif // ARITHMETICEVALUATOR_H