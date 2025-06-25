#include "functionevaluator.h"
#include <QStack>
#include <QDebug>

bool FunctionEvaluator::containsVariable(const QString &expression)
{
    try {
        const QList<Parser::Token> tokens = Parser::tokenize(expression);
        for (const auto &token : tokens) {
            if (token.type == Parser::Token::Variable) {
                return true;
            }
        }
    } catch (const std::exception &e) {
    }
    return false;
}

double FunctionEvaluator::evaluate(const QString &expression, const double xValue, bool *ok)
{
    if (expression.isEmpty()) {
        if (ok) *ok = false;
        return 0.0;
    }

    try {
        const QList<Parser::Token> tokens = Parser::tokenize(expression);
        const QList<Parser::Token> postfix = Parser::infixToPostfix(tokens);

        if (ok) *ok = true;
        return evaluatePostfix(postfix, xValue);
    } catch (const std::exception &e) {
        qWarning() << "Ошибка вычислителя функции:" << e.what();
        if (ok) *ok = false;
        return 0.0;
    }
}

QList<QPointF> FunctionEvaluator::generatePoints(const QString &expression, const double xMin, const double xMax, const int numPoints)
{
    QList<QPointF> points;

    if (numPoints < 2 || xMin >= xMax) {
        return points;
    }

    QList<Parser::Token> postfix;

    try {
        const QList<Parser::Token> tokens = Parser::tokenize(expression);
        postfix = Parser::infixToPostfix(tokens);
    } catch (const std::exception &e) {
        qWarning() << "Ошибка парсинга выражения для графика:" << e.what();
        return points;
    }

    const double step = (xMax - xMin) / (numPoints - 1);

    for (int i = 0; i < numPoints; ++i) {
        const double x = xMin + i * step;

        try {
            if (const double y = evaluatePostfix(postfix, x); !std::isnan(y) && !std::isinf(y)) {
                points.append(QPointF(x, y));
            }
        } catch (const std::exception &e) {
        }
    }

    return points;
}

double FunctionEvaluator::evaluatePostfix(const QList<Parser::Token> &tokens, const double variableValue)
{
    QStack<double> stack;

    for (const Parser::Token &token : tokens) {
        switch (token.type) {
            case Parser::Token::Number:
                stack.push(token.numValue);
                break;

            case Parser::Token::Variable:
                stack.push(variableValue);
                break;

            case Parser::Token::Operator:
                if (token.value == "!") {
                    if (stack.isEmpty())
                        throw std::invalid_argument("Недостаточно операндов для '!'");

                    stack.push(factorial(stack.pop()));
                } else {
                    if (stack.size() < 2)
                        throw std::invalid_argument("Недостаточно операндов для оператора " + token.value.toStdString());

                    const double right = stack.pop();
                    const double left = stack.pop();
                    stack.push(applyOperator(token.value, left, right));
                }
                break;

            case Parser::Token::Function:
                if (stack.isEmpty())
                    throw std::invalid_argument("Недостаточно аргументов для функции " + token.value.toStdString());
                stack.push(applyFunction(token.value, stack.pop()));
                break;

            default:
                throw std::invalid_argument("Неожиданный токен в постфикс-выражении");
        }
    }

    if (stack.size() != 1)
        throw std::invalid_argument("Неверное выражение");

    return stack.pop();
}

double FunctionEvaluator::applyOperator(const QString &op, const double left, const double right)
{
    switch (op[0].unicode()) {
    case '+': return left + right;
    case '-': return left - right;
    case '*': return left * right;
    case '/':
        if (qFuzzyIsNull(right)) throw std::runtime_error("Деление на ноль");
        return left / right;
    case '%':
        if (qFuzzyIsNull(right)) throw std::runtime_error("Деление на ноль в операции остатка");
        return fmod(left, right);
    case '^': return qPow(left, right);
    default: throw std::invalid_argument("Неизвестный оператор: " + op.toStdString());
    }
}

double FunctionEvaluator::applyFunction(const QString &func, double arg)
{
    if (func == "sin") return qSin(Parser::isDegrees()? qDegreesToRadians(arg) : arg);
    if (func == "cos") return qCos(Parser::isDegrees()? qDegreesToRadians(arg) : arg);
    if (func == "tan") return qTan(Parser::isDegrees()? qDegreesToRadians(arg) : arg);
    if (func == "ln") { if (arg <= 0) throw std::runtime_error("ln(x≤0)"); return qLn(arg); }
    if (func == "log") { if (arg <= 0) throw std::runtime_error("log(x≤0)"); return log10(arg); }
    if (func == "sqrt") { if (arg < 0) throw std::runtime_error("√(x<0)"); return qSqrt(arg); }
    if (func == "abs") return qFabs(arg);
    if (func == "arctan") {
        const double res = qAtan(arg);
        return Parser::isDegrees() ? qRadiansToDegrees(res) : res;
    }
    if (func == "arcsin") {
        if (arg < -1 || arg > 1) throw std::runtime_error("arcsin(x<-1 || x>1)");
        const double res = qAsin(arg);
        return Parser::isDegrees() ? qRadiansToDegrees(res) : res;
    }
    if (func == "arccos") {
        if (arg < -1 || arg > 1) throw std::runtime_error("arccos(x<-1 || x>1)");
        const double res = qAcos(arg);
        return Parser::isDegrees() ? qRadiansToDegrees(res) : res;
    }

    throw std::invalid_argument("Неизвестная функция: " + func.toStdString());
}

double FunctionEvaluator::factorial(const double n)
{
    if (n < 0 || n != qFloor(n))
        throw std::runtime_error("Факториал определён только для неотрицательных целых чисел");

    if (n == 0 || n == 1) return 1;

    double res = 1;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}
