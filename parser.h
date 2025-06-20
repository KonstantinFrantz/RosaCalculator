#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStack>
#include <QStringList>

class Parser
{
public:
    struct Token {
        enum Type {
            Number,
            Operator,
            Function,
            LeftParen,
            RightParen
        };

        Type type;
        QString value;
        double numValue;
        int precedence;
        bool rightAssociative;
    };

    static double evaluate(const QString &expression, bool *ok = nullptr);
    static QList<Token> tokenize(const QString &expression);
    static QList<Token> infixToPostfix(const QList<Token> &tokens);
    static double evaluatePostfix(const QList<Token> &tokens);
    static int getOperatorPrecedence(const QString &op);
    static bool isRightAssociative(const QString &op);
    static bool isFunction(const QString &str);
    static double applyOperator(const QString &op, double left, double right);
    static double applyFunction(const QString &func, double arg);
    static double factorial(double n);
};

#endif // PARSER_H
