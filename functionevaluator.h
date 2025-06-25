#ifndef FUNCTIONEVALUATOR_H
#define FUNCTIONEVALUATOR_H

#include <QPointF>
#include "parser.h"

class FunctionEvaluator
{
public:
    static bool containsVariable(const QString &expression);
    static double evaluate(const QString &expression, double xValue, bool *ok = nullptr);
    static QList<QPointF> generatePoints(const QString &expression, double xMin, double xMax, int numPoints);

private:
    static double evaluatePostfix(const QList<Parser::Token> &tokens, double variableValue);
    static double applyOperator(const QString &op, double left, double right);
    static double applyFunction(const QString &func, double arg);
    static double factorial(double n);
};

#endif // FUNCTIONEVALUATOR_H
